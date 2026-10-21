#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

const uint8_t ADDR[3] = {0x35, 0x22, 0x78};   // U3 (A2), U4 (B2), U5 (C2); addresses of hall sensors
const float RANGE_MT = 133.0;                 // x2 parts, low range
const uint8_t BTN[2] = {27, 28};              // SW1 = D1 (GPIO27), SW2 = D2 (GPIO28); pressed = LOW
const uint32_t DEBOUNCE_MS = 20;
Adafruit_NeoPixel ring(8, 26, NEO_GRB + NEO_KHZ800);   // D0 = GPIO26

bool btnRaw[2], btnState[2];   // btnState = debounced, true = pressed
uint32_t btnT[2];

void wr(uint8_t a, uint8_t reg, uint8_t v) {
  Wire1.beginTransmission(a);
  Wire1.write(reg);
  Wire1.write(v);
  Wire1.endTransmission();
}

void readButtons() {
  for (int i = 0; i < 2; i++) {
    bool r = !digitalRead(BTN[i]);
    if (r != btnRaw[i]) { btnRaw[i] = r; btnT[i] = millis(); }   // changed: restart timer
    else if (millis() - btnT[i] >= DEBOUNCE_MS) btnState[i] = r; // stable long enough
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT); digitalWrite(25, HIGH);  // user LED off
  pinMode(23, OUTPUT); digitalWrite(23, LOW);   // onboard RGB LED power off
  for (uint8_t p : BTN) pinMode(p, INPUT_PULLUP);

  ring.begin();
  ring.fill(ring.Color(16, 16, 16));
  ring.show();

  Wire1.setSDA(6); // D4
  Wire1.setSCL(7); // D5
  Wire1.begin();
  Wire1.setClock(400000);
  for (uint8_t a : ADDR) {
    wr(a, 0x08, 0x01);   // INT_CONFIG_1: MASK_INTB (INT tied to GND)
    wr(a, 0x00, 0x14);   // DEVICE_CONFIG_1: MAG_TEMPCO = 0%, CONV_AVG = 32x
    wr(a, 0x02, 0x70);   // SENSOR_CONFIG_1: enable X, Y, Z
    wr(a, 0x01, 0x12);   // DEVICE_CONFIG_2: LP_LN = low noise, continuous mode
  }
}

void loop() {
  readButtons();

  static uint32_t last;
  if (millis() - last < 50) return;   // print every 50 ms, buttons sampled every loop
  last = millis();

  for (uint8_t a : ADDR) {
    Wire1.beginTransmission(a); Wire1.write(0x12); Wire1.endTransmission(false);
    Wire1.requestFrom(a, (uint8_t)6);
    for (int i = 0; i < 3; i++) {
      uint8_t hi = Wire1.read(), lo = Wire1.read();
      int16_t raw = (hi << 8) | lo;
      Serial.print(raw * RANGE_MT / 32768.0, 2); Serial.print('\t');
    }
  }
  Serial.print(btnState[0]); Serial.print('\t');
  Serial.println(btnState[1]);
}
