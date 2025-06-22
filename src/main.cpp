#include "interface.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Interface gui;

    QIcon appIcon(":/icons/icons/nmea_icon.ico");
    app.setWindowIcon(appIcon);
    gui.setWindowTitle("NMEA IO Hub");

    gui.show();
    return app.exec();
}
