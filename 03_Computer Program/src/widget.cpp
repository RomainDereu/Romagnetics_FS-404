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

    //setting up the names of the notes
    ui->note1->update_note_main_label(QString("Note 1"));
    ui->note2->update_note_main_label(QString("Note 2"));
    ui->note3->update_note_main_label(QString("Note 3"));

}


Widget::~Widget()
{
    delete ui;
    delete usbDevice;
}
