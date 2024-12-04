#include <Qserialportinfo>
#include <QByteArray>
#include <vector>
#include <string>
#include "widget.h"
#include "./ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    for (int x = 0; x < list_midi_channels.size(); x++){
        ui->note1_midich_combo->addItem(QString::number(list_midi_channels[x]));
    }

    for (std::vector<std::string>::iterator t = list_notes.begin(); t != list_notes.end(); ++t){
        ui->note1_note_combo->addItem(t->c_str());
    }


    usbDevice = new QSerialPort(this);
    connect(usbDevice,SIGNAL(readyRead()),this,SLOT(onSerialDataAvailable()));
    baudrate = 31250;
    serialDeviceIsConnected = false;
    getAvalilableSerialDevices();

}



Widget::~Widget()
{
    delete ui;
    delete usbDevice;
}

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

        //Changing the code to a Qstring to make it readable
        QString messagestring;
        for(int i = 0; i < qbytemessage.size(); i++)
            messagestring.append("0x"+QString::number(static_cast<unsigned char>(qbytemessage[i]),16).toUpper().rightJustified(2,'0')+ " ");

        ui->logPlainTextEdit->appendPlainText("Message to device: " + messagestring);
    }
}
void Widget::serialRead()
{
    if(serialDeviceIsConnected == true)
    {
        QByteArray serialMessage= usbDevice->readAll();
        ui->logPlainTextEdit->appendPlainText("Arduino sent: " + serialMessage.toHex(' '));
        serialMessage.clear();
    }

    //Romain to be deleted

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
