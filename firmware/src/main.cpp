/*
Cosmic Mouse firmware
Copyright (c) 2026 Tyler Bowers
Licensed under the GNU AGPL-3.0
With select help from Claude (implementing model in C++ & HID)
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TinyUSB.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

Adafruit_NeoPixel ring(RING_LEDS, RING_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_USBD_HID usb_hid;

// HID descriptor 
static const uint8_t hidDesc[] = {
  0x05, 0x01, 0x09, 0x08, 0xA1, 0x01, // Generic Desktop, Multi-axis Controller, Application
  0xA1, 0x00, 0x85, 0x01, // Physical, Report ID 1
  0x16, 0xA2, 0xFE, // logical min
  0x26, 0x5E, 0x01, // logical max
  0x36, 0xA2, 0xFE, // physical min
  0x46, 0x5E, 0x01, // physical max
  0x09, 0x30, 0x09, 0x31, 0x09, 0x32, // translation axes: X, Y, Z
  0x09, 0x33, 0x09, 0x34, 0x09, 0x35, // rotation axes: RX, RY, RZ
  0x75, 0x10, 0x95, 0x06, 0x81, 0x06, 0xC0, // report size 16, 6 x int16, Input, end collection
  0xA1, 0x02, 0x85, 0x03, 0x05, 0x09, // Logical, Report ID 3: 2 buttons
  0x19, 0x01, 0x29, 0x02, // usage min/max
  0x15, 0x00, 0x25, 0x01, // logical min/max
  0x35, 0x00, 0x45, 0x01, // physical min/max
  0x75, 0x01, 0x95, 0x02, 0x81, 0x02, // report size 1, 2 x button, Input
  0x95, 0x06, 0x81, 0x03, 0xC0, 0xC0 // report size 6, Input, end collection
};

struct Calib {
  bool valid = false;
  float sigma = 1; // magnet polarity (+1 / -1; changed during cal)
  float p0[6]; // pose at rest
  float off[9]; // what the model cant explain at rest (mT) (residuals)
} cal;

float pose[6]; // current pose: x y z (mm), roll pitch yaw (rad)
bool btnRaw[2], btnState[2];
uint32_t btnT[2]; // time for debounce

// i/o
void wr(uint8_t a, uint8_t reg, uint8_t v){
  Wire1.beginTransmission(a);
  Wire1.write(reg);
  Wire1.write(v);
  Wire1.endTransmission();
}

bool readSensors(float B[9]){ // board-frame field (mT), polarity-corrected
  for (int s = 0; s < 3; s++){ // for each sensor U3,U4,U5
    Wire1.beginTransmission(ADDR[s]);
    Wire1.write(0x12);
    if (Wire1.endTransmission(false) != 0 || Wire1.requestFrom(ADDR[s], (uint8_t)6) != 6) return false;
    float v[3];
    for (int i = 0; i < 3; i++){
      uint8_t hi = Wire1.read(), lo = Wire1.read();
      int16_t raw = (int16_t)((hi << 8) | lo);
      if (raw == 32767 || raw == -32768){
        return false;   // saturated: skip this frame
      }
      v[i] = raw * RANGE_MT / 32768.0f;
    }
    float x = AXIS_SX * v[0], y = AXIS_SY * v[1], z = AXIS_SZ * v[2];
    float c = cosf(radians(PHI_DEG[s])), sn = sinf(radians(PHI_DEG[s]));
    B[3 * s]     = cal.sigma * (c * x - sn * y);
    B[3 * s + 1] = cal.sigma * (sn * x + c * y);
    B[3 * s + 2] = cal.sigma * z;
  }
  return true;
}

void readButtons(){ //ezpz
  for (int i = 0; i < 2; i++){ // for each button
    bool r = !digitalRead(BTN[i]);
    if (r != btnRaw[i]){ // changed state (clicked)
      btnRaw[i] = r;
      btnT[i] = millis();
    }
    else if (millis() - btnT[i] >= DEBOUNCE_MS){
      btnState[i] = r;
    }
  }
}

void ringSolid(uint8_t r, uint8_t g, uint8_t b){ 
  ring.fill(ring.Color(r, g, b));
  ring.show();
}

void ringSpin(){
  int h = (millis() / 80) % RING_LEDS;
  ring.clear();
  ring.setPixelColor((h + RING_LEDS + 2) % RING_LEDS, ring.Color(0, 0, 8));
  ring.setPixelColor((h + RING_LEDS + 1) % RING_LEDS, ring.Color(0, 0, 16));
  ring.setPixelColor(h, ring.Color(0, 0, 32));
  ring.setPixelColor((h + RING_LEDS - 1) % RING_LEDS, ring.Color(0, 0, 16));
  ring.setPixelColor((h + RING_LEDS - 2) % RING_LEDS, ring.Color(0, 0, 8));
  ring.show();
}

// LED sleep: after SLEEP_AFTER_MS of no activity, fade slowly to off; on activity, ramp up quickly.
uint32_t lastActive;
float ledLevel = 1; // 0 = off, 1 = RING_BRIGHTNESS
uint8_t ringShown = 255; // brightness last written

void wakeRing(){ 
  lastActive = millis(); 
  ledLevel = 1;
  ringShown = 255;
}   // after calibration / spin

void updateRingSleep(bool active){
  static uint32_t lastUpdate;
  uint32_t now = millis(), dt = now - lastUpdate;
  if (dt < 10) return;
  lastUpdate = now;
  if (active){
    lastActive = now;
  }
  bool asleep = now - lastActive >= SLEEP_AFTER_MS;
  ledLevel += asleep ? -(float)dt / FADE_OUT_MS : (float)dt / FADE_IN_MS;
  ledLevel = constrain(ledLevel, 0.0f, 1.0f);
  uint8_t v = (uint8_t)lroundf(RING_BRIGHTNESS * ledLevel * ledLevel); // squared = smoother-looking fade
  if (v != ringShown){ 
    ringShown = v;
    ringSolid(v, v, v);
  }
}

// Model 
// Magnets: M_j = t + R q_j, axis m = R z.  Field at sensor i = sum over all magnets (includes cross-talk).
void forward(const float p[6], float out[9]){
  float cr = cosf(p[3]), sr = sinf(p[3]), cp = cosf(p[4]), sp = sinf(p[4]), cy = cosf(p[5]), sy = sinf(p[5]);
  float R[3][3] = {{cy * cp, cy * sp * sr - sy * cr, cy * sp * cr + sy * sr},
                   {sy * cp, sy * sp * sr + cy * cr, sy * sp * cr - cy * sr},
                   {-sp,     cp * sr,                cp * cr}};
  float m[3] = {R[0][2], R[1][2], R[2][2]}, M[3][3];
  for (int j = 0; j < 3; j++)
    for (int a = 0; a < 3; a++)
      M[j][a] = p[a] + R[a][0] * Q[j][0] + R[a][1] * Q[j][1] + R[a][2] * Q[j][2];
  for (int i = 0; i < 3; i++){
    float B[3] = {0, 0, 0};
    for (int j = 0; j < 3; j++){
      float r[3] = {S[i][0] - M[j][0], S[i][1] - M[j][1], S[i][2] - M[j][2]};
      float d2 = r[0] * r[0] + r[1] * r[1] + r[2] * r[2], d = sqrtf(d2);
      float md = (m[0] * r[0] + m[1] * r[1] + m[2] * r[2]) / d2, f = K_MAG / (d2 * d);
      for (int a = 0; a < 3; a++) B[a] += f * (3 * r[a] * md - m[a]);
    }
    for (int a = 0; a < 3; a++) out[3 * i + a] = B[a];
  }
}

// r = model(p) + offsets - measured
float residual(const float p[6], const float B[9], bool useOff, float r[9]){
  forward(p, r);
  float c = 0;
  for (int i = 0; i < 9; i++){ r[i] += (useOff ? cal.off[i] : 0) - B[i]; c += r[i] * r[i]; }
  return c;
}

bool solve6(float A[6][7]){                    // Gaussian elimination on [A | b]
  for (int c = 0; c < 6; c++){
    int pv = c;
    for (int r = c + 1; r < 6; r++) if (fabsf(A[r][c]) > fabsf(A[pv][c])) pv = r;
    if (fabsf(A[pv][c]) < 1e-12f) return false;
    for (int k = 0; k < 7; k++){ float t = A[c][k]; A[c][k] = A[pv][k]; A[pv][k] = t; }
    for (int r = 0; r < 6; r++) if (r != c){
      float f = A[r][c] / A[c][c];
      for (int k = c; k < 7; k++) A[r][k] -= f * A[c][k];
    }
  }
  for (int r = 0; r < 6; r++) A[r][6] /= A[r][r];
  return true;
}

// Levenberg–Marquardt: minimize sum |B_meas - B_pred(p)|^2. Returns RMS residual (mT).
float lm(const float B[9], float p[6], int iters, bool useOff){
  float r[9], rt[9], J[9][6], A[6][7], lam = 1e-3f;
  float c = residual(p, B, useOff, r);
  for (int it = 0; it < iters; it++){
    for (int b = 0; b < 6; b++){               // numeric Jacobian
      float pp[6]; memcpy(pp, p, sizeof pp); pp[b] += 1e-3f;
      residual(pp, B, useOff, rt);
      for (int i = 0; i < 9; i++) J[i][b] = (rt[i] - r[i]) / 1e-3f;
    }
    bool accepted = false; float step = 0;
    for (int tries = 0; tries < 8 && !accepted; tries++){
      for (int a = 0; a < 6; a++){             // (J^T J + lam diag) dp = -J^T r
        for (int b = 0; b < 6; b++){ float s = 0; for (int i = 0; i < 9; i++) s += J[i][a] * J[i][b]; A[a][b] = s; }
        float g = 0; for (int i = 0; i < 9; i++) g += J[i][a] * r[i];
        A[a][6] = -g; A[a][a] *= 1 + lam;
      }
      if (!solve6(A)) break;
      float pt[6];
      for (int a = 0; a < 6; a++) pt[a] = p[a] + A[a][6];
      float ct = residual(pt, B, useOff, rt);
      if (ct < c){                              // better: accept, trust the model more
        step = 0;
        for (int a = 0; a < 6; a++) step = max(step, fabsf(A[a][6]));
        memcpy(p, pt, sizeof pt); memcpy(r, rt, sizeof rt); c = ct;
        lam = max(lam / 10, 1e-7f); accepted = true;
      } else lam *= 10;                          // worse: smaller, safer step
    }
    if (!accepted || step < 1e-5f) break;
  }
  return sqrtf(c / 9);
}

// Calibration 
// Knob untouched: average the field, fit the rest pose with nominal k, keep the leftover as offsets.
bool calibrate(){
  Serial.println("# calibrating...");
  uint32_t t0 = millis();
  while (millis() - t0 < CAL_SETTLE_MS){
    ringSpin();
    readButtons();
    delay(10);
  }

  cal.valid = false; cal.sigma = 1;
  for (int attempt = 0; attempt < 5; attempt++){
    float sum[9] = {}, mn[9], mx[9], B[9];
    bool ok = true;
    for (int n = 0; n < CAL_SAMPLES && ok; n++){
      ok = readSensors(B);
      for (int i = 0; i < 9; i++){
        sum[i] += B[i];
        mn[i] = n ? min(mn[i], B[i]) : B[i];
        mx[i] = n ? max(mx[i], B[i]) : B[i];
      }
      ringSpin();
      delay(5);
    }
    if (!ok){ 
      Serial.println("# cal: I2C read failed");
      continue;
    }
    bool still = true;
    for (int i = 0; i < 9; i++){
      sum[i] /= CAL_SAMPLES; 
      if (mx[i] - mn[i] > CAL_MAX_MOTION){
        still = false;
      }
    }
    if (!still){
      Serial.println("# cal: knob moved, retrying");
      continue;
    }

    float bz = sum[2] + sum[5] + sum[8];        // polarity: make the moment point up
    cal.sigma = bz < 0 ? -1 : 1;
    for (int i = 0; i < 9; i++){
      sum[i] *= cal.sigma;
    }

    float p[6] = {0, 0, Z_GUESS_MM, 0, 0, 0};
    float rms = lm(sum, p, LM_ITERS_CAL, false);
    if (!(rms < CAL_MAX_RMS) || p[2] < 3 || p[2] > 25){
      Serial.printf("# cal: fit failed (rms %.2f mT, z %.2f mm) - magnets missing or misplaced?\n", rms, p[2]);
      break;
    }
    float pred[9];
    forward(p, pred);
    for (int i = 0; i < 9; i++){
      cal.off[i] = sum[i] - pred[i];
    } 
    memcpy(cal.p0, p, sizeof p);
    memcpy(pose, p, sizeof p);
    cal.valid = true;
    Serial.printf("# cal OK: z %.2f mm, x %.2f y %.2f mm, roll %.2f pitch %.2f yaw %.2f deg, rms %.3f mT, polarity %+.0f\n",
                  p[2], p[0], p[1], degrees(p[3]), degrees(p[4]), degrees(p[5]), rms, cal.sigma);
    wakeRing();
    return true;
  }
  Serial.println("# calibration FAILED");
  ringSolid(32, 0, 0);
  delay(1000);
  wakeRing();
  return false;
}

//  HID output 
int16_t shape(float v){ // v = fraction of full scale -> HID counts
  float a = fabsf(v);
  a = a < DEADZONE ? 0 : min((a - DEADZONE) / (1 - DEADZONE), 1.0f);
  return (int16_t)lroundf(copysignf(a, v) * HID_LOGICAL_MAX);
}

void sendHid(const int16_t ax[6], uint8_t buttons){
  static uint32_t last;
  static uint8_t pending = 0;                    // bit 0: translation (or combined), bit 1: rotation, bit 2: buttons
  if (millis() - last >= HID_INTERVAL_MS){
    last = millis();
    pending = 0b101;
  }
  if (!pending || !usb_hid.ready()){
    return;      // one report in flight at a time
  }
  if (pending & 1){
    usb_hid.sendReport(1, ax, 12);
    pending &= ~1;
  } else if (pending & 2){
    usb_hid.sendReport(2, ax + 3, 6);
    pending &= ~2;
  } else {
    usb_hid.sendReport(3, &buttons, 1);
    pending &= ~4;
  }
}

uint8_t hostButtons() {
  static bool chord = false;
  static uint32_t pressedAt[2];
  static bool wasDown[2];
  uint8_t out = 0;
  bool both = btnState[0] && btnState[1];
  if (both) chord = true;
  if (chord && !btnState[0] && !btnState[1]) chord = false;   // chord ends when both are released
  for (int i = 0; i < 2; i++) {
    if (btnState[i] && !wasDown[i]) pressedAt[i] = millis();
    wasDown[i] = btnState[i];
    if (!chord && btnState[i] && millis() - pressedAt[i] >= CHORD_MS) out |= 1 << i;
  }
  return out;
}

//  Main 
void setup(){
  if (!TinyUSBDevice.isInitialized()) TinyUSBDevice.begin(0);
  TinyUSBDevice.setID(USB_VID, USB_PID);
  TinyUSBDevice.setManufacturerDescriptor(USB_MANUFACTURER);
  TinyUSBDevice.setProductDescriptor(USB_PRODUCT);
  usb_hid.setReportDescriptor(hidDesc, sizeof hidDesc);
  usb_hid.setPollInterval(2);
  usb_hid.begin();
  if (TinyUSBDevice.mounted()){
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }   // re-enumerate with new IDs

  Serial.begin(115200);
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);   // user LED off
  pinMode(23, OUTPUT);
  digitalWrite(23, LOW);    // onboard RGB LED off
  for (uint8_t p : BTN) pinMode(p, INPUT_PULLUP);
  ring.begin();

  Wire1.setSDA(6);
  Wire1.setSCL(7);             // D4 / D5
  Wire1.begin();
  Wire1.setClock(400000);
  for (uint8_t a : ADDR) {
    wr(a, 0x08, 0x01);   // INT_CONFIG_1: MASK_INTB (INT tied to GND)
    wr(a, 0x00, 0x14);   // DEVICE_CONFIG_1: MAG_TEMPCO = 0%, CONV_AVG = 32x
    wr(a, 0x02, 0x70);   // SENSOR_CONFIG_1: enable X, Y, Z
    wr(a, 0x03, 0x03);   // SENSOR_CONFIG_2: X_Y_RANGE = 1, Z_RANGE = 1 -> ±80 mT (x1 parts)
    wr(a, 0x01, 0x12);   // DEVICE_CONFIG_2: low noise, continuous mode (last: starts conversions)
  }
  calibrate();           // 1 s settle, then calibrate
}

static const int16_t ZERO_AXES[6] = {};

void loop(){
  readButtons();

  // Hold both buttons -> recalibrate once they're released
  static uint32_t bothSince = 0;
  if (btnState[0] && btnState[1]){
    if (!bothSince) bothSince = millis();
    if (millis() - bothSince >= RECAL_HOLD_MS){
      while (btnState[0] || btnState[1]){
        readButtons();
        ringSpin();
        sendHid(ZERO_AXES, 0); 
        delay(5);
      }
      calibrate();
      bothSince = 0;
    }
  } else bothSince = 0;

  // Solve the pose (warm start from the last frame)
  static int16_t ax[6];
  static uint32_t lastSolve;
  static float rms = 0;
  bool asleep = millis() - lastActive >= SLEEP_AFTER_MS;
  uint32_t solveEvery = asleep ? SLEEP_SOLVE_MS : SOLVE_MS;
  if (cal.valid && millis() - lastSolve >= solveEvery){
    lastSolve = millis();
    float B[9];
    if (readSensors(B)){
      float p[6];
      memcpy(p, pose, sizeof p);
      rms = lm(B, p, LM_ITERS_FRAME, true);
      bool bad = !(rms < MAX_FRAME_RMS);
      for (int i = 0; i < 6; i++){
        bad |= isnan(p[i]);
      }
      if (bad){
        memcpy(pose, cal.p0, sizeof pose);   // lost track: restart from rest next frame
      } else {
        memcpy(pose, p, sizeof p);
      }

      float d[6], fs[6] = {TRANS_FULLSCALE_MM, TRANS_FULLSCALE_MM, TRANS_FULLSCALE_MM,
                           ROT_FULLSCALE_DEG, ROT_FULLSCALE_DEG, ROT_FULLSCALE_DEG};
      for (int i = 0; i < 6; i++){
        d[i] = pose[i] - cal.p0[i];
      }
      for (int i = 3; i < 6; i++){
        d[i] = degrees(d[i]);
      }
      for (int i = 0; i < 6; i++){
        ax[i] = bad ? 0 : AXIS_SIGN[i] * shape(d[AXIS_SRC[i]] / fs[AXIS_SRC[i]]);
      }
    }
  }

  // Both buttons together are reserved for recalibration, so don't send that combo to the host
  bool bothHeld = btnState[0] && btnState[1];
  uint8_t buttons = hostButtons();
  sendHid((cal.valid && !bothHeld) ? ax : ZERO_AXES, buttons);

  // LED sleep: any output outside the deadzone or any button counts as activity
  bool active = btnState[0] || btnState[1];
  for (int i = 0; i < 6; i++){
    active |= cal.valid && !bothHeld && ax[i] != 0;
  }
  updateRingSleep(active);

#if DEBUG_SERIAL
  static uint32_t lastPrint;
  if (cal.valid && millis() - lastPrint >= 50){
    lastPrint = millis();
    Serial.printf("%7.3f %7.3f %7.3f mm  %6.2f %6.2f %6.2f deg  rms %.3f  hid %4d %4d %4d %4d %4d %4d  btn %d%d\n",
                  pose[0] - cal.p0[0], pose[1] - cal.p0[1], pose[2] - cal.p0[2],
                  degrees(pose[3] - cal.p0[3]), degrees(pose[4] - cal.p0[4]), degrees(pose[5] - cal.p0[5]),
                  rms, ax[0], ax[1], ax[2], ax[3], ax[4], ax[5], btnState[0], btnState[1]);
  }
#endif
}
