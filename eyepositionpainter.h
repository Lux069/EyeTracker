#ifndef EYEPOSITIONPAINTER_H
#define EYEPOSITIONPAINTER_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <cmath>

class EyePositionPainter : public QWidget
{
    Q_OBJECT
public:
    EyePositionPainter(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents); //Overlay hat keine Interaktion mit der Maus
        setAttribute(Qt::WA_NoSystemBackground); //Es wird kein Hintergrund gefüllt
        setAttribute(Qt::WA_AlwaysStackOnTop); //Bleibt immer über der Kamera
    }

public slots:
    void PaintEyePosition(bool calActive);
    void updatePosition(QPoint pupil);
    void getCalData(QVector<QPoint> calData){calVector = calData; qDebug() << "Dieser Vektor wurde übergeben: " << calVector;}

private:
    QVector<QPoint> calVector = {QPoint(0,0),QPoint(width(),0), QPoint(0,height()), QPoint(width(), height()) };
    int mEyeX;         // X-Position des Kreises
    int mEyeY;         // Y-Position des Kreises
    bool mCalActive = false;
    void paintEvent(QPaintEvent *){

         int r = 50;   // Radius des Kreises zur Augenpositionsdarstellung

    if (!mCalActive) {
         QPainter painter(this);
         painter.setRenderHint(QPainter::Antialiasing);
         painter.setPen(QPen(Qt::cyan, 5));
         painter.setBrush(Qt::NoBrush);
         painter.drawEllipse(QPoint(mEyeX - r/2 ,mEyeY - r/2) , r , r);
    } else {}
    };
};

#endif // EYEPOSITIONPAINTER_H
