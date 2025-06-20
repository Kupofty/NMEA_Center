QT += core gui
QT += serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += include

SOURCES += \
    src/nmea_handler.cpp \
    src/main.cpp \
    src/gps_data_displayer.cpp \
    src/serial_reader.cpp

HEADERS += \
    include/gps_data_displayer.h \
    include/serial_reader.h \
    include/nmea_handler.h

FORMS += \
    ui/gps_data_displayer.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
