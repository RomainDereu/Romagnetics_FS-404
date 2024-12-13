#include <Qserialportinfo>
#include <QByteArray>
#include <vector>
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
        //Romain delete
        std::memcpy(serialMessageByte,serialMessage.constData(),1);;
        qDebug() << serialMessageByte;
        if (*serialMessageByte == 0xfd){
            ui->logPlainTextEdit->appendPlainText("Transfer successful");
            return 1;
            }
        else{
            ui->logPlainTextEdit->appendPlainText("Transfer failed");
            return 0;
            }
        }
    else{return 0;}
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
        usbDevice->setPortName(serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].portName());
        deviceDescription = serialComPortList[ui->serialPortSelect_comboBox->currentIndex()].description();
        ui->logPlainTextEdit->appendPlainText("connecting to: " + usbDevice->portName());
        if(usbDevice->open(QIODevice::ReadWrite))
        {
            //Now the serial port is open try to set configuration
            if(!usbDevice->setBaudRate(baudrate))        //Depends on your boud-rate on the Device
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
        ui->logPlainTextEdit->appendPlainText("Can't connect, another device is connected");
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
