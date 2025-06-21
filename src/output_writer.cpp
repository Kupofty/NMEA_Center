#include "output_writer.h"


/////////////
/// Class ///
/////////////

OutputWriter::OutputWriter(QObject *parent) : QObject{parent}
{

}

OutputWriter::~OutputWriter()
{

}



/////////////////
/// Functions ///
/////////////////
void OutputWriter::sendData(const QString &line)
{

}



////////////////
/// Get data ///
////////////////
bool OutputWriter::getOutputNMEA()
{
    return outputNMEA;
}



//////////////////////
/// Update outputs ///
//////////////////////
void OutputWriter::updateOutputNMEA(bool check)
{
    outputNMEA = check;
}

void OutputWriter::updateOutputGGA(bool check)
{
    outputGGA = check;
}

void OutputWriter::updateOutputRMC(bool check)
{
    outputRMC = check;
}

void OutputWriter::updateOutputGSV(bool check)
{
    outputGSV = check;
}

void OutputWriter::updateOutputGLL(bool check)
{
    outputGLL = check;
}

void OutputWriter::updateOutputGSA(bool check)
{
    outputGSA = check;
}

void OutputWriter::updateOutputVTG(bool check)
{
    outputVTG = check;
}

void OutputWriter::updateOutputOthers(bool check)
{
    outputOthers = check;
}



////////////////////
/// Public slots ///
////////////////////
void OutputWriter::publishGGA(const QString &line)
{
    if(outputGGA)
        sendData(line);
}

void OutputWriter::publishRMC(const QString &line)
{
    if(outputRMC)
        sendData(line);
}

void OutputWriter::publishGSV(const QString &line)
{
    if(outputGSV)
        sendData(line);
}

void OutputWriter::publishGLL(const QString &line)
{
    if(outputGLL)
        sendData(line);
}

void OutputWriter::publishGSA(const QString &line)
{
    if(outputGSA)
        sendData(line);
}

void OutputWriter::publishVTG(const QString &line)
{
    if(outputVTG)
        sendData(line);
}

void OutputWriter::publishOthers(const QString &line)
{
    if(outputOthers)
        sendData(line);
}

