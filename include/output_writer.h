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
        bool getOutputNMEA();

        //Update authorized outputs
        void updateOutputNMEA(bool check);
        void updateOutputGGA(bool check);
        void updateOutputRMC(bool check);
        void updateOutputGSV(bool check);
        void updateOutputGLL(bool check);
        void updateOutputGSA(bool check);
        void updateOutputVTG(bool check);
        void updateOutputOthers(bool check);

    public slots:
        void publishGGA(const QString &line);
        void publishRMC(const QString &line);
        void publishGSV(const QString &line);
        void publishGLL(const QString &line);
        void publishGSA(const QString &line);
        void publishVTG(const QString &line);
        void publishOthers(const QString &line);

    private:
        //Allowed output sentences
        bool outputNMEA = 0;
        bool outputGGA = 0;
        bool outputRMC = 0;
        bool outputGSV = 0;
        bool outputGLL = 0;
        bool outputGSA = 0;
        bool outputVTG = 0;
        bool outputOthers = 0;
};

#endif // OUTPUT_WRITER_H
