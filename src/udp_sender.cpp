#include "udp_sender.h"

/////////////
/// Class ///
/////////////
UdpSender::UdpSender(QObject *parent) : OutputWriter(parent)
{

}

UdpSender::~UdpSender()
{

}



/////////////////
/// Functions ///
/////////////////
void UdpSender::sendData(const QString &line)
{
    if(getOutputNMEA())
    {
        QByteArray data = line.toUtf8();
        udpSocket.writeDatagram(data, udpAddress, udpPort);
    }
}


///////////////////////
/// Update settings ///
///////////////////////
void UdpSender::updateUdpPort(int port)
{
    udpPort = port;
}

void UdpSender::updateUdpMethod(const QHostAddress &method)
{
    udpAddress = method;
}

