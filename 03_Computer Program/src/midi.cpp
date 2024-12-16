#include "widget.h"
#include "./ui_widget.h"
#include <qdebug>
#include <QTime>

void Widget::fs404notesmessage(){
    if(serialDeviceIsConnected == true)
    {

        //converting the values
        std::array<unsigned char, 3> noteInfo1Hex = ui->note1->sendNoteInfo();
        std::array<unsigned char, 3> noteInfo2Hex = ui->note2->sendNoteInfo();
        std::array<unsigned char, 3> noteInfo3Hex = ui->note3->sendNoteInfo();


        /* Message format:
        Header       (1 byte)           - Header Byte (value 0xfe)
        Data         (9 bytes)          - Info for the three notes
        Checksum     (1 byte)           - Message integrity check
        Message end  (1 byte)           - Signaling the end of the message (value 0xff)
        Total message size = 12 bythes
        */

        //list of error messages
        //Reduced to one char to be sent as a single bit
        const unsigned char noMessageReceived = 0xfb; // 251
        const unsigned char messageError = 0xfc; // 252
        const unsigned char allreceived = 0xfd; // 253
        const unsigned char messageStart = 0xfe; // 254
        const unsigned char messageEnd = 0xff; // 254
        unsigned char checksum = 0; // initializing at 0

        //Sending the values to the microcontroller
        unsigned char messageraw[] = {messageStart,
                                      noteInfo1Hex[0], noteInfo1Hex[1], noteInfo1Hex[2],
                                      noteInfo2Hex[0], noteInfo2Hex[1], noteInfo2Hex[2],
                                      noteInfo3Hex[0], noteInfo3Hex[1], noteInfo3Hex[2],
                                      checksum,
                                      messageEnd};

        //Calculating the Checksum
        for(int x=0; x<10;x++){checksum += messageraw[x];}
        messageraw[10] = checksum;

        //Converting the unsigned char array into an char array
        char messagechar[12];
        for (size_t i = 0; i < 12; i++)
            messagechar[i] = static_cast<unsigned char>(messageraw[i]);
        QByteArray messageQByte =  QByteArray::fromRawData(messagechar, sizeof(messagechar));;

        bool transferwassuccessful = 0;
        int attempts = 0;
        //Making up to 10 loops if transfers aren't successful
        do {
            serialWrite(messageQByte);
            //displaying the message sent
            QString  messagePlainText = QString(messageQByte.toHex(' '));
            ui->logPlainTextEdit->appendPlainText(messagePlainText);

            //trying to send the message 10 times.
            //Sending an error message if the transfer doesn't work
            transferwassuccessful = serialRead();
            delay();
            attempts ++;
        }
        while (attempts < 10 && transferwassuccessful == 0 );



    }
    else {
        ui->logPlainTextEdit->appendPlainText("The pedal isn't connected");
    }
}




 void delay()
 {
     QTime dieTime= QTime::currentTime().addSecs(1);
     while (QTime::currentTime() < dieTime)
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
 }
