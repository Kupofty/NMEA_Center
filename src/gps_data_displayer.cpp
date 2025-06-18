#include "gps_data_displayer.h"
#include "ui_gps_data_displayer.h"

/////////////
/// CLASS ///
/////////////
GPS_Data_Displayer::GPS_Data_Displayer(QWidget *parent) : QMainWindow(parent), ui(new Ui::GPS_Data_Displayer), gps(new GPS_Reader)
{
    //Setup UI
    ui->setupUi(this);
    ui->tabWidget->setCurrentWidget(ui->tab_connection);
    this->showMaximized();

    //Setup GPS GUI
    listAvailablePorts();

    //Connects
    QObject::connect(gps, &GPS_Reader::newLineReceived, this, &GPS_Data_Displayer::handleGpsLine);

}

GPS_Data_Displayer::~GPS_Data_Displayer()
{
    closeGPS();
    delete ui;
}


/////////////////
/// FUNCTIONS ///
/////////////////
void GPS_Data_Displayer::handleGpsLine(const QByteArray line)
{
    QString nmeaText = QString::fromUtf8(line).trimmed();

    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
    {
        if(line.startsWith("$GPTXT"))
            ui->plainTextEdit_txt->appendPlainText(nmeaText);

        else if (line.startsWith("$GPGGA"))
            ui->plainTextEdit_gga->appendPlainText(nmeaText);

        else if(line.startsWith("$GPRMC"))
            ui->plainTextEdit_rmc->appendPlainText(nmeaText);

        else if(line.startsWith("$GPGSV"))
            ui->plainTextEdit_gsv->appendPlainText(nmeaText);

        else if(line.startsWith("$GPGLL"))
            ui->plainTextEdit_gll->appendPlainText(nmeaText);

        else if(line.startsWith("$GPGSA"))
            ui->plainTextEdit_gsa->appendPlainText(nmeaText);

        else if(line.startsWith("$GPVTG"))
            ui->plainTextEdit_vtg->appendPlainText(nmeaText);

        else
            ui->plainTextEdit_others->appendPlainText(nmeaText);
    }
}

void GPS_Data_Displayer::clearRawSentencesScreens()
{
    ui->plainTextEdit_gga->clear();
    ui->plainTextEdit_rmc->clear();
    ui->plainTextEdit_gsv->clear();
    ui->plainTextEdit_gll->clear();
    ui->plainTextEdit_gsa->clear();
    ui->plainTextEdit_vtg->clear();
}

void GPS_Data_Displayer::closeGPS()
{
    if(gps->isSerialOpen())
    {
        gps->closeSerialDevice();
        ui->plainTextEdit_connection_status->setPlainText("Connection closed");
    }
    else
        ui->plainTextEdit_connection_status->setPlainText("Connection not opened");
}

void GPS_Data_Displayer::listAvailablePorts()
{
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports)
    {
        ui->comboBox_available_ports->addItem(port.portName());
    }
}


//////////////////////
/// ON_PUSH_BUTTON ///
//////////////////////
void GPS_Data_Displayer::on_pushButton_clear_raw_sentences_screens_clicked()
{
    clearRawSentencesScreens();
}

void GPS_Data_Displayer::on_pushButton_connect_clicked()
{
    gps->setPortName(ui->comboBox_available_ports->currentText());

    QString result;
    if(gps->openSerialDevice())
    {
        result =  "Connected to " + gps->getPortName();
        ui->plainTextEdit_txt->clear();
        ui->tabWidget->setCurrentWidget(ui->tab_raw_text);
    }
    else
        result =  "Failed to open " + gps->getPortName() + " : " + gps->getErrorString();

    ui->plainTextEdit_connection_status->setPlainText(result);
}

void GPS_Data_Displayer::on_pushButton_disconnect_clicked()
{
    closeGPS();
}

void GPS_Data_Displayer::on_pushButton_refresh_available_ports_list_clicked()
{
    ui->comboBox_available_ports->clear();
    listAvailablePorts();
}

