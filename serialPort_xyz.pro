#-------------------------------------------------
#
# Project created by QtCreator 2018-05-28T19:42:31
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += network
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialPort_xyz
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

CONFIG   += C++11
