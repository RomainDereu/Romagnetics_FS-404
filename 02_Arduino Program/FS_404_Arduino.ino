/*
  Copyright 2024-2025
  Romain Dereu
  Released under the MIT License
*/

#include <EEPROM.h>

// The pins for the selector switch
const int leftSwitchPin = 3;
const int rightSwitchPin = 2;

// The main on off swich
int onOffSwitchPin = 5;
bool previousButtonSwitch;

// The pin for the LED
const int ledon = 7;


// The notes that will be played. Read from Eeprom
const int cmd1 = EEPROM.read(1);
const int pitch1 = EEPROM.read(2);
const int velocity1 = EEPROM.read(3);

const int cmd2 = EEPROM.read(4);
const int pitch2 = EEPROM.read(5);
const int velocity2 = EEPROM.read(6);

const int cmd3 = EEPROM.read(7);
const int pitch3 = EEPROM.read(8);
const int velocity3 = EEPROM.read(9);


void setup() {

  /*
  //Leave the following part of the code uncommented if you want to reset the FS-404 to its default values
  //Plays pads 1 to 3 in midi mode B & midi channels 1 & 2
  int defaultNotes[] = {0x00, 0x90, 0x18, 0x80, 0x90 , 0x19, 0x80, 0x90 , 0x1a ,0x7F};

  //Writing to the Eeprom
  for(int i=1; i<11; i++){
    EEPROM.write(i, defaultNotes[i]);
   }
  */

  Serial.begin(31250);

  // Setting of switch  and ON OFF pins
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  pinMode(onOffSwitchPin, INPUT_PULLUP);
  // Setting of led pins
  pinMode(ledon, OUTPUT);
  // The power led is always on
  digitalWrite(ledon, HIGH);
}

int switchposition() {
  // Reading the state of both the switch and the on off button
  int leftSwitchRead = digitalRead(leftSwitchPin);
  int rightSwitchRead = digitalRead(rightSwitchPin);
  if (leftSwitchRead == HIGH && rightSwitchRead == LOW) {
    return 0;
  } else if (leftSwitchRead == HIGH && rightSwitchRead == HIGH) {
    return 1;
  } else if (leftSwitchRead == LOW && rightSwitchRead == HIGH) {
    return 2;
  } else {
    return;
  }
}

// Plays a MIDI note once
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
  return;
}


// Checks the type of the note
// Sends note off if the message is a note
void noteOff(int cmd, int pitch, int velocity) {

  //Note off is triggered under the following conditions
  //A note on has been sent (not applicable to note off)
  //The velocity is not 0 or 127 (reserved for command bits)
  if(cmd > 0x8a && cmd < 0xA0 && velocity != 0x00 && velocity != 0x7f){
    int noteOffCmd = cmd - 0x10;
    Serial.write(noteOffCmd);
    Serial.write(pitch);
    Serial.write(velocity);
  }
  return;
}

// Plays a MIDI note and then sends a corresponding note off 100ms later
void sendnote(int cmd, int pitch, int velocity) {
  noteOn(cmd, pitch, velocity);
  delay(100);
  noteOff(cmd, pitch, velocity);
  return;
}

// Choosing which note will be played
void sendmidi(int toggleSwitchPosition) {
  if (toggleSwitchPosition == 0) {
    sendnote(cmd1, pitch1, velocity1);
  } else if (toggleSwitchPosition == 1) {
    sendnote(cmd2, pitch2, velocity2);
  } else if (toggleSwitchPosition == 2) {
    sendnote(cmd3, pitch3, velocity3);
  }
  return;
}

//Limits the button presses to non consecutive button presses
bool debounce(bool previousButtonSwitch) {
  if (previousButtonSwitch == LOW) {
    return 0;
  }

  else {
    delay(5);
    bool currentState = digitalRead(onOffSwitchPin);
    if (currentState == LOW) {
      return 1;
    } else {
      return 0;
    }
  }
}

//Updates the notes to the Unit
void notesupdate() {
  /* Message format:
    Header       (1 byte)           - Header Byte (value 0xfe)
    Data         (9 bytes)          - Info for the three notes
    Checksum     (1 byte)           - Message integrity check
    Message end  (1 byte)           - Signaling the end of the message (value 0xff)
    Total message size = 12 bythes
*/

  byte buffer[12];
  Serial.readBytesUntil(0xff, buffer, 12);
  delay(100);
  //Checksum of the message
  bool checksumResult = messagecheck(buffer);

  if (checksumResult == 1) {
    //Writing to the Eeprom
    for (int i = 1; i < 11; i++) { EEPROM.write(i, buffer[i]); }
  }

  else {
    return;
  }
}

//Checks if the received message is good
bool messagecheck(byte *buffer) {

  //list of error messages
  //Reduced to one char to be sent as a single bit
  const byte noMessageReceived = 0xfb;  // 251
  const byte messageError = 0xfc;       // 252
  const byte allreceived = 0xfd;        // 253
  const byte messageStart = 0xfe;       // 254

  //Checking if data was sent at all using the first bit (should be 0xff)
  if (buffer[0] != messageStart) {
    Serial.write(noMessageReceived);
    return 0;
  }

  byte checksum = 0;
  for (int x = 0; x < 10; x++) { checksum += buffer[x]; }

  //Checking the value of the checksum against the last received value
  if (checksum != buffer[10]) {
    Serial.write(checksum);
    Serial.write(messageError);
    return 0;
  }

  else {
    Serial.write(allreceived);
    return 1;
  }
}

void loop() {
  //Checks whether Serial is connected. If this is the case, the pedal will be in update mode
  if (Serial.available()) { notesupdate(); }

  // Checking the toggle and on off switches
  int toggleSwitchPosition = switchposition();
  bool currentSwitchPosition = digitalRead(onOffSwitchPin);

  if (currentSwitchPosition == 0 && debounce(previousButtonSwitch)) {
    sendmidi(toggleSwitchPosition);
  }
  //Saving the button position to avoid double triggers
  previousButtonSwitch = digitalRead(onOffSwitchPin);
}
