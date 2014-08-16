#ifndef __STEG__BITIOMANIP
#define __STEG__BITIOMANIP
#include "mainwindow.h"
#include <QDebug>

const QVector<bool> * MainWindow::getBitsFromPayloads()
{
    qDebug() << "Getting bits from payloads...";

    QVector<bool> * payloadBits = new QVector<bool>;

    for(int i = 0; i < _payloads->size(); i++)
    {
        const QVector<bool> * temp(getBitsFromBytes(getBytesFromFile(_payloads->at(i))));

        for(int bit = 0; bit < temp->size(); bit++) payloadBits->push_back(temp->at(bit));
    }

    qDebug() << "done.";

    return payloadBits;
}

const QByteArray * MainWindow::getBytesFromFile(const QString & fileName)
{
    QByteArray * fileBytes = NULL;
    QFile * file = new QFile(fileName);

    file->open(QIODevice::ReadOnly);

    if(file->isOpen())
    {
        fileBytes = new QByteArray(file->readAll());
        file->close();
    }

    return fileBytes;
}

const QVector<bool> * MainWindow::getBitsFromBytes(const QByteArray * fileBytes)
{
    qDebug() << "Now getting bits...";

    QVector<bool> * fileBits = new QVector<bool>;

    //Iterate through each byte
    for(int byte = 0; byte < fileBytes->size(); byte++)
    {
        //For each bit in the byte
        for(int bit = 7; bit >= 0; bit--)
        {
            //If the first bit is set, push 'true'
            if((fileBytes->at(byte) >> bit) & 1) fileBits->push_back(true);
            else fileBits->push_back(false);
        }
    }

    return fileBits;
}

const QByteArray * MainWindow::getBytesFromBits(const QVector<bool> * bits)
{
    QByteArray * bytes = new QByteArray;

    if(bits->size() % 8 == 0)
        for(int i = 0; i < bits->size(); i+=8)
        {
            char temp = 0;

            for(int j = 0; j < 8; j++)
            {
                if(bits->at(i+j)) temp += 1;

                if(j != 7) temp = temp << 1;
            }

            bytes->push_back(temp);
        }

    return bytes;
}

bool MainWindow::writeBytesToFile(const QByteArray * bytes, const QString & fileName)
{
    QFile * file = new QFile(fileName);

    file->open(QIODevice::WriteOnly);

    if(file->isOpen())
    {
        file->write(*bytes);
        file->close();

        return true;
    }

    return false;
}

#endif
