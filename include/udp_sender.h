#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include "output_writer.h"

#include <QUdpSocket>
#include <QHostAddress>

class UdpSender : public OutputWriter
{
    Q_OBJECT

public:
    explicit UdpSender(QObject *parent = nullptr);
    ~UdpSender();

    //Specific to UDP
    void sendData(const QString &data) override;

    // UDP-specific config
    void updateUdpPort(int port);
    void updateUdpMethod(const QHostAddress &udpAddress);

private:
    QUdpSocket udpSocket;
    QHostAddress udpAddress = QHostAddress::Broadcast;
    int udpPort = 1024;
};

#endif // UDP_SENDER_H
