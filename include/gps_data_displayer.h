#ifndef GPS_DATA_DISPLAYER_H
#define GPS_DATA_DISPLAYER_H

#include <QMainWindow>
#include <QTcpSocket>

#include "serial_reader.h"
#include "nmea_handler.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class GPS_Data_Displayer;
}
QT_END_NAMESPACE

class GPS_Data_Displayer : public QMainWindow
{
    Q_OBJECT

    public:
        GPS_Data_Displayer(QWidget *parent = nullptr);
        ~GPS_Data_Displayer();

    private:
        Ui::GPS_Data_Displayer *ui;
        SerialReader *serial;
        NMEA_Handler *nmea_handler;

    private:
        void closeSerial();
        void clearRawSentencesScreens();
        void listAvailablePorts();
        void connectSignalSlot();

    private slots:
        void on_pushButton_clear_raw_sentences_screens_clicked();
        void on_pushButton_connect_clicked();
        void on_pushButton_disconnect_clicked();
        void on_pushButton_refresh_available_ports_list_clicked();

    public slots:
        void displayTXTSentence(const QString line);
        void displayGGASentence(const QString line);
        void displayRMCSentence(const QString line);
        void displayGSVSentence(const QString line);
        void displayGLLSentence(const QString line);
        void displayGSASentence(const QString line);
        void displayVTGSentence(const QString line);
        void displayOtherSentence(const QString line);

        void updateSatellitesInView(int totalSatellites);
        void updatePosition(double latitude, double longitude);

        void updateGsvFrequency(double frequency);
};

#endif // GPS_DATA_DISPLAYER_H
