#ifndef NMEA_HANDLER_H
#define NMEA_HANDLER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QTime>

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
        QElapsedTimer timer_gsv, timer_gga, timer_vtg, timer_gll, timer_gsa, timer_rmc;
        qint64 lastUpdateTimeGGA = -1, lastUpdateTimeGSV = -1, lastUpdateTimeRMC = -1, lastUpdateTimeVTG = -1, lastUpdateTimeGLL = -1, lastUpdateTimeGSA = -1;

    private:
        void handleGGA(const QList<QByteArray> &fields);
        void handleRMC(const QList<QByteArray> &fields);
        void handleGSV(const QList<QByteArray> &fields);
        void handleGLL(const QList<QByteArray> &fields);
        void handleGSA(const QList<QByteArray> &fields);
        void handleVTG(const QList<QByteArray> &fields);

        double calculateCoordinates(const QString &valueStr, const QString &direction);
        double calculateFrequency(QElapsedTimer &timer, qint64 &lastTime);
        double removeAsterisk(const QByteArray lastField);
        void checkMinimumLineSize(const QList<QByteArray> &fields, int minSize);

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

        void newDecodedGSA(double pdop, double hdop, double vdop, double freqHz);
        void newDecodedGSV(int totalSatellites, double freqHz);
        void newDecodedGLL(QString utc, double latitude, double longitude, double freqHz);
        void newDecodedGGA(QString utc, double latitude, double longitude, int fixQuality, int numSatellites, double hdop, double altitude, double freqHz);
        void newDecodedVTG(double track_true, double track_mag, double speed_kn, double speedKmh, double freqHz);
        void newDecodedRMC(QString utcDate, QString utcTime, double latitude, double longitude, double speedMps, double course, double magVar, double freqHz);

};

#endif // NMEA_HANDLER_H
