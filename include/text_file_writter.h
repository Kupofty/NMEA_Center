#ifndef TEXT_FILE_WRITTER_H
#define TEXT_FILE_WRITTER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class TextFileWritter : public QObject
{
    Q_OBJECT

    public:
        explicit TextFileWritter(QObject *parent = nullptr);

        bool createFile(const QString &file_path);
        void closeFile();

    private:
        QFile file;

    public slots:
        void writeRawSentences(const QByteArray &line);


};

#endif // TEXT_FILE_WRITTER_H
