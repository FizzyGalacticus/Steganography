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
