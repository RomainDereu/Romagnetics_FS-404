#include <Qserialportinfo>
#include <QByteArray>
#include "widget.h"
#include "./ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    usbDevice = new QSerialPort(this);
    baudrate = 31250;
    serialDeviceIsConnected = false;
    getAvalilableSerialDevices();

}


Widget::~Widget()
{
    delete ui;
    delete usbDevice;
}
