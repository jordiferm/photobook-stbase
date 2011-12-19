# --------------------------------------------------------------------------
# General
PROJECT(st_tpopsl)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_imagelib_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_imagelib_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_embeddedwidgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_embeddedwidgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_dom_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR} )

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)

# --------------------------------------------------------------------------
# Configure Qt

#SET(QT_USE_QTNETWORK TRUE) 
SET(QT_USE_QTSQL TRUE) #STAlbumWidget needs it from ophotocollection.h
SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTNETWORK TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Configure GUI

OPTION(ST_TTPOPSL_KINETICSCROLL
  "Kinetic Scroll support (Requires Qt>=4.6.0)"
  ON)

IF(ST_TTPOPSL_KINETICSCROLL)
	SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "ST_TTPOPSL_KINETICSCROLL" )
ENDIF(ST_TTPOPSL_KINETICSCROLL)

# --------------------------------------------------------------------------
# Project files

# Resource files
SET(prj_RCCS ./images/st-tpopsl-images.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	tpphotoselwidget.cpp
	tpphotolistview.cpp
	tpphotodelegate.cpp
	tpproductlistview.cpp
	tpinputdialog.cpp
	tpphotoeditor.cpp
	addremovewidget.cpp
	tpiphotoeditor.cpp
)

SET(prj_MOC_HDRS
	tpphotoselwidget.h
	tpphotolistview.h
	tpphotodelegate.h
	tpproductlistview.h
	tpinputdialog.h
	tpphotoeditor.h
	addremovewidget.h
	tpiphotoeditor.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_tpopsl_SOURCE_DIR}/sttpopslexport.h.in"
               "${st_tpopsl_BINARY_DIR}/sttpopslexport.h")
INCLUDE_DIRECTORIES( ${st_tpopsl_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_tpopsl SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

#TARGET_LINK_LIBRARIES(st_tpopsl ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_kphoscos)
SET_TARGET_PROPERTIES(st_tpopsl PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_tpopsl ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_embeddedwidgets st_dom)
INSTALL(TARGETS st_tpopsl DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
