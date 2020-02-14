QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../lsMisc/stdQt/stdQt.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../lsMisc/stdQt/stdQt.h \
    mainwindow.h \
    stdafx.h

win32 {
    HEADERS += \
            ../../lsMisc/GetLastErrorString.h
    SOURCES += ../../lsMisc/stdQt/stdQt_win32.cpp \
            ../../lsMisc/GetLastErrorString.cpp

    win32-g++ {
        message("win32-g++")
        LIBS += -lshlwapi -lPsapi
    }
    win32-msvc* {
        message("win32-msvc*")
        LIBS += User32.lib
    }
}
linux {
    SOURCES += ../../lsMisc/stdQt/stdQt_linux.cpp
}

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    info/info.txt \
    xmlsample/340AC0000000033_20190701_431AC0000000006.xml \
    xmlsample/421AC0000000066_20170401_427AC0000000059.xml \
    xmlsample/ichiran.xml \
    xmlsample/info.txt \
    xmlsample/sample01.xml
