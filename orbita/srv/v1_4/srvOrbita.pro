#-------------------------------------------------
#
# Project created by QtCreator
# Designer Sergey Sytov 2019
#-------------------------------------------------
# 1.00 begin version
# 1.01 add db
#in sh_ccydalib.c change
#was static unsigned short mes_tab_id;   // tab_id для U70_ECS_MESSAGES
#replace on
#static signed short mes_tab_id;   // tab_id для U70_ECS_MESSAGES
# 1.10 with db
# 1.20 27.09.2019 get adc data new alghoritm
# 1.30 01.10.2019 cosmetical. remove //
# 1.40 21.10.2019 in main.cpp events processing change! 
#   11.11.19 in main demonize() remove getppid. was error in work with systemd (start from init with pid=1)

QT       += core
QT       += network
QT       -= gui

VERSION = 1.40
FPGAVERS =1
DEFINES +=APP_FPGAVERSION=0xa516
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
MOC_DIR = ../moc
OBJECTS_DIR = ../obj
include (../config.pro)
DESTDIR = ../../$${CURRENT_BUILD}

TARGET = srvOrbita
target.file=srvOrbita
target.path=/home/work/programs
INSTALLS=target

TEMPLATE  = app
CONFIG   += console
CONFIG   += app
CONFIG   -= app_bundle
CONFIG   += exception


SOURCES += main.cpp \
    SimpleGPIO.cpp \
    spi.c \
    hw.cpp \
    orbBehav.cpp \
    progerror.cpp \
    db.c \
    dbBehav.cpp

HEADERS += \
    SimpleGPIO.h \
    spi.h \
    hw.h \
    fpga.h \
    orbBehav.h \
    progerror.h \
    /usr/usera/voevodin/EqContStation/sh_ccydalib/sh_ccydalib.h \
    db.h \
    dbBehav.h

OBJECTS += /usr/usera/voevodin/EqContStation/sh_ccydalib/sh_ccydalib.o
LIBS += /usr/usera/voevodin/rt-data/ccydalib/ccydalib.a
LIBS += -lrt -lresolv

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


