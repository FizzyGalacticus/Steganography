#ifndef __STEG__BITIOMANIP
#define __STEG__BITIOMANIP
#include "mainwindow.h"
#include <QDebug>
#include <QRgb>
#include <QColor>

const QVector<bool> * MainWindow::getBitsFromNumber(const unsigned int &number)
{
    QVector<bool> * numberBits = new QVector<bool>;

    for(int i = sizeof(number); i > 0; i--)
    {
        char temp = 0;

        temp = (number >> (8*(i-1)));
        const QVector<bool> * tempBits = getBitsFromBytes(new QByteArray(&temp,sizeof(temp)));

        numberBits = mergeBits(numberBits,tempBits);
    }

    return numberBits;
}

unsigned int MainWindow::getNumberOfFilesFromBits(const QVector<bool> * bits)
{
    if(bits->size() >= int(sizeof(unsigned int)*8))
    {
        qDebug() << tr("Getting number back!");
        unsigned int number = 0;

        for(int i = 0; i < 32; i++)
        {
            if(bits->at(i)) number++;

            if(i < 31) number = number << 1;
        }

        return number;
    }
    else return 0;
}


QVector<bool> * MainWindow::mergeBits(const QVector<bool> * lhs, const QVector<bool> * rhs)
{
    QVector<bool> * newBits = new QVector<bool>;

    for(int i = 0; i < lhs->size(); i++)
        newBits->push_back(lhs->at(i));

    for(int i = 0; i < rhs->size(); i++)
        newBits->push_back(rhs->at(i));

    return newBits;
}

QVector<bool> * MainWindow::getBitsFromPayloads()
{
    qDebug() << tr("Getting bits from payloads...");

    QVector<bool> * payloadBits = new QVector<bool>;
    QFile * payloadFile = NULL;
    unsigned int fileSize = 0;

    payloadBits = mergeBits(getBitsFromNumber(_payloadFilenames->size()),payloadBits);

    for(int i = 0; i < _payloadFilenames->size(); i++)
    {
        payloadFile = new QFile(_payloadFilenames->at(i));
        if(payloadFile->exists()) fileSize = payloadFile->size();

        const QVector<bool> * temp(getBitsFromBytes(getBytesFromFile(_payloadFilenames->at(i))));

        if(fileSize) payloadBits = mergeBits(payloadBits, getBitsFromNumber(fileSize));
        payloadBits = mergeBits(payloadBits,temp);

        fileSize = 0;
    }

    qDebug() << tr("done.");

    return payloadBits;
}

const QByteArray * MainWindow::getBytesFromFile(const QString & fileName)
{
    QByteArray * fileBytes = NULL;
    QFile * file = new QFile(fileName);

    file->open(QIODevice::ReadOnly);

    if(file->isOpen())
    {
        qDebug() << tr("Getting bytes from:") << fileName;

        fileBytes = new QByteArray(file->readAll());
        file->close();
    }

    return fileBytes;
}

const QVector<bool> * MainWindow::getBitsFromBytes(const QByteArray * fileBytes)
{
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

void MainWindow::getPayloadBits(const QByteArray * bytes)
{

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

void MainWindow::putBitsIntoImage(QVector<bool> * payloadBits)
{
    QImage * stegImage = new QImage(*_coverImage);
    stegImage->convertToFormat(QImage::Format_RGB32);

    _progressBar->setRange(0, payloadBits->size());
    _progressBar->setValue(0);

    //Counter for keeping track of which bit we're on
    int bit = 0;

    for(int xPix = 0; xPix < stegImage->width() && payloadBits->size(); xPix++)
    {
        for(int yPix = 0; yPix < stegImage->height() && payloadBits->size(); yPix++)
        {
            stegImage->setPixel(xPix,yPix,putBitsIntoRGB(payloadBits,QColor(stegImage->pixel(xPix,yPix)).rgb()));

            bit += 3;
            _progressBar->setValue(bit);
        }
    }

    stegImage->save("steg.png","png",-1);
    _progressBar->setValue(0);
    delete stegImage;
}


int MainWindow::putBitIntoNumber(const int & value,const bool & bit)
{
    if(bit && value % 2 == 0) return value + 1;
    else if(!bit && value % 2 == 1) return value - 1;
    else return value;
}


QRgb MainWindow::putBitsIntoRGB(QVector<bool> * bits, QRgb rgb)
{
    QColor * newColor = new QColor(rgb);
    int r = newColor->red();
    int g = newColor->green();
    int b = newColor->blue();

    if(bits->size() >= 3)
    {
        newColor->setRed(putBitIntoNumber(newColor->red(),bits->at(0)));
        newColor->setGreen(putBitIntoNumber(newColor->green(),bits->at(1)));
        newColor->setBlue(putBitIntoNumber(newColor->blue(),bits->at(2)));
    }
    else if(bits->size() == 2)
    {
        newColor->setRed(putBitIntoNumber(newColor->red(), bits->at(0)));
        newColor->setGreen(putBitIntoNumber(newColor->green(), bits->at(1)));
    }
    else newColor->setRed(putBitIntoNumber(newColor->red(), bits->at(0)));

//    if(bits->size() >= 2)
//        g = putBitIntoNumber(g,bits->at(1));
//    if(bits->size() >= 3)
//        b = putBitIntoNumber(b,bits->at(2));

    if(bits->size() >=3) bits->remove(0,3);
    else bits->clear();

    return QColor(r,g,b,newColor->alpha()).rgba();
}

#endif
