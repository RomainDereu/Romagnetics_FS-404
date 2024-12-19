#include "note_interface.h"
#include "ui_note_interface.h"
#include "midi_note.h"
#include <vector>
#include <string>
#include <qdebug>

Note_interface::Note_interface(QWidget *parent)
    : QWidget(parent)
    , note_ui(new Ui::Note_interface)
{
    note_ui->setupUi(this);
    setup_noteui();

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

void Note_interface::setup_noteui()
{

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

    //Populating mode B midi channels
    const std::vector<std::string> list_midi_b = { "1/2", "2/3", "3/4", "4/5", "5/6", "6/7", "7/8", "8/9", "9/10", "10/11"};
    for (int x = 0; x < list_midi_b.size(); x++){
        note_ui->note_padb_midich_combo->addItem(QString::fromStdString(list_midi_b[x]));
    }

    //Populating the Command mode
    const std::vector<std::string> list_commands = { "Stop Playback","Looper Undo", "Looper Redo",
                                                    "DJ Play CH1", "DJ Play CH2", "DJ Pause CH1", "DJ Pause CH2" };
    for (int x = 0; x < list_commands.size(); x++){
        note_ui->note_command_select_label_2->addItem(QString::fromStdString(list_commands[x]));
    }

}

//Main interface between the UI and the notes being send
Midi_note  Note_interface::sendNoteInfo()
{
    //Checking which UI is active
    // 0 for "SP Midi A", 1 for "SP Midi B", 2 for "SP Commands", 3 for "Midi Notes", "CC"
    //more information on midi note in the midi_note cpp and header files
    QString activeUiCode = note_ui->note_list_mode_combo->currentText();
    Midi_note midi_note;


    if (activeUiCode == "SP Midi A") {
        midi_note = Midi_note(note_ui->note_pada_value_combo->currentText(),
                              note_ui->note_pada_value_combo->currentIndex(),
                              note_ui->note_pada_velocity_spin->value());
    }

    else if (activeUiCode == "SP Midi B") {
        midi_note = Midi_note("SP Midi B",
                              note_ui->note_padb_midich_combo->currentIndex(),
                              note_ui->note_padb_value_combo->currentIndex(),
                              note_ui->note_padb_velocity_spin->value());
    }

    else if (activeUiCode == "SP Commands") {
        midi_note = Midi_note(note_ui->note_command_select_label_2->currentText());
    }

    else if  (activeUiCode == "Midi Notes") {

        midi_note = Midi_note("Midi Notes",
                              note_ui->note_midich_spin->value(),
                              note_ui->note_note_combo->currentIndex(),
                              note_ui->note_velocity_spin->value());
    }

    //CC doesn't need its own function as the implementation is very simple
    else if (activeUiCode == "CC") {
        midi_note = Midi_note("CC",
                              note_ui->note_cc_midi_spin->value(),
                              note_ui->note_cc_number_spin->value(),
                              note_ui->note_cc_value_spin->value());
    }

    return midi_note;

}

//updating the note labe:
void Note_interface::update_note_main_label(QString label_text)
{
    note_ui->note_main_label->setText(label_text);
    return;
}

void Note_interface::update_note_value(int combo_index)
{
    note_ui->note_pada_value_combo->setCurrentIndex(combo_index);
}


Note_interface::~Note_interface()
{
    delete note_ui;
}
