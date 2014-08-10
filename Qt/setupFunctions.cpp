#ifndef __STEG__SETUP
#define __STEG__SETUP
#include "mainwindow.h"
#include <QHBoxLayout>

void MainWindow::_setupButtons()
{
    connect(_openCoverImageButton, SIGNAL(clicked()), this, SLOT(_openCoverImageButtonIsPressed()));

    QHBoxLayout * buttons = new QHBoxLayout;

    buttons->addWidget(_openCoverImageButton);

    _mainLayout->addLayout(buttons);
}

void MainWindow::_setupImageDisplay()
{
    QHBoxLayout * images = new QHBoxLayout;

    images->addWidget(_imageDisplay);

    _mainLayout->addLayout(images);
}

void MainWindow::_setup()
{
    _setupImageDisplay();
    _setupButtons();

    _centralWidget->setLayout(_mainLayout);
}

#endif
