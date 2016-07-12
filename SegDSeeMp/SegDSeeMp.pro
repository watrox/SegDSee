#-------------------------------------------------
#
# Project created by QtCreator 2013-10-14T17:36:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SegDSeeMp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    libSegD.cpp \
    procparmdialog.cpp \
    readerthread.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    libSegD.h \
    procparmdialog.h \
    readerthread.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    procparmdialog.ui \
    aboutdialog.ui

# ADD THIS LINE!
RC_FILE += app.rc

RESOURCES += \
    myrc.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GxLib/release/ -lGxLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GxLib/debug/ -lGxLib
else:unix: LIBS += -L$$OUT_PWD/../GxLib/ -lGxLib

INCLUDEPATH += $$PWD/../GxLib
DEPENDPATH += $$PWD/../GxLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GxLib/release/libGxLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GxLib/debug/libGxLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GxLib/release/GxLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GxLib/debug/GxLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GxLib/libGxLib.a
