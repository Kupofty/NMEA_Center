#include "interface.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Interface gui;

    //Icon & Name
    QIcon appIcon(":/icons/pictures/icons/nmea_icon.ico");
    app.setWindowIcon(appIcon);
    gui.setWindowTitle("NMEA Center");

    gui.show();
    return app.exec();
}
