#ifndef GPS_READER_H
#define GPS_READER_H

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QDebug>

class GPS_Reader : public QObject
{
    Q_OBJECT

    public:
        GPS_Reader();

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
        void newLineReceived(const QByteArray line);

    private slots:
        void readData();

    private:
        //Parameters
        QSerialPort serial;
        QByteArray buffer;


};

#endif // GPS_READER_H

