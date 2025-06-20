#ifndef GPS_DATA_DISPLAYER_H
#define GPS_DATA_DISPLAYER_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QUdpSocket>

#include "ui_gps_data_displayer.h"

#include "serial_reader.h"
#include "nmea_handler.h"
#include "udp_sender.h"

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
        UdpSender *udp_sender;

    private:
        void closeSerial();
        void clearRawSentencesScreens();
        void listAvailablePorts();
        void hideGUI();
        void connectSignalSlot();

    private slots:
        void on_pushButton_clear_raw_sentences_screens_clicked();
        void on_pushButton_connect_clicked();
        void on_pushButton_disconnect_clicked();
        void on_pushButton_refresh_available_ports_list_clicked();
        void on_spinBox_update_udp_port_valueChanged(int port);

        void on_checkBox_udp_output_gga_toggled(bool checked);
        void on_checkBox_udp_output_rmc_toggled(bool checked);
        void on_checkBox_udp_output_gsv_toggled(bool checked);
        void on_checkBox_udp_output_gll_toggled(bool checked);
        void on_checkBox_udp_output_gsa_toggled(bool checked);
        void on_checkBox_udp_output_vtg_toggled(bool checked);
        void on_pushButton_activate_udp_output_toggled(bool checked);
        void on_checkBox_udp_output_others_toggled(bool checked);

        void on_pushButton_check_all_udp_output_clicked();
        void on_pushButtonuncheck_all_udp_output_clicked();

        void on_comboBox_udp_host_address_currentTextChanged(const QString &arg1);
        void on_lineEdit_udp_ip_address_editingFinished();


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
