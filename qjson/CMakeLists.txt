PROJECT(qjson)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII" )

CMAKE_POLICY(SET CMP0003 NEW)

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})


# compile in debug mode
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel.")
ENDIF(NOT CMAKE_BUILD_TYPE)

IF("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")
  ADD_DEFINITIONS("-DQT_NO_DEBUG_OUTPUT")
ENDIF("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")

# Ability to disable verbose debug output
IF(QJSON_VERBOSE_DEBUG_OUTPUT)
  ADD_DEFINITIONS("-DQJSON_VERBOSE_DEBUG_OUTPUT")
endif(QJSON_VERBOSE_DEBUG_OUTPUT)

set(QJSON_LIB_MAJOR_VERSION "0")
set(QJSON_LIB_MINOR_VERSION "8")
set(QJSON_LIB_PATCH_VERSION "1")

set(QJSON_LIB_VERSION_STRING "${QJSON_LIB_MAJOR_VERSION}.${QJSON_LIB_MINOR_VERSION}.${QJSON_LIB_PATCH_VERSION}")

# --------------------------------------------------------------------------
# Configuration files

#CONFIGURE_FILE("${qjson_SOURCE_DIR}/src/qjsonexport.h.in"
#               "${qjson_BINARY_DIR}/qjsonexport.h")
#INCLUDE_DIRECTORIES( ${qjson_BINARY_DIR} )


# Subdirs
ADD_SUBDIRECTORY(src)

