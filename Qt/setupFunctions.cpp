#ifndef __STEG__SETUP
#define __STEG__SETUP
#include "mainwindow.h"
#include <QHBoxLayout>


void MainWindow::_setupProgressBar()
{
    QHBoxLayout * progressBarLayout = new QHBoxLayout;

    _progressBar->setTextVisible(false);

    progressBarLayout->addWidget(_progressBar);

    _mainLayout->addLayout(progressBarLayout);
}

void MainWindow::_setupButtons()
{
    connect(_openCoverImageButton, SIGNAL(clicked()), this, SLOT(_openCoverImageButtonIsPressed()));
    connect(_openPayloadButton, SIGNAL(clicked()), this, SLOT(_openPayloadButtonIsPressed()));
    connect(_hidePayloadButton, SIGNAL(clicked()), this, SLOT(_hidePayloadButtonIsPressed()));

    QHBoxLayout * buttons = new QHBoxLayout;

    buttons->addWidget(_openCoverImageButton);
    buttons->addWidget(_openPayloadButton);
    buttons->addWidget(_hidePayloadButton);

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
    _setupProgressBar();
    _setupButtons();

    _centralWidget->setLayout(_mainLayout);
}

#endif
