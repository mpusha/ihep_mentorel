#-------------------------------------------------
#
# Project created by QtCreator
# Designer Sergey Sytov 2022
# server for timer ch25 system
#-------------------------------------------------
# 1.0 Begin version

QT       += core
QT       += network
QT       -= gui

VERSION = 1.00
MOC_DIR = ../moc
OBJECTS_DIR = ../obj

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include (../config.pro)
DESTDIR = ../../$${CURRENT_BUILD}

TARGET = srvTimer
target.file=srvOrbita
target.path=/home/work/programs
INSTALLS=target

TEMPLATE  = app
CONFIG   += console
CONFIG   += app
CONFIG   -= app_bundle
CONFIG   += exception


SOURCES += main.cpp \
    timerBehave.cpp timer.cpp serial.cpp \
    SimpleGPIO.cpp settings.cpp

HEADERS += \
    timerBehave.h timer.h serial.h \
    SimpleGPIO.h settings.h

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
