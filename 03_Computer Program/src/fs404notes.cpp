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

    if(serialDeviceIsConnected == true)
    {
        //Fetching the info from the UI
        QVector<QString> noteInfo =  sendNoteInfo();
        //Creating the message for the 404
        QString note1message = "Note 1 Midi Channel: " + noteInfo[0] +
                               "  Note 1 Pitch: "  + noteInfo[1] +
                               "  Note 1 Velocity: "  + noteInfo[2];
        //Appending the log
        ui->logPlainTextEdit->appendPlainText(note1message);

        //Sending the values to the microcontroller
        static unsigned char messageraw[] = {0, 0x90, 0x18, 10, 0x90 , 0x19, 64, 0x90 , 0x1A ,64, 255};
        //Converting the unsigned char array into an char array
        static char messagechar[11];
        for (size_t i = 0; i < 11; i++)
            messagechar[i] = static_cast<unsigned char>(messageraw[i]);
        QByteArray messageQByte =  QByteArray::fromRawData(messagechar, sizeof(messagechar));;
        serialWrite(messageQByte);
        serialRead();
    }
    else {
        ui->logPlainTextEdit->appendPlainText("The pedal isn't connected");
        }
    }
