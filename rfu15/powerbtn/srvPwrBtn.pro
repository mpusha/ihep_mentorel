#-------------------------------------------------
#
# Project created by QtCreator
# Designer Sergey Sytov 2019
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui
VERSION = 1.00
#version 1.00 begin
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include (../config.pro)

TARGET = srvPwrBtn
target.file=TARGET
target.path=/home/serg/t
INSTALLS=target

TEMPLATE  = app
CONFIG   += console
CONFIG   += app
CONFIG   -= app_bundle
CONFIG   += exception

MOC_DIR = moc
OBJects_DIR = obj
DESTDIR = ../$${CURRENT_BUILD}

SOURCES += main.cpp \
    SimpleGPIO.cpp \
    hw.cpp api_watchdog.c

HEADERS += \
    SimpleGPIO.h \
    hw.h


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
