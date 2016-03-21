QT += gui widgets

CONFIG -= app_bundle debug_and_release
CONFIG += staticlib

TARGET = img2dat
TEMPLATE = lib
DESTDIR = ../lib/

SOURCES += \
    imageconverter.cpp \
    imageconverterdialog.cpp \

HEADERS += \
    imageconverter.h \
    imageconverterdialog.h \
    color.h \
    lameimage.h \

FORMS += \
    imageconverterdialog.ui \
