#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_openrpt_wrtembed)

# To locate ui generated .h files.
#INCLUDE_DIRECTORIES( ${st_openrpt_wrtembed_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_openrpt_wrtembed_SOURCE_DIR}/../../common/
	${st_openrpt_wrtembed_SOURCE_DIR}/../renderer/ 
	${st_openrpt_wrtembed_SOURCE_DIR}/../common/ 
	${st_openrpt_wrtembed_SOURCE_DIR}/../images/ 
	${st_openrpt_wrtembed_SOURCE_DIR}
	${st_openrpt_common_BINARY_DIR}
	${st_openrpt_renderer_BINARY_DIR})


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTSQL TRUE) 
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

#ADD_DEFINITIONS ( -DMAKELIB )

SET(prj_SRCS
	reportentities.cpp
	reportgridoptions.cpp
	reportsection.cpp
	reportwindow.cpp
	reporthandler.cpp 
	labeleditor.cpp 
	queryeditor.cpp 
	querylist.cpp 
	pagesetup.cpp 
	sectioneditor.cpp 
	detailsectiondialog.cpp 
	fieldeditor.cpp 
	texteditor.cpp 
	barcodeeditor.cpp 
	reportproperties.cpp 
	reportparameter.cpp 
	reportparameterlist.cpp 
	reportparameterlistitem.cpp 
	imageeditor.cpp 
	coloreditor.cpp 
	colorlist.cpp 
	grapheditor.cpp 
	detailgroupsectiondialog.cpp 
	editpreferences.cpp 
	dbfiledialog.cpp 
	querycombobox.cpp 
	data.cpp 
	../common/labelsizeinfo.cpp 
	../common/pagesizeinfo.cpp 
	../common/builtinformatfunctions.cpp
)


# --------------------------------------------------------------------------
# Project files
SET(prj_MOC_HDRS
	reportentities.h
	reportgridoptions.h
	reportsection.h
	reportwindow.h
	reporthandler.h 
	labeleditor.h 
	queryeditor.h 
	querylist.h 
	pagesetup.h 
	sectioneditor.h 
	detailsectiondialog.h 
	fieldeditor.h 
	texteditor.h 
	barcodeeditor.h 
	reportproperties.h 
	reportparameter.h 
	reportparameterlist.h 
	reportparameterlistitem.h 
	imageeditor.h 
	coloreditor.h 
	colorlist.h 
	grapheditor.h 
	detailgroupsectiondialog.h 
	editpreferences.h 
	dbfiledialog.h 
	querycombobox.h 
	data.h 
	../common/labelsizeinfo.h 
	../common/pagesizeinfo.h 
	../common/builtinformatfunctions.h
)



QT4_WRAP_UI(prj_UI_BUILT_SOURCES
	labeleditor.ui 
	queryeditor.ui 
	querylist.ui 
	pagesetup.ui 
	sectioneditor.ui 
	detailsectiondialog.ui 
	fieldeditor.ui 
	texteditor.ui 
	barcodeeditor.ui 
	reportproperties.ui 
	reportparameter.ui 
	reportparameterlist.ui 
	reportparameterlistitem.ui 
	imageeditor.ui 
	coloreditor.ui 
	colorlist.ui 
	grapheditor.ui 
	detailgroupsectiondialog.ui 
	editpreferences.ui 
	dbfiledialog.ui
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_AUTOMOC(${prj_SRCS})
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_openrpt_wrtembed_SOURCE_DIR}/stopenrptwrtembedexport.h.in"
               "${st_openrpt_wrtembed_BINARY_DIR}/stopenrptwrtembedexport.h")
INCLUDE_DIRECTORIES( ${st_openrpt_wrtembed_BINARY_DIR} )


#Resource files
SET(prj_RCCS 
	../images/OpenRPTWrtembed.qrc
	../images/OpenRPTCommon.qrc)
#SET(prj_RCCS ../images/OpenRPTCommon.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_openrpt_wrtembed SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS} ${prj_UI_BUILT_SOURCES})

SET_TARGET_PROPERTIES(st_openrpt_wrtembed PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_openrpt_wrtembed ${QT_LIBRARIES} st_openrpt_common st_openrpt_renderer)

INSTALL(TARGETS st_openrpt_wrtembed DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
