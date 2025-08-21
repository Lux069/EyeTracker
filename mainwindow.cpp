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
    , mKaliVerbinder(new Kalibrierung(this))

{
    ui->setupUi(this);

    // Vollbild starten
    showMaximized();

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

    //Implementierung von dem Beobachter Muster via Qt-Signal
    connect(mCameraHandler, &CameraHandler::frameReady,
            this,&MainWindow::displayFrame);

    //Verbinden der Start/Stop Buttons mit CameraHandler
    //Ersatz für den Slot aus Qt-Designer
    connect(ui->pbCamStart, &QPushButton::clicked, mCameraHandler, &CameraHandler::startCam);
    connect(ui->pbCamStop, &QPushButton::clicked, mCameraHandler, &CameraHandler::stopCam);
    connect(ui->pbCamCal, &QPushButton::clicked, mKaliVerbinder, &Kalibrierung::CamCal);
    connect(ui->pbCamCalConfirm, &QPushButton::clicked, mKaliVerbinder, &Kalibrierung::CamCalConfirm);

    //Overlay über dem Kameralabel:
    mOverlay = new Kalibrierung(ui->centralwidget);
    mOverlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    mOverlay->setAttribute(Qt::WA_NoSystemBackground);
    mOverlay->setAttribute(Qt::WA_TranslucentBackground);
    mOverlay->raise();

    // Position und Größe passend über das Label
    mOverlay->resize(ui->label->size());
    mOverlay->move(ui->label->pos());
    mOverlay->show();

    //Overlay update nach Button Klick
    connect(mKaliVerbinder, &Kalibrierung::updateOverlay, mOverlay, QOverload<>::of(&Kalibrierung::update));

}

MainWindow::~MainWindow()
{
    delete ui;
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
