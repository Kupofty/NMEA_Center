#include "gps_data_displayer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GPS_Data_Displayer gui;

    gui.show();
    return app.exec();
}
