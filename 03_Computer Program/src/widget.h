#ifndef WIDGET_H
#define WIDGET_H

#include <QSerialPort>
#include <Qwidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void on_sendto404_button_clicked();
    void on_connect_button_clicked();
    void on_disconnect_button_clicked();

private:
    void getAvalilableSerialDevices();
    bool serialRead();
    void serialWrite(QByteArray message);



    //Serial related
    qint32 baudrate;
    QSerialPort *usbDevice;
    std::vector<QSerialPortInfo> serialComPortList; //A list of the available ports for the dropdownmenue in the GUI

    QString deviceDescription;

    bool serialDeviceIsConnected;

    Ui::Widget *ui;

    void fs404notesmessage();

};

void delay();

#endif // WIDGET_H
