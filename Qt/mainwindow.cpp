#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QImageReader>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _openCoverImageButton(new QPushButton("Cover...")),
    _supportedImageFormats("Images ("),
    _imageDisplay(new QLabel(this))
{
    ui->setupUi(this);
    this->setMinimumSize(100,100);

    QWidget * centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout * main = new QVBoxLayout;
    QHBoxLayout * buttons = new QHBoxLayout;
    QHBoxLayout * images = new QHBoxLayout;

    //Create a string that contains all currently supported image formats
    for(int i = 0; i < QImageReader::supportedImageFormats().size(); i++)
    {
        _supportedImageFormats += ('*' + QImageReader::supportedImageFormats().at(i));
        if(i == QImageReader::supportedImageFormats().size()-1) _supportedImageFormats += ')';
        else _supportedImageFormats += ' ';
    }

    _openCoverImageButton->setParent(this);
    _openCoverImageButton->setGeometry(0,this->height()-30,50,30);
    connect(_openCoverImageButton, SIGNAL(clicked()), this, SLOT(_openCoverImageButtonIsPressed()));

    buttons->addWidget(_openCoverImageButton);
    images->addWidget(_imageDisplay);
    main->addLayout(images);
    main->addLayout(buttons);
    centralWidget->setLayout(main);
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
        delete _scaledImage;
        _coverFilename = images.at(0);
        _coverImage = new QImage(_coverFilename);
        _scaledImage = new QImage(_coverImage->scaled(_imageDisplay->width(),_imageDisplay->height()));
        _imageDisplay->setPixmap(QPixmap::fromImage(*_scaledImage));
    }

    qDebug() << "File opened!";
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
