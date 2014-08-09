#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _openFileButton(new QPushButton("Open...")),
    _supportedImageFormats("Images ("),
    _imageDisplay(new QLabel(this))
{
    ui->setupUi(this);

    //Create a string that contains all currently supported image formats
    for(int i = 0; i < QImageReader::supportedImageFormats().size(); i++)
    {
        _supportedImageFormats += ('*' + QImageReader::supportedImageFormats().at(i));
        if(i == QImageReader::supportedImageFormats().size()-1) _supportedImageFormats += ')';
        else _supportedImageFormats += ' ';
    }

    _openFileButton->setParent(this);
    _openFileButton->setGeometry(this->width()-50,this->height()-30,50,30);
    connect(_openFileButton, SIGNAL(clicked()), this, SLOT(_openFileButtonIsPressed()));
    _openFileButton->show();

    _imageDisplay->setGeometry(0,0,this->width(),this->height()-_openFileButton->height());
    _imageDisplay->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_openFileButtonIsPressed()
{
    qDebug() << "Opening file!";
    QStringList images = _openFileDialogue(_supportedImageFormats);

    //As long as list isn't empty, we have a file.
    if(images.size())
    {
        _coverFilename = images.at(0);
        _coverImage = new QImage(_coverFilename);
        _scaledImage = new QImage(_coverImage->scaled(this->width(),this->height()-_openFileButton->height()));
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
