INCLUDEPATH += $$PWD/src/

LIBS += -L$$PWD/lib/ -limg2dat

win32-msvc* {
	PRE_TARGETDEPS += $$PWD/lib/img2dat.lib
} else {
	PRE_TARGETDEPS += $$PWD/lib/libimg2dat.a
}

include($$PWD/icons/include.pri)
