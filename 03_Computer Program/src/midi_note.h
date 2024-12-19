#ifndef MIDI_NOTE_H
#define MIDI_NOTE_H
#include <QChar>
#include <QString>

class Midi_note
{
public:
    Midi_note();
    //Constructor for midi PadA
    Midi_note(QString pad_a_string, int pada_value_combo, int velocity_spin){
        QChar pad_a_first_char = pad_a_string.at(0);
        status_byte = letterToMidiChannela(pad_a_first_char);
        data_byte_1 = indexToPadANote(pada_value_combo);
        data_byte_2 = velocity_spin;
    }


    //Constructor for SP Midi B, Midi Notes and CC
    Midi_note(std::string type_of_note, int midi_combo, int value_combo, int velocity_spin){
        if (type_of_note == "SP Midi B"){
            std::array<unsigned char, 2 >midiChannelAndPadMidiB = numberToMidiChannelb(midi_combo, value_combo);
            status_byte = midiChannelAndPadMidiB[0];
            data_byte_1 = midiChannelAndPadMidiB[1];
            data_byte_2 = velocity_spin;
        }

        else if  (type_of_note == "Midi Notes"){
            status_byte = midichannel_to_hex(midi_combo);
            data_byte_1 = musical_note_to_hex(value_combo);
            data_byte_2 = velocity_spin;
        }

        else if  (type_of_note == "CC"){
            status_byte = midi_cc_channel_hex_values[midi_combo];
            data_byte_1 = value_combo;
            data_byte_2 = velocity_spin;
        }
    }

    Midi_note(QString command_string){
        command_to_hex(command_string);
    }

    //Constructor for midi commands

    //Returns the three processed bytes
    std::array<unsigned char, 3> returnmessage();

private:
    //The bytes that will be sent via serial
    unsigned char status_byte;
    unsigned char data_byte_1;
    unsigned char data_byte_2;

    //Used by Midi channel a
    unsigned char letterToMidiChannela(QChar firstLetter);
    unsigned char indexToPadANote(int comboIndex);
    //Number to midi channel B
    std::array<unsigned char, 2>numberToMidiChannelb(int channels_index, int note_padb_value_combo);
    //Used for Musical notes
    unsigned char midichannel_to_hex(int midi_channel);
    unsigned char musical_note_to_hex(int musical_note_position);
    //Used for CC Channel conversion
    std::array<unsigned char, 16 > midi_cc_channel_hex_values = {0,
                                                                0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
                                                                0xb5, 0xb6, 0xb7, 0xb8, 0xb9,
                                                                0xba, 0xbb, 0xbc, 0xbd, 0xbe};
    //Used for command conversion
    void command_to_hex(QString command_string);

};

#endif // MIDI_NOTE_H
