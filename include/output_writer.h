#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include <QObject>
#include <QCoreApplication>
#include <QDebug>
#include <QString>

class OutputWriter : public QObject
{
    Q_OBJECT

    public:
        explicit OutputWriter(QObject *parent = nullptr);
        virtual ~OutputWriter();

        virtual void sendData(const QString &line) = 0;

        //Get
        bool getSocketOutputActivated();

        //Update authorized outputs
        void updateSocketOutputActivated(bool check); //global on/off
        void updateOutputNMEA(const QString &type, bool check); //nmea specific

    public slots:
        void publishNMEA(const QString &type, const QString &nmeaText);

    private:
        bool socketOutputActivated = 0;
        QMap<QString, bool> outputFlags;
};

#endif // OUTPUT_WRITER_H
