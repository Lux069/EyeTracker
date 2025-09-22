#ifndef FILTER_H
#define FILTER_H


#include <QObject>              // Basis-Klasse + Q_OBJECT
#include <QPoint>
#include <QImage>               //Container für die Pixeldaten eines Bilds //width()/height()/format() – Metadaten lesen, bits()/scanLine(int) – direkten Pixelzugriff erhalten
#include <opencv2/opencv.hpp>   //um openCV lib zu nutzen
#include <iostream>             //ausgabe mit std::out
#include <QElapsedTimer>        //Zeitmessen für fps und optimierung
#include <QDebug>

#include "gesichtsausschnitt.h"
#include "augenausschnitt.h"
#include "mitteausschnitt.h"
#include "darstellung.h"
#include "darstellungauge.h"
#include "kon.h"

//#################################################
//---------- FILTERN ----------
//#################################################
class Filter: public QObject{
    //Klasse ist zuständig um aus einem übergebenen Bild der Webcam von der Klasse Kammera die Pupillenmitte zu bestimmen.
    //Außerdem werden Makierungen in das original Bild der Webam eingetragen die Gesicht, Augen und Mittelpunkte makieren.
    //Die Mittelpunkte der Pupillen werden in den Public variablen pm_r und pm_l gespeichert und bieten die Schnittstelle für Gaze-Tracking (Oliver)
    Q_OBJECT

public:
    QElapsedTimer fps_zeit; //Zeitmessung für FPS berechnung um Codeperformance zu optimieren

    Filter(QObject* parent=nullptr): QObject(parent){
        //beim Instanziiren von k wird Konstrucktormethode aufgerufen
        //Die Konstrucktormethode startet die Webcam und stellt eine Verbindung her.
        //Beim erhalten eines neuen Frames wird Auslöser-Methode "neues_bild(img_an_filter)" aufgerufen.
        //Durch diese Auslöser-Methode wird in diesem Filter-Obj die Reaktions-Methode "berechne_eye(img_von_kamera)" aufgerufen
        //Der Übergebeparameter img_von_kamera/img_an_filter wird von neues_bild an berechne_eye übergeben
        //Der Mechanismuss, entsteht durch die Methode connect
        // - das Aufrufen einer Auslöser-Methode (auch Signal genannt) im Quellen-obj der Klasse A
        // - bewirkt das Aufrufen einer Empfänger-Methode im Empfänger-obj der Klasse B
        // - wobei ein Parameter von Auslöser- an Empfänger-Methode übergeben wird
        //connect(Quelle-obj, Auslöser-metho_ptr, Empfänger-obj, Reaktion-metho_ptr), übergeben werden/wird Parameter der beiden Methoden
        //QObject::connect(mCamera, &Kamera::neues_bild, this, &Filter::berechne_eye); //in Klass
    }
public slots:

    void berechne_eye(QImage img_von_kamera){

        //qDebug() << "berechne_eye aufgerufen, Frame size:" << img_von_kamera.size();
        //Reaktions-Methode reagiert auf auf Veränderung von QImange von Kammera
        //übergeben wird ein Bild der Kamera "img_von_kamera"
        //die Funktion erzeugt ein QImage mark_img_ganz mit Makierungen für Kopf, Augen und Pupillenmitten.
        //Und zwei QImages für dei beiden Augenausschnitte
        //Diese werden an Fenster weitergegben mit connect
        //if(!fps_zeit.isValid()){fps_zeit.start();};                                                                 //wenn noch nicht gestartet, timer für framezeit starten
        //if(fps_zeit.elapsed() != 0){std::cout << "FPS:  " << (1/((double)fps_zeit.restart()/1000)) << std::endl;};  //timer für framezeit ausgeben und neustarten

        cv::Mat cv_img = Kon::qimage_to_mat(img_von_kamera);                //bild von Kammera zu cv::Mat konvertieren
        cv::Mat cv_grayscale_img = Kon::qimage_to_mat_gray(img_von_kamera); //bild von Kammera zu cv::Mat in graustufen konvertieren

        GesichtsAusschnitt g(cv_grayscale_img, cv::Point(0, 0));        //graustufen cv::Mat Bild von Kammera an Konstrucktor von Gesichtsausschnitt übergeben
        cv::Mat g_extr_teil_bild = g.extr_teil_bild();                  //verhindert doppelten aufruf von g.extr_teil_bild -> kein doppeltes offset, kein doppelte berechnung
        AugenAusschnitt a_r(g_extr_teil_bild, g.offsett, 'r');          //cv::Mat Bild_Ausschnitt von Gesichtsausschnitt an Konstrucktor Augenausschnitt übergeben
        AugenAusschnitt a_l(g_extr_teil_bild, g.offsett, 'l');          // "" (für links)
        MitteAusschnitt m_r(a_r.extr_teil_bild(), a_r.offsett);         //cv::Mat Bild_Ausschnitt von Augenausschnitt an Konstrucktor Mittelausschnitt übergeben
        MitteAusschnitt m_l(a_l.extr_teil_bild(), a_l.offsett);         // "" (für links)
        cv::Mat mittelpixel_r = m_r.extr_teil_bild();                   //Pixel der Pupillenmitte ausschneiden
        cv::Mat mittelpixel_l = m_l.extr_teil_bild();                   // "" (für links)
        cv::Point mittelpunkt_r = m_r.rahmen.tl();                      //Punkt der Pupillenmitte
        cv::Point mittelpunkt_l = m_l.rahmen.tl();                      // "" (für links)
        QPoint pm_r = {mittelpunkt_r.x, mittelpunkt_r.y};                      //cv::Point zu QPoint konvertieren
        QPoint pm_l = {mittelpunkt_l.x, mittelpunkt_l.y};                      // "" (für links)

        //Berechnung von Mittelpunkt zwischen den beiden Pupillen,
        QPoint pm_m = {pm_l.x() + ((pm_r.x()-pm_l.x())/2), pm_l.y() + ((pm_r.y()-pm_l.y())/2)};

        emit pm(pm_m);


        //Konstrucktor von Darstellung enthällt alle Rahmen von Gesicht, Augen und Pupillenmitten, sowie das zu cv::Mat konvertierte Originalbild der Kammera
        //und fügt sie zu einem Bild zusammen
        Darstellung d(cv_img, g.rahmen, a_r.rahmen, a_l.rahmen, m_r.rahmen, m_l.rahmen);
        //Konstrucktor von DarstellungAuge erhält die bearbeiteten Teililder der beiden Augen sowie die Mittelpunkte und fügt sie zu zwei Bildern zusammen
        DarstellungAuge d_a(m_r.filter_teil_bild, m_l.filter_teil_bild, m_r.rahmen_lokal, m_l.rahmen_lokal);
        QImage mark_img_ganz = Kon::mat_to_qimage(d.cv_img);    //das generierte Bild von Objekt d wird von cv::Mat konvertiert zu QImage;
        QImage mark_img_r = Kon::mat_to_qimage(d_a.cv_img_a_r); //das eine generierte Bild von Objekt d_a wird von cv::Mat konvertiert zu QImage;
        QImage mark_img_l = Kon::mat_to_qimage(d_a.cv_img_a_l); //für das andere linke Auge

        emit neues_tracking_bild(mark_img_ganz, mark_img_r, mark_img_l);    //Auslösermethode für connect von Fenster übergibt die markierten Bilder
    };

    //QPoint pm_r;    //public zugriff auf Mittelpunkte der Augen
    //QPoint pm_l;
signals:
    void neues_tracking_bild(QImage mark_img_ganz, QImage mark_img_r, QImage mark_img_l); //übergeben von Gesammtbild und bearbeiteten Augen mit Makierungen an Fenster
    void pm(QPoint Mittelpunkt);
};

#endif // FILTER_H
