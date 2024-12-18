#include <Qserialportinfo>
#include <QByteArray>
#include <vector>
#include <QTime>
#include <qdebug>
#include "widget.h"
#include "./ui_widget.h"


void Widget::getAvalilableSerialDevices()
{
    serialComPortList.clear();
    ui->serialPortSelect_comboBox->clear();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        serialComPortList.push_back(serialPortInfo);
        ui->serialPortSelect_comboBox->addItem(serialPortInfo.portName() +" "+serialPortInfo.description());
    }
}
void Widget::serialWrite(QByteArray qbytemessage)
{
    if(serialDeviceIsConnected == true)
    {
        usbDevice->write(qbytemessage); // Send the message to the device
    }
}
bool Widget::serialRead()
{
    if(serialDeviceIsConnected == true)
    {
        QByteArray serialMessage = usbDevice->read(1);
        unsigned char* serialMessageByte = new unsigned char[1];
        std::memcpy(serialMessageByte,serialMessage.constData(),1);;
        if (*serialMessageByte == 0xfd){
            ui->logPlainTextEdit->appendPlainText("Transfer successful");
            delete[] serialMessageByte;
            return 1;
            }
        else{
            ui->logPlainTextEdit->appendPlainText("Transfer failed");
            delete[] serialMessageByte;
            return 0;
            }
        }
    else{
            return 0;}
    }


void Widget::on_sendto404_button_clicked()
{
    //Getting the values of the buttons and sending them to the microcontroller
    fs404notesmessage();
}


void Widget::on_connect_button_clicked()
{
    if(serialDeviceIsConnected == false)
    {
        //No connexion can be set if no ports are available
        if(QSerialPortInfo::availablePorts().isEmpty()){
            serialDeviceIsConnected = false;
            }

        else{
            usbDevice->setPortName(serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].portName());
            deviceDescription = serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].description();
            ui->logPlainTextEdit->appendPlainText("connecting to: " + usbDevice->portName());
        }
        if(usbDevice->open(QIODevice::ReadWrite))
        {
            //Now the serial port is open try to set configuration
            //Depends on your boud-rate on the Device
            //Romain remplacer tous les qDebug avant deploy
            if(!usbDevice->setBaudRate(baudrate))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setDataBits(QSerialPort::Data8))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setParity(QSerialPort::NoParity))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setStopBits(QSerialPort::OneStop))
                qDebug()<<usbDevice->errorString();

            if(!usbDevice->setFlowControl(QSerialPort::NoFlowControl))
                qDebug()<<usbDevice->errorString();

            //If any error was returned the serial il corrctly configured

            ui->logPlainTextEdit->appendPlainText("Connection to: " + usbDevice->portName() + " " + deviceDescription + " connected");
            serialDeviceIsConnected = true;
        }
        else
        {
            ui->logPlainTextEdit->appendPlainText("Connection to: " + usbDevice->portName() + " " + deviceDescription + " not connected");
            ui->logPlainTextEdit->appendPlainText("         Error: " + usbDevice->errorString());
            serialDeviceIsConnected = false;
        }
    }
    else
    {
        ui->logPlainTextEdit->appendPlainText("Can't connect, please check the USB device");
    }
}


void Widget::on_disconnect_button_clicked()
{
    if(serialDeviceIsConnected)
    {
        usbDevice->close();
        serialDeviceIsConnected = false;
        ui->logPlainTextEdit->appendPlainText("Connection to: " + usbDevice->portName() + " " + deviceDescription + " closed");
    }
    else
    {
        ui->logPlainTextEdit->appendPlainText("Can't disconnect, no device is connected");
    }
}

void Widget::fs404notesmessage(){
    if(serialDeviceIsConnected == true)
    {
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

        //creating the objects for the notes
        std::array<unsigned char, 3> noteInfo1Hex = ui->note1->sendNoteInfo().returnmessage();
        std::array<unsigned char, 3> noteInfo2Hex = ui->note2->sendNoteInfo().returnmessage();
        std::array<unsigned char, 3> noteInfo3Hex = ui->note3->sendNoteInfo().returnmessage();


        //Passing the return to the message raw array
        unsigned char messageraw[12];
        messageraw[0] = messageStart;
        //Pasting the info of the three notes
        std::copy(std::begin(noteInfo1Hex), std::end(noteInfo1Hex), messageraw + 1);
        std::copy(std::begin(noteInfo2Hex), std::end(noteInfo2Hex), messageraw + 4);
        std::copy(std::begin(noteInfo3Hex), std::end(noteInfo3Hex), messageraw + 7);
        //Calculating the Checksum
        for(int x=0; x<10;x++){checksum += messageraw[x];}
        messageraw[10] = checksum;
        messageraw[11] = messageEnd;

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
