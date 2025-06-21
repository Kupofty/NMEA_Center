#include "serial_writer.h"

SerialWriter::SerialWriter(QObject *parent) : OutputWriter(parent)
{

}


SerialWriter::~SerialWriter()
{

}



/////////////////
/// Functions ///
/////////////////
void SerialWriter::sendData(const QString &line)
{
    if(getOutputNMEA())
    {

    }
}


///////////////////////
/// Update settings ///
///////////////////////
