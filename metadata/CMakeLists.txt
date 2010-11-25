# --------------------------------------------------------------------------
# General
PROJECT(st_metadata)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_database_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE) 
SET(QT_USE_QTXML TRUE) 
INCLUDE(${QT_USE_FILE})


# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	mtdversion.cpp
	fsqlmetadata.cpp
	fsqldatabasemanager.cpp
	fsqlmdutils.cpp
)

SET(prj_MOC_HDRS
	mtdversion.h
	fsqlmetadata.h
	fsqldatabasemanager.h
	fsqlmdutils.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_metadata_SOURCE_DIR}/stmetadataexport.h.in"
               "${st_metadata_BINARY_DIR}/stmetadataexport.h")
INCLUDE_DIRECTORIES( ${st_metadata_BINARY_DIR} )




# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_metadata SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_metadata PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_metadata ${QT_LIBRARIES} st_database)

INSTALL(TARGETS st_metadata DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
