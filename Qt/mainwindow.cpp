#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

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
}
