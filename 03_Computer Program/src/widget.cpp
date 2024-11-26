#include <Qserialportinfo>
#include <vector>
#include <string>
#include "widget.h"
#include "./ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //Romain may need to be moved some place else
    //Defining the values for the Comboboxes
    std::vector<std::string> list_midi_channels = {"Omni", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16" };
    std::vector<std::string> list_notes = {"C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0", "C1" };

    for (std::vector<std::string>::iterator t = list_midi_channels.begin(); t != list_midi_channels.end(); ++t){
        ui->note1_midich_combo->addItem(t->c_str());
    }

    for (std::vector<std::string>::iterator t = list_notes.begin(); t != list_notes.end(); ++t){
        ui->note1_note_combo->addItem(t->c_str());
    }


    usbDevice = new QSerialPort(this);
    connect(usbDevice,SIGNAL(readyRead()),this,SLOT(onSerialDataAvailable()));

    baudrate = QSerialPort::Baud115200;

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
    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
    serialComPortList.clear();
    ui->serialPortSelect_comboBox->clear();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        QString dbgStr = "Vendor ID: ";


        if(serialPortInfo.hasVendorIdentifier())
        {
            dbgStr+= serialPortInfo.vendorIdentifier();
        }
        else
        {
            dbgStr+= " - ";
        }
        dbgStr+= "  Product ID: ";
        if(serialPortInfo.hasProductIdentifier())
        {
            dbgStr+= "Serial Port has identifier";
        }
        else
        {
            dbgStr+= " - ";
        }
        dbgStr+= " Name: " + serialPortInfo.portName();
        dbgStr+= " Description: "+serialPortInfo.description();
        qDebug()<<dbgStr;
        serialComPortList.push_back(serialPortInfo);
        ui->serialPortSelect_comboBox->addItem(serialPortInfo.portName() +" "+serialPortInfo.description());
    }
}
void Widget::serialWrite(QString message)
{
    if(serialDeviceIsConnected == true)
    {
        usbDevice->write(message.toUtf8()); // Send the message to the device
        qDebug() << "Message to device: "<<message;
    }
}
void Widget::serialRead()
{
    if(serialDeviceIsConnected == true)
    {
        serialBuffer += usbDevice->readAll(); // Read the available data
    }
}
void Widget::onSerialDataAvailable()
{
    if(serialDeviceIsConnected == true)
    {
        serialRead(); // Read a chunk of the Message
        //To solve that problem I send a end char "]" in My case. That helped my to know when a message is complete

        if(serialBuffer.indexOf("]") != -1) //Message complete
        {
            qDebug() << "Message from device: "<<serialBuffer;
            serialWrite("echoFromGui");

            //Do something with de message here

            serialBuffer = "";  //Clear the buffer;
        }
    }
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
        qDebug() << "connecting to: "<<usbDevice->portName();
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

            qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " connected";
            serialDeviceIsConnected = true;
        }
        else
        {
            qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " not connected";
            qDebug() <<"         Error: "<<usbDevice->errorString();
            serialDeviceIsConnected = false;
        }
    }
    else
    {
        qDebug() << "Can't connect, another device is connected";
    }
}


void Widget::on_disconnect_button_clicked()
    {
        if(serialDeviceIsConnected)
        {
            usbDevice->close();
            serialDeviceIsConnected = false;
            qDebug() << "Connection to: "<< usbDevice->portName() << " " << deviceDescription << " closed";
        }
        else
        {
            qDebug() << "Can't disconnect, no device is connected";
        }
}
