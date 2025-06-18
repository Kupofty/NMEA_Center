QT += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += include

SOURCES += \
    src/gps_reader.cpp \
    src/main.cpp \
    src/gps_data_displayer.cpp

HEADERS += \
    include/gps_data_displayer.h \
    include/gps_reader.h

FORMS += \
    ui/gps_data_displayer.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
