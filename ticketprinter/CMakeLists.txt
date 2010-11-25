# --------------------------------------------------------------------------
# General
PROJECT(st_ticketprinter)


INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_openrpt_common_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_openrpt_common_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_openrpt_renderer_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_openrpt_renderer_BINARY_DIR} )

IF (WIN32)
	ADD_DEFINITIONS ( -D_TTY_WIN_ )
ELSE (WIN32)
	ADD_DEFINITIONS ( -D_TTY_POSIX_ )
ENDIF (WIN32)

INCLUDE_DIRECTORIES( ${st_serialport_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_serialport_BINARY_DIR} )

# --------------------------------------------------------------------------
# USB Support

INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/FindUSB.cmake)

# LibMagick
IF(NOT LIBUSB_FOUND)
  MESSAGE(SEND_ERROR "LibUSB library not found.")
ENDIF(NOT LIBUSB_FOUND)


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTSQL TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	printerescpos.cpp
	ticketprinter.cpp
	usbprinterdevice.cpp
	sticketprintersettings.cpp
	sticketprintersettingswidget.cpp
	sticketprinter.cpp
)

SET(prj_MOC_HDRS
	printerescpos.h
	ticketprinter.h
	usbprinterdevice.h
	sticketprintersettings.h
	sticketprintersettingswidget.h
	sticketprinter.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_ticketprinter_SOURCE_DIR}/stticketprinterexport.h.in"
               "${st_ticketprinter_BINARY_DIR}/stticketprinterexport.h")
INCLUDE_DIRECTORIES( ${st_ticketprinter_BINARY_DIR} ${LIBUSB_INCLUDE_DIR})


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_ticketprinter SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_ticketprinter PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_ticketprinter ${QT_LIBRARIES} ${LIBUSB_LIBRARIES} st_widgets st_openrpt_common st_openrpt_renderer st_serialport)

INSTALL(TARGETS st_ticketprinter DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
