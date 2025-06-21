#include "text_file_writter.h"


/////////////
/// Class ///
/////////////
TextFileWritter::TextFileWritter(QObject *parent) : QObject{parent}
{

}

TextFileWritter::~TextFileWritter()
{

}



///////////////////
/// Handle File ///
///////////////////
bool TextFileWritter::createFile(const QString &filePath)
{
    file.setFileName(filePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        return true;
    else
        return false;
}

void TextFileWritter::closeFile()
{
    if(file.isOpen())
        file.close();
}

void TextFileWritter::writeRawSentences(const QByteArray &line)
{
    if(file.isOpen())
    {
        QTextStream out(&file);
        out << line << '\n';
        out.flush();
    }
}
