# --------------------------------------------------------------------------
# General
PROJECT(st_dbmanag)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_database_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_dbmanag_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_dbmanag_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_metadata_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_metadata_BINARY_DIR} )


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	fgenmanager.cpp
	fpixmapselector.cpp
	frecdialog.cpp
	frecordwidget.cpp
	fsql1nmappersync.cpp
	ftablemanager.cpp
)

SET(prj_MOC_HDRS
	fgenmanager.h
	fpixmapselector.h
	frecdialog.h
	frecordwidget.h
	fsql1nmappersync.h
	ftablemanager.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_dbmanag_SOURCE_DIR}/stdbmanagexport.h.in"
               "${st_dbmanag_BINARY_DIR}/stdbmanagexport.h")
INCLUDE_DIRECTORIES( ${st_dbmanag_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_dbmanag SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_dbmanag PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_dbmanag ${QT_LIBRARIES} st_core st_widgets st_database st_dbmanag st_metadata)

INSTALL(TARGETS st_dbmanag DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
