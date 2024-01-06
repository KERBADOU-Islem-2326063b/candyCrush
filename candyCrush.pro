TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -L/usr/local/lib -lyaml-cpp
include(MinGL2_IUT_AIX/mingl.pri)

SOURCES += \
        main.cpp

DISTFILES += \
    config.yaml
