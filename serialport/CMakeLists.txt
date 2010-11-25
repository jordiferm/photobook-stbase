# --------------------------------------------------------------------------
# General
PROJECT(st_serialport)

IF (WIN32)
	ADD_DEFINITIONS ( -D_TTY_WIN_ )
ELSE (WIN32)
	ADD_DEFINITIONS ( -D_TTY_POSIX_ )
ENDIF (WIN32)

# --------------------------------------------------------------------------
# Configure Qt

#SET(QT_USE_QTXML TRUE) 
INCLUDE(${QT_USE_FILE})


# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	qextserialbase.cpp
	qextserialport.cpp
)

IF (WIN32)
	SET(prj_SRCS ${prj_SRCS} 
		win_qextserialport.cpp)
ELSE (WIN32)
	SET(prj_SRCS ${prj_SRCS} 
		posix_qextserialport.cpp)
ENDIF(WIN32)

SET(prj_MOC_HDRS
	qextserialbase.h
	qextserialport.h
)

IF (WIN32)
	SET(prj_MOC_HDRS ${prj_MOC_HDRS} 
		win_qextserialport.h)
ELSE (WIN32)
	SET(prj_MOC_HDRS ${prj_MOC_HDRS} 
		posix_qextserialport.h)
ENDIF(WIN32)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_serialport_SOURCE_DIR}/stserialportexport.h.in"
               "${st_serialport_BINARY_DIR}/stserialportexport.h")
INCLUDE_DIRECTORIES( ${st_serialport_BINARY_DIR} )

# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_serialport SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_serialport PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_serialport ${QT_LIBRARIES})
INSTALL(TARGETS st_serialport DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
