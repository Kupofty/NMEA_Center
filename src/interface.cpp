#include "interface.h"

/////////////
/// CLASS ///
/////////////
Interface::Interface(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::Interface),
    serial_reader(new SerialReader),
    serial_writer(new SerialWriter),
    nmea_handler(new NMEA_Handler),
    udp_reader(new UdpReader),
    udp_writer(new UdpWriter),
    text_file_writer(new TextFileWritter)
{
    //Setup UI
    ui->setupUi(this);
    ui->tabWidget->setCurrentWidget(ui->tab_connection);
    this->showMaximized();

    //Hide widgets
    hideGUI();

    //Setup GPS GUI
    listAvailablePorts(ui->comboBox_serial_input_port_list);

    //Qt connects
    connectSignalSlot();

}

Interface::~Interface()
{
    delete fileRecordingSizeTimer;

    delete serial_reader;
    delete nmea_handler;
    delete udp_writer;
    delete text_file_writer;
    delete ui;
}

void Interface::connectSignalSlot()
{
    //// INPUT /////
    //Serial data
    QObject::connect(serial_reader, &SerialReader::newLineReceived, nmea_handler, &NMEA_Handler::handleRawSentences);
    QObject::connect(serial_reader, &SerialReader::newLineReceived, text_file_writer, &TextFileWritter::writeRawSentences);

    //UDP data
    QObject::connect(udp_reader, &UdpReader::newLineReceived, nmea_handler, &NMEA_Handler::handleRawSentences);
    QObject::connect(udp_reader, &UdpReader::newSenderDetails, this, &Interface::updateUdpSenderDetails);

    //// OUTPUT /////
    //Send raw NMEA to UDP publisher
    QObject::connect(nmea_handler, &NMEA_Handler::newGGASentence, udp_writer, &UdpWriter::publishGGA);
    QObject::connect(nmea_handler, &NMEA_Handler::newRMCSentence, udp_writer, &UdpWriter::publishRMC);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSVSentence, udp_writer, &UdpWriter::publishGSV);
    QObject::connect(nmea_handler, &NMEA_Handler::newGLLSentence, udp_writer, &UdpWriter::publishGLL);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSASentence, udp_writer, &UdpWriter::publishGSA);
    QObject::connect(nmea_handler, &NMEA_Handler::newVTGSentence, udp_writer, &UdpWriter::publishVTG);
    QObject::connect(nmea_handler, &NMEA_Handler::newHDTSentence, udp_writer, &UdpWriter::publishHDT);
    QObject::connect(nmea_handler, &NMEA_Handler::newDBTSentence, udp_writer, &UdpWriter::publishDBT);
    QObject::connect(nmea_handler, &NMEA_Handler::newVHWSentence, udp_writer, &UdpWriter::publishVHW);
    QObject::connect(nmea_handler, &NMEA_Handler::newOtherSentence, udp_writer, &UdpWriter::publishOthers);

    //Send raw NMEA to serial publisher
    QObject::connect(nmea_handler, &NMEA_Handler::newGGASentence, serial_writer, &SerialWriter::publishGGA);
    QObject::connect(nmea_handler, &NMEA_Handler::newRMCSentence, serial_writer, &SerialWriter::publishRMC);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSVSentence, serial_writer, &SerialWriter::publishGSV);
    QObject::connect(nmea_handler, &NMEA_Handler::newGLLSentence, serial_writer, &SerialWriter::publishGLL);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSASentence, serial_writer, &SerialWriter::publishGSA);
    QObject::connect(nmea_handler, &NMEA_Handler::newVTGSentence, serial_writer, &SerialWriter::publishVTG);
    QObject::connect(nmea_handler, &NMEA_Handler::newHDTSentence, serial_writer, &SerialWriter::publishHDT);
    QObject::connect(nmea_handler, &NMEA_Handler::newDBTSentence, serial_writer, &SerialWriter::publishDBT);
    QObject::connect(nmea_handler, &NMEA_Handler::newVHWSentence, serial_writer, &SerialWriter::publishVHW);
    QObject::connect(nmea_handler, &NMEA_Handler::newOtherSentence, serial_writer, &SerialWriter::publishOthers);


    //// DISPLAY /////

    //GUI
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &Interface::scrollDownPlainText);

    //Display raw NMEA from nmea_handler
    QObject::connect(nmea_handler, &NMEA_Handler::newNMEASentence, this, &Interface::displayRawNmeaSentence);

    //Display decoded NMEA data
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGGA, this, &Interface::updateDataGGA);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGLL, this, &Interface::updateDataGLL);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGSV, this, &Interface::updateDataGSV);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedVTG, this, &Interface::updateDataVTG);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGSA, this, &Interface::updateDataGSA);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedRMC, this, &Interface::updateDataRMC);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedHDT, this, &Interface::updateDataHDT);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedDBT, this, &Interface::updateDataDBT);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedVHW, this, &Interface::updateDataVHW);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedZDA, this, &Interface::updateDataZDA);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedDPT, this, &Interface::updateDataDPT);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedMWD, this, &Interface::updateDataMWD);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedMTW, this, &Interface::updateDataMTW);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedMWV, this, &Interface::updateDataMWV);

    //Timers
    fileRecordingSizeTimer = new QTimer(this);
    connect(fileRecordingSizeTimer, &QTimer::timeout, this, &Interface::updateFileSize);
}



///////////
/// GUI ///
///////////
void Interface::hideGUI()
{
    ui->horizontalFrame_udp_ip_address->hide();
}




////////////////////
/// Serial Input ///
////////////////////

// Connection
void Interface::closeInputSerial()
{
    if(serial_reader->isSerialOpen())
    {
        serial_reader->closeSerialDevice();
        ui->plainTextEdit_connection_status->setPlainText(serial_reader->getPortName()+" closed");
    }
    else
        ui->plainTextEdit_connection_status->setPlainText("Connection not opened");
}

void Interface::on_pushButton_connect_serial_input_clicked()
{
    //Update serial settings
    serial_reader->setPortName(ui->comboBox_serial_input_port_list->currentText());
    serial_reader->setBaudRate((ui->comboBox_serial_input_port_baudrate->currentText()).toInt());

    //Try to connect
    QString result;
    if(serial_reader->openSerialDevice())
    {
        result =  "Connected to " + serial_reader->getPortName();
        ui->plainTextEdit_txt->clear();
        updateGuiAfterSerialConnection(true);
    }
    else
        result =  "Failed to open " + serial_reader->getPortName() + " : " + serial_reader->getErrorString();

    //Display connection status
    ui->plainTextEdit_connection_status->setPlainText(result);
}

void Interface::on_pushButton_disconnect_serial_input_clicked()
{
    closeInputSerial();
    updateGuiAfterSerialConnection(false);
}

void Interface::updateGuiAfterSerialConnection(bool connectSuccess)
{
    ui->horizontalFrame_serial_input_connection->setEnabled(!connectSuccess);
    ui->pushButton_connect_serial_input->setEnabled(!connectSuccess);
    ui->pushButton_disconnect_serial_input->setEnabled(connectSuccess);
    clearDecodedDataScreens();
}


//COM ports
void Interface::listAvailablePorts(QComboBox *comboBox)
{
    comboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports)
    {
        comboBox->addItem(port.portName());
    }
}

void Interface::on_pushButton_refresh_available_ports_list_clicked()
{
    listAvailablePorts(ui->comboBox_serial_input_port_list);
}



/////////////////
/// UDP Input ///
/////////////////
void Interface::on_pushButton_connect_udp_input_clicked()
{
    int udp_port_input = ui->spinBox_port_input_udp->value();
    int udp_port_output = ui->spinBox_update_udp_port_output->value();

    //Check if port already used by UDP output
    if (ui->pushButton_activate_udp_output->isChecked() && (udp_port_input == udp_port_output))
    {
        QMessageBox::warning(this, "UDP Port Error",
                             "Input UDP port conflicts with output UDP port.\nPlease choose a different port.");
        ui->spinBox_port_input_udp->setValue(ui->spinBox_port_input_udp->value() + 1);
        return;
    }

    udp_reader->updatePort(udp_port_input);
    QString result = udp_reader->connect();
    ui->plainTextEdit_connection_status_udp->setPlainText(result);

    if(udp_reader->isBounded())
        updateGuiAfterUdpConnection(true);
}

void Interface::on_pushButton_disconnect_udp_input_clicked()
{
    CloseInputUdp();
    updateGuiAfterUdpConnection(false);
}

void Interface::CloseInputUdp()
{
    QString result = udp_reader->disconnect();
    ui->plainTextEdit_connection_status_udp->setPlainText(result);
}

void Interface::updateGuiAfterUdpConnection(bool connectSuccess)
{
    ui->spinBox_port_input_udp->setEnabled(!connectSuccess);
    ui->pushButton_connect_udp_input->setEnabled(!connectSuccess);
    ui->pushButton_disconnect_udp_input->setEnabled(connectSuccess);
    ui->plainTextEdit_udp_sender_details->clear();
    clearDecodedDataScreens();
}

void Interface::updateUdpSenderDetails()
{
    ui->plainTextEdit_udp_sender_details->setPlainText(udp_reader->getSenderDetails());
}



///////////////////////////
/// Raw Nmea Sentences  ///
///////////////////////////

//Display On Screens
void Interface::displayRawNmeaSentence(const QString &type, const QString &nmeaText)
{
    //Link NMEA to plainText screens
    QMap<QString, QPlainTextEdit*> nmeaSentenceMap = getSentenceMap();

    // Always show TXT regardless of freeze
    if (type == "TXT")
        ui->plainTextEdit_txt->appendPlainText(nmeaText);

    // Don't display sentences if freeze button is checked
    if (ui->pushButton_freeze_raw_sentences_screens->isChecked())
        return;

    if (type != "TXT" && nmeaSentenceMap.contains(type))
        nmeaSentenceMap[type]->appendPlainText(nmeaText);
}

QMap<QString, QPlainTextEdit*> Interface::getSentenceMap() const
{
    static const QStringList keys = {
        "OTHER", "GGA", "RMC", "GSV", "GLL",
        "GSA", "VTG", "DBT", "VHW", "HDT",
        "DPT", "MWD", "ZDA", "MTW", "MWV"
    };

    QList<QPlainTextEdit*> editors = getPlainTextEditors();
    QMap<QString, QPlainTextEdit*> map;

    for (int i = 0; i < keys.size() && i < editors.size(); ++i)
        map.insert(keys[i], editors[i]);

    return map;
}


//Clear Screens
void Interface::clearRawSentencesScreens()
{
    const QList<QPlainTextEdit*> &editors = getPlainTextEditors();

    for (QPlainTextEdit* editor : editors)
        editor->clear();
}

void Interface::on_pushButton_clear_raw_sentences_screens_clicked()
{
    clearRawSentencesScreens();
}


// Scroll screens
void Interface::scrollDownPlainText(int index)
{
    //Scroll down all the screens when changing to the raw data tab
    if(index == ui->tabWidget->indexOf(ui->tab_gps_raw_data))
    {
        const QList<QPlainTextEdit*> &editors = getPlainTextEditors();

        for (QPlainTextEdit* editor : editors)
            editor->verticalScrollBar()->setValue(editor->verticalScrollBar()->maximum());
    }
}


//Get list of plainTexts
QList<QPlainTextEdit*> Interface::getPlainTextEditors() const
{
    return
    {
        ui->plainTextEdit_others,
        ui->plainTextEdit_gga,
        ui->plainTextEdit_rmc,
        ui->plainTextEdit_gsv,
        ui->plainTextEdit_gll,
        ui->plainTextEdit_gsa,
        ui->plainTextEdit_vtg,
        ui->plainTextEdit_dbt,
        ui->plainTextEdit_vhw,
        ui->plainTextEdit_hdt,
        ui->plainTextEdit_dpt,
        ui->plainTextEdit_mwd,
        ui->plainTextEdit_zda,
        ui->plainTextEdit_mtw,
        ui->plainTextEdit_mwv
    };
}



//////////////////////////////////
/// Display Decoded NMEA data  ///
//////////////////////////////////

//Data
void Interface::updateDataGGA(QString time, double latitude, double longitude, int fixQuality, int numSatellites, double hdop, double altitude, double freqHz)
{
    ui->label_utcTime_gga->setText(time);
    ui->lcdNumber_latitude_gga->display(latitude);
    ui->lcdNumber_longitude_gga->display(longitude);
    ui->lcdNumber_fixQuality_gga->display(fixQuality);
    ui->lcdNumber_satellites_gga->display(numSatellites);
    ui->lcdNumber_hdop_gga->display(hdop);
    ui->lcdNumber_altitude_gga->display(altitude);
    ui->lcdNumber_frequency_gga->display(freqHz);
}

void Interface::updateDataGLL(QString utc, double latitude, double longitude, double freqHz)
{
    ui->label_utcTime_gll->setText(utc);
    ui->lcdNumber_latitude_gll->display(latitude);
    ui->lcdNumber_longitude_gll->display(longitude);
    ui->lcdNumber_frequency_gll->display(freqHz);
}

void Interface::updateDataGSV(int satellitesInView, double frequency)
{
    ui->lcdNumber_satellites_gsv->display(satellitesInView);
    ui->lcdNumber_frequency_gsv->display(frequency);
}

void Interface::updateDataVTG(double track_true, double track_mag, double speed_knot, double speedKmh, double frequency)
{
    ui->lcdNumber_track_true_vtg->display(track_true);
    ui->lcdNumber_track_mag_vtg->display(track_mag);
    ui->lcdNumber_speed_knot_vtg->display(speed_knot);
    ui->lcdNumber_speed_kmh_vtg->display(speedKmh);
    ui->lcdNumber_frequency_vtg->display(frequency);
}

void Interface::updateDataGSA(double pdop, double hdop, double vdop, double freqHz)
{
    ui->lcdNumber_pdop_gsa->display(pdop);
    ui->lcdNumber_hdop_gsa->display(hdop);
    ui->lcdNumber_vdop_gsa->display(vdop);
    ui->lcdNumber_frequency_gsa->display(freqHz);
}

void Interface::updateDataRMC(QString utcDate, QString utcTime, double latitude, double longitude, double speedMps, double course, double magVar, double freqHz)
{
    ui->label_date_rmc->setText(utcDate);
    ui->label_utcTime_rmc->setText(utcTime);
    ui->lcdNumber_latitude_rmc->display(latitude);
    ui->lcdNumber_longitude_rmc->display(longitude);
    ui->lcdNumber_sog_rmc->display(speedMps);
    ui->lcdNumber_cog_rmc->display(course);
    ui->lcdNumber_magVar_rmc->display(magVar);
    ui->lcdNumber_frequency_rmc->display(freqHz);

}

void Interface::updateDataHDT(double heading, double freqHz)
{
    ui->lcdNumber_heading_hdt->display(heading);
    ui->lcdNumber_frequency_hdt->display(freqHz);
}

void Interface::updateDataDBT(double depthFeet, double depthMeters, double depthFathom, double freqHz)
{
    ui->lcdNumber_depth_feet_dbt->display(depthFeet);
    ui->lcdNumber_depth_meter_dbt->display(depthMeters);
    ui->lcdNumber_depth_fathom_dbt->display(depthFathom);
    ui->lcdNumber_frequency_dbt->display(freqHz);
}

void Interface::updateDataVHW(double headingTrue, double headingMag, double speedKnots, double speedKmh, double freqHz)
{
    ui->lcdNumber_heading_true_vhw->display(headingTrue);
    ui->lcdNumber_heading_mag_vhw->display(headingMag);
    ui->lcdNumber_speed_knot_vhw->display(speedKnots);
    ui->lcdNumber_speed_kmh_vhw->display(speedKmh);
    ui->lcdNumber_frequency_vhw->display(freqHz);
}

void Interface::updateDataZDA(QString date, QString time, QString offsetTime, double freqHz)
{
    ui->label_date_zda->setText(date);
    ui->label_utcTime_zda->setText(time);
    ui->label_localZone_zda->setText(offsetTime);
    ui->lcdNumber_frequency_zda->display(freqHz);
}

void Interface::updateDataDPT(double depth, double offset, double freqHz)
{
    ui->lcdNumber_depth_dpt->display(depth);
    ui->lcdNumber_depth_offset_dpt->display(offset);
    ui->lcdNumber_frequency_dpt->display(freqHz);
}

void Interface::updateDataMWD(double dir1, QString dir1Unit, double dir2, QString dir2Unit, double speed1, QString speed1Unit, double speed2, QString speed2Unit, double freqHz)
{
    ui->lcdNumber_windDirection_mwd->display(dir1);
    ui->label_windDirectionUnit_mwd->setText("Wind Direction (" + dir1Unit + ") :" );
    ui->lcdNumber_windDirection_mwd_2->display(dir2);
    ui->label_windDirectionUnit_mwd_2->setText("Wind Direction (" + dir2Unit + ") :" );
    ui->lcdNumber_windSpeed_mwd->display(speed1);
    ui->label_windSpeedUnit_mwd->setText("Wind Speed (" + speed1Unit + ") :" );
    ui->lcdNumber_windSpeed_mwd_2->display(speed2);
    ui->label_windSpeedUnit_mwd_2->setText("Wind Speed (" + speed2Unit + ") :" );
    ui->lcdNumber_frequency_mwd->display(freqHz);
}

void Interface::updateDataMTW(double temp, QString tempUnit, double freqHz)
{
    ui->lcdNumber_waterTemp_mtw->display(temp);
    ui->label_waterTempUnit_mtw->setText("Water Temp. (°" + tempUnit + ") :");
    ui->lcdNumber_frequency_mtw->display(freqHz);
}

void Interface::updateDataMWV(double angle, QString ref, double speed, QString unit, double freqHz)
{
    ui->lcdNumber_windAngle_mwv->display(angle);
    ui->label_windAngleUnit_mwv->setText("Wing Angle (" + ref + ") :" );
    ui->lcdNumber_windSpeed_mwv->display(speed);
    ui->label_windSpeedUnit_mwv->setText("Wing Speed (" + unit + ") :" );
    ui->lcdNumber_frequency_mwv->display(freqHz);
}


//Clear
void Interface::clearDecodedDataScreens()
{
    //RMC
    ui->label_utcTime_rmc->clear();
    ui->label_date_rmc->clear();
    ui->lcdNumber_latitude_rmc->display(0);
    ui->lcdNumber_longitude_rmc->display(0);
    ui->lcdNumber_sog_rmc->display(0);
    ui->lcdNumber_cog_rmc->display(0);
    ui->lcdNumber_magVar_rmc->display(0);
    ui->lcdNumber_frequency_rmc->display(0);

    //GGA
    ui->label_utcTime_gga->clear();
    ui->lcdNumber_latitude_gga->display(0);
    ui->lcdNumber_longitude_gga->display(0);
    ui->lcdNumber_satellites_gga->display(0);
    ui->lcdNumber_fixQuality_gga->display(0);
    ui->lcdNumber_hdop_gga->display(0);
    ui->lcdNumber_altitude_gga->display(0);
    ui->lcdNumber_frequency_gga->display(0);

    //GSA
    ui->lcdNumber_pdop_gsa->display(0);
    ui->lcdNumber_hdop_gsa->display(0);
    ui->lcdNumber_vdop_gsa->display(0);
    ui->lcdNumber_frequency_gsa->display(0);

    //GLL
    ui->lcdNumber_latitude_gll->display(0);
    ui->lcdNumber_longitude_gll->display(0);
    ui->lcdNumber_frequency_gll->display(0);
    ui->label_utcTime_gll->clear();

    //GSV
    ui->lcdNumber_satellites_gsv->display(0);
    ui->lcdNumber_frequency_gsv->display(0);

    //VTG
    ui->lcdNumber_track_true_vtg->display(0);
    ui->lcdNumber_track_mag_vtg->display(0);
    ui->lcdNumber_speed_kmh_vtg->display(0);
    ui->lcdNumber_speed_knot_vtg->display(0);
    ui->lcdNumber_frequency_vtg->display(0);

    //HDT
    ui->lcdNumber_heading_hdt->display(0);
    ui->lcdNumber_frequency_hdt->display(0);

    //DBT
    ui->lcdNumber_depth_feet_dbt->display(0);
    ui->lcdNumber_depth_meter_dbt->display(0);
    ui->lcdNumber_depth_fathom_dbt->display(0);
    ui->lcdNumber_frequency_dbt->display(0);

    //VHW
    ui->lcdNumber_heading_true_vhw->display(0);
    ui->lcdNumber_heading_mag_vhw->display(0);
    ui->lcdNumber_speed_knot_vhw->display(0);
    ui->lcdNumber_speed_kmh_vhw->display(0);
    ui->lcdNumber_frequency_vhw->display(0);
}




//////////////////////////
/// Serial Output Data ///
//////////////////////////

//Settings
void Interface::closeOutputSerial()
{
    if(serial_writer->isSerialOpen())
    {
        serial_writer->closeSerialDevice();
        ui->plainTextEdit_connection_status_output_serial->setPlainText(serial_writer->getPortName()+" closed");
    }
    else
        ui->plainTextEdit_connection_status_output_serial->setPlainText("Connection not opened");
}

void Interface::on_pushButton_refresh_available_port_serial_output_clicked()
{
    listAvailablePorts(ui->comboBox_serial_output_port_list);

    //Remove input serial as output choice
    if(serial_reader->isSerialOpen())
    {
        int indexToRemove = ui->comboBox_serial_output_port_list->findText(ui->comboBox_serial_input_port_list->currentText());
        if (indexToRemove != -1)
            ui->comboBox_serial_output_port_list->removeItem(indexToRemove);
    }
}

void Interface::on_pushButton_connect_serial_output_clicked()
{
    //Update serial settings
    serial_writer->setPortName(ui->comboBox_serial_output_port_list->currentText());
    serial_writer->setBaudRate((ui->comboBox_serial_output_port_baudrate->currentText()).toInt());

    //Try to connect
    QString result;
    if(serial_writer->openSerialDevice())
        result =  "Connected to " + serial_writer->getPortName();
    else
        result =  "Failed to open " + serial_writer->getPortName() + " : " + serial_writer->getErrorString();

    //Display connection status
    ui->plainTextEdit_connection_status_output_serial->setPlainText(result);
}

void Interface::on_pushButton_disconnect_serial_output_clicked()
{
    closeOutputSerial();
}


//Outputs
void Interface::on_pushButton_activate_serial_output_toggled(bool checked)
{
    if (!checked)
    {
        serial_writer->updateOutputNMEA(false);
        return;
    }

    if (serial_writer->isSerialOpen())
    {
        serial_writer->updateOutputNMEA(true);
    }
    else
    {
        QMessageBox::warning(this, "Serial Output Not Available",
                             "No serial output port is currently opened.\n\n"
                             "Please select a valid port and click 'Connect' before enabling serial output.");
        ui->pushButton_activate_serial_output->setChecked(false);
    }
}

void Interface::on_checkBox_serial_output_gga_toggled(bool checked)
{
    serial_writer->updateOutputGGA(checked);
}

void Interface::on_checkBox_serial_output_gsv_toggled(bool checked)
{
    serial_writer->updateOutputGSV(checked);
}

void Interface::on_checkBox_serial_output_rmc_toggled(bool checked)
{
    serial_writer->updateOutputRMC(checked);
}

void Interface::on_checkBox_serial_output_gsa_toggled(bool checked)
{
    serial_writer->updateOutputGSA(checked);
}

void Interface::on_checkBox_serial_output_gll_toggled(bool checked)
{
    serial_writer->updateOutputGLL(checked);
}

void Interface::on_checkBox_serial_output_vtg_toggled(bool checked)
{
    serial_writer->updateOutputVTG(checked);
}

void Interface::on_checkBox_serial_output_hdt_toggled(bool checked)
{
    serial_writer->updateOutputHDT(checked);
}

void Interface::on_checkBox_serial_output_dbt_toggled(bool checked)
{
    serial_writer->updateOutputDBT(checked);
}

void Interface::on_checkBox_serial_output_vhw_toggled(bool checked)
{
    serial_writer->updateOutputVHW(checked);
}

void Interface::on_checkBox_serial_output_others_toggled(bool checked)
{
    serial_writer->updateOutputOthers(checked);
}

void Interface::on_pushButton_check_all_serial_output_clicked()
{
    ui->checkBox_serial_output_gga->setChecked(true);
    ui->checkBox_serial_output_rmc->setChecked(true);
    ui->checkBox_serial_output_gsv->setChecked(true);
    ui->checkBox_serial_output_gll->setChecked(true);
    ui->checkBox_serial_output_gsa->setChecked(true);
    ui->checkBox_serial_output_vtg->setChecked(true);
    ui->checkBox_serial_output_hdt->setChecked(true);
    ui->checkBox_serial_output_dbt->setChecked(true);
    ui->checkBox_serial_output_vhw->setChecked(true);
    ui->checkBox_serial_output_others->setChecked(true);
}

void Interface::on_pushButton_uncheck_all_serial_output_clicked()
{
    ui->checkBox_serial_output_gga->setChecked(false);
    ui->checkBox_serial_output_rmc->setChecked(false);
    ui->checkBox_serial_output_gsv->setChecked(false);
    ui->checkBox_serial_output_gll->setChecked(false);
    ui->checkBox_serial_output_gsa->setChecked(false);
    ui->checkBox_serial_output_vtg->setChecked(false);
    ui->checkBox_serial_output_hdt->setChecked(false);
    ui->checkBox_serial_output_dbt->setChecked(false);
    ui->checkBox_serial_output_vhw->setChecked(false);
    ui->checkBox_serial_output_others->setChecked(false);
}




///////////////////////
/// UDP Output Data ///
///////////////////////

//UDP Settings
void Interface::on_spinBox_update_udp_port_output_valueChanged(int udp_port)
{
    if(checkUdpOutputPortIsFree())
        udp_writer->updateUdpPort(udp_port);
}

void Interface::on_comboBox_udp_host_address_currentTextChanged(const QString &udpMethod)
{
    if(udpMethod == "Broadcast")
    {
        udp_writer->updateUdpMethod(QHostAddress::Broadcast);
        ui->horizontalFrame_udp_ip_address->hide();
    }
    else if(udpMethod == "Unicast" || udpMethod == "Multicast")
    {
        emit ui->lineEdit_udp_ip_address->editingFinished();
        ui->horizontalFrame_udp_ip_address->show();
    }
}

void Interface::on_lineEdit_udp_ip_address_editingFinished()
{
    udp_writer->updateUdpMethod(QHostAddress(ui->lineEdit_udp_ip_address->text()));
}

bool Interface::checkUdpOutputPortIsFree()
{
    int udp_input_port = ui->spinBox_port_input_udp->value();
    int udp_output_port = ui->spinBox_update_udp_port_output->value();

    //Check if port already used by UDP input
    if (udp_reader->isBounded() && (udp_output_port == udp_input_port) )
    {
        QMessageBox::warning(this, "UDP Port Error", "Output UDP port conflicts with input UDP port.\nPlease choose a different port.");
        ui->pushButton_activate_udp_output->setChecked(false);
        return false;
    }
    else
        return true;
}


//Check data to outpout
void Interface::on_pushButton_activate_udp_output_toggled(bool checked)
{
    if(checked)
        if(checkUdpOutputPortIsFree())
            udp_writer->updateOutputNMEA(checked);
}

void Interface::on_checkBox_udp_output_gga_toggled(bool checked)
{
    udp_writer->updateOutputGGA(checked);
}

void Interface::on_checkBox_udp_output_rmc_toggled(bool checked)
{
    udp_writer->updateOutputRMC(checked);
}

void Interface::on_checkBox_udp_output_gsv_toggled(bool checked)
{
    udp_writer->updateOutputGSV(checked);
}

void Interface::on_checkBox_udp_output_gll_toggled(bool checked)
{
    udp_writer->updateOutputGLL(checked);
}

void Interface::on_checkBox_udp_output_gsa_toggled(bool checked)
{
    udp_writer->updateOutputGSA(checked);
}

void Interface::on_checkBox_udp_output_vtg_toggled(bool checked)
{
    udp_writer->updateOutputVTG(checked);
}

void Interface::on_checkBox_udp_output_hdt_toggled(bool checked)
{
    udp_writer->updateOutputHDT(checked);
}

void Interface::on_checkBox_udp_output_dbt_toggled(bool checked)
{
    udp_writer->updateOutputDBT(checked);
}

void Interface::on_checkBox_udp_output_vhw_toggled(bool checked)
{
    udp_writer->updateOutputVHW(checked);
}

void Interface::on_checkBox_udp_output_others_toggled(bool checked)
{
    udp_writer->updateOutputOthers(checked);
}

void Interface::on_pushButton_check_all_udp_output_clicked()
{
    ui->checkBox_udp_output_gga->setChecked(true);
    ui->checkBox_udp_output_rmc->setChecked(true);
    ui->checkBox_udp_output_gsv->setChecked(true);
    ui->checkBox_udp_output_gll->setChecked(true);
    ui->checkBox_udp_output_gsa->setChecked(true);
    ui->checkBox_udp_output_vtg->setChecked(true);
    ui->checkBox_udp_output_hdt->setChecked(true);
    ui->checkBox_udp_output_dbt->setChecked(true);
    ui->checkBox_udp_output_vhw->setChecked(true);
    ui->checkBox_udp_output_others->setChecked(true);
}

void Interface::on_pushButton_uncheck_all_udp_output_clicked()
{
    ui->checkBox_udp_output_gga->setChecked(false);
    ui->checkBox_udp_output_rmc->setChecked(false);
    ui->checkBox_udp_output_gsv->setChecked(false);
    ui->checkBox_udp_output_gll->setChecked(false);
    ui->checkBox_udp_output_gsa->setChecked(false);
    ui->checkBox_udp_output_vtg->setChecked(false);
    ui->checkBox_udp_output_hdt->setChecked(false);
    ui->checkBox_udp_output_dbt->setChecked(false);
    ui->checkBox_udp_output_vhw->setChecked(false);
    ui->checkBox_udp_output_others->setChecked(false);
}



/////////////////////
/// Save TXT File ///
/////////////////////
void Interface::on_pushButton_folder_path_documents_clicked()
{
    ui->plainTextEdit_txt_file_path->setPlainText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
}

void Interface::on_pushButton_folder_path_downloads_clicked()
{
    ui->plainTextEdit_txt_file_path->setPlainText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

void Interface::on_pushButton_browse_folder_path_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Output Directory",
                                                        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


    if (!dirPath.isEmpty())
        ui->plainTextEdit_txt_file_path->setPlainText(dirPath);
}

void Interface::on_pushButton_automatic_txt_file_name_clicked()
{
    QString automaticFileName = "Log_NMEA_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    ui->plainTextEdit_txt_file_name->setPlainText(automaticFileName);
}

void Interface::on_pushButton_save_txt_file_toggled(bool checked)
{
    if(checked)
    {
        QString dirPath = ui->plainTextEdit_txt_file_path->toPlainText().trimmed();
        QString fileName = ui->plainTextEdit_txt_file_name->toPlainText().trimmed();

        if (dirPath.isEmpty() || fileName.isEmpty()) {
            QMessageBox::warning(this, "Missing Information", "Please select an output folder and enter a file name before saving.");
            ui->pushButton_save_txt_file->setChecked(false);
            return;
        }

        int result = text_file_writer->createFile(getRecordingFilePath());
        if(!result)
            return;

        fileRecordingSizeTimer->start(1000);
        ui->pushButton_save_txt_file->setText(" Stop Recording");
    }
    else
    {
        fileRecordingSizeTimer->stop();
        ui->label_file_txt_size->setText("Not recording");
        text_file_writer->closeFile();
        ui->pushButton_save_txt_file->setText(" Record Data To File");
    }

}

void Interface::updateFileSize()
{
    QFile file(getRecordingFilePath());
    if (file.exists())
    {
        qint64 size = file.size();
        ui->label_file_txt_size->setText(QString("%1 Kb").arg(static_cast<int>(std::round(size / 1000.0))));
    }
    else
    {
        ui->label_file_txt_size->setText("File missing");
    }
}

QString Interface::getRecordingFilePath()
{
    QString dirPath = ui->plainTextEdit_txt_file_path->toPlainText().trimmed();
    QString fileName = ui->plainTextEdit_txt_file_name->toPlainText().trimmed();
    QString fileExtension = ui->comboBox_txt_file_extension->currentText();
    QString fullPath = QDir(dirPath).filePath(fileName + fileExtension);

    return fullPath;
}
