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
            delete[] serialMessageByte;
            return 1;
            }
        else{
            delete[] serialMessageByte;
            return 0;
            }
        }
    else{
            return 0;}
    }


void Widget::on_sendto404_button_clicked()
{
    //Connect to the device if this is
    if(serialDeviceIsConnected == true)
    {
    //Getting the values of the buttons and sending them to the microcontroller
    fs404notesmessage();
    }
    else {
        ui->logPlainTextEdit->appendPlainText("The pedal isn't connected. Please click on Connect");
    }
}


void Widget::on_connect_button_clicked()
{
    if(serialDeviceIsConnected == false)
    {
        //No connexion can be set if no ports are available
        if(QSerialPortInfo::availablePorts().isEmpty()){
            ui->logPlainTextEdit->appendPlainText("The pedal isn't connected. Please connect to the USB port");
            return;
            }

        else{
            usbDevice->setPortName(serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].portName());
            deviceDescription = serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].description();
            ui->logPlainTextEdit->appendPlainText("Connecting to: " + usbDevice->portName());
        }
        if(usbDevice->open(QIODevice::ReadWrite))
        {
            //Now the serial port is open try to set configuration
            //Depends on your boud-rate on the Device
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

void Widget::fs404notesmessage() {

    ui->logPlainTextEdit->appendPlainText("Starting data transfer");

    const unsigned char messageStart = 0xFE;
    const unsigned char messageEnd   = 0xFF;

    // Gather UI note definitions
    std::array<unsigned char, 4> n0 = ui->notebase->sendNoteInfo().returnmessage();
    std::array<unsigned char, 4> n1 = ui->note1->sendNoteInfo().returnmessage();
    std::array<unsigned char, 4> n2 = ui->note2->sendNoteInfo().returnmessage();
    std::array<unsigned char, 4> n3 = ui->note3->sendNoteInfo().returnmessage();

    // === 20-byte message buffer ===
    unsigned char msg[20];
    msg[0] = messageStart;

    // 16 data bytes (4×4 fields)
    msg[1]  = n0[0];
    msg[2]  = n0[1];
    msg[3]  = n0[2];
    msg[4]  = n0[3];

    msg[5]  = n1[0];
    msg[6]  = n1[1];
    msg[7]  = n1[2];
    msg[8]  = n1[3];

    msg[9]  = n2[0];
    msg[10] = n2[1];
    msg[11] = n2[2];
    msg[12] = n2[3];

    msg[13] = n3[0];
    msg[14] = n3[1];
    msg[15] = n3[2];
    msg[16] = n3[3];

    // === NEW BYTE: Kill switch activation flag ===
    msg[17] = ui->killSwitchBox->isChecked() ? 1 : 0;

    // === Checksum over bytes [0..17] ===
    unsigned char checksum = 0;
    for (int i = 0; i <= 17; ++i) checksum += msg[i];
    msg[18] = checksum;

    // End byte
    msg[19] = messageEnd;

    // Convert to QByteArray
    QByteArray messageQByte(reinterpret_cast<const char*>(msg), 20);
    QString messagePlainText = QString(messageQByte.toHex(' '));

    bool transferOk = false;
    for (int tries = 0; tries < 10 && !transferOk; ++tries) {
        serialWrite(messageQByte);
        transferOk = serialRead();
        delay();
    }

    if (transferOk) {
        ui->logPlainTextEdit->appendPlainText("The following message was successfully sent");
        ui->logPlainTextEdit->appendPlainText(messagePlainText);
    } else {
        ui->logPlainTextEdit->appendPlainText("The message couldn't be saved. Please try again");
        ui->logPlainTextEdit->appendPlainText(messagePlainText);
    }
}




void delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
