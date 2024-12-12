#include "note_interface.h"
#include "ui_note_interface.h"
#include <vector>
#include <string>
#include <QDebug>

Note_interface::Note_interface(QWidget *parent)
    : QWidget(parent)
    , note_ui(new Ui::Note_interface)
{
    note_ui->setupUi(this);

    for (int x = 0; x < list_midi_channels.size(); x++){
        note_ui->note_midich_combo->addItem(QString::number(list_midi_channels[x]));
    }

    for (std::vector<std::string>::iterator t = list_notes.begin(); t != list_notes.end(); ++t){
        note_ui->note_note_combo->addItem(t->c_str());
    }

}

std::vector<int>  Note_interface::sendNoteInfo()
{
    std::vector<int> NoteInfo;
    //Fetching the info for the current note
    NoteInfo.push_back(note_ui->note_midich_combo->currentIndex());
    NoteInfo.push_back(note_ui->note_note_combo->currentIndex());
    NoteInfo.push_back(note_ui->note_velocity_spin->value());

    //Romain the length of NoteInfo needs to be checked to avoid bugs
    return NoteInfo;
}

Note_interface::~Note_interface()
{
    delete note_ui;
}



