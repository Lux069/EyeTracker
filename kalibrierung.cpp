#include "kalibrierung.h"

void Kalibrierung::CamCal(){
    calFlag = true;
    qDebug() << "Klick";
    mStep = 0;

    if (mStep < showCircle.size()) {
        // Zeige neuen Kreis
        showCircle[mStep] = true;
        update(); // löst paintEvent aus
        emit updateOverlay();  // Overlay updaten
        mStep++;
    }
    else {
        // Alle Kreise wieder ausblenden
        std::fill(showCircle.begin(), showCircle.end(), false);
        mStep = 0;
        update();
        emit updateOverlay();  // Overlay updaten
        calFlag = false;
    }

}

void Kalibrierung::CamCalConfirm(){
    if(calFlag == true){
        if (mStep < showCircle.size()) {
            // Zeige neuen Kreis
            showCircle[mStep] = true;
            update(); // löst paintEvent aus
            emit updateOverlay();  // Overlay updaten
            mStep++;
        }
        else {
            // Alle Kreise wieder ausblenden
            std::fill(showCircle.begin(), showCircle.end(), false);
            mStep = 0;
            update();
            emit updateOverlay();  // Overlay updaten
            calFlag = false;
        }

    }
}

