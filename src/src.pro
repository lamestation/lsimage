QT += gui widgets

CONFIG -= app_bundle debug_and_release
CONFIG += staticlib

TARGET = img2dat
TEMPLATE = lib
DESTDIR = ../lib/

SOURCES += \
    img2dat.cpp \

HEADERS += \
    img2dat.h \

FORMS += \
    img2dat.ui \
