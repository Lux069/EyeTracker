#include "camerahandler.h"
#include <QMediaCaptureSession>
#include <QDebug>

CameraHandler::CameraHandler(QObject *parent)
    : QObject(parent),
    mCamera(new QCamera(QMediaDevices::defaultVideoInput())),
    mVideoSink(new QVideoSink),
    mCaptureSession(new QMediaCaptureSession)
{
    // Kamera mit CaptureSession verbinden
    mCaptureSession->setCamera(mCamera.data());
    mCaptureSession->setVideoSink(mVideoSink.data());

    // Reagiere auf neue Frames
    connect(mVideoSink.data(), &QVideoSink::videoFrameChanged,
            this, &CameraHandler::onVideoFrameChanged);

    qDebug() << "CameraHandler Konstruktion abgeschlossen.";
}

void CameraHandler::startCam()
{
    mCamera->start();
}

void CameraHandler::stopCam()
{
    mCamera->stop();
}

void CameraHandler::onVideoFrameChanged(const QVideoFrame &frame)
{
    if (!frame.isValid()){
        return;
    }

    QImage image = frame.toImage();
    if (image.isNull()) {
        return;
    }

    emit frameReady(image);
    //qDebug() << "Signal frameReady wurde gesendet.";
}
