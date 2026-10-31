#pragma once
/*
Cosmic Mouse firmware config
Copyright (c) 2026 Tyler Bowers
Licensed under the GNU AGPL-3.0
*/

// USB identity 
#define USB_VID            0x1209 //vendor id
#define USB_PID            0x0001 //product id
#define USB_MANUFACTURER   "Tyler"
#define USB_PRODUCT        "Cosmic Mouse"

/*
Random funny strings, unrelated to project, idk why they are here:
0x256F 
0xC635
"3Dconnexion"
"SpaceMouse Compact"
Random funny fact: In Linux distros the spacenav daemon only looks at the VID and PID of the device,
*/

#define HID_LOGICAL_MAX     350 // max magnitude of HID values
#define HID_INTERVAL_MS     8 // how often a full set of reports is sent

// Output feel
const float TRANS_FULLSCALE_MM = 1.5f; // knob translation that gives full-scale output
const float ROT_FULLSCALE_DEG = 8.0f; // knob rotation that gives full-scale output
const float DEADZONE = 0.08f; // fraction of full scale ignored around center

// Output axis i = AXIS_SIGN[i] * measured axis AXIS_SRC[i]
// Measured axes: 0 x, 1 y, 2 z (mm), 3 roll (about x), 4 pitch (about y), 5 yaw (about z) in the board frame
// (+X right, +Y toward U3 / USB end, +Z out from the base).
// Output order: TX, TY, TZ, RX, RY, RZ.
const uint8_t AXIS_SRC[6] = {0, 1, 2, 3, 4, 5};
const int8_t AXIS_SIGN[6] = {1, -1, -1, 1, -1, -1};

// Print pose over serial
#define DEBUG_SERIAL 1

// Hardware
const uint8_t ADDR[3] = {0x35, 0x22, 0x78}; // U3 (A2), U4 (B2), U5 (C2)
const float RANGE_MT = 80.0f; // x1 parts, high range
const uint8_t BTN[2] = {27, 28}; // SW1 = D1 (GPIO27), SW2 = D2 (GPIO28); pressed = LOW
const uint32_t DEBOUNCE_MS = 20; // for buttons
const uint8_t RING_PIN = 26, RING_LEDS = 8; // D0 = GPIO26
const uint8_t RING_BRIGHTNESS = 16; // max brightness

// Sleep behaviour
const uint32_t SLEEP_AFTER_MS = 10000; 
const uint32_t FADE_OUT_MS = 500; // slow fade to off, due to brightness levels longer times can seem stair stepped
const uint32_t FADE_IN_MS = 250; // quick ramp on activity

// Geometry (values calculated from cad software, but may vary due to assembly tolerances)
const float S[3][3] = {{-0.12f, 14.1f, 0}, {12.271f, -6.946f, 0}, {-12.151f, -7.154f, 0}}; // sensor positions U3,U4,U5 (radius 14.5 mm @ 120 deg)
const float Q[3][3] = {{0, 16.0f, 0}, {13.852f, -8.008f, 0}, {-13.852f, -8.008f, 0}}; // magnet positions (radius 16 mm @ 120 deg)
const float PHI_DEG[3] = {0, -120, 120}; // footprint rotation of U3, U4, U5
const float AXIS_SX = -1, AXIS_SY = 1, AXIS_SZ = -1; // TMAG5273 axes: +X toward pins 1-3, +Y toward pin 1 (radially outward), +Z into the board
const float K_MAG = 1420.0f * 3.14159265f * 2.35f * 2.35f * 4.7f / (4 * 3.14159265f); // Magnet: 4.7 x 4.7 mm N52.  k = Br * V / 4pi  (mT * mm^3) See model.md

// Calibration
const uint32_t CAL_SETTLE_MS = 1000; // delay (after boot or button release)
const int CAL_SAMPLES = 200; // 0.5s of averaging (CAL_SAMPLES*SOLVE_MS)
const float CAL_MAX_MOTION = 0.3f; // max perturbation during calibration (mT)
const float CAL_MAX_RMS = 3.0f; // reject if model can't explain the rest field (diff between inversion method's expectation and known positions, see model.md)
const float Z_GUESS_MM = 9.0f; // starting guess for knob height above the Hall plane
const uint32_t RECAL_HOLD_MS = 3000; // hold both buttons this long to recalibrate
const uint32_t CHORD_MS = 80; // time between button chords (both pressed within this time will block button presses from being sent)

// Solver
const int LM_ITERS_FRAME = 3; // iterations per frame (warm start from last frame)
const int LM_ITERS_CAL = 50; // iterations during calibration
const float MAX_FRAME_RMS = 2.0f; // mT: above this the fit is rejected for that frame
const uint32_t SOLVE_MS = 3; // sensors update every ~2.4 ms at 32x averaging
const uint32_t SLEEP_SOLVE_MS = 50; // when asleep, don't solve every frame
