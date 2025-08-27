#ifndef KALIBRIERUNG_H
#define KALIBRIERUNG_H

#include <QObject>
#include <QWidget>
#include <QPainter>

class Kalibrierung : public QWidget
{
    Q_OBJECT
public:
    explicit Kalibrierung(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents); //Overlay hat keine Interaktion mit der Maus
        setAttribute(Qt::WA_NoSystemBackground); //Es wird kein Hintergrund gefüllt
        setAttribute(Qt::WA_AlwaysStackOnTop); //Bleibt immer über der Kamera
    }
    void CamCal();
    void CamCalConfirm();

signals:
    void updateOverlay();   // Signal: Overlay soll neu malen
    void overlayStatusChanged(bool active);



private:
    int mStep;
    bool calFlag = false;

    QVector<bool> showCircle = {false, false, false, false};
    void paintEvent(QPaintEvent *){

        int r = 30;   // Radius der Kalibrierkreise
        int w = width();
        int h = height();

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::red, 3));
        painter.setBrush(Qt::NoBrush);

        QPoint positions[4] = {
            QPoint(r + 10 , r + 10 ),
            QPoint(w - r -10, r + 10),
            QPoint(r +10, h - r -10),
            QPoint(w - r -10, h - r -10)
        };

        for (int i = 0; i < showCircle.size(); ++i) {
            if (showCircle[i]) {
                painter.drawEllipse(positions[i], r, r);
            }
        }
    }
};



#endif // KALIBRIERUNG_H
