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
#include <QProgressBar>
#include <QMessageBox>

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
    //Buttons
    void _openCoverImageButtonIsPressed();
    void _openPayloadButtonIsPressed();
    void _hidePayloadButtonIsPressed();
    void _openStegImageButtonIsPressed();
    void _recoverFilesButtonIsPressed();

private:
    Ui::MainWindow *ui;

    //Central Widget & Layout
    QWidget * _centralWidget;
    QVBoxLayout * _mainLayout;

    //Message Box
    QMessageBox * _popUpMessage;

    //Image Labels
    QLabel * _coverLabel;
    QLabel * _stegLabel;
    QLabel * _coverImageDisplay;
    QLabel * _stegImageDisplay;

    //Buttons
    QPushButton * _openCoverImageButton;
    QPushButton * _openPayloadButton;
    QPushButton * _hidePayloadButton;
    QPushButton * _openStegImageButton;
    QPushButton * _recoverFilesButton;

    //Open File Dialogue
    QStringList _openFileDialogue(const QString &);

    //Image Manipulation Variables
    QString _coverFilename;
    QString _stegFilename;
    QString _supportedImageFormats;
    QImage * _coverImage;
    QImage * _stegImage;

    //Payload Variables
    QStringList * _payloadFilenames;
    int numberOfPayloads;
    const QVector<QVector<bool> * > * _payloadBits;

    //Progress Bar
    QProgressBar * _progressBar;

    //Setup functions
    void _setup();
    void _setupLabels();
    void _setupButtons();
    void _setupProgressBar();

    //General bit manipulation
    const QVector<bool> * getBitsFromNumber(const unsigned int &);
    unsigned int getNumberOfFilesFromBits(const QVector<bool> *);
    QVector<bool> * mergeBits(const QVector<bool> *, const QVector<bool> *);

    //Get bits from files
    QVector<bool> * getBitsFromPayloads();
    const QByteArray * getBytesFromFile(const QString &);
    const QVector<bool> * getBitsFromBytes(const QByteArray *);
    void getPayloadBits(const QByteArray *);

    //Send bits to files
    const QByteArray * getBytesFromBits(const QVector<bool> *);
    bool writeBytesToFile(const QByteArray *, const QString &);

    //Put bits into image
    void putBitsIntoImage(QVector<bool> *);
    int putBitIntoNumber(const int &,const bool &);
    QRgb putBitsIntoRGB(QVector<bool> *,QRgb);

};

#endif // MAINWINDOW_H
