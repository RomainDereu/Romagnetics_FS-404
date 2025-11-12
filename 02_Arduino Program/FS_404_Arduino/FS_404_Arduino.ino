/*
  Copyright 2024-2025 Romain Dereu
*/

#include <EEPROM.h>

// The pins for the selector switch
const int leftSwitchPin  = 3;
const int rightSwitchPin = 2;

// The main on off swich
int  onOffSwitchPin = 5;
bool previousButtonSwitch;

// The pins for the led
const int LED1  = 9;
const int LED2  = 8;
const int LED3  = 7;
const int LEDON = 6;

const byte DATA_BYTES = 16;              // bytes 1..16
const byte MSG_LEN    = 1 + DATA_BYTES + 1 + 1; // header + data + checksum + end = 19
const byte HEADER_BYTE = 0xFE;
const byte END_BYTE    = 0xFF;

enum : byte { PLAY_NOTE = 0, 
              NEXT_NOTE, 
              PREVIOUS_NOTE, 
              RESET_NOTE,
              KILL_ALL};



// Switch leds are the three that change according to the switch
const int switchLedNumbers[] = { LED1, LED2, LED3 };

struct Note {
  byte type;
  byte cmd;
  byte pitch;
  byte velocity;
};
Note notes[4];

Note lastNote;


void setup() {


  /*
  // Leave the following part of the code uncommented if you want to reset the FS-404 to its default values
  // Plays pads 1 to 3 in midi mode B & midi channels 1 & 2
    const byte defaultNotes[DATA_BYTES] = {
      0x00, 0x90, 0x30, 0x7F,  // note 0 (base note)
      0x00, 0x90, 0x30, 0x7F,  // note 1
      0x00, 0x90, 0x31, 0x7F,  // note 2
      0x00, 0x90, 0x32, 0x7F   // note 3
    };

    for (int i = 0; i < DATA_BYTES; i++) {
      EEPROM.write(1 + i, defaultNotes[i]);
    }
  */

    // The notes that will be played. Read from Eeprom
  for (int i = 0; i < 4; i++) {
    notes[i].type     = EEPROM.read(1 + i * 4);
    notes[i].cmd      = EEPROM.read(2 + i * 4);
    notes[i].pitch    = EEPROM.read(3 + i * 4);
    notes[i].velocity = EEPROM.read(4 + i * 4);
  }

  lastNote = notes[0];


  Serial.begin(31250);

  // Setting of switch  and ON OFF pins
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  pinMode(onOffSwitchPin, INPUT_PULLUP);

  previousButtonSwitch = digitalRead(onOffSwitchPin);

  // Setting of led pins
  pinMode(LEDON, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // The power led is always on
  digitalWrite(LEDON, HIGH);
}

int switchposition() {
  // Reading the state of both the switch and the on off button
  int leftSwitchRead  = digitalRead(leftSwitchPin);
  int rightSwitchRead = digitalRead(rightSwitchPin);

  if (leftSwitchRead == HIGH && rightSwitchRead == LOW) {
    return 0;
  } else if (leftSwitchRead == HIGH && rightSwitchRead == HIGH) {
    return 1;
  } else if (leftSwitchRead == LOW && rightSwitchRead == HIGH) {
    return 2;
  } else {
    return -1;
  }
}

void updatelight(int toggleSwitchPosition) {
  for (int i = 0; i < 3; i++) {
    if (i == toggleSwitchPosition) {
      digitalWrite(switchLedNumbers[i], HIGH);
    } else {
      digitalWrite(switchLedNumbers[i], LOW);
    }
  }
  return;
}

// Plays a MIDI note once
void noteOn(const Note &note) {
  Serial.write(note.cmd);
  Serial.write(note.pitch);
  Serial.write(note.velocity);
  return;
}

// Checks the type of the note
// Sends note off if the message is a note
void noteOff(const Note &note) {
  // Only generate Note Off when the status was Note On (0x90..0x9F)
  if (note.cmd >= 0x90 && note.cmd <= 0x9F) {
    byte noteOffCmd = note.cmd - 0x10; // becomes 0x8n
    Serial.write(noteOffCmd);
    Serial.write(note.pitch);
    Serial.write(0);
  }
}

void noteOnOff(const Note &note) {
  noteOn(note);
  delay(100);
  noteOff(note);
}

void killAllNotes() {
  for (byte ch = 0; ch < 16; ch++) {
    Serial.write(0xB0 | ch); Serial.write(64);  Serial.write(0);  // Sustain Off
    Serial.write(0xB0 | ch); Serial.write(120); Serial.write(0);  // All Sound Off
    Serial.write(0xB0 | ch); Serial.write(123); Serial.write(0);  // All Notes Off
  }
}


void sendnote(Note note) { 
  auto clampPitch = [](int v) -> byte {
    if (v < 0)   return 0;
    if (v > 127) return 127;
    return (byte)v;
  };

  if (note.type == PLAY_NOTE) {
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == NEXT_NOTE) {
    note = lastNote;
    note.pitch = clampPitch(note.pitch + 1);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == PREVIOUS_NOTE) {
    note = lastNote;
    note.pitch = clampPitch(note.pitch - 1);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == RESET_NOTE) {
    note = notes[0];
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == KILL_ALL) {
  killAllNotes();
}
}


// Choosing which note will be played
void sendmidi(int toggleSwitchPosition) {
  if (toggleSwitchPosition >= 0 && toggleSwitchPosition < 3) {
    sendnote(notes[toggleSwitchPosition + 1]);
  }
  return;
}

// Limits the button presses to non consecutive button presses
bool debounce(bool previousButtonSwitch) {
  if (previousButtonSwitch == LOW) {
    return 0;
  } else {
    delay(5);
    bool currentState = digitalRead(onOffSwitchPin);
    if (currentState == LOW) {
      return 1;
    } else {
      return 0;
    }
  }
}

// Updates the notes to the Unit
void notesupdate() {
  /* Message format (4 notes × 4 fields):
     [0] Header       : 0xFE
     [1..16] Data     : t1 c1 p1 v1 t2 c2 p2 v2 t3 c3 p3 v3 t4 c4 p4 v4
     [17] Checksum    : sum of bytes [0..16] modulo 256
     [18] End         : 0xFF
     Total size = 19 bytes
  */

  byte buffer[MSG_LEN];
  Serial.setTimeout(1000);
  int n = Serial.readBytes(buffer, MSG_LEN);
  if (n != MSG_LEN) return;

  if (buffer[MSG_LEN - 1] != END_BYTE) return; // verify end byte first

  // Checksum & header validation
  bool ok = messagecheck(buffer);
  if (!ok) return;

  // Persist data bytes [1..16] to EEPROM addresses [1..16]
  for (int i = 1; i <= DATA_BYTES; i++) {
    EEPROM.write(i, buffer[i]);
  }

  // Refresh in-RAM notes immediately
  for (int i = 0; i < 4; i++) {
    int base = 1 + i * 4;
    notes[i].type     = EEPROM.read(base + 0);
    notes[i].cmd      = EEPROM.read(base + 1);
    notes[i].pitch    = EEPROM.read(base + 2);
    notes[i].velocity = EEPROM.read(base + 3);
  }
}

// Checks if the received message is good
bool messagecheck(const byte *buffer) {
  // status bytes
  const byte noMessageReceived = 0xFB;
  const byte messageError      = 0xFC;
  const byte allreceived       = 0xFD;

  // Check header
  if (buffer[0] != HEADER_BYTE) {
    Serial.write(noMessageReceived);
    return 0;
  }

  // Compute checksum over bytes [0..16] (header + 16 data bytes)
  uint8_t checksum = 0;
  for (int x = 0; x <= DATA_BYTES; x++) { // 0..16 inclusive
    checksum += buffer[x];
  }

  // Compare against checksum at index 17
  if (checksum != buffer[1 + DATA_BYTES]) {
    Serial.write(checksum);
    Serial.write(messageError);
    return 0;
  } else {
    Serial.write(allreceived);
    return 1;
  }
}

void loop() {
  // Checks whether Serial is connected. If this is the case, the pedal will be in update mode
if (Serial.available() && Serial.peek() == 0xFE) {    notesupdate();
  }

  // Checking the toggle and on off switches
  int  toggleSwitchPosition  = switchposition();
  bool currentSwitchPosition = digitalRead(onOffSwitchPin);

  // updating the light value and sending midi if the note is on
  if (toggleSwitchPosition >= 0) {
    updatelight(toggleSwitchPosition);
  }

  if (currentSwitchPosition == 0 && debounce(previousButtonSwitch)) {
    if (toggleSwitchPosition >= 0) {
      sendmidi(toggleSwitchPosition);
    }
  }

  // Saving the button position to avoid double triggers
  previousButtonSwitch = digitalRead(onOffSwitchPin);
}
