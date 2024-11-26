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

    //Making the UI data available to the rest of the code
    QVector<QString>  sendNoteInfo();


private slots:
    void on_sendto404_button_clicked();
    void onSerialDataAvailable();
    void on_connect_button_clicked();
    void on_disconnect_button_clicked();

private:
    void getAvalilableSerialDevices();
    void serialRead();
    void serialWrite(QString message);

    qint32 baudrate;
    QSerialPort *usbDevice;
    std::vector<QSerialPortInfo> serialComPortList; //A list of the available ports for the dropdownmenue in the GUI

    QString deviceDescription;
    QString serialBuffer;

    bool serialDeviceIsConnected;

    Ui::Widget *ui;

    //Contains the information for fs404notes.cpp
    void fs404notesmessage();

};

#endif // WIDGET_H
