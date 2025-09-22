#ifndef MITTEAUSSCHNITT_H
#define MITTEAUSSCHNITT_H


#include <opencv2/opencv.hpp>   //um openCV lib zu nutzen
#include <cmath>                //für std::round, std::lround, std::sqrt
#include <algorithm>            //für std::max

#include "ausschnitt.h"          // Basisklasse

class MitteAusschnitt : public Ausschnitt{
    //Diese Klasse berechnet den Pupillenmittelpunkt des übergebenen grayscale Augenausschnitts "alt_teil_bild"
    //Das berechnete Pupillenmittelpunktspixel wird in "neu_teil_bild" zurückgegeben
    //Der berechnete Pupillenmittelpunkt c_opt wird als cv::Rekt in Rahmen gespiechert und im anschluss durch offsett in globale KS transformiert
private:
    cv::Point c_opt;                                    //Pupillenmitte
    cv::Point c;                                        //mögliche Pupillenmitte
    cv::Mat w;                                          //Gewichtung da Pupille schwarz (schwarze Pixel sind warscheinlicher Pupillenmitte)
    cv::Mat g;                                          //Gradienten der Bildpixel (als 2-Kanal [x, y])
    cv::Mat g_betrag;                                   //Betrag von gradient (1-Kanal)
    cv::Mat Kx = (cv::Mat_<float>(3,3) <<   -1,  0,  1, //Sobel Kern für gradientenberechnung
                  -2,  0,  2,
                  -1,  0,  1);

    cv::Mat Ky = (cv::Mat_<float>(3,3) <<   -1, -2, -1,
                  0,  0,  0,
                  1,  2,  1);

public:
    cv::Mat filter_teil_bild;   //Hilfsvariable zum debuggen/ Anzeigen von Gradienten im Augenteilbild
    cv::Rect rahmen_lokal;      //Pupillenmittelpunkt der sich auf das KS von Teilbild "filter_teil_bild" bezieht

    MitteAusschnitt(cv::Mat alt, cv::Point ofs): Ausschnitt(alt,ofs){}; //Konstrucktor

    cv::Mat extr_teil_bild() override{
        alt_teil_bild.convertTo(alt_teil_bild, CV_32FC1);   //von CV_8UC1 (8-Bit int 1-Kanal) -> CV_32FC1 (32-Bit float 1-Kanal)

        int alt_zeilen = alt_teil_bild.rows;  //demension von übergebenen Bild vor downscaling
        int alt_spalten = alt_teil_bild.cols;

        if(alt_zeilen != alt_spalten){  //wenn kein Auge gefunden -> rechteckiger ausschnitt von Gesichtshälfte ist nicht quadratisch
            rahmen.x = 0;               //Pupillenmitte auf KS Ursprung setzen, da keine Mitte gefunden
            rahmen.y = 0;
            rahmen.width = 1;
            rahmen.height = 1;

            cv::Mat teil_bild(alt_teil_bild, rahmen);
            neu_teil_bild = teil_bild;
            return neu_teil_bild;
        }else{

            //##### RUNTERSCALIEREN #####
            int zeilen = 70;                                                                                //Format für downscaling vorgeben
            int spalten = zeilen;                                                                           //da quadratisch
            cv::resize(alt_teil_bild, alt_teil_bild, cv::Size(zeilen, spalten), 0, 0, cv::INTER_LINEAR);    //Runterscalieren von originalgröße auf 70x70 px

            //##### Gausfilter #####
            int k = (int)std::round(spalten * 0.025);                           //Kernelgröße aus Breite bestimmen spalten * 0.025 kommt aus paper (0.005*Gesicht_px ist ca. 0.025*Auge_px)
            k = std::max(k, 3);                                                 //mindestens 3 sonst kaum Glättung
            if ((k % 2) == 0) k += 1;                                           //muss ungerade sein
            cv::GaussianBlur(alt_teil_bild, alt_teil_bild, cv::Size(k, k), 0);  //Filter anwenden
            //filter_teil_bild = alt_teil_bild;                                 //zum debuggen entkommentieren um gefiltertes Augenteilbild anzuzeigen

            //##### GEWICHTE BERECHNEN #####
            w = 255 - alt_teil_bild;                                            //Gewichte sind invertierte Graustufenpixel
            cv::Mat w_1D = w.reshape(1, 1).clone();                             //Gewichtematrix in 1D-Array umwandeln
            cv::sort(w_1D, w_1D, cv::SORT_DESCENDING | cv::SORT_EVERY_ROW);     //absteigend Sortieren
            float w_schwell_5p = w_1D.at<float>(0, (int)(0.05 * w_1D.cols));    //Schwellwert berechnen. Über Schwellwert befinden sich oberen 5% der dunkelsten Pixel


            //##### GRADIENTEN BERECHNEN #####
            g = cv::Mat::zeros(zeilen, spalten, CV_32FC2);          //Matrix für Gradienten mit 0,0 inizialisiern
            g_betrag = cv::Mat::zeros(zeilen, spalten, CV_32FC1);   //Matrix für Gradientenbeträge mit 0 inizialisiern

            //Schleife durchläuft Zeilen und Spalten und lässt Rand von 1px aus da Kern 3x3 ist; Rand ist 0,0
            for (int y = 1; y < zeilen - 1; ++y) {                                      //durchläuft zeilen y
                for (int x = 1; x < spalten - 1; ++x) {                                 //durchläuft spalten x
                    float g_x = 0;                                                      //Gradiant in x-Richtung
                    float g_y = 0;                                                      //Gradiant in y-Richtung
                    for (int k_y = -1; k_y <= 1; ++k_y) {                               //durchläuft zeilen der Kerne Kx und Ky (mitte ist 2,2)
                        for (int k_x = -1; k_x <= 1; ++k_x) {                           //durchläuft zeilen der Kerne Kx und Ky (mitte ist 2,2)
                            float px  = alt_teil_bild.at<float>(y + k_y, x + k_x);      //pixel im Bild; im Datentyp float auslesen, VORSICHT: für cv (rows, colums) = (y, x)
                            float kX_px  = Kx.at<float>(k_y + 1, k_x + 1);              //gewicht aus Kern Kx
                            float kY_px  = Ky.at<float>(k_y + 1, k_x + 1);              //gewicht aus Kern Ky
                            g_x = g_x + px * kX_px;                                     //Faltung
                            g_y = g_y + px * kY_px;                                     //Faltung
                        }
                    }
                    g.at<cv::Vec2f>(y, x)[0] = g_x;                                     //Gradient gx in Kanal 0 des 2D-float-Vektors an Pixelposition (x, y) eintragen
                    g.at<cv::Vec2f>(y, x)[1] = g_y;                                     //Gradient gx in Kanal 1 des 2D-float-Vektors an Pixelposition (x, y) eintragen
                    g_betrag.at<float>(y, x) = sqrt(g_x*g_x+g_y*g_y);                   //Betrag berechnen und in 1-Kanal eintragen
                }
            }

            //Gradienten auf 0 setzen, wenn: gradient < 0.3 * standartabweichung + durchschnitt. (siehe Paper review)
            cv::Scalar mu;                                          //Durchschnitt; cv::Scalar -> Verkor bis 4 dim/Kanal
            cv::Scalar sigma;                                       //Standartabweichung
            cv::Rect ohne_nullen(1, 1, spalten - 2, zeilen - 2);    //Rahmen ohne die Nullen am Rand
            cv::Mat g_betrag_ohne_nullen(g_betrag, ohne_nullen);    //Gradientenmatrix ohne Nullen
            cv::meanStdDev(g_betrag_ohne_nullen, mu, sigma);        //Methode berechnet Durschschnitt und Standartabweichung für jeden Kanal eines Bildes
            float schwell = (float)(mu[0] + 0.3 * sigma[0]);        //Formel aus Paper-review; [0] da nur 1 Kanal

            for (int y = 1; y < zeilen - 1; ++y) {              //forschleife durschläuft alle Zeilen und Spalten
                for (int x = 1; x < spalten - 1; ++x) {
                    if (g_betrag.at<float>(y, x) < schwell) {   //Gradientenbetrag unter berechnetem Schwellwert
                        g.at<cv::Vec2f>(y, x)[0] = 0;           //dann g_x =0
                        g.at<cv::Vec2f>(y, x)[1] = 0;           //und g_y = 0
                        g_betrag.at<float>(y, x) = 0;           //und g_betrag = 0
                    }
                }
            }

            g_betrag.convertTo(filter_teil_bild, CV_8UC1);  //Gradienten des Augenausschnitts werden zum debuggen in Fenster angezeigt; vorher auf erwartetes Format

            //##### AUGENMITTELPUNKT C BESTIMMEN #####
            //Formel aus paper: c* = argmax_c {w_c * sum(d_cp^T * g_p)^2}
            //c     - mögliche Pupillenmitte    (cx, cy)
            //c*    - warscheinliche Pupillenmitte (c_opt.x, c_opt.y)
            //w_c   - Gewichtug der möglichen Pupillenmitten. Gewichtung ist höher für schwarze Pixel.
            //d_cp  - normierter Vektor von aktueller Pixelposition p zur möglichen Pupillenmitte (cx-px, cy-py)
            //g_p   - Gradient an der Aktuellen Pixelposition (g_p[0], g_p[1])

            float summe_opt = 0;                                                        //größe Summe/Maximum an warscheinlicher Pupillenmitte
            //Schleife über alle c mit Koordinaten (cx,cy)
            for (int cy = 1; cy < zeilen - 1; ++cy) {
                for (int cx = 1; cx <  spalten - 1; ++cx) {
                    float w_c = w.at<float>(cy, cx);                                    //hohes gewicht für schwarze pixel in Punkt c
                    if (w_c <= w_schwell_5p){continue;}                                 //wenn pixel nicht schwarz -> ausschließen pupillenmitte (Rechenaufwand!)
                    float summe = 0;                                                    //summe aller quadrierten sklarprodukte für einen punkt c
                    //Schleife über alle p mit Koordinaten (px, py)
                    for (int py = 1; py < zeilen - 1; ++py) {
                        for (int px = 1; px <  spalten - 1; ++px) {
                            cv::Vec2f g_p = g.at<cv::Vec2f>(py, px);                    //Gradient als 2D-float Vekor auslesen
                            if (px == cx && py == cy) { continue; }                     //wenn kein d_cp vorhanden; schutz vor div durch 0 bei laenge = 0; (Rechenaufwand!)
                            if (g_betrag.at<float>(py, px) == 0) {continue;}            //wenn g = (0,0); (Rechenaufwand!)

                            // ormiertes displacment d_cp berehcnen d_cp = p - c / ||p - c||
                            float d_cp_x = (float)(px - cx);
                            float d_cp_y = (float)(py - cy);
                            float laenge = std::sqrt(d_cp_x*d_cp_x + d_cp_y*d_cp_y);
                            d_cp_x = d_cp_x/laenge;
                            d_cp_y = d_cp_y/laenge;

                            float summand = d_cp_x * g_p[0] + d_cp_y * g_p[1];  // gradienten g_p per skalarprodukt mit d_cp multiplizieren. scalar_p = d_cp^T * g_p
                            if (summand > 0) {                                  // nur d_cp^T * g_p > 0 berücksichtigen. Vektoren zeigen in gleiche Richtung (s.h Paper)
                                summe += summand * summand;                     // summand berechen: summand = (d_cp^T * g_p)^2
                            }
                        }
                    }
                    //wenn summe in c größer als vorheriger schleifendurchlauf c_opt = c
                    w_c = 1; //Auskommentieren um Gewichte zu deaktivieren, z.B. bei starker heller reflektion von Bildschrim in Pupille
                    if (w_c * summe > summe_opt) {
                        c_opt.x = cx;
                        c_opt.y = cy;
                        summe_opt = w_c * summe;
                    }
                }
            }

            //Berechneten Punkt als cv::Rekt umwandeln, da rect das erwartete Format
            rahmen.x = c_opt.x;
            rahmen.y = c_opt.y;
            rahmen.width = 1;
            rahmen.height = 1;

            rahmen_lokal = rahmen;  //abspeichern von Punkt in lokalem KS von Auge vor der Verschiebung

            cv::Mat teil_bild(alt_teil_bild, rahmen);       //Konstrucktor enthält Bild und Rahmen und instanziirt neues bild mit pixeln innerhalb von Rahmen
            teil_bild.convertTo(neu_teil_bild, CV_8UC1);    //Rückkonvertierung von CV_32FC1 zurück zu 8

            //Verschiebung von rahmen wegen down scaling
            rahmen.x = (int)(std::lround((double)rahmen.x * (double)alt_spalten / (double)spalten));
            rahmen.y = (int)(std::lround((double)rahmen.y * (double)alt_zeilen / (double)zeilen));

            //Rahmen verschieben in globales KS, durch das übergebene Offset des vorherigen AugeAusschnitt Objekt.
            rahmen.x = rahmen.x + offsett.x;
            rahmen.y = rahmen.y + offsett.y;
            return neu_teil_bild;
        };  //ende else
        };
};

#endif // MITTEAUSSCHNITT_H
