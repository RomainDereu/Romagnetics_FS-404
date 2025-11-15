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




// Mode A pads (16 pads)
const byte MODE_A_PADS[16] = {
  0x30, 0x31, 0x32, 0x33,
  0x2c, 0x2d, 0x2e, 0x2f,
  0x28, 0x29, 0x2a, 0x2b,
  0x24, 0x25, 0x26, 0x27
};


// Mode B pads (5 banks × 16 pads = 80)
const byte MODE_B_PADS[80] = {
  // Banks A and F
  0x18, 0x19, 0x2a, 0x2b,
  0x14, 0x15, 0x16, 0x17,
  0x10, 0x11, 0x12, 0x13,
  0x0c, 0x0d, 0x0e, 0x0f,
  // Banks B and G
  0x28, 0x29, 0x3a, 0x3b,
  0x24, 0x25, 0x26, 0x27,
  0x20, 0x21, 0x22, 0x23,
  0x1c, 0x1d, 0x1e, 0x1f,
  // Banks C and H
  0x38, 0x39, 0x4a, 0x4b,
  0x34, 0x35, 0x36, 0x37,
  0x30, 0x31, 0x32, 0x33,
  0x2c, 0x2d, 0x2e, 0x2f,
  // Banks D and I
  0x48, 0x49, 0x5a, 0x5b,
  0x44, 0x45, 0x46, 0x47,
  0x40, 0x41, 0x42, 0x43,
  0x3c, 0x3d, 0x3e, 0x3f,
  // Banks E and J
  0x58, 0x59, 0x6a, 0x6b,
  0x54, 0x55, 0x56, 0x57,
  0x50, 0x51, 0x52, 0x53,
  0x4c, 0x4d, 0x4e, 0x4f
};

enum : byte {
  PLAY_NOTE = 0, 
  NEXT_NOTE_A,
  NEXT_NOTE_LOOP_A,
  NEXT_NOTE_B,
  NEXT_NOTE_LOOP_B, 
  PREVIOUS_NOTE_A, 
  PREVIOUS_NOTE_LOOP_A,
  PREVIOUS_NOTE_B,
  PREVIOUS_NOTE_LOOP_B,
  SAME_NOTE,
  RESET_NOTE,
  KILL_ALL
};



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

int findIndexModeA(byte pitch) {
  for (int i = 0; i < 16; i++) {
    if (MODE_A_PADS[i] == pitch) {
      return i;
    }
  }
  return -1; // not found
}

int findIndexModeB(byte pitch) {
  for (int i = 0; i < 80; i++) {
    if (MODE_B_PADS[i] == pitch) {
      return i;
    }
  }
  return -1; // not found
}

// MIDI channel helpers
byte getChannel(const Note &note) {
  return (note.cmd & 0x0F) + 1; // 1..16
}

void setChannel(Note &note, byte ch) {
  if (ch < 1)  ch = 1;
  if (ch > 16) ch = 16;
  note.cmd = (note.cmd & 0xF0) | ((ch - 1) & 0x0F);
}

// group = 0..4 (A..E), layer = 0 (A-E) or 1 (F-J)
byte modeBChannelFor(int group, int layer) {
  if (group < 0) group = 0;
  if (group > 4) group = 4;
  if (layer < 0) layer = 0;
  if (layer > 1) layer = 1;
  // channels: (1,2), (3,4), (5,6), (7,8), (9,10)
  return (byte)(group * 2 + layer + 1);
}

int modeBLayerFromChannel(byte ch) {
  // Limit to 1..10 for Mode B
  if (ch < 1)  ch = 1;
  if (ch > 10) ch = 10;
  return (ch - 1) % 2; // odd -> 0, even -> 1
}

void advanceModeA(Note &note, int dir, bool loop) {
  int idx = findIndexModeA(note.pitch);
  if (idx < 0) return; // pitch not in Mode A table

  const int padCount = 16;
  byte ch = getChannel(note);
  if (ch < 1)  ch = 1;
  if (ch > 10) ch = 10; // Mode A uses channels 1..10 as banks

  if (dir > 0) { // NEXT
    if (idx < padCount - 1) {
      idx++;
    } else { // idx == 15, last pad
      idx = 0; // wrap pad index
      if (!loop) {
        // Move to next bank (channel) 1..10
        ch++;
        if (ch > 10) ch = 1;
      }
    }
  } else if (dir < 0) { // PREVIOUS
    if (idx > 0) {
      idx--;
    } else { // idx == 0, first pad
      idx = padCount - 1; // wrap pad index
      if (!loop) {
        // Move to previous bank (channel)
        if (ch > 1) ch--;
        else        ch = 10;
      }
    }
  }

  note.pitch = MODE_A_PADS[idx];
  setChannel(note, ch);
}


void advanceModeB(Note &note, int dir, bool loop) {
  int idx = findIndexModeB(note.pitch);
  if (idx < 0) return; // pitch not in Mode B table

  const int padCount = 80;
  byte ch = getChannel(note);
  int layer = modeBLayerFromChannel(ch); // 0 = A-E, 1 = F-J

  if (dir > 0) { // NEXT
    if (idx < padCount - 1) {
      idx++;
    } else { // idx == 79
      idx = 0; // wrap pad index
      if (!loop) {
        // after going through 80 pads, toggle A-E <-> F-J
        layer ^= 1;
      }
    }
  } else if (dir < 0) { // PREVIOUS
    if (idx > 0) {
      idx--;
    } else { // idx == 0
      idx = padCount - 1; // wrap
      if (!loop) {
        layer ^= 1;
      }
    }
  }

  int group = idx / 16;       // 0..4
  note.pitch = MODE_B_PADS[idx];

  // Channel encodes both bank group (A..E) and layer (A-E vs F-J)
  setChannel(note, modeBChannelFor(group, layer));
}


void sendnote(Note note) { 
  if (note.type == PLAY_NOTE) {
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == NEXT_NOTE_A) {
    note = lastNote;
    advanceModeA(note, +1, false);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == NEXT_NOTE_LOOP_A) {
    note = lastNote;
    advanceModeA(note, +1, true);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == PREVIOUS_NOTE_A) {
    note = lastNote;
    advanceModeA(note, -1, false);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == PREVIOUS_NOTE_LOOP_A) {
    note = lastNote;
    advanceModeA(note, -1, true);
    noteOnOff(note);
    lastNote = note;
  }

  else if (note.type == NEXT_NOTE_B) {
    note = lastNote;
    advanceModeB(note, +1, false);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == NEXT_NOTE_LOOP_B) {
    note = lastNote;
    advanceModeB(note, +1, true);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == PREVIOUS_NOTE_B) {
    note = lastNote;
    advanceModeB(note, -1, false);
    noteOnOff(note);
    lastNote = note;
  }
  else if (note.type == PREVIOUS_NOTE_LOOP_B) {
    note = lastNote;
    advanceModeB(note, -1, true);
    noteOnOff(note);
    lastNote = note;
  }

  else if (note.type == SAME_NOTE) {
    note = lastNote;
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
