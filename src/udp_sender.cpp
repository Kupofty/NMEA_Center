#include "udp_sender.h"

/////////////
/// Class ///
/////////////
UdpSender::UdpSender()
{

}



/////////////////
/// Functions ///
/////////////////
void UdpSender::sendData(const QString line)
{
    if(outputNMEA)
    {
        QByteArray data = line.toUtf8();
        udpSocket.writeDatagram(data, udpAddress, udpPort);
    }
}



////////////////////
/// Public slots ///
////////////////////
void UdpSender::publishGGA(const QString line)
{
    if(outputGGA)
        sendData(line);
}

void UdpSender::publishRMC(const QString line)
{
    if(outputRMC)
        sendData(line);
}

void UdpSender::publishGSV(const QString line)
{
    if(outputGSV)
        sendData(line);
}

void UdpSender::publishGLL(const QString line)
{
    if(outputGLL)
        sendData(line);
}

void UdpSender::publishGSA(const QString line)
{
    if(outputGSA)
        sendData(line);
}

void UdpSender::publishVTG(const QString line)
{
    if(outputVTG)
        sendData(line);
}

void UdpSender::publishOthers(const QString line)
{
    if(outputOthers)
        sendData(line);
}



///////////////////////
/// Update settings ///
///////////////////////
void UdpSender::updateUdpPort(int port)
{
    udpPort = port;
}

void UdpSender::updateUdpMethod(QHostAddress method)
{
    udpAddress = method;
}



//////////////////////
/// Update outputs ///
//////////////////////
void UdpSender::updateOutputNMEA(bool check)
{
    outputNMEA = check;
}

void UdpSender::updateOutputGGA(bool check)
{
    outputGGA = check;
}

void UdpSender::updateOutputRMC(bool check)
{
    outputRMC = check;
}

void UdpSender::updateOutputGSV(bool check)
{
    outputGSV = check;
}

void UdpSender::updateOutputGLL(bool check)
{
    outputGLL = check;
}

void UdpSender::updateOutputGSA(bool check)
{
    outputGSA = check;
}

void UdpSender::updateOutputVTG(bool check)
{
    outputVTG = check;
}

void UdpSender::updateOutputOthers(bool check)
{
    outputOthers = check;
}
