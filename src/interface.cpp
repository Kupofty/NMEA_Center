#include "interface.h"

/////////////
/// CLASS ///
/////////////
Interface::Interface(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::Interface),
    serial(new SerialReader),
    serial_writer(new SerialWriter),
    nmea_handler(new NMEA_Handler),
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

    delete serial;
    delete nmea_handler;
    delete udp_writer;
    delete text_file_writer;
    delete ui;
}

void Interface::connectSignalSlot()
{
    //Serial data
    QObject::connect(serial, &SerialReader::newLineReceived, nmea_handler, &NMEA_Handler::handleRawSentences);
    QObject::connect(serial, &SerialReader::newLineReceived, text_file_writer, &TextFileWritter::writeRawSentences);

    //Send raw NMEA to UDP publisher
    QObject::connect(nmea_handler, &NMEA_Handler::newGGASentence, udp_writer, &UdpWriter::publishGGA);
    QObject::connect(nmea_handler, &NMEA_Handler::newRMCSentence, udp_writer, &UdpWriter::publishRMC);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSVSentence, udp_writer, &UdpWriter::publishGSV);
    QObject::connect(nmea_handler, &NMEA_Handler::newGLLSentence, udp_writer, &UdpWriter::publishGLL);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSASentence, udp_writer, &UdpWriter::publishGSA);
    QObject::connect(nmea_handler, &NMEA_Handler::newVTGSentence, udp_writer, &UdpWriter::publishVTG);
    QObject::connect(nmea_handler, &NMEA_Handler::newOtherSentence, udp_writer, &UdpWriter::publishOthers);

    //Send raw NMEA to serial publisher
    QObject::connect(nmea_handler, &NMEA_Handler::newGGASentence, serial_writer, &UdpWriter::publishGGA);
    QObject::connect(nmea_handler, &NMEA_Handler::newRMCSentence, serial_writer, &UdpWriter::publishRMC);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSVSentence, serial_writer, &UdpWriter::publishGSV);
    QObject::connect(nmea_handler, &NMEA_Handler::newGLLSentence, serial_writer, &UdpWriter::publishGLL);
    QObject::connect(nmea_handler, &NMEA_Handler::newGSASentence, serial_writer, &UdpWriter::publishGSA);
    QObject::connect(nmea_handler, &NMEA_Handler::newVTGSentence, serial_writer, &UdpWriter::publishVTG);
    QObject::connect(nmea_handler, &NMEA_Handler::newOtherSentence, serial_writer, &UdpWriter::publishOthers);

    //Display raw NMEA from nmea_handler
    QObject::connect(nmea_handler, &NMEA_Handler::newNMEASentence, this, &Interface::displayRawNmeaSentence);

    //Display decoded NMEA data
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGGA, this, &Interface::updateDataGGA);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGLL, this, &Interface::updateDataGLL);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGSV, this, &Interface::updateDataGSV);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedVTG, this, &Interface::updateDataVTG);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedGSA, this, &Interface::updateDataGSA);
    QObject::connect(nmea_handler, &NMEA_Handler::newDecodedRMC, this, &Interface::updateDataRMC);






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
    if(serial->isSerialOpen())
    {
        serial->closeSerialDevice();
        ui->plainTextEdit_connection_status->setPlainText(serial->getPortName()+" closed");
    }
    else
        ui->plainTextEdit_connection_status->setPlainText("Connection not opened");
}

void Interface::on_pushButton_connect_clicked()
{
    //Update serial settings
    serial->setPortName(ui->comboBox_serial_input_port_list->currentText());
    serial->setBaudRate((ui->comboBox_serial_input_port_baudrate->currentText()).toInt());

    //Try to connect
    QString result;
    if(serial->openSerialDevice())
    {
        result =  "Connected to " + serial->getPortName();
        ui->plainTextEdit_txt->clear();
        updateGuiAfterSerialConnection(true);
    }
    else
        result =  "Failed to open " + serial->getPortName() + " : " + serial->getErrorString();

    //Display connection status
    ui->plainTextEdit_connection_status->setPlainText(result);
}

void Interface::on_pushButton_disconnect_clicked()
{
    closeInputSerial();
    updateGuiAfterSerialConnection(false);
}

void Interface::updateGuiAfterSerialConnection(bool connectSuccess)
{
    ui->horizontalFrame_serial_input_connection->setEnabled(!connectSuccess);
    ui->pushButton_connect->setEnabled(!connectSuccess);
    ui->pushButton_disconnect->setEnabled(connectSuccess);
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




///////////////////////////
/// Raw Nmea Sentences  ///
///////////////////////////

//Display On Screens
void Interface::displayRawNmeaSentence(const QString &type, const QString &nmeaText)
{
    static const QMap<QString, QPlainTextEdit*> sentenceMap = {
        { "TXT",   ui->plainTextEdit_txt },
        { "GGA",   ui->plainTextEdit_gga },
        { "RMC",   ui->plainTextEdit_rmc },
        { "GSV",   ui->plainTextEdit_gsv },
        { "GLL",   ui->plainTextEdit_gll },
        { "GSA",   ui->plainTextEdit_gsa },
        { "VTG",   ui->plainTextEdit_vtg },
        { "OTHER", ui->plainTextEdit_others }
    };

    // Always show TXT regardless of freeze
    if (type == "TXT")
        ui->plainTextEdit_txt->appendPlainText(nmeaText);

    // Don't update rest if freeze is checked
    if (ui->pushButton_freeze_raw_sentences_screens->isChecked())
        return;

    if (type != "TXT" && sentenceMap.contains(type))
        sentenceMap[type]->appendPlainText(nmeaText);
}

//Clear Screens
void Interface::clearRawSentencesScreens()
{
    QList<QPlainTextEdit*> editors = {
        ui->plainTextEdit_gga,
        ui->plainTextEdit_rmc,
        ui->plainTextEdit_gsv,
        ui->plainTextEdit_gll,
        ui->plainTextEdit_gsa,
        ui->plainTextEdit_vtg
    };

    for (QPlainTextEdit* editor : editors)
        editor->clear();
}

void Interface::on_pushButton_clear_raw_sentences_screens_clicked()
{
    clearRawSentencesScreens();
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


///////////////////////
/// UDP Output Data ///
///////////////////////

//UDP Settings
void Interface::on_spinBox_update_udp_port_valueChanged(int udp_port)
{
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


//Check data to outpout
void Interface::on_pushButton_activate_udp_output_toggled(bool checked)
{
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
    ui->checkBox_udp_output_others->setChecked(true);
}

void Interface::on_pushButtonuncheck_all_udp_output_clicked()
{
    ui->checkBox_udp_output_gga->setChecked(false);
    ui->checkBox_udp_output_rmc->setChecked(false);
    ui->checkBox_udp_output_gsv->setChecked(false);
    ui->checkBox_udp_output_gll->setChecked(false);
    ui->checkBox_udp_output_gsa->setChecked(false);
    ui->checkBox_udp_output_vtg->setChecked(false);
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
    if(serial->isSerialOpen())
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
    ui->checkBox_serial_output_others->setChecked(true);
}

void Interface::on_pushButtonuncheck_all_serial_output_clicked()
{
    ui->checkBox_serial_output_gga->setChecked(false);
    ui->checkBox_serial_output_rmc->setChecked(false);
    ui->checkBox_serial_output_gsv->setChecked(false);
    ui->checkBox_serial_output_gll->setChecked(false);
    ui->checkBox_serial_output_gsa->setChecked(false);
    ui->checkBox_serial_output_vtg->setChecked(false);
    ui->checkBox_serial_output_others->setChecked(false);
}

