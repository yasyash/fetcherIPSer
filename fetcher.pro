
QT += core sql network concurrent serialport
QT -= gui

TEMPLATE = app
VERSION = 0.1.7

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
        tcpsock.cpp\
        surgard.cpp\
        dusttcpsock.cpp \
        meteotcpsock.cpp \
        serinus.cpp \
        grimm.cpp \
        liga.cpp\
        soapC.cpp\
        soapWSHttpBinding_USCOREIAutoChromWCFHostProxy.cpp\
        stdsoap2.cpp



HEADERS +=\
    3rdparty/qextserialport/qextserialport.h \
    3rdparty/qextserialport/qextserialenumerator.h \
    3rdparty/libmodbus/src/modbus.h \
    service.h \
    processor.h \
    app.h\
    ups_status.h\
    tcpsock.h\
    surgard.h\
    dusttcpsock.h \
    meteotcpsock.h \
    serinus.h \
    grimm.h \
    liga.h\
    soapWSHttpBinding_USCOREIAutoChromWCFHostProxy.h \
    soapH.h \
    soapStub.h \
    WSHttpBinding_USCOREIAutoChromWCFHost.nsmap \
    wsa5.h




INCLUDEPATH += 3rdparty/libmodbus \
               3rdparty/libmodbus/src \
               3rdparty/qextserialport \
               src

               # /usr/local/include/net-snmp
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
#DEFINES += QTNETSNMP_LIBRARY
#DEFINES += DUSTUDP_H



LIBS += -lnetsnmp -lnetsnmpmibs -lqt-net-snmp
