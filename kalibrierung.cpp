#include "kalibrierung.h"

void Kalibrierung::CamCal(){
    calFlag = true;
    mCalData.resize(4);
    //mCalData.fill(QPoint(0,0),4);
    //qDebug() << "Klick";
    emit overlayStatusChanged(calFlag);
    mStep = 0;
    showCircle = {false, false, false,false};




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
        emit overlayStatusChanged(calFlag);
    }

}

void Kalibrierung::CamCalConfirm(){
    if(calFlag == true){
        if (mStep < showCircle.size()) {
            mCalData[mStep-1] = mData;

            // Zeige neuen Kreis
            showCircle[mStep] = true;
            update(); // löst paintEvent aus

            if (mStep >= 1){
                showCircle[mStep - 1] = false;
            }
            emit updateOverlay();  // Overlay updaten
            mStep++;
        }
        else {
            // Alle Kreise wieder ausblenden
            mCalData[mStep-1] = mData;
            emit sendCalData(mCalData);

            showCircle = {false, false, false,false};
            mStep = 0;
            update();
            emit updateOverlay();  // Overlay updaten
            calFlag = false;
            emit overlayStatusChanged(calFlag);
        }

    }
}

void Kalibrierung::calibrationData(QPoint position){
    //Hier wird die mData Variable durchgehend mit der Pupillenposition aus Filter geupdated
    if (position != QPoint(0,0)){
        mData = position;
    }
};
