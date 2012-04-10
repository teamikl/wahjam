######################################################################
# Automatically generated by qmake (2.01a) Sun Apr 8 19:07:53 2012
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui
QT += network

# Core ninjam/ code does not use wide characters
win32:DEFINES -= UNICODE

# Input
HEADERS += usercon.h \
           Server.h \
           ninjamsrv.h \
           ../../WDL/jnetlib/jnetlib.h \
           ../../WDL/jnetlib/netinc.h \
           ../../WDL/jnetlib/util.h \
           ../../WDL/jnetlib/asyncdns.h \
           ../../WDL/jnetlib/connection.h \
           ../../WDL/jnetlib/httpget.h \
           ../../WDL/jnetlib/httpserv.h \
           ../../WDL/jnetlib/listen.h \
           ../netmsg.h \
           ../../WDL/queue.h \
           ../../WDL/heapbuf.h \
           ../mpb.h \
           ../../WDL/string.h \
           ../../WDL/sha.h \
           ../../WDL/ptrlist.h \
           ../../WDL/rng.h \
           ../../WDL/lineparse.h
SOURCES += ninjamsrv.cpp \
           usercon.cpp \
           Server.cpp \
           ../../WDL/jnetlib/util.cpp \
           ../../WDL/jnetlib/asyncdns.cpp \
           ../../WDL/jnetlib/connection.cpp \
           ../../WDL/jnetlib/httpget.cpp \
           ../../WDL/jnetlib/httpserv.cpp \
           ../../WDL/jnetlib/listen.cpp \
           ../netmsg.cpp \
           ../mpb.cpp \
           ../../WDL/sha.cpp \
           ../../WDL/rng.cpp
