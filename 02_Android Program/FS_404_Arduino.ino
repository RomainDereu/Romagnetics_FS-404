/*
  Romain Dereu
*/

#include <EEPROM.h>

// The pins for the selector switch
int leftSwitchPin = 3;
int rightSwitchPin = 2;

// The main on off swich
int onOffSwitchPin = 5;
int onOffSwitchLast = 1;

// The pins for the led
int led1 = 10;
int led2 = 9;
int led3 = 8;
int ledon = 7;
// Switch leds are the three that change according to the switch
int switchLedNumbers[] = {led1, led2, led3};

// The notes that will be played. Read from Eeprom
int cmd1 = EEPROM.read(1);
int pitch1 = EEPROM.read(2);
int velocity1 = EEPROM.read(3);

int cmd2 = EEPROM.read(4);
int pitch2 = EEPROM.read(5);
int velocity2 = EEPROM.read(6);

int cmd3 = EEPROM.read(7);
int pitch3 = EEPROM.read(8);
int velocity3 = EEPROM.read(9);


void setup() {
  //Leave the following part of the code uncommented if you want to reset the FS-404 to default values
  
  
  /*
  //0x90 is for midi 1
  //Plays pads 1 to 3
  //The notes are played at 64 velocity
  int defaultNotes[] = {0, 0x90, 0x0C, 64, 0x90 , 0x0D, 64, 0x90 , 0x0E ,64};

  //Writing to the Eeprom
  for(int i=1; i<=sizeof(defaultNotes); i++){
    EEPROM.write(i, defaultNotes[i]);
   }
  
   
  //9600 baud for debugging
  Serial.begin(9600);
  for(int i=1; i<=9; i++){
    Serial.println(EEPROM.read(i));
   }
  Serial.end();
  */

  

  Serial.begin(31250);

  //setting of switch  and ON OFF pins
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  pinMode(onOffSwitchPin, INPUT_PULLUP);  
  //setting of led pins
  for(int i = led1; i<=ledon; i++)
  {   
   pinMode(i, OUTPUT);
  }

  // The power led is always on
  digitalWrite(ledon, HIGH);
}



int switchposition() {
    //Reading the state of both the switch and the on off button
    int leftSwitchRead = digitalRead(leftSwitchPin);
    int rightSwitchRead = digitalRead(rightSwitchPin);
    if (leftSwitchRead == HIGH && rightSwitchRead == LOW) {return 0;}
    else if (leftSwitchRead == HIGH && rightSwitchRead == HIGH) {return 1;}
    else if (leftSwitchRead == LOW && rightSwitchRead == HIGH) {return 2;}
    else{return 3;}
  }

void updatelight(int toggleSwitchPosition) {
  for(int i=0; i<=2; i++){    
    if (i == toggleSwitchPosition) {digitalWrite(switchLedNumbers[i], HIGH); }
    else {digitalWrite(switchLedNumbers[i], LOW);}
    }
  }

// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
  }

void sendmidi(int toggleSwitchPosition) {
  if (toggleSwitchPosition == 0){noteOn(cmd1, pitch1, velocity1);}

  else if (toggleSwitchPosition == 1){noteOn(cmd2, pitch2, velocity2);}

  else if (toggleSwitchPosition == 2){noteOn(cmd3, pitch3, velocity3);}

  }

void loop() {
  // Checking the toggle and on off switches
  int toggleSwitchPosition = switchposition();
  int onOffSwitchRead = digitalRead(onOffSwitchPin);

  //updating the light value and sending midi if the note is on
  updatelight(toggleSwitchPosition);
  
    if (onOffSwitchRead == LOW && onOffSwitchLast != LOW){
        sendmidi(toggleSwitchPosition);
        delay(500);
          }
  onOffSwitchLast = onOffSwitchRead;
}