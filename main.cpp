#include "mainwindow.h"
#include <QApplication>
#include <QSerialPortInfo>
#include "options_first.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

        // Example use QSerialPort
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
            serial.close();
    }

    options_first option_window;
    option_window.show();

//    MainWindow w;
//    w.show();

    return a.exec();


}
