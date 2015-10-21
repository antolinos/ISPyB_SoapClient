CONFIG += qt
QT += xml
QT += network 
QT += xmlpatterns

SOURCES += main.cpp
SOURCES += ispyb.cpp
SOURCES += qtsoap.cpp

SOURCES += parser.cpp
HEADERS += parser.h

SOURCES += json_scanner.cpp
HEADERS += json_scanner.h

SOURCES += json_parser.cc
HEADERS += json_parser.hh

SOURCES += serializer.cpp
HEADERS += serializer.h

HEADERS += qjson.h
HEADERS += qtsoap.h
HEADERS += ispyb.h


TARGET = client
