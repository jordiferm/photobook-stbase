# --------------------------------------------------------------------------
# General
PROJECT(st_ooffice)


INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )

INCLUDE_DIRECTORIES( ${st_ooffice_BINARY_DIR} )


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTSQL TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	oofficetemplate.cpp
)

SET(prj_MOC_HDRS
	oofficetemplate.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_ooffice_SOURCE_DIR}/stoofficeexport.h.in"
			   "${st_ooffice_BINARY_DIR}/stoofficeexport.h")
INCLUDE_DIRECTORIES( ${st_ooffice_BINARY_DIR} ${LIBUSB_INCLUDE_DIR})


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_ooffice SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_ooffice PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_ooffice ${QT_LIBRARIES} st_core)

INSTALL(TARGETS st_ooffice DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
