# --------------------------------------------------------------------------
# General
PROJECT(st_oromel)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_imagelib_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_imagelib_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_sindarl_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_sindarl_BINARY_DIR})
INCLUDE_DIRECTORIES( ${st_dom_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR})
INCLUDE_DIRECTORIES( ${st_photobook_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_photobook_BINARY_DIR})
INCLUDE_DIRECTORIES( ${st_metadata_SOURCE_DIR}) #RemotePublisherManager
INCLUDE_DIRECTORIES( ${st_metadata_BINARY_DIR} )#RemotePublisherManager
INCLUDE_DIRECTORIES( ${st_database_SOURCE_DIR})#RemotePublisherManager
INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )#RemotePublisherManager

INCLUDE(${st_digikam_SOURCE_DIR}/includedirs.cmake)

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE) #kphoscos
SET(QT_USE_QTXML TRUE) #phototemplates
SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QT3SUPPORT TRUE) #digikam (canvas.h).
SET(QT_USE_QTNETWORK TRUE)

INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Definitions


# --------------------------------------------------------------------------
# Project files

# Resource files
SET(prj_RCCS ./images/st-oromel-images.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	basicfixeswidget.cpp
	oeditoractionswidget.cpp
	oeditorbottomactionswidget.cpp
	oimageeditor.cpp
	oimagenavigatorwidget.cpp
	odigikamtoolswidget.cpp
	ocolortoolswidget.cpp
	ofiltertoolswidget.cpp
	stfolderimageview.cpp
	stfolderimagesingleview.cpp
	sporomeimageslistview.cpp
	owebinfowidget.cpp
	sttomwatchdog.cpp
	obigimagepreviewwidget.cpp
	starlabmanager.cpp
	remotepublishermanager.cpp
	iddesctablemodel.cpp
	starlababstractmanager.cpp
	starlablocalmanager.cpp
)

SET(prj_MOC_HDRS
	basicfixeswidget.h
	oeditoractionswidget.h
	oeditorbottomactionswidget.h
	oimageeditor.h
	oimagenavigatorwidget.h
	odigikamtoolswidget.h
	ocolortoolswidget.h
	ofiltertoolswidget.h
	stfolderimageview.h
	stfolderimagesingleview.h
	sporomeimageslistview.h
	owebinfowidget.h
	sttomwatchdog.h
	obigimagepreviewwidget.h
	starlabmanager.h
	remotepublishermanager.h
	iddesctablemodel.h
	starlababstractmanager.h
	starlablocalmanager.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_oromel_SOURCE_DIR}/storomelexport.h.in"
			   "${st_oromel_BINARY_DIR}/storomelexport.h")
INCLUDE_DIRECTORIES( ${st_oromel_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_oromel SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_oromel PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_oromel ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_digikam st_dom st_photobook st_sindarl)
INSTALL(TARGETS st_oromel DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
