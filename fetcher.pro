
QT += core sql network concurrent
QT -= gui

TEMPLATE = app
VERSION = 0.1.0

TARGET = fetcher

CONFIG   += console
CONFIG   -= app_bundle


include( common.pri )


SOURCES += \
        main.cpp\
        3rdparty/qextserialport/qextserialport.cpp\
        3rdparty/libmodbus/src/modbus.c \
        3rdparty/libmodbus/src/modbus-data.c \
        3rdparty/libmodbus/src/modbus-rtu.c \
        3rdparty/libmodbus/src/modbus-tcp.c \
        3rdparty/libmodbus/src/modbus-ascii.c \
        service.cpp \
        processor.cpp\
        ups_status.cpp\
        nutclient.cpp\
        tcpsock.cpp\
        surgard.cpp\
        dusttcpsock.cpp

HEADERS +=\
    3rdparty/qextserialport/qextserialport.h \
    3rdparty/qextserialport/qextserialenumerator.h \
    3rdparty/libmodbus/src/modbus.h \
    service.h \
    processor.h \
    app.h\
    ups_status.h\
    nutclient.h\
    tcpsock.h\
    surgard.h\
    dusttcpsock.h


INCLUDEPATH += 3rdparty/libmodbus \
               3rdparty/libmodbus/src \
               3rdparty/qextserialport \
               src
unix {
    SOURCES += 3rdparty/qextserialport/posix_qextserialport.cpp	\
           3rdparty/qextserialport/qextserialenumerator_unix.cpp
    DEFINES += _TTY_POSIX_
}

win32 {
    SOURCES += 3rdparty/qextserialport/win_qextserialport.cpp \
           3rdparty/qextserialport/qextserialenumerator_win.cpp
    DEFINES += _TTY_WIN_  WINVER=0x0501
    LIBS += -lsetupapi -lws2_32
}

include(deployment.pri)

DEFINES += WITH_PURE_VIRTUAL
DEFINES += DUSTTCP_H
#DEFINES += DUSTUDP_H

