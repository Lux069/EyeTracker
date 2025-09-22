#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QHBoxLayout>
#include "camerahandler.h"
#include "eyepositionpainter.h"
#include "kalibrierung.h"
#include "Position_Localizer/filter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void displayFrame(const QImage &frame);
    void resizeEvent(QResizeEvent *event);
private:
    Ui::MainWindow *ui;
    CameraHandler *mCameraHandler;
    QLabel *mVideoLabel;
    Kalibrierung *mOverlay = nullptr;
    EyePositionPainter *mEyePainter;
    Filter *mFilter;
    void setupConnections();
    void setupOverlays();
    };



#endif // MAINWINDOW_H
