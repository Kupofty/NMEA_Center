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
    checkMinimumLineSize(fields, 6);

    // Parse UTC time (field 1)
    QString timeStr = fields[1];
    QTime utcTime = QTime::fromString(timeStr.left(6), "hhmmss");

    // Parse position (fields 2, 3, 4, 10)
    QString latStr = fields[2]; QString latDir = fields[3];
    QString lonStr = fields[4]; QString lonDir = fields[5];
    double latitude = calculateCoordinates(latStr, latDir);
    double longitude = calculateCoordinates(lonStr, lonDir);
    if (std::isnan(latitude) || std::isnan(longitude))
        return;

    // Parse fix quality (field 6)
    int fixQuality = fields[6].toInt();

    // Parse number of satellites (field 7)
    int numSatellites = fields[7].toInt();

    // Parse HDOP (field 8)
    double hdop = fields[8].toDouble();

    // Parse altitude and units (fields 9s)
    double altitude = fields[9].toDouble();

    //Calculate frequency
    double freqHz = calculateFrequency(timer_gga, lastUpdateTimeGGA);

    emit newDecodedGGA(utcTime.toString(), latitude, longitude, fixQuality, numSatellites, hdop, altitude, freqHz);
}

//RMC
void NMEA_Handler::handleRMC(const QList<QByteArray> &fields)
{
    checkMinimumLineSize(fields, 12);

    // Parse time
    QString timeStr = fields[1];
    QTime utcTime = QTime::fromString(timeStr.left(6), "hhmmss");

    // Status: A=active, V=void
    QChar status = fields[2][0];
    if (status != 'A')
        return;

    // Parse latitude
    double latitude = calculateCoordinates(fields[3], fields[4]);

    // Parse longitude
    double longitude = calculateCoordinates(fields[5], fields[6]);

    // Speed over ground (knots) and convert to m/s
    double speedKnots = fields[7].toDouble();
    double speedMps = speedKnots * 0.514444;

    // Course over ground
    double course = fields[8].toDouble();

    // Parse date
    QString dateStr = fields[9];
    QDate utcDate = QDate::fromString(dateStr, "ddMMyy");
    if (utcDate.isValid()) {
        int year = utcDate.year();
        if (year < 2000)
            utcDate = utcDate.addYears(100);
    }
    QString formattedDate = utcDate.toString("dd/MM/yyyy");

    // Magnetic variation
    double magVar = fields[10].toDouble();
    if (!fields[11].isEmpty())
    {
        QChar varDir = fields[11][0];
        if (varDir == 'W')
            magVar = -magVar;
    }

    //Freq
    double freqHz = calculateFrequency(timer_rmc, lastUpdateTimeRMC);

    // Emit or process parsed data
    emit newDecodedRMC(formattedDate, utcTime.toString(), latitude, longitude, speedMps, course, magVar, freqHz);
}


//GSV
void NMEA_Handler::handleGSV(const QList<QByteArray> &fields)
{
    //Check size
    checkMinimumLineSize(fields, 4);

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
    checkMinimumLineSize(fields, 7);

    // Parse position
    QString latStr = fields[1]; QString latDir = fields[2];
    QString lonStr = fields[3]; QString lonDir = fields[4];
    double latitude = calculateCoordinates(latStr, latDir);
    double longitude = calculateCoordinates(lonStr, lonDir);
    if (std::isnan(latitude) || std::isnan(longitude))
        return;

    // Parse UTC time
    QString timeStr = fields[5];
    QTime utcTime = QTime::fromString(timeStr.left(6), "hhmmss");

    // Status and mode
    QChar status = fields[6][0];
    //QChar mode = fields.size() > 7 ? fields[7][0] : QChar('N');

    //Calculate frequency
    double freqHz = calculateFrequency(timer_gll, lastUpdateTimeGLL);

    if (status == 'A')
        emit newDecodedGLL(utcTime.toString(), latitude, longitude, freqHz);
}

//GSA
void NMEA_Handler::handleGSA(const QList<QByteArray> &fields)
{
    checkMinimumLineSize(fields, 18);

    double pdop = fields[15].toDouble();
    double hdop = fields[16].toDouble();
    double vdop = removeAsterisk(fields[17]);

    double freqHz = calculateFrequency(timer_gsa, lastUpdateTimeGSA);
    emit newDecodedGSA(pdop, hdop, vdop, freqHz);
}

//VTG
void NMEA_Handler::handleVTG(const QList<QByteArray> &fields)
{
    //checkMinimumLineSize(fields, 10); //bug ocpn, smaller sentence

    // Track True
    bool okTrackTrue = false;
    double trackTrue = fields[1].toDouble(&okTrackTrue);

    // Track Magnetic
    bool okTrackMag = false;
    double trackMag = fields[3].toDouble(&okTrackMag);

    // Speed Knots
    bool okSpeedKnots = false;
    QByteArray speedKnotsStr = fields[5];
    if (fields.size() < 10)
    {
        if (speedKnotsStr.endsWith('N')) //bug on OCPN
            speedKnotsStr.chop(1);
    }
    double speedKnots = speedKnotsStr.toDouble(&okSpeedKnots);

    // Parse Speed Km/h
    bool okSpeedKmh = false;
    double speedKmh = fields[7].toDouble(&okSpeedKmh);

    //Calculate frequency
    double freqHz = calculateFrequency(timer_vtg, lastUpdateTimeVTG);

    emit newDecodedVTG(trackTrue, trackMag, speedKnots, speedKmh, freqHz);
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

double NMEA_Handler::removeAsterisk(const QByteArray lastField)
{
    QString strStripped = QString::fromLatin1(lastField).split('*').first();
    double value = strStripped.toDouble();
    return value;
}

void NMEA_Handler::checkMinimumLineSize(const QList<QByteArray> &fields, int minSize)
{
    if (fields.size() < minSize)
        return;
}
