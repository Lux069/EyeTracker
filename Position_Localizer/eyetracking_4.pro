QT += widgets multimedia
CONFIG += c++17 app_bundle link_pkgconfig
TEMPLATE = app

PKGCONFIG += opencv4

SOURCES += main.cpp \
    augenausschnitt.cpp \
    ausschnitt.cpp \
    darstellung.cpp \
    darstellungauge.cpp \
    fenster.cpp \
    filter.cpp \
    gesichtsausschnitt.cpp \
    kamera.cpp \
    kon.cpp \
    mitteausschnitt.cpp

macx: QMAKE_INFO_PLIST = info.plist
DISTFILES += info.plist

QMAKE_RPATHDIR += /opt/homebrew/opt/opencv/lib

# >>> Cascades in Contents/Resources kopieren
QMAKE_BUNDLE_DATA += cascades
cascades.files = /opt/homebrew/share/opencv4/haarcascades/haarcascade_frontalface_default.xml \
                 /opt/homebrew/share/opencv4/haarcascades/haarcascade_eye_tree_eyeglasses.xml
cascades.path = Contents/Resources

HEADERS += \
    augenausschnitt.h \
    ausschnitt.h \
    darstellung.h \
    darstellungauge.h \
    fenster.h \
    filter.h \
    gesichtsausschnitt.h \
    kamera.h \
    kon.h \
    mitteausschnitt.h
