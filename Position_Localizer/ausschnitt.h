#ifndef AUSSCHNITT_H
#define AUSSCHNITT_H

#include <opencv-install/include/opencv2/core.hpp>   //um openCV lib zu nutzen

//#################################################
//---------- MITTELPUNKT BESTIMMEN ----------
//#################################################
class Ausschnitt {
    //Quadratischer Ausschnitt eines Bildes vom Typ cv::Mat. Klasse enthält das Originalbild "alt_teil_bild" und erzeut einen
    //rechteckigen Aussschnitt der Region of Interest (ROI) "neu_teil_bild". Um aus einem Orignialbild die neu ROI zu erhalten wird die Methode
    //"extr_teil_bild()" genutzt. Der Rückgabe Wert der Methode ist die ROI "neu_teil_bild".
    //Die Grenzen des ausgeschnittenen Bereichs werden im cv::Rect "rahmen" gespeichert.
    //Die Breite und Höhe sowie die obere linke Ecke des Rahmens in Bezug auf des verabeitete Bild sind im Rahmenobjekt gespeichert.
    //Um die Position des Rahmens nicht nur in Bezug auf das verarbeitete Bild, sondern in bezug auf des Ausgangsbild zu erhalten,
    //wird jeweils dem Rahmen die Verschiebung des letzten Rahmens durch die Variable "offsett" dazuaddiert. (Koordinatentransformation)
    //Diese ROI und das neue Offset wird an ein Objekt der nächsten Klasse bei der instanziirung übergeben.
    //Die Abfolge ist: GesichtsAusschnitt -> AugenAusschnitt -> MitteAusschnitt. In der letzten Klasse "MitteAusschnitt" ist die ROI
    //sowie der Rahmen nur ein einziger Pixel, die Pupillenmitte.
    //Instanziirung:
    /*
        GesichtsAusschnitt g(cv_grayscale_img, cv::Point(0, 0));        //graustufen cv::Mat Bild von Kammera an Konstrucktor von Gesichtsausschnitt übergeben
        cv::Mat g_extr_teil_bild = g.extr_teil_bild();                  //verhindert doppelten aufruf von g.extr_teil_bild -> kein doppeltes offset, kein doppelte berechnung
        AugenAusschnitt a_r(g_extr_teil_bild, g.offsett, 'r');          //cv::Mat Bild_Ausschnitt von Gesichtsausschnitt an Konstrucktor Augenausschnitt übergeben
        AugenAusschnitt a_l(g_extr_teil_bild, g.offsett, 'l');          // "" (für links)
        MitteAusschnitt m_r(a_r.extr_teil_bild(), a_r.offsett);         //cv::Mat Bild_Ausschnitt von Augenausschnitt an Konstrucktor Mittelausschnitt übergeben
        MitteAusschnitt m_l(a_l.extr_teil_bild(), a_l.offsett);         // "" (für links)
        cv::Mat mittelpixel_r = m_r.extr_teil_bild();                   //Pixel der Pupillenmitte ausgeschnitten
        cv::Mat mittelpixel_l = m_l.extr_teil_bild();                   // "" (für links)
        cv::Point mittelpunkt_r = m_r.rahmen.tl();                      //Punkt der Pupillenmitte
        cv::Point mittelpunkt_l = m_l.rahmen.tl();                      // "" (für links)
    */
protected:
    cv::Mat alt_teil_bild;  //übergebene ausgeschnittener Bild-Bereich (ROI - region of interest) als Mat
    cv::Mat neu_teil_bild;  //extrahierter ausgeschnittener Bild-Bereich (ROI - region of interest) als Mat
public:
    cv::Rect rahmen;        //Grenzen des ausgeschnittenen Bild-Bereichs
    cv::Point offsett;      //verschiebung des Rahmens für transformation in globalen KS

    Ausschnitt(cv::Mat alt, cv::Point ofs): alt_teil_bild(alt), offsett(ofs){}; //Konstrucktor bekommt das vorherig extraierte Teilbild (zu begin das gnaze Bild) und den letzten offset
    virtual cv::Mat extr_teil_bild() = 0;                                       //Methode extrahiert eine neue ROI/Teilausschnitt
    void verschiebe_rahmen(){
        //Mathode verschiebt die Koordinaten des Rahmens eines neuen Teilbilds ins globals KS und ändert anschließend das Offset für die nächste Instanziirung
        rahmen.x = rahmen.x + offsett.x;  //Rahmen verschieben durch das übergebene Offset des vorherigen Objekts
        rahmen.y = rahmen.y + offsett.y;
        offsett.x = rahmen.x;             //zu übergebene Offsett für das nächste Objekt ändern (dieses wird im Konstrucktor übergeben)
        offsett.y = rahmen.y;
    };
};

#endif // AUSSCHNITT_H
