#include "gps_data_displayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GPS_Data_Displayer gui;
    gui.show();
    return app.exec();
}

// TO-DO LIST & IDEAS
// change name to NMEA_Center (for gps only for now)
// accept UDP or TCP connection (inputs)
// output data on com, tcp or udp
// handle disconnection + alert msg (sudden disconnection)
