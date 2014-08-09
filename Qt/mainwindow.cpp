#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _openFile(new QPushButton("Open..."))
{
    ui->setupUi(this);

    _openFile->setParent(this);
    _openFile->setGeometry(0,0,50,30);
    connect(_openFile, SIGNAL(clicked()), this, SLOT(_openFileButtonIsPressed()));
    _openFile->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_openFileButtonIsPressed()
{
    qDebug() << "Opening file!";
    QStringList images = _openFileDialogue();

    if(images.size()) _origFilename = images.at(0);
}

QStringList MainWindow::_openFileDialogue()
{
    QString supportedFormats = "Images (";
    for(int i = 0; i < QImageReader::supportedImageFormats().size(); i++)
    {
        supportedFormats += ('*' + QImageReader::supportedImageFormats().at(i));
        if(i == QImageReader::supportedImageFormats().size()-1) supportedFormats += ')';
        else supportedFormats += ' ';
    }

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
