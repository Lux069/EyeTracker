#ifndef EYEPOSITIONPAINTER_H
#define EYEPOSITIONPAINTER_H

#include <QObject>
#include <QWidget>
#include <QPainter>

class EyePositionPainter : public QWidget
{
    Q_OBJECT
public:
    explicit EyePositionPainter(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents); //Overlay hat keine Interaktion mit der Maus
        setAttribute(Qt::WA_NoSystemBackground); //Es wird kein Hintergrund gefüllt
        setAttribute(Qt::WA_AlwaysStackOnTop); //Bleibt immer über der Kamera
    }

    void setEyePosition(int x, int y)
    {
        mEyeX = x;
        mEyeY = y;
        update();  // Löst Neuzeichnung (paintEvent) aus
    };

public slots:
    void PaintEyePosition(bool calActive);
    void updatePosition(int x, int y){
        if (!mCalActive) {
            mEyeX = x;
            mEyeY = y;
            update();  // Löst Neuzeichnung aus
        };
    }

private:

    int mEyeX;         // X-Position des Kreises
    int mEyeY;         // Y-Position des Kreises
    bool mCalActive = false;

    void paintEvent(QPaintEvent *){

         int r = 30;   // Radius des Kreises zur Augenpositionsdarstellung

    if (!mCalActive) {
         QPainter painter(this);
         painter.setRenderHint(QPainter::Antialiasing);
         painter.setPen(QPen(Qt::yellow, 3));
         painter.setBrush(Qt::NoBrush);
         painter.drawEllipse(QPoint(mEyeX - r/2 ,mEyeY - r/2) , r , r);
    } else {}
    };
};

#endif // EYEPOSITIONPAINTER_H
