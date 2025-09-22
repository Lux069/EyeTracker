//Eyetracker von Marek Brackmann
//Ziel ist es aus einem Gesichtsframe die Pupillenmitten bestimmen.
//Die Pupillenmitte wird als Pixelposition abgespeichert.
//Als eingabe wurde ein live Video der Webcam verwendet.
//Um die Funktion zu überprüfen wurde die Position der Pupillenmitten als grüne Pixel im live Video eingezeichnet.

//Der Eyetracker bassiert einer Haupt-Quellen zum bestimmen des Pupillenmittelpunktes:
//ACCURATE EYE CENTRE LOCALISATION BY MEANS OF GRADIENTS (Fabian Timm and Erhardt Barth)
//https://thume.ca/projects/2012/11/04/simple-accurate-eye-center-tracking-in-opencv/#:~:text=I%20came%20across%20a%20paper2,the%20image’s%20most%20prominent%20circle
//Einem review des Papers von Tristan Hume:
//Paper: https://www.inb.uni-luebeck.de/fileadmin/files/PUBPDFS/TiBa11b.pdf
//Und einer anleitung zum nutzen von OpenCV Cascaden
//https://www.youtube.com/watch?v=Yq9JKSLke3Q

//Qt 6.9.0, openCV2 4.12.0, macOS 14.6.1 auf MacBook Air M3
#include <QApplication>         //Klasse zum erstellen von Hauptschleife //exec() - startet die Event-Schleife
#include "fenster.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Fenster w;                      //Fenster Instanziiren
    w.showMaximized();              //als maximiertes Fenster anzeigen
    return a.exec();                //Start von Hauptschleife/Eventloop
}


