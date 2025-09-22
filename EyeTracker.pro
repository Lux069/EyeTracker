QT       += core gui
QT       += multimedia

#MAC
CONFIG += c++17 app_bundle link_pkgconfig
TEMPLATE = app
#MAC-ENDE

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Position_Localizer/augenausschnitt.cpp \
    Position_Localizer/ausschnitt.cpp \
    Position_Localizer/darstellung.cpp \
    Position_Localizer/darstellungauge.cpp \
    Position_Localizer/filter.cpp \
    Position_Localizer/gesichtsausschnitt.cpp \
    Position_Localizer/kon.cpp \
    Position_Localizer/mitteausschnitt.cpp \
    camerahandler.cpp \
    eyepositionpainter.cpp \
    kalibrierung.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Position_Localizer/augenausschnitt.h \
    Position_Localizer/ausschnitt.h \
    Position_Localizer/darstellung.h \
    Position_Localizer/darstellungauge.h \
    Position_Localizer/filter.h \
    Position_Localizer/gesichtsausschnitt.h \
    Position_Localizer/kon.h \
    Position_Localizer/mitteausschnitt.h \
    camerahandler.h \
    eyepositionpainter.h \
    kalibrierung.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PWD/opencv-install/include
INCLUDEPATH += $$PWD/opencv-install/include/opencv2
INCLUDEPATH += $$PWD/opencv-install/etc


LIBS += -L"$$PWD/opencv-install/x64/mingw/lib" \
        -lopencv_core4120 \
        -lopencv_imgproc4120 \
        -lopencv_highgui4120 \
        -lopencv_videoio4120 \
        -lopencv_objdetect4120


