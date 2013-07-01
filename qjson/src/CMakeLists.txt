set(qjson_MOC_HDRS
  parserrunnable.h
  serializerrunnable.h
)

qt4_wrap_cpp(qjson_MOC_SRCS ${qjson_MOC_HDRS})

set (qjson_SRCS parser.cpp qobjecthelper.cpp json_scanner.cpp json_parser.cc parserrunnable.cpp serializer.cpp serializerrunnable.cpp)
set (qjson_HEADERS parser.h parserrunnable.h qobjecthelper.h serializer.h serializerrunnable.h qjson_export.h)

# Required to use the intree copy of FlexLexer.h
INCLUDE_DIRECTORIES(.)



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(qjson SHARED ${qjson_SRCS} ${qjson_MOC_SRCS} ${qjson_HEADERS})

SET_TARGET_PROPERTIES(qjson PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES( qjson ${QT_LIBRARIES})

INSTALL(TARGETS qjson EXPORT qjson-export DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
