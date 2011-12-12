# --------------------------------------------------------------------------
# General
PROJECT(st_photobook)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_imagelib_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_imagelib_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
# For Template Types
INCLUDE_DIRECTORIES( ${st_dom_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR} )


# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTSVG TRUE) 
SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Definitions

SET(TEMPLATEINFO_URLBASE "http://sites.google.com/site/orometemplates" CACHE STRING "Base Url for template info.")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "TEMPLATEINFO_URLBASE=\"${TEMPLATEINFO_URLBASE}\"")


# --------------------------------------------------------------------------
# Project files

#Resource files
SET(prj_RCCS ./images/st-photobook-images.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	designinfo.cpp
	templateinfo.cpp
	templatepaths.cpp
	templateinfolist.cpp
	rendersettings.cpp
	metainfo.cpp
	collectioninfo.cpp
	ichangescontrol.cpp
	templatescene.cpp
	corneritem.cpp
	abstractgraphicsitem.cpp
	graphicsclipartitem.cpp
	graphicsphotoitem.cpp
	graphicsitemmodifier.cpp
	graphicsitemoperation.cpp
	graphicsmonthitem.cpp
	graphicspageitem.cpp
	graphicstextitem.cpp
	updateitemevent.cpp
	buildoptions.cpp
	candidatecalculator.cpp
	document.cpp
	pagelist.cpp
	metainfowidget.cpp
	documentpageview.cpp
	documentviewwidget.cpp
	templategenerator.cpp
	pagethumbnailmodel.cpp
	pagethumbnailproxymodel.cpp
	checkedproxymodel.cpp
	doccheckedproxymodel.cpp
	imageslistviewbase.cpp
	doccollectionmodel.cpp
	imageslistview.cpp
	imageboxlistview.cpp
	templateinfomodel.cpp
	designinfolist.cpp
	designinfomodel.cpp
	templatescenemimedata.cpp
	resource.cpp
	resourcelist.cpp
	resourcemodel.cpp
	resourcemimedata.cpp
	designinfotablemodel.cpp
	systemtemplates.cpp
	publictemplatesdatabase.cpp
	templatedatacontext.cpp
)

SET(prj_MOC_HDRS
	designinfo.h
	templateinfo.h
	templatepaths.h
	templateinfolist.h
	rendersettings.h
	metainfo.h
	collectioninfo.h
	ichangescontrol.h
	templatescene.h
	corneritem.h
	abstractgraphicsitem.h
	graphicsclipartitem.h
	graphicsphotoitem.h
	graphicsitemmodifier.h
	graphicsitemoperation.h
	graphicsmonthitem.h
	graphicspageitem.h
	graphicstextitem.h
	updateitemevent.h
	buildoptions.h
	candidatecalculator.h
	document.h
	pagelist.h
	metainfowidget.h
	documentpageview.h
	documentviewwidget.h
	templategenerator.h
	pagethumbnailmodel.h
	pagethumbnailproxymodel.h
	checkedproxymodel.h
	doccheckedproxymodel.h
	imageslistviewbase.h
	doccollectionmodel.h
	imageslistview.h
	imageboxlistview.h
	templateinfomodel.h
	designinfolist.h
	designinfomodel.h
	templatescenemimedata.h
	resource.h
	resourcelist.h
	resourcemodel.h
	resourcemimedata.h
	designinfotablemodel.h
	systemtemplates.h
	publictemplatesdatabase.h
	templatedatacontext.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_photobook_SOURCE_DIR}/stphotobookexport.h.in"
			   "${st_photobook_BINARY_DIR}/stphotobookexport.h")
INCLUDE_DIRECTORIES( ${st_photobook_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_photobook SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_photobook PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_photobook ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_dom)
INSTALL(TARGETS st_photobook DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
