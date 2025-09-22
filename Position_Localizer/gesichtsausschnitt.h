#ifndef GESICHTSAUSSCHNITT_H
#define GESICHTSAUSSCHNITT_H

#include "ausschnitt.h"           // Basisklasse Ausschnitt
#include <vector>                 // std::vector
#include <opencv2/opencv.hpp>   //um openCV lib zu nutzen

class GesichtsAusschnitt : public Ausschnitt{
    //Klasse für Ausschnitt des Gesichts. Bekommt Bild von Webcam und gibt Bildausschnitt des Gesichts aus.
public:
    GesichtsAusschnitt(cv::Mat alt, cv::Point ofs): Ausschnitt(alt, ofs){};     //Konstrucktor

    cv::Mat extr_teil_bild() override{

        //Obj von Typ cv::CascadeClassifier namens face_cascade initalisiert mit Pfad zur Haar-Cascarde (static Var bleibt nach aufruf von Methode bestehen)
        //OLLI:
        static cv::CascadeClassifier face_cascade("C:/Users/olive/OneDrive - FH Muenster/MoKoPro/Projekt/EyeTracker/opencv-install/etc/haarcascades/haarcascade_frontalface_default.xml");
        //Marek:
        //static cv::CascadeClassifier face_cascade("/opt/homebrew/Cellar/opencv/4.12.0_1/share/opencv4/haarcascades/haarcascade_frontalface_default.xml");


        std::vector<cv::Rect> faces; //dynamisches Array von Rechtecken für alle identifizierten Faces im Bild
        //Methode wendet Haar-Cascaden-Filter auf alt_teil_bild an. alle herausgefilterten gesichtsbereiche werden in faces als cv::Rect gespeichert
        face_cascade.detectMultiScale(alt_teil_bild, faces, 1.1, 3, cv::CASCADE_SCALE_IMAGE, cv::Size(200,200)); //Paramter von Cascade noch verstehen !!!!

        if (faces.empty()) {                                                    //wenn kein gesicht gefunden wurde
            rahmen = cv::Rect(0, 0, alt_teil_bild.cols, alt_teil_bild.rows);    //rahmen über genzes Bild
        }else{
            rahmen = faces[0];                                                  //rahmen wird als erstes gefundenes Gesicht initalisiert
            int max_flaeche = faces[0].area();                                  //größte Fläche als fläche des erstes Gesicht initalisiert

            for (int i = 1; i < faces.size(); ++i){                             //itteriert über alle gefundenen Gesichter
                int flaeche = faces[i].area();
                if (flaeche > max_flaeche) {                                    //wenn Fläche von aktuellem Gesicht größer als vorheriges größtes Gesicht
                    max_flaeche = flaeche;                                      //dann ist Fläche = neue größte Fläche
                    rahmen = faces[i];                                          //und Rahmen ist aktuelles größtes Gesicht
                }
            }
        }

        cv::Mat teil_bild(alt_teil_bild, rahmen);   //Konstrucktor enthält Bild und Rahmen und instanziirt neues bild mit pixeln innerhalb von Rahmen
        neu_teil_bild = teil_bild;
        verschiebe_rahmen();                        //hier wird das offsett bestimmt was an das nächste obj übergeben wird. (aber keine Verschibung von rahmen)
        return neu_teil_bild;                       //Gibt Teilbild des Gesichts zurück
    };
};

#endif // GESICHTSAUSSCHNITT_H
