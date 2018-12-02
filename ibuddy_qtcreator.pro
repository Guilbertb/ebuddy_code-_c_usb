TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -ansi  #-std=c++11

SOURCES += \
    main.c \
    USBibuddy.c

HEADERS += \
    USBibuddy.h

LIBS += -L /usr/local/lib -lusb -ansi
