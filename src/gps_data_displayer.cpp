#include "gps_data_displayer.h"

/////////////
/// CLASS ///
/////////////
GPS_Data_Displayer::GPS_Data_Displayer(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::GPS_Data_Displayer),
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

GPS_Data_Displayer::~GPS_Data_Displayer()
{
    delete serial;
    delete nmea_handler;
    delete udp_writer;
    delete text_file_writer;
    delete ui;
}

void GPS_Data_Displayer::connectSignalSlot()
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



///////////
/// GUI ///
///////////
void GPS_Data_Displayer::hideGUI()
{
    ui->horizontalFrame_udp_ip_address->hide();
}




////////////////////
/// Serial Input ///
////////////////////

// Connection
void GPS_Data_Displayer::closeInputSerial()
{
    if(serial->isSerialOpen())
    {
        serial->closeSerialDevice();
        ui->plainTextEdit_connection_status->setPlainText(serial->getPortName()+" closed");
    }
    else
        ui->plainTextEdit_connection_status->setPlainText("Connection not opened");
}

void GPS_Data_Displayer::on_pushButton_connect_clicked()
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

void GPS_Data_Displayer::on_pushButton_disconnect_clicked()
{
    closeInputSerial();
    updateGuiAfterSerialConnection(false);
}

void GPS_Data_Displayer::updateGuiAfterSerialConnection(bool connectSuccess)
{
    ui->horizontalFrame_serial_input_connection->setEnabled(!connectSuccess);
    ui->pushButton_connect->setEnabled(!connectSuccess);
    ui->pushButton_disconnect->setEnabled(connectSuccess);
}

//COM ports
void GPS_Data_Displayer::listAvailablePorts(QComboBox *comboBox)
{
    comboBox->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports)
    {
        comboBox->addItem(port.portName());
    }
}

void GPS_Data_Displayer::on_pushButton_refresh_available_ports_list_clicked()
{
    listAvailablePorts(ui->comboBox_serial_input_port_list);
}




///////////////////////////
/// Raw Nmea Sentences  ///
///////////////////////////

//Display On Screens
void GPS_Data_Displayer::displayTXTSentence(const QString &nmeaText)
{
    ui->plainTextEdit_txt->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGGASentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gga->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayRMCSentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_rmc->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGSVSentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gsv->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGLLSentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gll->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayGSASentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_gsa->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayVTGSentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_vtg->appendPlainText(nmeaText);
}

void GPS_Data_Displayer::displayOtherSentence(const QString &nmeaText)
{
    if(!ui->pushButton_freeze_raw_sentences_screens->isChecked())
        ui->plainTextEdit_others->appendPlainText(nmeaText);
}


//Clear Screens
void GPS_Data_Displayer::clearRawSentencesScreens()
{
    ui->plainTextEdit_gga->clear();
    ui->plainTextEdit_rmc->clear();
    ui->plainTextEdit_gsv->clear();
    ui->plainTextEdit_gll->clear();
    ui->plainTextEdit_gsa->clear();
    ui->plainTextEdit_vtg->clear();
}

void GPS_Data_Displayer::on_pushButton_clear_raw_sentences_screens_clicked()
{
    clearRawSentencesScreens();
}




//////////////////////////////////
/// Display Decoded NMEA data  ///
//////////////////////////////////

//Data
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




///////////////////////
/// UDP Output Data ///
///////////////////////

//UDP Settings
void GPS_Data_Displayer::on_spinBox_update_udp_port_valueChanged(int udp_port)
{
    udp_writer->updateUdpPort(udp_port);
}

void GPS_Data_Displayer::on_comboBox_udp_host_address_currentTextChanged(const QString &udpMethod)
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

void GPS_Data_Displayer::on_lineEdit_udp_ip_address_editingFinished()
{
    udp_writer->updateUdpMethod(QHostAddress(ui->lineEdit_udp_ip_address->text()));
}


//Check data to outpout
void GPS_Data_Displayer::on_pushButton_activate_udp_output_toggled(bool checked)
{
    udp_writer->updateOutputNMEA(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_gga_toggled(bool checked)
{
    udp_writer->updateOutputGGA(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_rmc_toggled(bool checked)
{
    udp_writer->updateOutputRMC(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_gsv_toggled(bool checked)
{
    udp_writer->updateOutputGSV(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_gll_toggled(bool checked)
{
    udp_writer->updateOutputGLL(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_gsa_toggled(bool checked)
{
    udp_writer->updateOutputGSA(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_vtg_toggled(bool checked)
{
    udp_writer->updateOutputVTG(checked);
}

void GPS_Data_Displayer::on_checkBox_udp_output_others_toggled(bool checked)
{
    udp_writer->updateOutputOthers(checked);
}

void GPS_Data_Displayer::on_pushButton_check_all_udp_output_clicked()
{
    ui->checkBox_udp_output_gga->setChecked(true);
    ui->checkBox_udp_output_rmc->setChecked(true);
    ui->checkBox_udp_output_gsv->setChecked(true);
    ui->checkBox_udp_output_gll->setChecked(true);
    ui->checkBox_udp_output_gsa->setChecked(true);
    ui->checkBox_udp_output_vtg->setChecked(true);
    ui->checkBox_udp_output_others->setChecked(true);
}

void GPS_Data_Displayer::on_pushButtonuncheck_all_udp_output_clicked()
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
void GPS_Data_Displayer::on_pushButton_folder_path_documents_clicked()
{
    ui->plainTextEdit_txt_file_path->setPlainText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
}

void GPS_Data_Displayer::on_pushButton_folder_path_downloads_clicked()
{
    ui->plainTextEdit_txt_file_path->setPlainText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

void GPS_Data_Displayer::on_pushButton_browse_folder_path_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Output Directory",
                                                        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


    if (!dirPath.isEmpty())
        ui->plainTextEdit_txt_file_path->setPlainText(dirPath);
}

void GPS_Data_Displayer::on_pushButton_automatic_txt_file_name_clicked()
{
    QString automaticFileName = "Log_NMEA_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss");
    ui->plainTextEdit_txt_file_name->setPlainText(automaticFileName);
}

void GPS_Data_Displayer::on_pushButton_save_txt_file_toggled(bool checked)
{

    if(checked)
    {
        QString dirPath = ui->plainTextEdit_txt_file_path->toPlainText().trimmed();
        QString fileName = ui->plainTextEdit_txt_file_name->toPlainText().trimmed();
        QString fileExtension = ui->comboBox_txt_file_extension->currentText();
        QString totalFilePath = QDir(dirPath).filePath(fileName + fileExtension);

        if (dirPath.isEmpty() || fileName.isEmpty()) {
            QMessageBox::warning(this, "Missing Information", "Please select an output folder and enter a file name before saving.");
            ui->pushButton_save_txt_file->setChecked(false);
            return;
        }

        int result = text_file_writer->createFile(totalFilePath);
        if(!result)
            return;

        ui->pushButton_save_txt_file->setText(" Stop Recording");
    }
    else
    {
        text_file_writer->closeFile();
        ui->pushButton_save_txt_file->setText(" Record Data To File");
    }

}




//////////////////////////
/// Serial Output Data ///
//////////////////////////

//Settings
void GPS_Data_Displayer::closeOutputSerial()
{
    if(serial_writer->isSerialOpen())
    {
        serial_writer->closeSerialDevice();
        ui->plainTextEdit_connection_status_output_serial->setPlainText(serial_writer->getPortName()+" closed");
    }
    else
        ui->plainTextEdit_connection_status_output_serial->setPlainText("Connection not opened");
}

void GPS_Data_Displayer::on_pushButton_refresh_available_port_serial_output_clicked()
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

void GPS_Data_Displayer::on_pushButton_connect_serial_output_clicked()
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

void GPS_Data_Displayer::on_pushButton_disconnect_serial_output_clicked()
{
    closeOutputSerial();
}


//Outputs
void GPS_Data_Displayer::on_pushButton_activate_serial_output_toggled(bool checked)
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

void GPS_Data_Displayer::on_checkBox_serial_output_gga_toggled(bool checked)
{
    serial_writer->updateOutputGGA(checked);
}

void GPS_Data_Displayer::on_checkBox_serial_output_gsv_toggled(bool checked)
{
    serial_writer->updateOutputGSV(checked);
}

void GPS_Data_Displayer::on_checkBox_serial_output_rmc_toggled(bool checked)
{
    serial_writer->updateOutputRMC(checked);
}

void GPS_Data_Displayer::on_checkBox_serial_output_gsa_toggled(bool checked)
{
    serial_writer->updateOutputGSA(checked);
}

void GPS_Data_Displayer::on_checkBox_serial_output_gll_toggled(bool checked)
{
    serial_writer->updateOutputGLL(checked);
}

void GPS_Data_Displayer::on_checkBox_serial_output_vtg_toggled(bool checked)
{
    serial_writer->updateOutputVTG(checked);
}

void GPS_Data_Displayer::on_checkBox_serial_output_others_toggled(bool checked)
{
    serial_writer->updateOutputOthers(checked);
}

void GPS_Data_Displayer::on_pushButton_check_all_serial_output_clicked()
{
    ui->checkBox_serial_output_gga->setChecked(true);
    ui->checkBox_serial_output_rmc->setChecked(true);
    ui->checkBox_serial_output_gsv->setChecked(true);
    ui->checkBox_serial_output_gll->setChecked(true);
    ui->checkBox_serial_output_gsa->setChecked(true);
    ui->checkBox_serial_output_vtg->setChecked(true);
    ui->checkBox_serial_output_others->setChecked(true);
}

void GPS_Data_Displayer::on_pushButtonuncheck_all_serial_output_clicked()
{
    ui->checkBox_serial_output_gga->setChecked(false);
    ui->checkBox_serial_output_rmc->setChecked(false);
    ui->checkBox_serial_output_gsv->setChecked(false);
    ui->checkBox_serial_output_gll->setChecked(false);
    ui->checkBox_serial_output_gsa->setChecked(false);
    ui->checkBox_serial_output_vtg->setChecked(false);
    ui->checkBox_serial_output_others->setChecked(false);
}

