#include "midi_note.h"

Midi_note::Midi_note() {}

//Returns the three notes message when called
std::array<unsigned char, 3> Midi_note::returnmessage(){
    std::array<unsigned char, 3> returnmessage = {status_byte, data_byte_1, data_byte_2};
    return returnmessage;
}


//SP Midi A
unsigned char Midi_note::letterToMidiChannela(QChar firstLetter){
    std::vector<char> pad_letters = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
    int midi_channel = 1;
    for(int x=0; x<pad_letters.size(); x++){
        if (firstLetter == pad_letters[x]){
            midi_channel = x+1;
        }
    }
    unsigned char hexmidichannel = midichannel_to_hex(midi_channel);
    return hexmidichannel;
}

unsigned char Midi_note::indexToPadANote(int comboIndex){

    int noteIndex = comboIndex % 16;

    //In this mode, there are 10 MIDI channels - one for each bank in the SP.
    //Bank A is controlled by MIDI channel 1, bank B is MIDI channel 2
    //until bank J which is controlled midi channel 10.
    std::vector<unsigned char> list_pads_modea_hex = {0x30, 0x31, 0x32, 0x33,
                                                      0x2c, 0x2d, 0x2e, 0x2f,
                                                      0x28, 0x29, 0x2a, 0x2b,
                                                      0x24, 0x25, 0x26, 0x27};
    return list_pads_modea_hex[noteIndex];
}



//SP Midi B
std::array <unsigned char, 2>Midi_note::numberToMidiChannelb(int channels_index, int note_padb_value_combo){
    //Defining if the note is in the upper or the lower register of the midi channels
    bool lowerOrHigher;
    if (note_padb_value_combo > 79){lowerOrHigher = 1;}
    else {lowerOrHigher = 0;}

    std::vector<unsigned char> list_midi_channels_hex = {0,
                                                         0x90, 0x91, 0x92, 0x93,
                                                         0x94, 0x95, 0x96, 0x97,
                                                         0x98, 0x99, 0x9A, 0x9B};

    //Creating the midi channel message
    unsigned char midi_channel_message = list_midi_channels_hex[(channels_index*2)+1+lowerOrHigher];

    //note_padb_value_combo has a value from 0 to 160
    //Substracting 80 if the value exceeds 80 to get the correct note
    int note_padb_value_80 = note_padb_value_combo;
    if (note_padb_value_80 > 79 ){note_padb_value_80 = note_padb_value_80 - 80;}

    //This mode uses two MIDI channels: 1 and 2, each encompassing 5 banks in the SP
    std::array<unsigned char, 80> list_pads_modeb_hex = {//Banks A and F
                                                         0x18, 0x19, 0x2a, 0x2b,
                                                         0x14, 0x15, 0x16, 0x17,
                                                         0x10, 0x11, 0x12, 0x13,
                                                         0x0c, 0x0d, 0x0e, 0x0f,
                                                         //Banks B and G
                                                         0x28, 0x29, 0x3a, 0x3b,
                                                         0x24, 0x25, 0x26, 0x27,
                                                         0x20, 0x21, 0x22, 0x23,
                                                         0x1c, 0x1d, 0x1e, 0x1f,
                                                         //Banks C and H
                                                         0x38, 0x39, 0x4a, 0x4b,
                                                         0x34, 0x35, 0x36, 0x37,
                                                         0x30, 0x31, 0x32, 0x33,
                                                         0x2c, 0x2d, 0x2e, 0x2f,
                                                         //Banks D and I
                                                         0x48, 0x49, 0x5a, 0x5b,
                                                         0x44, 0x45, 0x46, 0x47,
                                                         0x40, 0x41, 0x42, 0x43,
                                                         0x3c, 0x3d, 0x3e, 0x3f,
                                                         //Banks E and J
                                                         0x58, 0x59, 0x6a, 0x6b,
                                                         0x54, 0x55, 0x56, 0x57,
                                                         0x50, 0x51, 0x52, 0x53,
                                                         0x4c, 0x4d, 0x4e, 0x4f,
                                                         };

    unsigned char note_midi_message = list_pads_modeb_hex[note_padb_value_80];

    //Returning the possible midi channels based on the QString Channels
    std::array <unsigned char, 2> returnvalue = {midi_channel_message, note_midi_message};
    return returnvalue;
}


//Used by Midi Notes
unsigned char Midi_note::midichannel_to_hex(int midi_channel){

    //The first value is the midi channel
    std::vector<unsigned char> list_midi_channels_hex = {0,
                                                         0x90, 0x91, 0x92, 0x93,
                                                         0x94, 0x95, 0x96, 0x97,
                                                         0x98, 0x99, 0x9A, 0x9B,
                                                         0x9C, 0x9D, 0x9E, 0x9F };
    return list_midi_channels_hex[midi_channel];
}

unsigned char Midi_note::musical_note_to_hex(int musical_note_position){
    //The second value is the note
    std::vector<unsigned char> list_notes_hex = {
                                                 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                                 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                                 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,
                                                 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                                                 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
                                                 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                                                 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53,
                                                 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
                                                 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
                                                 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
                                                 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F };
    return list_notes_hex[musical_note_position];
}

//Qstrings are directrly translated to note information
void Midi_note::command_to_hex(QString command_string){


    if (command_string == "Stop Playback"){
        //Midi CH1 CC85 Vel 127
        status_byte = 0xB0;
        data_byte_1 = 0x55;
        data_byte_2 = 0x7f;
    }


    else if (command_string == "Looper Undo"){
        //Midi CH1 CC91 Vel 127
        status_byte = 0xB0;
        data_byte_1 = 0x5b;
        data_byte_2 = 0x00;
    }

    else if (command_string == "Looper Redo"){
        //Midi CH1 CC91 Vel 0
        status_byte = 0xB0;
        data_byte_1 = 0x5b;
        data_byte_2 = 0x7f;
    }

    else if (command_string == "DJ Play CH1"){
        //Midi Channel 1 CC 20 Vel 127
        status_byte = 0xB0;
        data_byte_1 = 0x14;
        data_byte_2 = 0x7f;
    }

    else if (command_string == "DJ Play CH2"){
        //Midi Channel 2 CC 20 Vel 127
        status_byte = 0xB0;
        data_byte_1 = 0x14;
        data_byte_2 = 0x7f;
    }

    else if (command_string == "DJ Stop CH1"){
        //Midi Channel 1 CC 20 Vel 0
        status_byte = 0xB0;
        data_byte_1 = 0x14;
        data_byte_2 = 0x00;
    }

    else if (command_string == "DJ Stop CH2"){
        //Midi Channel 2 CC 20 Vel0
        status_byte = 0xB0;
        data_byte_1 = 0x14;
        data_byte_2 = 0x00;
    }

    else{qDebug("No command value exists for this string");}

}
