# --------------------------------------------------------------------------
# General
PROJECT(st_ordermanagerl)

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

#SET(MARKETPLACEINFO_BASE_URL "http://sites.google.com/a/starblitz-k.com/marketplaceinfo")


# --------------------------------------------------------------------------
# Project files

#Resource files
SET(prj_RCCS
	./database/database.qrc
)

QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	stomwatchdog.cpp
)

SET(prj_MOC_HDRS
	stomwatchdog.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_ordermanagerl_SOURCE_DIR}/stordermanagerlexport.h.in"
			   "${st_ordermanagerl_BINARY_DIR}/stordermanagerlexport.h")
INCLUDE_DIRECTORIES( ${st_ordermanagerl_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_ordermanagerl SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_ordermanagerl PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_ordermanagerl ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_dom st_phototemplates st_dbmanag st_metadata st_database)
INSTALL(TARGETS st_ordermanagerl DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
