# --------------------------------------------------------------------------
# General
PROJECT(st_phototemplates)

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
SET(QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Definitions

SET(TEMPLATEINFO_BASE_URL "http://sites.google.com/site/orometemplates" CACHE STRING "Base Url for template info.")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "TEMPLATEINFO_BASE_URL=\"${TEMPLATEINFO_BASE_URL}\"")


# --------------------------------------------------------------------------
# Project files

#Resource files
SET(prj_RCCS ./images/st-phototemplates-images.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# Source files
SET(prj_SRCS
	stphotolayout.cpp
	stupdateitemevent.cpp
	stgraphicsphotoitem.cpp
	stgraphicssimpletextitem.cpp
	stgraphicspageitem.cpp
	sttemplatescene.cpp
	stalbumpagegraphicsview.cpp
	stgraphicsitemoperation.cpp
	stalbumwidget.cpp
	stalbumtemplatemodel.cpp
	stalbumcolormodel.cpp
	stfontthumbnailmodel.cpp
	stalbumbackgroundmodel.cpp
	stgraphicstextitem.cpp
	stabstractgraphicsitem.cpp
	stphotobook.cpp
	stphotobookthumbnailmodel.cpp
	stgraphicsclipartitem.cpp
	stphotobookclipartmodel.cpp
	stphotobookmaskmodel.cpp
	stphotobookframemodel.cpp
	stphotobookbackgroundmodel.cpp
	sttemplatemodel.cpp
	stgraphicssuperimposeitem.cpp
	stgraphicsmonthitem.cpp
	stcollectiontemplatemodel.cpp
	sttemplatepaths.cpp
	stphotobookinfowidget.cpp
	stphotobookcheckedproxymodel.cpp
	stgraphicsitemmodifier.cpp
	stcorneritem.cpp
	stphotobookfolderdocsmodel.cpp
	stphotobookdocsproxymodel.cpp
	sttemplateproxymodel.cpp
	pbimageselectwidget.cpp
	sttemplatemimedata.cpp
	stmaskmimedata.cpp
	stframemimedata.cpp
	sptoolbarlistview.cpp
	spimageslistview.cpp
	spimageslistviewbase.cpp
	spimageboxlistview.cpp
	spthumbnailsizewidget.cpp
	stphotobookcollectioninfo.cpp
	stcheckedproxymodel.cpp
	stncopiesproxymodel.cpp
	stcandidatecalculator.cpp
	stphotobookcollectionmodel.cpp
	stphotolayouttemplategenerator.cpp
	stphotobookbuildoptions.cpp
)

SET(prj_MOC_HDRS
	stphotolayout.h
	stupdateitemevent.h
	stgraphicsphotoitem.h
	stgraphicssimpletextitem.h
	stgraphicspageitem.h
	sttemplatescene.h
	stalbumpagegraphicsview.h
	stgraphicsitemoperation.h
	stalbumwidget.h
	stalbumtemplatemodel.h
	stalbumcolormodel.h
	stfontthumbnailmodel.h
	stalbumbackgroundmodel.h
	stgraphicstextitem.h
	stabstractgraphicsitem.h
	stphotobook.h
	stphotobookthumbnailmodel.h
	stgraphicsclipartitem.h
	stphotobookclipartmodel.h
	stphotobookmaskmodel.h
	stphotobookframemodel.h
	stphotobookbackgroundmodel.h
	sttemplatemodel.h
	stgraphicssuperimposeitem.h
	stgraphicsmonthitem.h
	stcollectiontemplatemodel.h
	sttemplatepaths.h
	stphotobookinfowidget.h
	stphotobookcheckedproxymodel.h
	stgraphicsitemmodifier.h
	stcorneritem.h
	stphotobookfolderdocsmodel.h
	stphotobookdocsproxymodel.h
	sttemplateproxymodel.h
	pbimageselectwidget.h
	sttemplatemimedata.h
	stmaskmimedata.h
	stframemimedata.h
	sptoolbarlistview.h
	spimageslistview.h
	spimageslistviewbase.h
	spimageboxlistview.h
	spthumbnailsizewidget.h
	stphotobookcollectioninfo.h
	stcheckedproxymodel.h
	stncopiesproxymodel.h
	stcandidatecalculator.h
	stphotobookcollectionmodel.h
	stphotolayouttemplategenerator.h
	stphotobookbuildoptions.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_phototemplates_SOURCE_DIR}/stphototemplatesexport.h.in"
               "${st_phototemplates_BINARY_DIR}/stphototemplatesexport.h")
INCLUDE_DIRECTORIES( ${st_phototemplates_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_phototemplates SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_phototemplates PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_phototemplates ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib st_widgets st_dom)
INSTALL(TARGETS st_phototemplates DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
