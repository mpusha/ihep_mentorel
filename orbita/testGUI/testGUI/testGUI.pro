#-------------------------------------------------
#
# Project created by QtCreator 2019-04-02T11:27:27
#
#-------------------------------------------------

QT       += core gui

CONFIG  += widgets
CONFIG   -= app_bundle
CONFIG += exception
TEMPLATE = app

VERSION = 1.00.00
#version 1.00.00 begin
DEFINES += APP_VERSION=\\\"$$VERSION\\\"


TARGET = testGUI
target.file=testGUI
target.path=/home/serg/t
INSTALLS=target


MOC_DIR = moc
OBJECTS_DIR = obj



SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CFLAGS_RELEASE -= -O
QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE -= -O3
#QMAKE_CXXFLAGS +=O0
