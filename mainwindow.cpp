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
   }

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayFrame(QImage &frame)
{
    // Video im Label anzeigen
    if (ui->label) {
        ui->label->setPixmap(QPixmap::fromImage(frame).scaled(
            ui->label->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    } else {
        qDebug() << "Fehler beim Anzeigen des Frames!";
    }
}
