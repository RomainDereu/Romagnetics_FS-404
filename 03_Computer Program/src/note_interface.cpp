#include "note_interface.h"
#include "ui_note_interface.h"
#include <vector>
#include <string>
#include <qdebug>

Note_interface::Note_interface(QWidget *parent)
    : QWidget(parent)
    , note_ui(new Ui::Note_interface)
{
    note_ui->setupUi(this);

    const std::vector<std::string> list_modes = { "SP Midi A", "SP Midi B", "SP Commands", "Midi Notes", "CC" };

    //Populating the upload modes
    for (int x = 0; x < list_modes.size(); x++){
        note_ui->note_list_mode_combo->addItem(QString::fromStdString(list_modes[x]));
    }

    //Defining the values for the Comboboxes and their respective hex values
    //All the values are declared explicitely for better readability
    const std::vector<std::string> list_notes = {
        "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
        "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
        "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
        "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
        "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
        "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
        "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
        "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
        "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
        "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9", "G#9", "A9", "A#9", "B9",
        "C10", "C#10", "D10", "D#10", "E10", "F10", "F#10", "G10"
    };

    //Populating Midi Notes Mode 3
    for (int x = 0; x < list_notes.size(); x++){
        note_ui->note_note_combo->addItem(QString::fromStdString(list_notes[x]));
    }

    const std::vector<std::string> list_pads = {
        "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12", "A13", "A14", "A15", "A16",
        "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "B10", "B11", "B12", "B13", "B14", "B15", "B16",
        "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "C10", "C11", "C12", "C13", "C14", "C15", "C16",
        "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15", "D16",
        "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "E10", "E11", "E12", "E13", "E14", "E15", "E16",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16",
        "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9", "G10", "G11", "G12", "G13", "G14", "G15", "G16",
        "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9", "H10", "H11", "H12", "H13", "H14", "H15", "H16",
        "I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8", "I9", "I10", "I11", "I12", "I13", "I14", "I15", "I16",
        "J1", "J2", "J3", "J4", "J5", "J6", "J7", "J8", "J9", "J10", "J11", "J12", "J13", "J14", "J15", "J16",
    };

    //Populating the SP Midi A pads
    for (int x = 0; x < list_pads.size(); x++){
        note_ui->note_pada_value_combo->addItem(QString::fromStdString(list_pads[x]));
    }

    //Populating the SP Midi B pads
    for (int x = 0; x < list_pads.size(); x++){
        note_ui->note_padb_value_combo->addItem(QString::fromStdString(list_pads[x]));
    }

    const std::vector<std::string> list_midi_b = { "1/2", "2/3", "3/4", "4/5", "5/6", "6/7", "7/8", "8/9", "9/10", "10/11"};

    //Populating mode B midi channels
    for (int x = 0; x < list_midi_b.size(); x++){
        note_ui->note_padb_midich_combo->addItem(QString::fromStdString(list_midi_b[x]));
    }

}

//Updating the page shown by QStackedWidget
void Note_interface::on_note_list_mode_combo_currentTextChanged(const QString &arg1)
{
    //Checking the current value
    QString currentmode = note_ui->note_list_mode_combo->currentText();

    //case  SP Midi A
    if (currentmode == "SP Midi A"){
        note_ui->note_stackedWidget->setCurrentWidget(note_ui->note_0padsa);
        return;
    }

    //case SP Midi B
    else if (currentmode == "SP Midi B"){
        note_ui->note_stackedWidget->setCurrentWidget(note_ui->note_1padb);
        return;
    }


    //case SP Commands
    else if (currentmode == "SP Commands"){
        note_ui->note_stackedWidget->setCurrentWidget(note_ui->note_2commands);
        return;
    }

    //case Midi Notes
    else if (currentmode == "Midi Notes"){
        note_ui->note_stackedWidget->setCurrentWidget(note_ui->note_3notes);
        return;
    }

    //case CC
    else if (currentmode == "CC"){
        note_ui->note_stackedWidget->setCurrentWidget(note_ui->note_4cc);
        return;
    }

    else{qDebug()<< "UI error: No midi note mode selected";}

}

//Main interface between the UI and the notes being send
std::array<unsigned char, 3>  Note_interface::sendNoteInfo()
{
    //Checking which UI is active
    // 0 for "SP Midi A", 1 for "SP Midi B", 2 for "SP Commands", 3 for "Midi Notes", "CC"
    QString activeUiCode = note_ui->note_list_mode_combo->currentText();
    std::array<unsigned char, 3> NoteInfo;

    if (activeUiCode == "SP Midi A") {
        //getting the raw values
        QString note_pada_value_combo_string = note_ui->note_pada_value_combo->currentText();
        int note_pada_value_combo_index = note_ui->note_pada_value_combo->currentIndex();
        //From letter to channel and from index to note
        QChar note_pada_value_combo_first_char = note_pada_value_combo_string.at(0);

        //Final values to be uploaded
        unsigned char note_pada_midi_channel = letterToMidiChannela(note_pada_value_combo_first_char);
        unsigned char note_pada_note = indexToPadANote(note_pada_value_combo_index);
        unsigned char velocity_spin_value = note_ui->note_pada_velocity_spin->value();


        //Translating to hex value
        NoteInfo[0] = note_pada_midi_channel;
        NoteInfo[1] = note_pada_note;
        NoteInfo[2] = velocity_spin_value -1;
    }

    else if (activeUiCode == "SP Midi B") {
        //Getting the raw values
        int note_padb_value_combo_string = note_ui->note_padb_midich_combo->currentIndex();
        int note_padb_value_combo = note_ui->note_padb_value_combo->currentIndex();
        unsigned char note_padb_velocity_spin = note_ui->note_padb_velocity_spin->value();

        //Midi channel and note are calculated together since both informaion is needed
        std::array<unsigned char, 2 >midiChannelAndPadMidiB = numberToMidiChannelb(note_padb_value_combo_string, note_padb_value_combo);


        //getting the raw values
        NoteInfo[0] = midiChannelAndPadMidiB[0];
        NoteInfo[1] = midiChannelAndPadMidiB[1];
        NoteInfo[2] = note_padb_velocity_spin-1;
    }

    else if (activeUiCode == "SP Commands") {
        //Translating to hex value
        NoteInfo[0] = 0x01;
        NoteInfo[1] = 0x01;
        NoteInfo[2] = 0x01;
    }

    else if  (activeUiCode == "Midi Notes") {
    //getting the raw values
    int note_midi_ch = note_ui->note_midich_spin->value();
    int note_note_combo = note_ui->note_note_combo->currentIndex();
    unsigned char velocity_spin_value = note_ui->note_velocity_spin->value() ;
    //Translating to hex value
    NoteInfo[0] = midichannel_to_hex(note_midi_ch);
    NoteInfo[1] = musical_note_to_hex(note_note_combo);
    NoteInfo[2] = velocity_spin_value -1;
    }

    else if (activeUiCode == "CC") {
        //Translating to hex value
        NoteInfo[0] = 0x01;
        NoteInfo[1] = 0x01;
        NoteInfo[2] = 0x01;
    }

    //Romain the length of NoteInfo and the values need to be checked
    return NoteInfo;
}

//updating the note labe:
void Note_interface::update_note_main_label(QString label_text)
{
    note_ui->note_main_label->setText(label_text);
    return;
}




//Functions used by Midi mode A
//Letter to midi channel
unsigned char Note_interface::letterToMidiChannela(QChar firstLetter){
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


//Transforms the string from Midi channel B to the correct midi channel it should be send to
std::array <unsigned char, 2>Note_interface::numberToMidiChannelb(int channels_index, int note_padb_value_combo){
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

//Index to pad note
unsigned char Note_interface::indexToPadANote(int comboIndex){


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



//Function used by Note 3
//Returns the midi byte when provided with the midi channel
unsigned char Note_interface::midichannel_to_hex(int midi_channel){

    //The first value is the midi channel
    std::vector<unsigned char> list_midi_channels_hex = {0,
                                                         0x90, 0x91, 0x92, 0x93,
                                                         0x94, 0x95, 0x96, 0x97,
                                                         0x98, 0x99, 0x9A, 0x9B,
                                                         0x9C, 0x9D, 0x9E, 0x9F };
    return list_midi_channels_hex[midi_channel];
}

//Returns the note byte when provided with the note in ABC notation
unsigned char Note_interface::musical_note_to_hex(int musical_note_position){
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


Note_interface::~Note_interface()
{
    delete note_ui;
}
