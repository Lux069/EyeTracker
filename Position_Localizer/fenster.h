#ifndef FENSTER_H
#define FENSTER_H

#include <QApplication>         //Klasse zum erstellen von Hauptschleife //exec() - startet die Event-Schleife
#include <QWidget>              //Basisklasse für alle Widgets (GUI-Elemente) wie Fenster, Anzeigeflächen, Knöpfe //show(), setParent(), setLayout(),
#include <QLabel>               //widget zum einfachen anzeigen von Bildern/Video oder Text in Fenster //setText(), setPixmap()
#include <QVBoxLayout>          //Positionierung von Widgets //addWidget(QWidget*) – Widget unten anhängen, insertWidget(int, QWidget*) – an Position einfügen
#include <QImage>               //Container für die Pixeldaten eines Bilds //width()/height()/format() – Metadaten lesen, bits()/scanLine(int) – direkten Pixelzugriff erhalten
#include <QPixmap>              //Container für ein einfaches Bild um in QLable anzuzeigen
#include "filter.h"             //Filter liefert das Signal


//#################################################
//---------- ANZEIGE ----------
//#################################################
class Fenster: public QWidget{
    //Klasse empfängt Bilder als Komposition von Filter und stellt es in einem Fenster da.
    //Ablauf: QImage -> QPixmap -> QLabel |Child| -> QVBoxLayout |Child| -> VideoWindow |Parent|
    Q_OBJECT                        //??? ChatGPT sagt: Qt-Metaobjekt ??? wo für ka???
private:
    //##### VARIABLEN/KLASSEN DEFFINIEREN #####
    Filter f;                           //Objekt von Filterklasse (Komposition)
    QPixmap px_img;                     //Volles Webcambild mit Gesichts, Augen und Mittelpunksmakierungen
    QPixmap px_r;                       //Ausschnitt von rechtem Auge
    QPixmap px_l;                       //Ausschnitt von linkem Auge
    QLabel* feld;                       //Anzeigefeld in dem eine Pixmap dargestellt werden kann für px_img
    QLabel* feld_r;                     //für px_r
    QLabel* feld_l;                     //für px_l
    QVBoxLayout* raster;                //Vertikales Layout von QLabels


    //##### CONNECT REAKTIONSMETHODE #####
    void display_img(QImage mark_img_ganz, QImage mark_img_r, QImage mark_img_l){   //Methode wandelt erhaltenes QImage zu Pixmap und sstellt es in einem QLable da
        px_img = QPixmap::fromImage(mark_img_ganz);                                 //Immage zu Pixmap konvertiern
        px_r = QPixmap::fromImage(mark_img_r);
        px_l = QPixmap::fromImage(mark_img_l);
        px_img = px_img.scaledToHeight(700);                                        //auf höhe von 700px scalieren. Verhältniss bleibt
        px_r = px_r.scaledToHeight(300);
        px_l = px_l.scaledToHeight(300);
        feld->setPixmap(px_img);                                                    //Pixmap in Anzeigefeld darstellen
        feld_r->setPixmap(px_r);
        feld_l->setPixmap(px_l);
    };

public:
    //Konstrucktor für Fenster. Instanziirt Objekt w als erstes/oberstes Widget/Fenster "*parent = nullptr"
    //Beim Konstrucktoraufruf wird die cooect-Methode aufgerufen durch welche Bilder von Filter übergeben werden.
    //Der Konstrucktoraufruf instanziirt auch die Anzeigefelder und deren Position im Fenster
    Fenster(QWidget *parent = nullptr) : QWidget(parent){

        //##### WIDGET FÜR VIDEOANZEIGE IM GUI EINFÜGEN #####
        feld = new QLabel(this);    //Anzeigefeld für ganze Webcam; Anzeigefeld-Obj Kind von Fenster-obj (this); zum anzeigen von Pixmap; auf Heap-Speicher
        feld_r = new QLabel(feld);  //Anzeigefeld für rechtes Auge; Anzeigefeld-Obj Kind von feld
        feld_l = new QLabel(feld);  //für linkes Auge

        feld_r->setFixedSize(300, 300); //Größe von Anzeigefeld für rechtes Auge
        feld_l->setFixedSize(300, 300); //für linkes Auge

        feld->setAlignment(Qt::AlignBottom);    //Hauptanzeigefeld an Fensterunterkannte positioniern
        feld_r->move(600, 0);                   //Anzeigefeld für rechtes Auge positionieren
        feld_l->move(0, 0);                     //für linkes Auge

        raster = new QVBoxLayout(this);     //Raster-Obj (Horizontal) Kind von Fenster (this) für Anzeigefelt erstellen auf Heap-Speicher
        raster->addWidget(feld);            //Anzeigefelt in Raster einfügen

        //##### QIMAGE VON KAMERA ZU FENSTER ÜBERGEBEN #####
        //Sender Obj von Klasse Filter, Empfänger Obj (this) von Klasse Fenster. Übergeben werden drei QImages (gesamt, rechtes und linkes Auge)
        QObject::connect(&f, &Filter::neues_tracking_bild, this, &Fenster::display_img); //Aufruf von Statischer Methode connect
    }
};


#endif // FENSTER_H
