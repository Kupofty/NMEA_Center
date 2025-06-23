#include "nmea_handler.h"


/////////////
/// Class ///
/////////////
NMEA_Handler::NMEA_Handler()
{

}

NMEA_Handler::~NMEA_Handler()
{

}



////////////////////////////
/// Detect NMEA Sentence ///
////////////////////////////
void NMEA_Handler::handleRawSentences(const QByteArray &line)
{
    //Remove malformed sentences
    if (!line.startsWith('$'))
        return;

    QString nmeaText = QString::fromUtf8(line).trimmed();
    QList<QByteArray> fields = line.split(',');
    QString nmeaFormat = QString::fromUtf8(line.mid(3, 3));

    static const QSet<QString> supportedFormats = { "GGA", "RMC", "GSV", "GLL", "GSA", "VTG", "TXT" };
    if (!supportedFormats.contains(nmeaFormat))
        nmeaFormat ="OTHER";
    emit newNMEASentence(nmeaFormat, nmeaText);

    if      (nmeaFormat == "GGA") handleGGA(fields);
    else if (nmeaFormat == "RMC") handleRMC(fields);
    else if (nmeaFormat == "GSV") handleGSV(fields);
    else if (nmeaFormat == "GLL") handleGLL(fields);
    else if (nmeaFormat == "GSA") handleGSA(fields);
    else if (nmeaFormat == "VTG") handleVTG(fields);
}



///////////////////
/// Handle Data ///
///////////////////

//GGA
void NMEA_Handler::handleGGA(const QList<QByteArray> &fields)
{
    //Check size
    if (fields.size() < 6)
        return;

    // Parse position
    QString latStr = fields[2]; QString latDir = fields[3];
    QString lonStr = fields[4]; QString lonDir = fields[5];
    double latitude = calculateCoordinates(latStr, latDir);
    double longitude = calculateCoordinates(lonStr, lonDir);
    if (std::isnan(latitude) || std::isnan(longitude))
        return;

    //Calculate frequency
    double freqHz = calculateFrequency(timer_gga, lastUpdateTimeGGA);

    emit newDecodedGGA(latitude, longitude, freqHz);

}

//RMC
void NMEA_Handler::handleRMC(const QList<QByteArray> &fields)
{
    Q_UNUSED(fields);
}

//GSV
void NMEA_Handler::handleGSV(const QList<QByteArray> &fields)
{
    //Check size
    if (fields.size() < 4)
        return;

    // Parse GSV
    bool ok1 = false, ok2 = false;
    int sentenceNumber = fields[2].toInt(&ok1);
    int totalSatellites = fields[3].toInt(&ok2);
    if (!ok1 || !ok2)
        return;

    //GSV is composed of multiples sub-messages
    if (sentenceNumber == 1 )
    {
        double freqHz = calculateFrequency(timer_gsv, lastUpdateTimeGSV);
        emit newDecodedGSV(totalSatellites, freqHz);
    }
}

//GLL
void NMEA_Handler::handleGLL(const QList<QByteArray> &fields)
{
    Q_UNUSED(fields);
}

//GSA
void NMEA_Handler::handleGSA(const QList<QByteArray> &fields)
{
    Q_UNUSED(fields);
}

//VTG
void NMEA_Handler::handleVTG(const QList<QByteArray> &fields)
{
    Q_UNUSED(fields);
}



/////////////////////////
/// Generic Functions ///
/////////////////////////
double NMEA_Handler::calculateCoordinates(const QString &valueStr, const QString &direction)
{
    bool ok = false;
    double raw = valueStr.toDouble(&ok);
    if (!ok)
        return std::numeric_limits<double>::quiet_NaN();

    double degrees = floor(raw / 100.0);
    double minutes = raw - (degrees * 100.0);
    double result = degrees + (minutes / 60.0);

    if (direction == "S" || direction == "W")
        result *= -1.0;

    return result;
}

double NMEA_Handler::calculateFrequency(QElapsedTimer &timer, qint64 &lastUpdateTime)
{
    if (lastUpdateTime == -1)
    {
        timer.start();
        lastUpdateTime = 0;
        return 0;
    }

    qint64 now = timer.elapsed();
    qint64 delta_time = now - lastUpdateTime;
    lastUpdateTime = now;

    if (delta_time > 0)
    {
        double freqHz = 1000.0 / static_cast<double>(delta_time);
        return freqHz;
    }

    return 0;
}
