#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_openrpt_renderer)

# To locate ui generated .h files.
#INCLUDE_DIRECTORIES( ${st_openrpt_renderer_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_openrpt_renderer_SOURCE_DIR}/../common/
	${st_openrpt_renderer_SOURCE_DIR}/../../MetaSQL/ 
	${st_openrpt_renderer_SOURCE_DIR}/../../common/ 
	${st_openrpt_common_BINARY_DIR} )


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTSQL TRUE) 
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

ADD_DEFINITIONS ( -DMAKELIB )

SET(prj_MOC_SRCS
	openreports_wrapper.cpp 
	3of9.cpp 
	ext3of9.cpp 
	code128.cpp 
	codeean.cpp 
	graph.cpp 
	orutils.cpp 
	orprerender.cpp 
	orprintrender.cpp 
	renderobjects.cpp 
	previewdialog.cpp
	../../MetaSQL/metasql.cpp 
	../common/builtinformatfunctions.cpp 
	../common/labelsizeinfo.cpp 
	../common/pagesizeinfo.cpp 
)


# --------------------------------------------------------------------------
# Project files
SET(prj_MOC_HDRS
	openreports.h 
	barcodes.h 
	graph.h 
	orutils.h 
	orprerender.h 
	orprintrender.h 
	renderobjects.h 
	previewdialog.h
	../../MetaSQL/metasql.h 
	../common/builtinformatfunctions.h 
	../common/labelsizeinfo.h 
	../common/pagesizeinfo.h 
)


QT4_AUTOMOC(${prj_SRCS})


# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_openrpt_renderer_SOURCE_DIR}/stopenrptrendererexport.h.in"
               "${st_openrpt_renderer_BINARY_DIR}/stopenrptrendererexport.h")
INCLUDE_DIRECTORIES( ${st_openrpt_renderer_BINARY_DIR} )


#Resource files
#SET(prj_RCCS ../OpenRPT/images/OpenRPTCommon.qrc)
#QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_openrpt_renderer SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_openrpt_renderer PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_openrpt_renderer ${QT_LIBRARIES} st_openrpt_common)

INSTALL(TARGETS st_openrpt_renderer DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
