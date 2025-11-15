#include <Qserialportinfo>
#include <QByteArray>
#include <qstackedwidget.h>
#include "widget.h"
#include "./ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //Hiding the Moving commands.
    ui->notebase->setIsBaseNote(true);

    usbDevice = new QSerialPort(this);
    baudrate = 31250;
    serialDeviceIsConnected = false;
    getAvalilableSerialDevices();

    //setting up the names of the notes
    ui->notebase->update_note_main_label(QString("Base Note"));
    ui->note1->update_note_main_label(QString("Note 1"));
    ui->note2->update_note_main_label(QString("Note 2"));
    ui->note3->update_note_main_label(QString("Note 3"));

    //setting up pads to 1 & 3
    ui->note2->update_note_value(1);
    ui->note3->update_note_value(2);
}

void Widget::on_refresh_button_clicked(){getAvalilableSerialDevices();}



Widget::~Widget()
{
    delete ui;
    delete usbDevice;
}


