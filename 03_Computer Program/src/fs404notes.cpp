#include "widget.h"
#include "./ui_widget.h"

#include <QDebug>

QVector<QString>  Widget::sendNoteInfo()
{
    QVector<QString> NoteInfo;
    //Fetching the info for Note 1
    NoteInfo.append(ui->note1_midich_combo->currentText());
    NoteInfo.append(ui->note1_note_combo->currentText());
    NoteInfo.append(QString::number(ui->note1_velocity_spin->value()));

    //Romain the length of NoteInfo needs to be checked to avoid bugs
    return NoteInfo;
}



void Widget::fs404notesmessage(){

    //Fetching the info from the UI
    QVector<QString> noteInfo =  sendNoteInfo();
    //Creating the message for the 404
    QString note1message = "Note 1 Midi Channel: " + noteInfo[0] +
                           "\nNote 1 Pitch: "  + noteInfo[1] +
                           "\nNote 1 Velocity: "  + noteInfo[2];
    //Appending the log
    ui->plainTextEdit->appendPlainText(note1message);

    //Sending the values to the microcontroller
    //Romain TBD

}
