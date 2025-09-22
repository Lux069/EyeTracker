#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mCameraHandler(new CameraHandler(this))
    , mOverlay(nullptr)
    , mFilter(new Filter(this))

{
    ui->setupUi(this);
    qRegisterMetaType<QImage>("QImage");

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
    mEyePainter->resize(mOverlay->size());
    mEyePainter->move(0,0);
    mEyePainter->show();
}

void MainWindow::setupConnections(){
    //Übergabe des Webcambildes zur Ausgabe in der GUI
    connect(mCameraHandler, &CameraHandler::frameReady, this, &MainWindow::displayFrame);

    //Verbinden der Start/Stop Buttons mit CameraHandler
    connect(ui->pbCamStart, &QPushButton::clicked, mCameraHandler, &CameraHandler::startCam);
    connect(ui->pbCamStop, &QPushButton::clicked, mCameraHandler, &CameraHandler::stopCam);

    //Verbinden der Buttons aus der GUI mit Kalibrierlogik
    connect(ui->pbCamCal, &QPushButton::clicked, mOverlay, &Kalibrierung::CamCal);
    connect(ui->pbCamCalConfirm, &QPushButton::clicked, mOverlay, &Kalibrierung::CamCalConfirm);

    //Aufsetzen von Multi-Threading für bessere Perfomance
    QThread* workerThread = new QThread(this);
    Filter* worker = new Filter();
    worker->moveToThread(workerThread);

    //Neues Bild aus der Webcam geht an die Bildverarbeitung

    connect(mCameraHandler, &CameraHandler::frameReady,
            this, [this, worker](const QImage &frame) {
                static QElapsedTimer timer; //Timer wird hier gestartet
                if (!timer.isValid() || timer.elapsed() > 250) { // Durchlass eines Bildes alle 250ms ≈ 4 FPS

                    // Das folgende Ungetüm muss gemacht werden, damit die Bilder kontrolliert in dem anderen Thread landen.
                    // Grundsätzlich bedeutet das hier, dass wenn die if-Kondition true ist, ein Bild in die Bearbeitungschlange des Bildverarbeitungs-Thread geschickt wird (QueuedConenction)
                    // Das Q_ARG bedingt, dass das Bild by value übergeben wird --> bessere Performance
                    // Der Fachmann nennt dieses Konstrukt "Lambda", was in diesem Fall nichts anderes als eine gesteurte connect Funktion ist. Quasi wie ein Türsteher im Club, welcher nur ein Bild alle x ms reinlässt.
                    QMetaObject::invokeMethod(worker, "berechne_eye",
                                              Qt::QueuedConnection,
                                              Q_ARG(QImage, frame));
                    timer.restart();
                }
            });
    //Pupillenposition wird aus der Bildverarbeitung zur Darstellung weitergereicht
    connect(worker, &Filter::pm, mEyePainter, &EyePositionPainter::updatePosition);
    //Pupillenposition an Kalibrierung
    connect(worker, &Filter::pm, mOverlay, &Kalibrierung::calibrationData);

    // Thread-Start
    workerThread->start();

    //Status, ob Kalibirerung aktiv ist geht von Kalibrierlogik zur Darstellung des Blickpunktes
    connect(mOverlay, &Kalibrierung::overlayStatusChanged, mEyePainter, &EyePositionPainter::PaintEyePosition);

    //Kalbriervektor geht an das Mapping der Blickposition
    connect(mOverlay, &Kalibrierung::sendCalData, mEyePainter, &EyePositionPainter::getCalData);


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

void MainWindow::displayFrame(const QImage &frame)
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
        if (mEyePainter) {
            mEyePainter->resize(ui->label->size());
            mEyePainter->move(ui->label->pos());
            mEyePainter->update();
        }

    } else {

        qDebug() << "Fehler beim Anzeigen des Frames!";
    }
}
