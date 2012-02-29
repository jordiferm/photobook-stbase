# --------------------------------------------------------------------------
# General
PROJECT(st_sindarl)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_dbmanag_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_dbmanag_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_metadata_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_metadata_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_database_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_dom_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR})
INCLUDE_DIRECTORIES( ${st_photobook_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_photobook_BINARY_DIR})

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Definitions

#SET(MARKETPLACEINFO_BASE_URL "http://sites.google.com/a/Aili Image-k.com/marketplaceinfo")
SET(MARKETPLACEINFO_BASE_URL "http://sites.google.com/site/orometemplates" CACHE STRING "Marketplace info Base URL.")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "MARKETPLACEINFO_BASE_URL=\"${MARKETPLACEINFO_BASE_URL}\"")


# --------------------------------------------------------------------------
# Project files

#Resource files
SET(prj_RCCS
	./database/sindardatabase.qrc
	./database/defaultdatabases/sindarldefaultdatabases.qrc
)

QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	kpprinterinfo.cpp
	wproductmanager.cpp
	sindardatabase.cpp
	defaultdbselectiondialog.cpp
)

SET(prj_MOC_HDRS
	kpprinterinfo.h
	wproductmanager.h
	sindardatabase.h
	defaultdbselectiondialog.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_sindarl_SOURCE_DIR}/stsindarlexport.h.in"
			   "${st_sindarl_BINARY_DIR}/stsindarlexport.h")
INCLUDE_DIRECTORIES( ${st_sindarl_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_sindarl SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_sindarl PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_sindarl ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_digikam st_dom st_dbmanag st_metadata st_database st_photobook)
INSTALL(TARGETS st_sindarl DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
