#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <QObject>
#include <QCamera>
#include <QMediaDevices>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>
#include <QScopedPointer>
#include <QMediaCaptureSession>
#include <QPermissions>


class CameraHandler : public QObject
{
    Q_OBJECT
    //Wird gebraucht, um später mit connect() auf diese Klasse zuzugreifen.
    //Das macht es einfacher mit Pointern umzugehen da man die dann nicht manuell löschen muss.
public:
    CameraHandler(QObject *parent = nullptr);
    void startCam();
    void stopCam();

signals:
    // Ein Signal wird ausgegeben, wenn ein neues Bild von der Kamera verfügbar ist.
    void frameReady(const QImage &frame); // const, damit das Signal das exakte Bild übergibt


private slots:
    void onVideoFrameChanged(const QVideoFrame &frame);

private:
    QScopedPointer<QCamera> mCamera; //Qt Version von einem unique Pointer, ich will hier nicht die Standardbibliothek mitschleppen
    QScopedPointer<QVideoSink> mVideoSink;
    QScopedPointer<QMediaCaptureSession> mCaptureSession;
    QCameraPermission perm;

};

#endif // CAMERAHANDLER_H
