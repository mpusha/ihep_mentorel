#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T11:41:09
#
#-------------------------------------------------

QT       += core

QT       += gui
VERSION = 1.00.00

#version 1.00.00 begin
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
TARGET = testGUI
target.file=testGUI
target.path=/home/serg/t
INSTALLS=target

CONFIG   += widget
CONFIG   -= app_bundle

TEMPLATE = app
CONFIG += exception

MOC_DIR = moc
OBJECTS_DIR = obj

SOURCES += main.cpp \
    gui.cpp

HEADERS += gui.h

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


