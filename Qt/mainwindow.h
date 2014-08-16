#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStringList>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void resizeEvent (QResizeEvent *);

private slots:
    //Open File buttons
    void _openCoverImageButtonIsPressed();
    void _openPayloadButtonIsPressed();
    void _hidePayloadButtonIsPressed();

private:
    Ui::MainWindow *ui;

    //Central Widget & Layout
    QWidget * _centralWidget;
    QVBoxLayout * _mainLayout;

    //Buttons
    QPushButton * _openCoverImageButton;
    QPushButton * _openPayloadButton;
    QPushButton * _hidePayloadButton;

    //Open File Dialogue
    QStringList _openFileDialogue(const QString &);

    //Image Manipulation Variables
    QString _coverFilename;
    QString _supportedImageFormats;
    QImage * _coverImage;
    QImage * _scaledImage;
    QLabel * _imageDisplay;

    //Payload File Name Variable
    QStringList * _payloads;

    //Setup functions
    void _setup();
    void _setupImageDisplay();
    void _setupButtons();

    //Get bits from files
    const QVector<bool> * getBitsFromPayloads();
    const QByteArray * getBytesFromFile(const QString &);
    const QVector<bool> * getBitsFromBytes(const QByteArray *);

    //Send bits to files
    const QByteArray * getBytesFromBits(const QVector<bool> *);
    bool writeBytesToFile(const QByteArray *, const QString &);
};

#endif // MAINWINDOW_H
