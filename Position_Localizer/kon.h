#ifndef KON_H
#define KON_H

#include <QImage>               //Container für die Pixeldaten eines Bilds //width()/height()/format() – Metadaten lesen, bits()/scanLine(int) – direkten Pixelzugriff erhalten
#include <opencv2/opencv.hpp>   //um openCV lib zu nutzen
#include <cstring>              //für std::memcpy

//#################################################
//--------------- HILFSMETHODE ---------------
//#################################################
class Kon {
    //Klasse Kon (Konvertierung) enthält statische Methoden zum konvertieren von Bilddatentypen
public:
    static cv::Mat qimage_to_mat(QImage eingeng){
        //methode convertiert QImange zu cv::Mat
        if (eingeng.isNull()){ return cv::Mat();}                                           //gegen error falls leeres Bild übergeben
        QImage q_bild = eingeng.convertToFormat(QImage::Format_ARGB32);                     //convertieren von QImange auf  gleiches format wie cv::Mat Fromat (QImange kan verschidene Formate haben)
        cv::Mat m_bild(q_bild.height(), q_bild.width(), CV_8UC4);                           //neues Bild "m_bild" in Zielformat cv::Mat
        std::memcpy(m_bild.data, q_bild.bits(), static_cast<size_t>(q_bild.sizeInBytes())); //bits von QImage | A | R | G | B | werden in cv::Mat kopier (da Format gleich)
        return m_bild;
    };

    static cv::Mat qimage_to_mat_gray(QImage eingeng){
        //methode convertiert QImange zu cv::Mat mit graufilter
        if (eingeng.isNull()){ return cv::Mat();}
        QImage q_bild = eingeng.convertToFormat(QImage::Format_ARGB32);
        cv::Mat m_bild(q_bild.height(), q_bild.width(), CV_8UC4);
        std::memcpy(m_bild.data, q_bild.bits(), static_cast<size_t>(q_bild.sizeInBytes()));
        cv::Mat gray_bild;                                                                  //leeres cv::Mat Bild für graustufen
        cv::cvtColor(m_bild, gray_bild, cv::COLOR_BGRA2GRAY);                               //wandelt cv::Mat farb bild in graustufen um
        return gray_bild;
    };

    static cv::Mat matGray_to_matColor(cv::Mat mat_gray){
        //Methode konvertiert ein cv::Mat mit 1 kanal (graustufen) zu 4 kanal (farbe). Bild bleibt grau
        if (mat_gray.empty()) {return cv::Mat();}               //gegen error falls leeres Bild übergeben
        cv::Mat matColor;
        cv::cvtColor(mat_gray, matColor, cv::COLOR_GRAY2BGRA);  //Konvertieren zu 4 Kanal | B | G | R | A |
        return matColor;
    };

    static QImage mat_to_qimage(cv::Mat mat_color){
        //Mathode konvertiert 4 Kanal unsignd int (0 - 255) cv::Mat zu Qimage
        if (mat_color.empty()) {return QImage();}                                                               // Gegen Fehler bei leerem Bild
        QImage q_bild(mat_color.data, mat_color.cols, mat_color.rows, mat_color.step, QImage::Format_ARGB32);   //Pixeldaten, Breite, Höhe, Bytes pro Zeile format, 4-Kanal)
        return q_bild;
    };
};

#endif // KON_H
