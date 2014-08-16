#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setupFunctions.cpp"
#include <QDebug>
#include <QImageReader>
#include <QWidget>
#include <QResizeEvent>
#include <fstream>
#include <QFile>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _centralWidget(new QWidget(this)),
    _mainLayout(new QVBoxLayout),
    _openCoverImageButton(new QPushButton("Cover...")),
    _openPayloadButton(new QPushButton("Payload...")),
    _hidePayloadButton(new QPushButton("Hide Payload!")),
    _supportedImageFormats("Images ("),
    _coverImage(NULL),
    _scaledImage(NULL),
    _imageDisplay(new QLabel(this)),
    _payloads(NULL)
{
    ui->setupUi(this);
    this->setWindowTitle("Steganography Tool");
    this->setMinimumSize(100,100);
    this->setCentralWidget(_centralWidget);

    //Create a string that contains all currently supported image formats
    for(int i = 0; i < QImageReader::supportedImageFormats().size(); i++)
    {
        _supportedImageFormats += ('*' + QImageReader::supportedImageFormats().at(i));
        if(i == QImageReader::supportedImageFormats().size()-1) _supportedImageFormats += ')';
        else _supportedImageFormats += ' ';
    }

    _setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_openCoverImageButtonIsPressed()
{
    qDebug() << "Opening file!";
    QStringList images = _openFileDialogue(_supportedImageFormats);

    //As long as list isn't empty, we have a file.
    if(images.size())
    {
        _coverFilename = images.at(0);
        _coverImage = new QImage(_coverFilename);
        _scaledImage = new QImage(_coverImage->scaled(_imageDisplay->width(),_imageDisplay->height()));
        _imageDisplay->setPixmap(QPixmap::fromImage(*_scaledImage));

        qDebug() << "File opened!";
    }
    else qDebug() << "No file selected!";
}

void MainWindow::_openPayloadButtonIsPressed()
{
    qDebug() << "Opening file!";
    _payloads = new QStringList(_openFileDialogue(("")));

    if(_payloads->size()) qDebug() << "File(s) opened!";
    else qDebug() << "No file(s) selected!";
}

void MainWindow::_hidePayloadButtonIsPressed()
{
    if(_payloads)
    {
        if(!_coverImage)
        {
            qDebug() << "No cover image!";
            return;
        }

        const QVector<bool> * payloadBits = NULL;

        const unsigned int availableSpace = (_coverImage->width()*_coverImage->height()*3);

        unsigned int numberOfBitsNeeded = 0;
        for(int i = 0; i < _payloads->size(); i++)
        {
            QFile * payload = new QFile(_payloads->at(i));

            if(payload->exists()) numberOfBitsNeeded += (payload->size() * 8);
        }

        const unsigned int numberOfFiles = _payloads->size();

        if(availableSpace > numberOfBitsNeeded)
        {
            payloadBits = getBitsFromPayloads();

            qDebug() << payloadBits->size();
            qDebug() << availableSpace;
            //Work magic

            /*****************DEBUG INFORMATION*******************************/
            //        for(int i = 0; i < payloadBits->size(); i++)
            //            qDebug() << (payloadBits->at(i)?'1':'0');
            //        qDebug() << payloadBits->size();

                    if(writeBytesToFile(getBytesFromBits(payloadBits), "test"))
                        qDebug() << "Written payload!";
            /*****************************************************************/
        }
        else
        {
            qDebug() << "Cover image is not large enough to host given payload.";
            qDebug() << "Size needed:" << numberOfBitsNeeded << "bits.";
            qDebug() << "Space available:" << availableSpace << "bits.";
        }
    }
    else qDebug() << "No payloads!";
}

QStringList MainWindow::_openFileDialogue(const QString & formats)
{
    //Setup file dialogue
    QFileDialog openFileDialog(this);
    openFileDialog.setDirectory(QDir::homePath());
    openFileDialog.setNameFilter(tr(formats.toStdString().c_str()));
    openFileDialog.setViewMode(QFileDialog::List);
    openFileDialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList fileNames;

    if(openFileDialog.exec())
       fileNames = openFileDialog.selectedFiles();

    return fileNames;
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    if(_coverFilename.size())
    {
        delete _scaledImage;
        _scaledImage = new QImage(_coverImage->scaled(_imageDisplay->width(),_imageDisplay->height()));
        _imageDisplay->setPixmap(QPixmap::fromImage(*_scaledImage));
    }

    qDebug() << "Window size: " << event->size().width() << "x" << event->size().height();
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
