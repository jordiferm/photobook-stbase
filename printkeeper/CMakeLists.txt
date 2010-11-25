# --------------------------------------------------------------------------
# General
PROJECT(st_printkeeper)


INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	printkeeper.cpp
)

SET(prj_MOC_HDRS
	printkeeper.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_printkeeper_SOURCE_DIR}/stprintkeeperexport.h.in"
			   "${st_printkeeper_BINARY_DIR}/stprintkeeperexport.h")
INCLUDE_DIRECTORIES( ${st_printkeeper_BINARY_DIR} ${LIBUSB_INCLUDE_DIR})


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_printkeeper SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_printkeeper PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_printkeeper ${QT_LIBRARIES})

INSTALL(TARGETS st_printkeeper DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
