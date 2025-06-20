#ifndef SERIAL_READER_H
#define SERIAL_READER_H

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QDebug>

class SerialReader : public QObject
{
    Q_OBJECT

    public:
        SerialReader();
        ~SerialReader();

        bool openSerialDevice();
        void closeSerialDevice();

        //Set
        void setPortName(const QString &portName);
        void setBaudRate(qint32 baudRate);

        //Get
        QString getPortName();
        QString getErrorString();
        bool isSerialOpen();

    signals:
        void newLineReceived(const QByteArray &line);

    private slots:
        void readData();

    private:
        QSerialPort serial;
        QByteArray buffer;


};

#endif // SERIAL_READER_H

