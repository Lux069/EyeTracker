#ifndef AUGENAUSSCHNITT_H
#define AUGENAUSSCHNITT_H

#include <opencv2/core.hpp> //um openCV lib zu nutzen
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "ausschnitt.h"         // Basisklasse

class AugenAusschnitt : public Ausschnitt{
    //Klasse für Ausschnitt von einem Auge. Bekommt Bildausschnitt des Gesichts und gibt Bildausschnitt von einem Auge zurück.
private:
    char seite; //var bestimmt ob rechtes oder linkes Auge ausgeschnitten wird

public:
    AugenAusschnitt(cv::Mat alt, cv::Point ofs, char s): Ausschnitt(alt, ofs), seite(s){};  //Konstrucktor

    cv::Mat extr_teil_bild() override{

        //Obj von Typ cv::CascadeClassifier namens eye_cascade initalisiert mit Pfad zur Haar-Cascarde (static Var bleibt nach aufruf von Methode bestehen)
        //OLLI:
        static cv::CascadeClassifier eye_cascade("C:/Users/olive/OneDrive - FH Muenster/MoKoPro/Projekt/EyeTracker/opencv-install/etc/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
        //Marek:
        //static cv::CascadeClassifier eye_cascade("/opt/homebrew/Cellar/opencv/4.12.0_1/share/opencv4/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
        std::vector<cv::Rect> eyes; //dynamisches Array von Rechtecken für alle identifizierten augen im gesichts-teil-bild

        cv::Rect rahmen_seite;                              //neue Teilbildgrenzen von rechter oder linker Gesichtshälfte je nach instanziirung mit 'r' oder 'l'
        if (seite == 'r'){
            rahmen_seite.x = alt_teil_bild.cols/2;          //startet in hälfte des Teil-bilds von alt_teil_bild
            rahmen_seite.y = 0;
            rahmen_seite.width = alt_teil_bild.cols/2;      //neuer rahmen ist halb so breit -> hälfte des Gesichts
            rahmen_seite.height = alt_teil_bild.rows;
            offsett.x = offsett.x + alt_teil_bild.cols/2;   //des rechte Teilbild muss in das KS von dem übergebenen alt_teil_bild verschoben werden
        }else{
            rahmen_seite.x = 0;                             //startet an link Ecke des Teil-bilds alt_teil_bild
            rahmen_seite.y = 0;
            rahmen_seite.width = alt_teil_bild.cols/2;      //neuer rahmen ist halb so breit -> hälfte des Gesichts
            rahmen_seite.height = alt_teil_bild.rows;
        };

        cv::Mat alt_teil_bild_seite(alt_teil_bild, rahmen_seite); //aus neuen Teilbildgrenzen und übergebenen Teilbild "alt_teil_bild" wird neues Teilbild ausgeschnitten

        //Methode wendet Haar-Cascaden-Filter auf alt_teil_bild_seite an. alle herausgefilterten Augenbereiche werden in faces als cv::Rect gespeichert
        eye_cascade.detectMultiScale(alt_teil_bild_seite, eyes, 1.1, 3, cv::CASCADE_SCALE_IMAGE, cv::Size(20,20));


        if (eyes.empty()) {                                                                 //wenn kein Auge gefunden wurde
            rahmen = cv::Rect(0, 0, alt_teil_bild_seite.cols, alt_teil_bild_seite.rows);    //rahmen über Gesichtshälfte
        }else{
            rahmen = eyes[0];                               //rahmen wird als erstes gefundenes Auge initalisiert
            int max_flaeche = eyes[0].area();               //größte Fläche als Fläche des ersten Auge initalisiert
            for (int i = 1; i < eyes.size(); ++i) {         //itteriert über alle gefundenen Augen in Gesichtshälfte
                int flaeche = eyes[i].area();
                if (flaeche > max_flaeche) {                //wenn Fläche von aktuellem Auge größer als vorheriges größtes Auge
                    max_flaeche = flaeche;                  //dann ist Fläche = neue größte Fläche
                    rahmen = eyes[i];                       //und Rahmen ist aktuelles größtes Auge
                }
            }
        }

        cv::Mat teil_bild(alt_teil_bild_seite, rahmen); //Konstrucktor enthält Bild und Rahmen und instanziirt neues bild mit pixeln innerhalb von Rahmen
        neu_teil_bild = teil_bild;
        verschiebe_rahmen();                            //lokalen Rahmenposition in globalen Rahmenposition umwandeln.
        return neu_teil_bild;
    };
};

#endif // AUGENAUSSCHNITT_H
