#-------------------------------------------------
#
# Project created by QtCreator 2016-04-16T16:03:21
#
#-------------------------------------------------

QT       += core gui
QT      += widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = terminal
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SerialPort.cpp

HEADERS  += mainwindow.h \
    SerialPort.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

DISTFILES +=
