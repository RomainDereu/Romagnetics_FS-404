#include "widget.h"
#include "./ui_widget.h"
#include <QDebug>
#include <QTime>


void Widget::fs404notesmessage(){

    //In this mode, there are 10 MIDI channels - one for each bank in the SP.
    //Bank A is controlled by MIDI channel 1, bank B is MIDI channel 2
    //until bank J which is controlled midi channel 10.
    std::vector<unsigned char> list_pads_modea_hex = {0x30, 0x31, 0x32, 0x33,
                                                      0x2c, 0x2d, 0x2e, 0x2f,
                                                      0x28, 0x29, 0x2a, 0x2b,
                                                      0x24, 0x25, 0x26, 0x27};

    //This mode uses two MIDI channels: 1 and 2, each encompassing 5 banks in the SP
    std::vector<unsigned char> list_pads_modeb_hex = {//Banks A and F
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




    if(serialDeviceIsConnected == true)
    {
        //Fetching the info from the UI
        std::vector<int> noteInfo1 =  ui->note1->sendNoteInfo();
        std::vector<int> noteInfo2 =  ui->note2->sendNoteInfo();
        std::vector<int> noteInfo3 =  ui->note3->sendNoteInfo();

        //converting the values
        std::vector<unsigned char> noteInfo1Hex = notesToHex(noteInfo1);
        std::vector<unsigned char> noteInfo2Hex = notesToHex(noteInfo2);
        std::vector<unsigned char> noteInfo3Hex = notesToHex(noteInfo3);


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





 //Romain change to pointer
 //Translates the values of notes widget to the corresponding hex values
 std::vector<unsigned char> notesToHex(std::vector<int> noteInfo){
     std::vector<unsigned char> returnValue;
     //The first value is the midi channel
     std::vector<unsigned char> list_midi_channels_hex = {0x90, 0x91, 0x92, 0x93,
                                                          0x94, 0x95, 0x96, 0x97,
                                                          0x98, 0x99, 0x9A, 0x9B,
                                                          0x9C, 0x9D, 0x9E, 0x9F };
     returnValue.push_back(list_midi_channels_hex[noteInfo[0]]);

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
     returnValue.push_back(list_notes_hex[noteInfo[1]]);
     //The third value is the velocity
     returnValue.push_back(noteInfo[2]);

     return returnValue;
 };


 void delay()
 {
     QTime dieTime= QTime::currentTime().addSecs(1);
     while (QTime::currentTime() < dieTime)
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
 }
