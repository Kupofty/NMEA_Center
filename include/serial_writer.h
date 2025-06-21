#ifndef SERIAL_WRITER_H
#define SERIAL_WRITER_H

#include "output_writer.h"

#include <QSerialPort>

//Need to create serial class and inherit from it & from output_writer

class SerialWriter : public OutputWriter
{
    Q_OBJECT

    public:
        explicit SerialWriter(QObject *parent = nullptr);
        ~SerialWriter();

        void sendData(const QString &data) override;

        bool openSerialDevice();
        void closeSerialDevice();

        //Set
        void setPortName(const QString &portName);
        void setBaudRate(qint32 baudRate);

        //Get
        QString getPortName();
        QString getErrorString();
        bool isSerialOpen();

    private:
        QSerialPort serial;
        QByteArray buffer;
};

#endif // SERIAL_WRITER_H
