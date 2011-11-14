# --------------------------------------------------------------------------
# General
PROJECT(st_wizards)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_imagelib_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_imagelib_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_dom_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_photobook_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_photobook_BINARY_DIR})
INCLUDE_DIRECTORIES( ${st_tpopsl_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_tpopsl_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_oromel_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_oromel_BINARY_DIR} )

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)


# --------------------------------------------------------------------------
# Configure Qt

#SET(QT_USE_QTNETWORK TRUE) 
SET(QT_USE_QTSQL TRUE) #STAlbumWidget needs it from ophotocollection.h
SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE) 
INCLUDE(${QT_USE_FILE})


# --------------------------------------------------------------------------
# Project files

# Resource files
SET(prj_RCCS ./images/st-wizards-images.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	stprintwizard.cpp
	storderprintswizard.cpp
	stsendersettingsdialog.cpp
	disksourceselectionwidget.cpp
	stiwizard.cpp
	stiwizardpage.cpp
	stalbumwizard.cpp
	stphotoindexwizard.cpp
)

SET(prj_MOC_HDRS
	stprintwizard.h
	storderprintswizard.h
	stsendersettingsdialog.h
	disksourceselectionwidget.h
	stiwizard.h
	stiwizardpage.h
	stalbumwizard.h
	stphotoindexwizard.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_wizards_SOURCE_DIR}/stwizardsexport.h.in"
               "${st_wizards_BINARY_DIR}/stwizardsexport.h")
INCLUDE_DIRECTORIES( ${st_wizards_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_wizards SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_wizards PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_wizards ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_photobook st_dom st_tpopsl st_oromel)
INSTALL(TARGETS st_wizards DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
