#ifndef GPS_DATA_DISPLAYER_H
#define GPS_DATA_DISPLAYER_H

#include <QMainWindow>
#include "gps_reader.h"

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
        GPS_Reader *gps;
        void closeGPS();
        void clearRawSentencesScreens();
        void listAvailablePorts();

    private slots:
        void handleGpsLine(const QByteArray line);
        void on_pushButton_clear_raw_sentences_screens_clicked();
        void on_pushButton_connect_clicked();
        void on_pushButton_disconnect_clicked();
        void on_pushButton_refresh_available_ports_list_clicked();
};

#endif // GPS_DATA_DISPLAYER_H
