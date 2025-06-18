#include "gps_reader.h"


//Class
GPS_Reader::GPS_Reader()
{
    //Set serial parameters
    serial.setPortName("COM0");
    serial.setBaudRate(QSerialPort::Baud4800);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    //Connect
    connect(&serial, &QSerialPort::readyRead, this, &GPS_Reader::readData);
}


//Connection
bool GPS_Reader::openSerialDevice()
{
    return serial.open(QIODevice::ReadOnly);
}

void GPS_Reader::closeSerialDevice()
{
    serial.close();
}


//Handle data
void GPS_Reader::readData()
{
    const QByteArray data = serial.readAll();
    buffer.append(data);

    while (buffer.contains('\n'))
    {
        int index = buffer.indexOf('\n');
        QByteArray line = buffer.left(index).trimmed();
        buffer.remove(0, index + 1);

        emit newLineReceived(line); //for GUI
    }
}


//Set
void GPS_Reader::setPortName(const QString &portName)
{
    serial.setPortName(portName);
}

void GPS_Reader::setBaudRate(qint32 baudRate)
{
    serial.setBaudRate(baudRate);
}


//Get
QString GPS_Reader::getPortName()
{
    return serial.portName();
}

QString GPS_Reader::getErrorString()
{
    return serial.errorString();
}

bool GPS_Reader::isSerialOpen()
{
    return serial.isOpen();
}

