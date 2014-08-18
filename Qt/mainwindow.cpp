#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setupFunctions.cpp"
#include <QDebug>
#include <QImageReader>
#include <QWidget>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _centralWidget(new QWidget(this)),
    _mainLayout(new QVBoxLayout),
    _popUpMessage(new QMessageBox(this)),
    _coverLabel(new QLabel(tr("Cover Image"),this)),
    _stegLabel(new QLabel(tr("Steg Image"),this)),
    _coverImageDisplay(new QLabel(this)),
    _stegImageDisplay(new QLabel(this)),
    _openCoverImageButton(new QPushButton(tr("Cover Image..."))),
    _openPayloadButton(new QPushButton(tr("Payload..."))),
    _hidePayloadButton(new QPushButton(tr("Hide Payload!"))),
    _openStegImageButton(new QPushButton(tr("Steg Image..."))),
    _recoverFilesButton(new QPushButton(tr("Recover Files!"))),
    _supportedImageFormats("Images ("),
    _coverImage(NULL),
    _stegImage(NULL),
    _payloads(NULL),
    _progressBar(new QProgressBar)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Steganography Tool"));
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
    _payloads = new QStringList(_openFileDialogue(("")));
}

void MainWindow::_hidePayloadButtonIsPressed()
{
    if(_payloads)
    {
        if(!_coverImage)
        {
            qDebug() << tr("No cover image!");
            return;
        }

        const QVector<bool> * payloadBits = NULL;

        const unsigned int availableSpace = (_coverImage->width()*_coverImage->height()*3);

        unsigned int numberOfBitsNeeded = sizeof(unsigned int) + (_payloads->size() * sizeof(unsigned int));

        for(int i = 0; i < _payloads->size(); i++)
        {
            QFile * payload = new QFile(_payloads->at(i));

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

    qDebug() << tr("Opening file(s)!");

    if(openFileDialog.exec())
       fileNames = openFileDialog.selectedFiles();

    if(fileNames.size()) qDebug() << tr("Opened file(s)!");
    else qDebug() << tr("No file(s) selected!");

    return fileNames;
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    if(_coverImage) _coverImageDisplay->setPixmap(QPixmap::fromImage(_coverImage->scaled(_coverImageDisplay->width(),_coverImageDisplay->height())));
    if(_stegImage) _stegImageDisplay->setPixmap(QPixmap::fromImage(_stegImage->scaled(_stegImageDisplay->width(),_stegImageDisplay->height())));

    qDebug() << tr("Window size: ") << event->size().width() << "x" << event->size().height();
}
