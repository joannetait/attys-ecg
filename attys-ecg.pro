unix:!macx {

MOC_DIR = moc

OBJECTS_DIR = obj

LIBS += \
    -L/usr/local/lib \
    -lqwt-qt5 \
    -liir \
    -lattyscomm \
    -lbluetooth \
    -lfir

INCLUDEPATH += /usr/include/qwt

TMAKE_CXXFLAGS += -fno-exceptions

SOURCES = \
    dataplot.cpp \
    main.cpp \
    attys-ecg.cpp \
    ecg_rr_det.cpp\
    dialog.cpp\
    wigglywidget.cpp

HEADERS = \
    attys-ecg.h \
    dataplot.h \
    ecg_rr_det.h\
    dialog.h\
    wigglywidget.h

CONFIG		+= qt release c++11

QT             	+= widgets

RESOURCES     = application.qrc

target.path     = /usr/local/bin
INSTALLS        += target

}



macx {

MOC_DIR = moc

OBJECTS_DIR = obj

LIBS += \
    -L/usr/local/lib \
    -liir \
    -lattyscomm \
    -lfir

INCLUDEPATH += /usr/local/include

include ( /usr/local/Cellar/qwt/6.1.5/features/qwt.prf )

TMAKE_CXXFLAGS += -fno-exceptions

SOURCES = \
    dataplot.cpp \
    main.cpp \
    attys-ecg.cpp \
    ecg_rr_det.cpp

HEADERS = \
    attys-ecg.h \
    dataplot.h \
    ecg_rr_det.h

CONFIG		+= qt release c++11 qwt

QT             	+= widgets

RESOURCES     = application.qrc

target.path     = /usr/local/bin
INSTALLS        += target

}






win32 {

CONFIG += 64bit

MOC_DIR = moc

OBJECTS_DIR = obj

Debug:LIBS += \
    -L/qwt/lib \
    -lqwtd \
    -L/iir1/Debug \
    -liir_static \
    -L/fir1/Debug \
    -lfir_static \
    -lws2_32 \
    -L../attys-comm/Debug \
    -lattyscomm_static


Release:LIBS += \
    -L/qwt/lib \
    -lqwt \
    -L/iir1/Release \
    -liir_static \
    -L/fir1/Release \
    -lfir_static \
    -lws2_32 \
    -L../attys-comm/Release \
    -lattyscomm_static


INCLUDEPATH += /iir1
INCLUDEPATH += /fir1
INCLUDEPATH += ../attys-comm/
INCLUDEPATH += /qwt/src

SOURCES = \
    dataplot.cpp \
    main.cpp \
    attys-ecg.cpp \
    ecg_rr_det.cpp\
    dialog.cpp

HEADERS = \
    attys-ecg.h \
    dataplot.h \
    ecg_rr_det.h\
    radial_gradient.h\
    dialog.h

Debug:CONFIG		+= qt debug c++11
Release:CONFIG		+= qt c++11

QT   	+= widgets

RESOURCES     = application.qrc

}
