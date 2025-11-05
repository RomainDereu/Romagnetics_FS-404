/*
  CopRomain Dereu
  Fixed & hardened
*/

#include <EEPROM.h>

// ---------------- Pin definitions (use bytes, not int)
const uint8_t LEFT_SWITCH_PIN  = 3;
const uint8_t RIGHT_SWITCH_PIN = 2;
const uint8_t ONOFF_SWITCH_PIN = 5;

const uint8_t LED1  = 9;
const uint8_t LED2  = 8;
const uint8_t LED3  = 7;
const uint8_t LED_ON = 6;

// For dev visualization (remove for production)
const uint8_t SWITCH_LEDS[3] = { LED1, LED2, LED3 };

// ---------------- Note storage (RAM)
struct Note { uint8_t cmd, pitch, vel; };
Note notes[3];  // notes[0], notes[1], notes[2]

// EEPROM address map: 1..9 (cmd1,pitch1,vel1, cmd2,pitch2,vel2, cmd3,pitch3,vel3)
static const uint8_t kEepromBase = 1;   // 1..9 inclusive

// ---------------- Debounce/edge detection
static const uint16_t DEBOUNCE_MS = 25;
bool lastButtonState = HIGH;   // because INPUT_PULLUP
bool stableButtonState = HIGH;
unsigned long lastBounceTime = 0;

// ---------------- Update protocol
// Frame: [0xFE][9 data bytes][checksum][0xFF]
// checksum = sum of bytes 0..10 (i.e., including 0xFE header and the 9 data bytes)
// We will read exactly 12 bytes when we detect 0xFE as the first byte.
static const uint8_t MSG_START = 0xFE;
static const uint8_t MSG_END   = 0xFF;

// Optional status bytes back
static const uint8_t NO_MESSAGE   = 0xFB;
static const uint8_t MSG_ERROR    = 0xFC;
static const uint8_t MSG_OK       = 0xFD;

// ---------------- Helpers

void loadNotesFromEEPROM() {
  for (uint8_t i = 0; i < 3; ++i) {
    notes[i].cmd   = EEPROM.read(kEepromBase + (i * 3) + 0);
    notes[i].pitch = EEPROM.read(kEepromBase + (i * 3) + 1);
    notes[i].vel   = EEPROM.read(kEepromBase + (i * 3) + 2);
  }
}

void saveNotesToEEPROM(const uint8_t* data9) {
  // data9 layout: [cmd1,pitch1,vel1, cmd2,pitch2,vel2, cmd3,pitch3,vel3]
  for (uint8_t i = 0; i < 9; ++i) {
    EEPROM.write(kEepromBase + i, data9[i]);
  }
}

// Reads an exact 12-byte frame. Returns true if a valid frame was parsed into data9[9].
bool readUpdateFrame(uint8_t* data9) {
  // Look for start byte
  int b = Serial.read();
  if (b < 0) { return false; }
  if ((uint8_t)b != MSG_START) { 
    // Not a framed update; flush minimal and report
    Serial.write(NO_MESSAGE);
    return false; 
  }

  uint8_t buffer[12];
  buffer[0] = MSG_START;

  // Read the remaining 11 bytes with a timeout
  const unsigned long start = millis();
  uint8_t idx = 1;
  while (idx < 12 && (millis() - start) < 50) { // 50ms guard
    if (Serial.available()) {
      buffer[idx++] = (uint8_t)Serial.read();
    }
  }
  if (idx < 12) {
    Serial.write(MSG_ERROR);
    return false; // timed out / short read
  }

  // Validate end byte
  if (buffer[11] != MSG_END) {
    Serial.write(MSG_ERROR);
    return false;
  }

  // Compute checksum over buffer[0..10]
  uint8_t csum = 0;
  for (uint8_t i = 0; i <= 10; ++i) csum += buffer[i];

  if (csum != buffer[10]) {
    // Send back computed checksum for debugging, then error
    Serial.write(csum);
    Serial.write(MSG_ERROR);
    return false;
  }

  // Extract the 9 data bytes (buffer[1..9])
  for (uint8_t i = 0; i < 9; ++i) data9[i] = buffer[1 + i];

  Serial.write(MSG_OK);
  return true;
}

int readSwitchPosition() {
  // Using INPUT_PULLUP: LOW = pressed/connected to GND
  const uint8_t l = digitalRead(LEFT_SWITCH_PIN);
  const uint8_t r = digitalRead(RIGHT_SWITCH_PIN);

  if (l == HIGH && r == LOW)  return 0;
  if (l == HIGH && r == HIGH) return 1;
  if (l == LOW  && r == HIGH) return 2;
  return -1; // invalid or mid-transition
}

// Dev LED helper (no-op safe when index invalid)
void updateSwitchLeds(int pos) {
  for (uint8_t i = 0; i < 3; ++i) {
    digitalWrite(SWITCH_LEDS[i], (i == pos) ? HIGH : LOW);
  }
}

// MIDI
inline void midiWrite3(uint8_t a, uint8_t b, uint8_t c) {
  Serial.write(a); Serial.write(b); Serial.write(c);
}

void noteOn(uint8_t cmd, uint8_t pitch, uint8_t velocity) {
  midiWrite3(cmd, pitch, velocity);
}

void noteOff(uint8_t cmd, uint8_t pitch, uint8_t velocity) {
  // If cmd is Note On (0x90..0x9F), map to Note Off (0x80..0x8F)
  if (cmd >= 0x90 && cmd <= 0x9F) {
    uint8_t off = cmd - 0x10;
    midiWrite3(off, pitch, velocity);
  }
}

void sendNote(uint8_t cmd, uint8_t pitch, uint8_t velocity) {
  noteOn(cmd, pitch, velocity);
  delay(100);
  noteOff(cmd, pitch, velocity);
}

void sendMidiFor(int pos) {
  if (pos < 0 || pos > 2) return;
  sendNote(notes[pos].cmd, notes[pos].pitch, notes[pos].vel);
}

// Returns true exactly once per valid press (falling edge w/ debounce)
bool buttonPressedEdge() {
  bool reading = (digitalRead(ONOFF_SWITCH_PIN) == LOW); // active LOW
  unsigned long now = millis();

  if (reading != lastButtonState) {
    lastBounceTime = now;
    lastButtonState = reading;
  }

  if ((now - lastBounceTime) > DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == true) {
        // transitioned to pressed (LOW)
        return true;
      }
    }
  }
  return false;
}

// ---------------- Arduino lifecycle

void setup() {
  // Uncomment to reset to sane defaults ONCE, then recompile with it commented out
  /*
  // Defaults: play pads 1..3 on MIDI ch1 Note 0x18, 0x19, 0x1A with vel 0x7F
  const uint8_t defaultData[9] = {
    0x90, 0x18, 0x7F,
    0x90, 0x19, 0x7F,
    0x90, 0x1A, 0x7F
  };
  saveNotesToEEPROM(defaultData);
  */

  Serial.begin(31250); // MIDI baud

  pinMode(LEFT_SWITCH_PIN,  INPUT_PULLUP);
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(ONOFF_SWITCH_PIN, INPUT_PULLUP);

  pinMode(LED_ON, OUTPUT);
  digitalWrite(LED_ON, HIGH);

  // Dev LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // Initialize states
  lastButtonState   = digitalRead(ONOFF_SWITCH_PIN);
  stableButtonState = lastButtonState;

  // Load runtime notes
  loadNotesFromEEPROM();
}

void loop() {
  // --- Update mode: only act if a framed message begins with 0xFE
  if (Serial.available()) {
    // Peek the first byte to decide if it's an update frame
    int peeked = Serial.peek();
    if (peeked == MSG_START) {
      uint8_t data9[9];
      if (readUpdateFrame(data9)) {
        saveNotesToEEPROM(data9);
        loadNotesFromEEPROM(); // refresh RAM so changes are live immediately
      } else {
        // If bad frame, try to recover by flushing a little
        while (Serial.available()) { Serial.read(); if (Serial.peek() == -1) break; }
      }
    }
    // else: ignore normal MIDI or stray bytes
  }

  // --- UI & trigger
  int pos = readSwitchPosition();
  if (pos >= 0) { updateSwitchLeds(pos); }   // dev only

  if (buttonPressedEdge()) {
    if (pos >= 0) { sendMidiFor(pos); }
  }
}
