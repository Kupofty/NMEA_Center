#include "gps_data_displayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GPS_Data_Displayer w;
    w.show();
    return a.exec();
}

// TO-DO LIST & IDEAS
// change project to NMEA_DISPLAY with tab "gps", "sensor1", "sensor"2" etc...
// accept UDP or TCP connection
// print decoded data from nmea as speed=x, heading=x, position=x
// use new widgets
// push code to github
// new class to handle NMEA (to hide from main code)
// handle disconnection + alert msg
