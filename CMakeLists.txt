#
# JSON Parser and Serializer (LGPL)
# https://github.com/flavio/qjson/releases
#
project (QJSON CXX)

set (QJSON_HEADERS parser.h
                   parserrunnable.h
                   qobjecthelper.h
                   serializer.h
                   serializerrunnable.h
                   qjson_export.h
                   qjson.h)

set (QJSON_SOURCES parser.cpp
                   qobjecthelper.cpp
                   json_scanner.cpp
                   json_parser.cc
                   parserrunnable.cpp
                   serializer.cpp
                   serializerrunnable.cpp)

qt4_wrap_cpp (QJSON_MOC parserrunnable.h
                        serializerrunnable.h)

add_definitions (-DQJSON_MAKEDLL)

add_shared_library (qjson
                    SOURCES ${QJSON_HEADERS} ${QJSON_SOURCES} ${QJSON_MOC}
                    LIBRARIES ${QT_QTCORE_LIBRARY}
                    VERSION 0.8.1)

install_library (TARGETS qjson)
