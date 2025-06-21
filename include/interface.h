#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTimer>

#include "ui_interface.h"

#include "serial_reader.h"
#include "serial_writer.h"
#include "nmea_handler.h"
#include "udp_sender.h"
#include "text_file_writter.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Interface;
}
QT_END_NAMESPACE

class Interface : public QMainWindow
{
    Q_OBJECT

    public:
        Interface(QWidget *parent = nullptr);
        ~Interface();

    private:
        Ui::Interface *ui;
        SerialReader *serial;
        SerialWriter *serial_writer;
        NMEA_Handler *nmea_handler;
        UdpWriter *udp_writer;
        TextFileWritter *text_file_writer;
        QTimer *fileRecordingSizeTimer;

    private:
        void closeInputSerial();
        void closeOutputSerial();
        void clearRawSentencesScreens();
        void listAvailablePorts(QComboBox *comboBox);
        void hideGUI();
        void connectSignalSlot();
        void updateGuiAfterSerialConnection(bool connectSuccess);
        void updateFileSize();
        QString getRecordingFilePath();

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

        void on_comboBox_udp_host_address_currentTextChanged(const QString &hostAddress);
        void on_lineEdit_udp_ip_address_editingFinished();

        void on_pushButton_browse_folder_path_clicked();
        void on_pushButton_save_txt_file_toggled(bool checked);
        void on_pushButton_automatic_txt_file_name_clicked();
        void on_pushButton_folder_path_documents_clicked();
        void on_pushButton_folder_path_downloads_clicked();

        void on_pushButton_refresh_available_port_serial_output_clicked();
        void on_pushButton_connect_serial_output_clicked();
        void on_pushButton_disconnect_serial_output_clicked();
        void on_checkBox_serial_output_gga_toggled(bool checked);

        void on_pushButton_activate_serial_output_toggled(bool checked);
        void on_checkBox_serial_output_gsv_toggled(bool checked);
        void on_checkBox_serial_output_rmc_toggled(bool checked);
        void on_checkBox_serial_output_gsa_toggled(bool checked);
        void on_checkBox_serial_output_gll_toggled(bool checked);
        void on_checkBox_serial_output_vtg_toggled(bool checked);
        void on_checkBox_serial_output_others_toggled(bool checked);
        void on_pushButton_check_all_serial_output_clicked();
        void on_pushButtonuncheck_all_serial_output_clicked();

    public slots:
        void displayTXTSentence(const QString &line);
        void displayGGASentence(const QString &line);
        void displayRMCSentence(const QString &line);
        void displayGSVSentence(const QString &line);
        void displayGLLSentence(const QString &line);
        void displayGSASentence(const QString &line);
        void displayVTGSentence(const QString &line);
        void displayOtherSentence(const QString &line);

        void updateSatellitesInView(int totalSatellites);
        void updatePosition(double latitude, double longitude);

        void updateGsvFrequency(double frequency);
};

#endif // INTERFACE_H
