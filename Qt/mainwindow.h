#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStringList>
#include <QFileDialog>
#include <QImage>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //Open File button
    void _openCoverImageButtonIsPressed();

private:
    Ui::MainWindow *ui;

    //Open file button
    QPushButton * _openCoverImageButton;

    //Open File Dialogue
    QStringList _openFileDialogue(const QString &);

    //Image
    QString _coverFilename;
    QString _stegFilename;
    QString _supportedImageFormats;
    QImage * _coverImage;
    QImage * _scaledImage;
    QLabel * _imageDisplay;
};

#endif // MAINWINDOW_H
