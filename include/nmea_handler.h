#ifndef NMEA_HANDLER_H
#define NMEA_HANDLER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>

class NMEA_Handler : public QObject
{
    Q_OBJECT

    public:
        NMEA_Handler();
        ~NMEA_Handler();

    public slots:
        void handleRawSentences(const QByteArray &line);

    private:
        //Frequency timers
        QElapsedTimer timer_gsv, timer_gga;
        qint64 lastUpdateTimeGGA = -1, lastUpdateTimeGSV = -1, lastUpdateTimeRMC = -1;

    private:
        void handleGGA(const QList<QByteArray> &fields);
        void handleRMC(const QList<QByteArray> &fields);
        void handleGSV(const QList<QByteArray> &fields);
        void handleGLL(const QList<QByteArray> &fields);
        void handleGSA(const QList<QByteArray> &fields);
        void handleVTG(const QList<QByteArray> &fields);

        double calculateCoordinates(const QString &valueStr, const QString &direction);
        double calculateFrequency(QElapsedTimer &timer, qint64 &lastTime);

    signals:
        void newNMEASentence(const QString &type, const QString &nmeaText);
        void newTXTSentence(const QString &nmeaText);
        void newGGASentence(const QString &nmeaText);
        void newRMCSentence(const QString &nmeaText);
        void newGSVSentence(const QString &nmeaText);
        void newGLLSentence(const QString &nmeaText);
        void newGSASentence(const QString &nmeaText);
        void newVTGSentence(const QString &nmeaText);
        void newOtherSentence(const QString &nmeaText);

        void newDecodedGSV(int totalSatellites, double freqHz);
        void newDecodedGGA(double latitude, double longitude, double freqHz);

        void newGsvFrequency(double freq);
        void newGgaFrequency(double freq);

};

#endif // NMEA_HANDLER_H
