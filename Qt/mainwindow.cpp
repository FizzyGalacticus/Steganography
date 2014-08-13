#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setupFunctions.cpp"
#include <QDebug>
#include <QImageReader>
#include <QWidget>
#include <QResizeEvent>
#include <fstream>

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
    if(_payloads != NULL)
    {
        if(_coverImage == NULL)
        {
            qDebug() << "No cover image!";
            return;
        }

        const QVector<bool> * payloadBits = getBitsFromPayloads();
        const unsigned int availableSpace = (_coverImage->width()*_coverImage->height()*3);

        qDebug() << "Needed space: " << payloadBits->size();
        qDebug() << "Space available: " << availableSpace;

        if(availableSpace > payloadBits->size())
        {
            qDebug() << payloadBits->size();
            qDebug() << availableSpace;
            //Work magic
        }
        else
        {
            qDebug() << "Cover image is not large enough to host given payload.";
            qDebug() << "Size needed:" << payloadBits->size() << "bits.";
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

const QVector<char> * MainWindow::getBytesFromFile(const QString & fileName)
{
    QVector<char> * fileBytes = new QVector<char>;
    std::ifstream in;

    in.open(fileName.toStdString().c_str(), std::ios::in);
    if(in)
    {
        qDebug() << "Getting bytes from" << fileName << "...";
        char temp;

        while(!in.eof())
        {
            in.read(&temp,sizeof(temp));
            fileBytes->push_back(temp);
        }

        in.close();
    }
    else qDebug() << "Couldn't open " << fileName << " for reading!";

    return fileBytes;
}

const QVector<bool> * MainWindow::getBitsFromBytes(const QVector<char> * fileBytes)
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
