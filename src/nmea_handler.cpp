#include "nmea_handler.h"


/////////////
/// Class ///
/////////////
NMEA_Handler::NMEA_Handler()
{

}


////////////////////////////
/// Detect NMEA Sentence ///
////////////////////////////
void NMEA_Handler::handleRawSentences(const QByteArray &line)
{
    QString nmeaText = QString::fromUtf8(line).trimmed();
    const QList<QByteArray> fields = line.split(',');

    if(line.startsWith("$GPTXT"))
    {
        emit newTXTSentence(nmeaText);
    }

    else if (line.startsWith("$GPGGA"))
    {
        emit newGGASentence(nmeaText);
        handleGGA(fields);
    }

    else if(line.startsWith("$GPRMC"))
    {
        emit newRMCSentence(nmeaText);
        handleRMC(fields);
    }

    else if(line.startsWith("$GPGSV"))
    {
        emit newGSVSentence(nmeaText);
        handleGSV(fields);
    }

    else if(line.startsWith("$GPGLL"))
    {
        emit newGLLSentence(nmeaText);
        handleGLL(fields);
    }

    else if(line.startsWith("$GPGSA"))
    {
        emit newGSASentence(nmeaText);
        handleGSA(fields);
    }

    else if(line.startsWith("$GPVTG"))
    {
        emit newVTGSentence(nmeaText);
        handleVTG(fields);
    }

    else
    {
        emit newOtherSentence(nmeaText);
    }
}


///////////////////
/// Handle Data ///
///////////////////
void NMEA_Handler::handleGGA(const QList<QByteArray> &fields)
{
    if (fields.size() < 6)
        return;

    // Parse latitude
    QString latStr = fields[2];
    QString latDir = fields[3];

    // Parse longitude
    QString lonStr = fields[4];
    QString lonDir = fields[5];

    bool ok1 = false, ok2 = false;

    double latRaw = latStr.toDouble(&ok1);
    double lonRaw = lonStr.toDouble(&ok2);

    if (!ok1 || !ok2)
        return;

    // Convert latitude
    double latDeg = floor(latRaw / 100.0);
    double latMin = latRaw - (latDeg * 100.0);
    double latitude = latDeg + (latMin / 60.0);
    if (latDir == "S")
        latitude *= -1;

    // Convert longitude
    double lonDeg = floor(lonRaw / 100.0);
    double lonMin = lonRaw - (lonDeg * 100.0);
    double longitude = lonDeg + (lonMin / 60.0);
    if (lonDir == "W")
        longitude *= -1;

    emit newPosition(latitude, longitude);
}

void NMEA_Handler::handleRMC(const QList<QByteArray> &fields)
{

}

void NMEA_Handler::handleGSV(const QList<QByteArray> &fields)
{
    if (fields.size() < 4)
        return;

    // Get number of satellites
    bool ok = false;
    int totalSatellites = fields[3].toInt(&ok);
    if(!ok)
        return;

    emit newSatellitesInView(totalSatellites);

    // Frequency measurement
    if (lastUpdateTimeMsGSV == -1)
    {
        timer_gsv.start();
        lastUpdateTimeMsGSV = 0;
        return ;
    }

    qint64 now = timer_gsv.elapsed();
    qint64 delta_time = now - lastUpdateTimeMsGSV;
    lastUpdateTimeMsGSV = now;

    if (delta_time > 0)
    {
        double freqHz = 1000.0 / static_cast<double>(delta_time);
        if(freqHz<100)//Bug when 2 GSV are received to close to another --> +1000Hz
            emit newGsvFrequency(freqHz);
    }



}

void NMEA_Handler::handleGLL(const QList<QByteArray> &fields)
{

}

void NMEA_Handler::handleGSA(const QList<QByteArray> &fields)
{

}

void NMEA_Handler::handleVTG(const QList<QByteArray> &fields)
{


}
