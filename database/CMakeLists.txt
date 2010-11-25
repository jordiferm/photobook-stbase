# --------------------------------------------------------------------------
# General
PROJECT(st_database)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	fsqlquery.cpp
	fdbutils.cpp
	fsqldatabasesettings.cpp
	fdbconfigwidget.cpp
	sdbsettingsdialog.cpp
	fsqldatabase.cpp
	fsqlmodelviewutils.cpp
	fsqlrelationaltablemodel.cpp
	fsqlrecord.cpp
	fsqlrelationalimagemodel.cpp
	ftableview.cpp
	fsqlsearchwidget.cpp
	fsqlactiontableview.cpp
	fsqlsearchdialog.cpp
)

SET(prj_MOC_HDRS
	fsqlquery.h
	fdbutils.h
	fsqldatabasesettings.h
	fdbconfigwidget.h
	sdbsettingsdialog.h
	fsqldatabase.h
	fsqlmodelviewutils.h
	fsqlrelationaltablemodel.h
	fsqlrecord.h	
	fsqlrelationalimagemodel.h
	ftableview.h
	fsqlsearchwidget.h
	fsqlactiontableview.h
	fsqlsearchdialog.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})


# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_database_SOURCE_DIR}/stdatabaseexport.h.in"
               "${st_database_BINARY_DIR}/stdatabaseexport.h")
INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

#IF (WIN32)
#	ADD_LIBRARY(st_database STATIC ${prj_SRCS} ${prj_MOC_SRCS})
#ELSE (WIN32)
	ADD_LIBRARY(st_database SHARED ${prj_SRCS} ${prj_MOC_SRCS})
#ENDIF (WIN32)

SET_TARGET_PROPERTIES(st_database PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_database ${QT_LIBRARIES} st_widgets)
INSTALL(TARGETS st_database DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
