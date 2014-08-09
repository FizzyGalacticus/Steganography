#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _openFileButton(new QPushButton("Open...")),
    _imageDisplay(new QLabel(this))
{
    ui->setupUi(this);

    _openFileButton->setParent(this);
    _openFileButton->setGeometry(this->width()-50,this->height()-30,50,30);
    connect(_openFileButton, SIGNAL(clicked()), this, SLOT(_openFileButtonButtonIsPressed()));
    _openFileButton->show();

    _imageDisplay->setGeometry(0,0,this->width(),this->height()-_openFileButton->height());
    _imageDisplay->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_openFileButtonButtonIsPressed()
{
    qDebug() << "Opening file!";
    QStringList images = _openFileDialogue();

    //As long as list isn't empty, we have a file.
    if(images.size())
    {
        _origFilename = images.at(0);
        _origImage = new QImage(_origFilename);
        _scaledImage = new QImage(_origImage->scaled(this->width(),this->height()-_openFileButton->height()));
        _imageDisplay->setPixmap(QPixmap::fromImage(*_scaledImage));
    }

    qDebug() << "File opened!";
}

QStringList MainWindow::_openFileDialogue()
{
    //Create a string that contains all currently supported image formats
    QString supportedFormats = "Images (";
    for(int i = 0; i < QImageReader::supportedImageFormats().size(); i++)
    {
        supportedFormats += ('*' + QImageReader::supportedImageFormats().at(i));
        if(i == QImageReader::supportedImageFormats().size()-1) supportedFormats += ')';
        else supportedFormats += ' ';
    }

    //Setup file dialogue
    QFileDialog openFileDialog(this);
    openFileDialog.setDirectory(QDir::homePath().append("/My Pictures/"));
    openFileDialog.setNameFilter(tr(supportedFormats.toStdString().c_str()));
    openFileDialog.setViewMode(QFileDialog::List);
    openFileDialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList fileNames;

    if(openFileDialog.exec())
       fileNames = openFileDialog.selectedFiles();

    return fileNames;
}
