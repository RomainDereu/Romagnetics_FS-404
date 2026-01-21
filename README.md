# FS-404, Midi Pedal and Software for the Roland SP-404 MK2
All the software used to make the Romagnetics FS-404 Midi Pedal.

![Banner](/04_Other/Readme_pictures/Banner.png)

# 01_Pedal Schematics
Here you can find the wiring to an Arduino Nano.
Note: the FS-404 has been tested with the Arduino Uno as well as the Arduino Nano, but should be usable with any Arduino hardware.
The pedal can send three different midi message.
The message can be changed from the Computer Program.

![Schematics](/04_Other/Readme_pictures/Schematics.PNG)

The following parts are required to be able to build the pedal:
* 2 150 Ohm resistors for the midi connexion
* 1 220 Ohm resistor for the LED (Resistance may vary depending on your LED)
* 1 LED
* 1 Midi 5-pin female connector
* 1 ON/OFF Switch
* 1 Push button (guitar pedal switch)
* 1 Switch button (3 positions or ON OFF ON)
* 1 Guitar pedal enclosure (fits inside a 1590B)
* 1 Guitar pedal female barrel power connector
Optional:
* 1 ferrite bead to clean the outcoming signal.
* diode to prevent from potential back voltage

# 02_Arduino Program
Contains the Arduino sketch for the pedal.
Please make sure to check all your connections before powering up.

The notes played by the FS-404 can be changed using the companion software.

# 03_Computer Program
Contains the Desktop program to upgrade the notes that are sent by the unit.
The software has six different modes:

## SP MIDI A
In Midi A, all 160 pads can be selected to be played.
Midi channels are fixed as per Roland.

## SP MIDI B
All 160 pads for the Midi B.
Midi channel can be changed

## SP Commands
Custom commands for the SP-404 MK2, to control the DJ and Looper modes.

## MIDI Notes
All the available musical notes can be played on any of the 16 Midi channels.

## CC
All CC values can be played as well.

## Toggle
Based on the value of the base note, toggle between Previous, Next, Repeat, Reset and Kill Switch in either MIDI Mode A or B with or without Looping.

![Pedal](/04_Other/Readme_pictures/Pedal.jpg)

# 04_Update

To update the pedal:

1. Download the [ino file containing the Pedal's firmware at this link](/02_Arduino%20Program)
2. Download the [Arduino IDE at the following link](https://www.arduino.cc/en/software/)
3. Deconnect the 9V and MIDI ports and connect the Romagnetics FS-404 to your computer via USB.
   (Do not use a USBC to USBC cable. If needed, use two USB C to USB A cables connected together).
4. If the board doesn't open in the Arduino IDE, please click on the boards manager and add the LGT8fx Boards addon.

![Schematics](/04_Other/Readme_pictures/Update_board.PNG)

5. Click on upload and confirm that the pedal has been updated correctly in the output tab.
6. Disconnect the pedal to reboot the firmware.

To update the Software:

Whether on Mac on or Windows, simply download the new version at the following link:

[Windows Executable](/03_Computer%20Program/Windows_executable)
[Mac Executable](/03_Computer%20Program/Mac_executable)

# 05_Regarding the Mac Executable
If you get the following error when running the Mac programm for the first time:
"FS-404" Not Opened

![Mac_error_1](/04_Other/Readme_pictures/Mac_error_1.png)


Go to System Settings, Privacy & Security. Find the Open Anyway button to open the app

![Mac_error_2](/04_Other/Readme_pictures/Mac_error_2.png)


# 06_About
Don't hesitate to contact me if you have any questions regarding this project.
If you don't want to build the pedal yourself, you can purchase it at  [the following link](https://www.romagnetics.com/product/romagnetics-fs-404/):

# 07_Changelog
2024/12: Version 1.0.0
2025/11: Version 2.0.0: Addition of the commands
2026/01: Version 2.1.0