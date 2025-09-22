#include "eyepositionpainter.h"

void EyePositionPainter::PaintEyePosition(bool calActive){
    mCalActive = calActive;
    update();
}

void EyePositionPainter::updatePosition(QPoint pupil){
    if (!mCalActive) {

        double relX = double(pupil.x() - calVector[0].x()) /
                      double(calVector[1].x() - calVector[0].x());
        double relY = double(pupil.y() - calVector[0].y()) /
                      double(calVector[2].y() - calVector[0].y());

        relX = std::clamp(relX, 0.0, 1.0);
        relY = std::clamp(relY, 0.0, 1.0);

        // 2) Auf Overlay abbilden
        mEyeX = int(relX * width());
        mEyeY = int(relY * height());

        qDebug() << "calVector:" << calVector[0] << calVector[1]
                 << calVector[2] << calVector[3];

        qDebug() << "pupil:" << pupil << "-> rel:" << relX << relY
                 << "-> mapped:" << mEyeX << mEyeY;


        update();  // Löst Neuzeichnung aus
    };
}

