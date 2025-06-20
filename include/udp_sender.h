#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <QObject>
#include <QUdpSocket>
#include <QCoreApplication>
#include <QDebug>

class UdpSender : public QObject
{
    Q_OBJECT

    public:
        UdpSender();
        void sendData(const QString data);

        //Update members
        void updateUdpPort(int port);
        void updateOutputGGA(bool check);
        void updateOutputRMC(bool check);
        void updateOutputGSV(bool check);
        void updateOutputGLL(bool check);
        void updateOutputGSA(bool check);
        void updateOutputVTG(bool check);
        void updateOutputOthers(bool check);

    public slots:
        void publishGGA(const QString line);
        void publishRMC(const QString line);
        void publishGSV(const QString line);
        void publishGLL(const QString line);
        void publishGSA(const QString line);
        void publishVTG(const QString line);
        void publishOthers(const QString line);


    private:
        //Connection settings
        QUdpSocket udpSocket;
        QHostAddress udpAddress = QHostAddress::Broadcast;
        int udpPort = 1024;

        //Allowed output sentences
        bool outputGGA = 0;
        bool outputRMC = 0;
        bool outputGSV = 0;
        bool outputGLL = 0;
        bool outputGSA = 0;
        bool outputVTG = 0;
        bool outputOthers = 0;
};

#endif // UDP_SENDER_H
