TEMPLATE = lib
CONFIG += staticlib
QT += network
QT -= gui
QMAKE_CXXFLAGS += -Wno-write-strings
CONFIG += link_pkgconfig
PKGCONFIG += portaudio-2.0 portmidi ogg vorbis vorbisenc
win32:DEFINES -= UNICODE
SOURCES = audiostream_pa.cpp \
          mpb.cpp \
          netmsg.cpp \
          njclient.cpp \
          njmisc.cpp \
          UserPrivs.cpp
HEADERS = audiostream.h \
          mpb.h \
          netmsg.h \
          njclient.h \
          njmisc.h \
          UserPrivs.h \
          ConcurrentQueue.h
