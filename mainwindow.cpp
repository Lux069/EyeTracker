#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mCameraHandler(new CameraHandler(this))
    , mOverlay(nullptr)

{
    ui->setupUi(this);

    // Vollbild starten
    showMaximized();

    //Overlays initialisieren
    setupOverlays();

    //Signale/Slots verbinden
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processFrame(QImage &frame)
{

    //Hier passier die Magie der Bildverarbeitung
    //TODO: Link zu Mareks Code, welcher mir Koordinaten ausgibt und Kalibrierung

    int eyeX = frame.width() / 2;
    int eyeY = frame.height() / 2;

    // Setze die neuen Koordinaten im Overlay
    if (mEyePainter) {
        mEyePainter->updatePosition(eyeX, eyeY);
    }

    // Zeige den Frame im Label
    displayFrame(frame);
}
void MainWindow::setupOverlays(){
    //Label für Kamerabild wird definiert und skaliert sich mit dem Vollbild zentral mit Randabstand
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 50); // fester Rand
    layout->addWidget(ui->label);

    //Buttonleiste unter dem Kamerabild
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20); // Abstand zwischen Buttons
    buttonLayout->setContentsMargins(0,0,0,0); // kein Extra-Rand hier
    buttonLayout->addWidget(ui->pbCamStart);
    buttonLayout->addWidget(ui->pbCamStop);
    buttonLayout->addWidget(ui->pbCamCal);
    buttonLayout->addWidget(ui->pbCamCalConfirm);

    //Vereinigen der beiden Layouts
    layout->addLayout(buttonLayout);
    ui->centralwidget->setLayout(layout);

    //Overlay über dem Kameralabel:
    mOverlay = new Kalibrierung(ui->centralwidget);
    mOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    mOverlay->setAttribute(Qt::WA_NoSystemBackground);
    mOverlay->setAttribute(Qt::WA_TranslucentBackground);
    mOverlay->raise();
    // Position und Größe passend über das Label
    mOverlay->resize(ui->label->size());
    mOverlay->move(0,0);
    mOverlay->show();


    //Übergabe des Kalibirerstatus zum EyePositionPainter --> Augenposiiton wird gemalt wenn Kalibrierung nicht aktiv ist
    mEyePainter = new EyePositionPainter(ui->centralwidget);
    mEyePainter->setAttribute(Qt::WA_TransparentForMouseEvents);
    mEyePainter->setAttribute(Qt::WA_NoSystemBackground);
    mEyePainter->setAttribute(Qt::WA_TranslucentBackground);
    mEyePainter->raise();
    // Position und Größe passend über das Label
    mEyePainter->resize(ui->label->size());
    mEyePainter->move(0,0);
    mEyePainter->show();
}

void MainWindow::setupConnections(){
    //Übergabe des Webcambildes zur Ausgabe in der GUI
    connect(mCameraHandler, &CameraHandler::frameReady, this,&MainWindow::displayFrame);

    //Verbinden der Start/Stop Buttons mit CameraHandler
    connect(ui->pbCamStart, &QPushButton::clicked, mCameraHandler, &CameraHandler::startCam);
    connect(ui->pbCamStop, &QPushButton::clicked, mCameraHandler, &CameraHandler::stopCam);

    //Verbinden der Buttons aus der GUI mit Kalibrierlogik
    connect(ui->pbCamCal, &QPushButton::clicked, mOverlay, &Kalibrierung::CamCal);
    connect(ui->pbCamCalConfirm, &QPushButton::clicked, mOverlay, &Kalibrierung::CamCalConfirm);

    //Neues Bild aus der Webcam geht an die Bildverarbeitung
    connect(mCameraHandler, &CameraHandler::frameReady, this, &MainWindow::processFrame);
    //Status, ob Kalibirerung aktiv ist geht von Kalibrierlogik zur Darstellung des Blickpunktes
    connect(mOverlay, &Kalibrierung::overlayStatusChanged, mEyePainter, &EyePositionPainter::PaintEyePosition);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (ui->label && mOverlay) {
        mOverlay->resize(ui->label->size());
        mOverlay->move(ui->label->pos());
        mOverlay->update();
    }
}

void MainWindow::displayFrame(QImage &frame)
{
    // Video im Label anzeigen
    if (ui->label) {
        ui->label->setPixmap(QPixmap::fromImage(frame).scaled(
            ui->label->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));

        // Overlay immer passend über das Label legen
        if (mOverlay) {
            mOverlay->resize(ui->label->size());
            mOverlay->move(ui->label->pos());
            mOverlay->update();
        }
    } else {

        qDebug() << "Fehler beim Anzeigen des Frames!";
    }
}
