#ifndef DARSTELLUNGAUGE_H
#define DARSTELLUNGAUGE_H

#include <opencv2/opencv.hpp>   //um openCV lib zu nutzen
#include "kon.h"

class DarstellungAuge{
    //Hilfsklasse um AusschnittMitte zu progrmmieren.
    //Gibt cv_img wieder welches nur aus den 2 gefilterten ausschnitten der Augen besteht.
private:
    cv::Rect rahmen_r;  //Pupillenmittelpunkt bezogen auf KS von AugeAusschnitt
    cv::Rect rahmen_l;
public:
    cv::Mat cv_img_a_r; //übergebenes und bearbeitetes Bild von AugeAusschnitt an MitteAusschnitt
    cv::Mat cv_img_a_l;

    DarstellungAuge(cv::Mat a_r,
                    cv::Mat a_l,
                    cv::Rect r_r,
                    cv::Rect r_l)
        : rahmen_r(r_r),
        rahmen_l(r_l),
        cv_img_a_r(a_r),
        cv_img_a_l(a_l){

        //Bilder von Grayscale zu farbe zurückkonvertieren
        cv_img_a_r = Kon::matGray_to_matColor(cv_img_a_r);
        cv_img_a_l = Kon::matGray_to_matColor(cv_img_a_l);

        //Mittelpunkte als Rechtecke einzeichnen in bearbeitetes Bild von Auge
        cv::rectangle(cv_img_a_r, rahmen_r, cv::Scalar(0, 255, 0, 255), 2);
        cv::rectangle(cv_img_a_l, rahmen_l, cv::Scalar(0, 255, 0, 255), 2);

    };
};

#endif // DARSTELLUNGAUGE_H
