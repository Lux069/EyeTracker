#ifndef DARSTELLUNG_H
#define DARSTELLUNG_H

#include <opencv-install/include/opencv2/core.hpp>   //um openCV lib zu nutzen
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>

class Darstellung{
    //Diese Klasse bekommt von den Ausschnitt-Klassen die Grenzen als Rahmen und das Originalbild der Kammera übergebe.
    //Sie erstellt durch die Methode "zeichen_markierung()" ein cv::Mat Bild in dem alle Grenzen und die Pupillenmitte als grüne Pixel eingezeichnet sind
private:
    cv::Rect rahmen_gesicht;
    cv::Rect rahmen_auge_r;
    cv::Rect rahmen_auge_l;
    cv::Rect rahmen_mitte_r;
    cv::Rect rahmen_mitte_l;

public:
    cv::Mat cv_img;

    Darstellung(cv::Mat cv_img,
                cv::Rect rahmen_gesicht,
                cv::Rect rahmen_auge_r,
                cv::Rect rahmen_auge_l,
                cv::Rect rahmen_mitte_r,
                cv::Rect rahmen_mitte_l)
        : rahmen_gesicht(rahmen_gesicht),
        rahmen_auge_r(rahmen_auge_r),
        rahmen_auge_l(rahmen_auge_l),
        rahmen_mitte_r(rahmen_mitte_r),
        rahmen_mitte_l(rahmen_mitte_l),
        cv_img(cv_img){

        //Zeichnen von Grünen Rechtecken für Gesicht, Augen und Mittelpunkte in Bild von Webcam "cv_img"
        cv::rectangle(cv_img, rahmen_gesicht, cv::Scalar(0, 255, 0, 255), 2, cv::LINE_AA);      //Anti Alising da Bild in Pixmap skalliert
        cv::rectangle(cv_img, rahmen_auge_r, cv::Scalar(0, 255, 0, 255), 2, cv::LINE_AA);
        cv::rectangle(cv_img, rahmen_auge_l, cv::Scalar(0, 255, 0, 255), 2, cv::LINE_AA);
        cv::Rect rahmen_mitte_3x3_r (rahmen_mitte_r.x - 1, rahmen_mitte_r.y - 1, 3, 3);         //Mittelpunkt wird für sichtbarkeit von 1x1 auf 3x3 vergrößert
        cv::Rect rahmen_mitte_3x3_l(rahmen_mitte_l.x - 1, rahmen_mitte_l.y - 1, 3, 3);
        cv::rectangle(cv_img, rahmen_mitte_3x3_r, cv::Scalar(0, 255, 0, 255), 2, cv::LINE_AA);
        cv::rectangle(cv_img, rahmen_mitte_3x3_l, cv::Scalar(0, 255, 0, 255), 2, cv::LINE_AA);
    };
};

#endif // DARSTELLUNG_H
