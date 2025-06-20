#include "gps_data_displayer.h"
#include "ui_gps_data_displayer.h"

/////////////
/// CLASS ///
/////////////
GPS_Data_Displayer::GPS_Data_Displayer(QWidget *parent) : QMainWindow(parent), ui(new Ui::GPS_Data_Displayer), serial(new SerialReader), nmea_handler(new NMEA_Handler)
{
    //Setup UI
    ui->setupUi(this);
    ui->tabWidget->setCurrentWidget(ui->tab_connection);
    this->showMaximized();

    //Setup GPS GUI
    listAvailablePorts();

    //Qt connects
    connectSignalSlot();

}

GPS_Data_Displayer::~GPS_Data_Displayer()
{
    closeSerial();
    delete ui;
}

void GPS_Data_Displayer::connectSignalSlot()
{
    //Serial data
    QObject::connect(serial, &SerialReader::newLineReceived, nmea_handler, &NMEA_Handler::handleRawSentences);

    //Display raw NMEA from nmea_handler
    QObject::connect(nmea_handler, &NMEA_Handler::newTXTSentence, this, &GPS_Data_Displayer::displayTXTSentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newGGASentence, this, &GPS_Data_Displayer::displayGGASentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newRMCSentence, this, &GPS_Data_Displayer::displayRMCSentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSVSentence, this, &GPS_Data_Displayer::displayGSVSentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newGLLSentence, this, &GPS_Data_Displayer::displayGLLSentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSASentence, this, &GPS_Data_Displayer::displayGSASentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newVTGSentence, this, &GPS_Data_Displayer::displayVTGSentence);
    QObject::connect(nmea_handler, &NMEA_Handler::newOtherSentence, this, &GPS_Data_Displayer::displayOtherSentence);

    //Display decoded NMEA data
    QObject::connect(nmea_handler, &NMEA_Handler::newPosition, this, &GPS_Data_Displayer::updatePosition);
    QObject::connect(nmea_handler, &NMEA_Handler::newSatellitesInView, this, &GPS_Data_Displayer::updateSatellitesInView);

    //Display NMEA update frequency
    QObject::connect(nmea_handler, &NMEA_Handler::newGsvFrequency, this, &GPS_Data_Displayer::updateGsvFrequency);


}



/////////////////
/// FUNCTIONS ///
/////////////////
void GPS_Data_Displayer::clearRawSentencesScreens()
{
    ui->plainTextEdit_gga->clear();
    ui->plainTextEdit_rmc->clear();
    ui->plainTextEdit_gsv->clear();
    ui->plainTextEdit_gll->clear();
    ui->plainTextEdit_gsa->clear();
    ui->plainTextEdit_vtg->clear();
}

void GPS_Data_Displayer::closeSerial()
{
    if(serial->isSerialOpen())
    {
        serial->closeSerialDevice();
        ui->plainTextEdit_connection_status->setPlainText(serial->getPortName()+" closed");
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



////////////////////////
/// SLOTS UPDATE GUI ///
////////////////////////

// Raw Sentences
void GPS_Data_Displayer::displayTXTSentence(const QString nmeaText)
{
    ui->plainTextEdit_txt->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGGASentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gga->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayRMCSentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_rmc->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGSVSentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gsv->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGLLSentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gll->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGSASentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gsa->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayVTGSentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_vtg->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayOtherSentence(const QString nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_others->appendPlainText(nmeaText);
}


//Decoded NMEA
void GPS_Data_Displayer::updatePosition(double latitude, double longitude)
{
    ui->lcdNumber_latitude->display(latitude);
    ui->lcdNumber_longitude->display(longitude);
}

void GPS_Data_Displayer::updateSatellitesInView(int satellitesInView)
{
    ui->lcdNumber_satellitesInView->display(satellitesInView);
}


//Frequency
void GPS_Data_Displayer::updateGsvFrequency(double frequency)
{
    ui->lcdNumber_frequency_satellites->display(frequency);
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
    serial->setPortName(ui->comboBox_available_ports->currentText());

    QString result;
    if(serial->openSerialDevice())
    {
        result =  "Connected to " + serial->getPortName();
        ui->plainTextEdit_txt->clear();
        ui->tabWidget->setCurrentWidget(ui->tab_gps_decoded_data);
    }
    else
        result =  "Failed to open " + serial->getPortName() + " : " + serial->getErrorString();

    ui->plainTextEdit_connection_status->setPlainText(result);
}

void GPS_Data_Displayer::on_pushButton_disconnect_clicked()
{
    closeSerial();
}

void GPS_Data_Displayer::on_pushButton_refresh_available_ports_list_clicked()
{
    ui->comboBox_available_ports->clear();
    listAvailablePorts();
}







