#-------------------------------------------------
#
# Project created by QtCreator 2017-07-10T19:53:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = filsystem
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    igetput.cpp \
    install.cpp \
    controller.cpp \
    format.cpp \
    bfBlock.cpp \
    FileOperator.cpp \
    initHInode.cpp \
    showDir.cpp \
    MyFileSystem.cpp \
    edit.cpp \
    systable.cpp \
    access.cpp \
    adduser.cpp

HEADERS  += mainwindow.h \
    filesystem.h \
    edit.h \
    systable.h

FORMS    += mainwindow.ui \
    edit.ui \
    systable.ui

RESOURCES += \
    icon.qrc

DISTFILES +=
