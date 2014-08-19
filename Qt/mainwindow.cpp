#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setupFunctions.cpp"
#include <QDebug>
#include <QImageReader>
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

    unsigned int myint = 107591;
    const QVector<bool> * test = getBitsFromNumber(myint);
    qDebug() << "Number of bits:" << test->size();
    myint = getNumberFromBits(test);
    qDebug() << "New number:" << myint;
}

MainWindow::~MainWindow()
{
    delete ui;
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
