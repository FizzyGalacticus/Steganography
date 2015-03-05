#ifndef __STEG__SLOTS
#define __STEG__SLOTS
#include "mainwindow.h"
#include <QDebug>

void MainWindow::_openCoverImageButtonIsPressed()
{
    QStringList images = _openFileDialogue(_supportedImageFormats);

    //As long as list isn't empty, we have a file.
    if(images.size())
    {
        _coverFilename = images.at(0);
        _coverImage = new QImage(_coverFilename);
        _coverImageDisplay->setPixmap(QPixmap::fromImage(_coverImage->scaled(_coverImageDisplay->width(),_coverImageDisplay->height())));
    }
}

void MainWindow::_openPayloadButtonIsPressed()
{
    _payloadFilenames = new QStringList(_openFileDialogue(("")));
}

void MainWindow::_hidePayloadButtonIsPressed()
{
    if(_payloadFilenames)
    {
        if(!_coverImage)
        {
            qDebug() << tr("No cover image!");
            return;
        }

        QVector<bool> * payloadBits = NULL;

        const unsigned int availableSpace = (_coverImage->width()*_coverImage->height()*3);

        unsigned int numberOfBitsNeeded = sizeof(unsigned int) + (_payloadFilenames->size() * sizeof(unsigned int));

        for(int i = 0; i < _payloadFilenames->size(); i++)
        {
            QFile * payload = new QFile(_payloadFilenames->at(i));

            if(payload->exists()) numberOfBitsNeeded += (payload->size() * 8);
        }

        if(availableSpace > numberOfBitsNeeded)
        {
            payloadBits = getBitsFromPayloads();

            putBitsIntoImage(payloadBits);

            _stegImage = new QImage("steg.png");
            _stegImageDisplay->setPixmap(QPixmap::fromImage(_stegImage->scaled(_stegImageDisplay->width(),_stegImageDisplay->height())));

            _popUpMessage->setWindowTitle(tr("Yipee!"));
            _popUpMessage->setText(tr("Finished hiding payload!"));
            _popUpMessage->exec();
        }
        else
        {
            QString message = "Cover image is not large enough to host given payload.";
            message += "\nSize needed: " + QString::number(numberOfBitsNeeded) + " bits.";
            message += "\nSpace available: " + QString::number(availableSpace) + " bits.";

            qDebug() << message;
            _popUpMessage->setWindowTitle(tr("Doh!"));
            _popUpMessage->setText(tr(message.toStdString().c_str()));
            _popUpMessage->exec();
        }
    }
    else qDebug() << tr("No payloads!");
}

void MainWindow::_openStegImageButtonIsPressed()
{
    QStringList images = _openFileDialogue(_supportedImageFormats);

    if(images.size())
    {
        _stegFilename = images.at(0);
        _stegImage = new QImage(_stegFilename);
        _stegImageDisplay->setPixmap(QPixmap::fromImage(_stegImage->scaled(_stegImageDisplay->width(),_stegImageDisplay->height())));
    }
}

void MainWindow::_recoverFilesButtonIsPressed()
{
    getPayloadBits();
    QVector<bool> * payloadBits = new QVector<bool>;
    unsigned int numberOfFiles = 0;

    for(int xPix = 0; xPix < _stegImage->width() && payloadBits->size() < int(sizeof(unsigned int)*8); xPix++)
    {
        for(int yPix = 0; yPix < _stegImage->height() && payloadBits->size() < int(sizeof(unsigned int)*8); yPix++)
        {
            payloadBits->push_back(QColor(_stegImage->pixel(xPix,yPix)).red() & 1);
            payloadBits->push_back(QColor(_stegImage->pixel(xPix,yPix)).green() & 1);
            payloadBits->push_back(QColor(_stegImage->pixel(xPix,yPix)).blue() & 1);
        }
    }

    numberOfFiles = getNumberOfFilesFromBits(payloadBits);

    qDebug() << "Bits pulled from image:" << payloadBits->size();
    qDebug() << tr("Number of files hidden in image:") << numberOfFiles;
}

#endif
