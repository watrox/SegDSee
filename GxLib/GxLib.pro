#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T10:08:09
#
#-------------------------------------------------

QT       += widgets

TARGET = GxLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    myscrollarea.cpp \
    mystringtable.cpp \
    util2.cpp \
    furlib.cpp \
    myobjptrlist.cpp \
    gfx.cpp \
    gfxobj.cpp \
    gfxobjseis.cpp \
    gfxobjseishdrs.cpp \
    gfxobjseissect.cpp \
    gfxobjtaxis.cpp \
    gfxsctlist.cpp \
    gfxsrc.cpp \
    gfxsrcseis.cpp \
    gfxsrcseismem.cpp \
    gfxsrcseismemsegd.cpp \
    gfxsrcseistest.cpp \
    gfxutil.cpp \
    gfxview.cpp \
    gfxsrcfile.cpp \
    seisfile.cpp \
    sgyfile.cpp \
    hdrdef.cpp \
    gfxobjhsrslab.cpp

HEADERS += \
    myscrollarea.h \
    mystringtable.h \
    util2.h \
    furlib.h \
    myobjptrlist.h \
    gfx.h \
    gfxobj.h \
    gfxobjseis.h \
    gfxobjseishdrs.h \
    gfxobjseissect.h \
    gfxobjtaxis.h \
    gfxsctlist.h \
    gfxsrc.h \
    gfxsrcseis.h \
    gfxsrcseismem.h \
    gfxsrcseismemsegd.h \
    gfxsrcseistest.h \
    gfxutil.h \
    gfxview.h \
    gfxsrcfile.h \
    seisfile.h \
    sgyfile.h \
    hdrdef.h \
    gfxobjhsrslab.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
