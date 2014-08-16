#ifndef __STEG__BITIOMANIP
#define __STEG__BITIOMANIP
#include "mainwindow.h"
#include <QDebug>
#include <QRgb>

const QVector<bool> * MainWindow::getBitsFromNumber(const unsigned int &number)
{
    QVector<bool> * numberBits = new QVector<bool>;

    for(int i = 0; i < sizeof(number); i++)
    {
        char temp = 0;

        temp = (number << (8*i));
        const QVector<bool> * tempBits = getBitsFromBytes(new QByteArray(&temp,sizeof(temp)));

        for(int bit = 0; bit < tempBits->size(); bit++) numberBits->push_back(tempBits->at(bit));
    }

    return numberBits;
}

unsigned int MainWindow::getNumberFromBits(const QVector<bool> * bits)
{
    if(bits->size() == sizeof(unsigned int)*8)
    {
        qDebug() << "Getting number back!";
        unsigned int number = 0;

        for(int i = 0; i < bits->size(); i++)
        {
            if(bits->at(i)) number++;

            if(i != (bits->size() - 1)) number = number << 1;
        }

        return number;
    }
    else return 0;
}

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
        qDebug() << "Getting bytes from:" << fileName;

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

    qDebug() << "Received" << fileBits->size() << "bits!";

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

void MainWindow::putBitsIntoImage(const QVector<bool> * payloadBits)
{
    QImage * stegImage = new QImage(*_coverImage);

    //Counter for keeping track of which bit we're on
    int bit = 0;

    for(int xPix = 0; xPix < stegImage->width(); xPix++)
    {
        for(int yPix = 0; yPix < stegImage->height(); yPix++)
        {
            for(int color = 0; color < 3 && bit < payloadBits->size(); color++)
            {
                int value = 0;
                unsigned int newColor = 0;

                switch(color)
                {
                    case 0:
                        value = QColor(stegImage->pixel(xPix,yPix)).red();
                        value = putBitIntoNumber(value,bit);
                        newColor += (value << 16);
                        newColor += (QColor(stegImage->pixel(xPix,yPix)).green() << 8);
                        newColor += (QColor(stegImage->pixel(xPix,yPix)).blue());
                    break;
                    case 1:
                        value = QColor(stegImage->pixel(xPix,yPix)).green();
                        value = putBitIntoNumber(value,bit);
                        newColor += (QColor(stegImage->pixel(xPix,yPix)).red() << 16);
                        newColor += (value << 8);
                        newColor += (QColor(stegImage->pixel(xPix,yPix)).blue());
                    break;
                    case 2:
                        value = QColor(stegImage->pixel(xPix,yPix)).blue();
                        value = putBitIntoNumber(value,bit);
                        newColor += (QColor(stegImage->pixel(xPix,yPix)).red() << 16);
                        newColor += (QColor(stegImage->pixel(xPix,yPix)).green() << 8);
                        newColor += (value);
                    break;
                }

                bit++;
            }

            if(bit >= payloadBits->size()) break;
        }

        if(bit >= payloadBits->size()) break;
    }

    stegImage->save("steg.png","png",-1);
    delete stegImage;
}


int MainWindow::putBitIntoNumber(const int & value,const bool & bit)
{
    if(bit && value % 2 == 0) return value + 1;
    else if(!bit && value % 2 == 1) return value - 1;
    else return value;
}

#endif
