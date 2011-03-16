# --------------------------------------------------------------------------
# General
PROJECT(st_prnlib)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_imagelib_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_imagelib_BINARY_DIR} )

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTNETWORK TRUE) 
SET(QT_USE_QTSQL TRUE)
INCLUDE(${QT_USE_FILE})


# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	abstractimageeffectmanager.cpp
	abstractimageeffectmodel.cpp
	abstractimageeffectwidget.cpp
	advancedeffectfilterwidget.cpp
	gammaeffectwidget.cpp
	imageeffectmodel.cpp
	imageenhancementeffectwidget.cpp
	imagepreviewwidget.cpp
	kpiprinter.cpp
	labeledsliderwidget.cpp
	sharpeneffectwidget.cpp
	kpprinterinfo.cpp
)

SET(prj_MOC_HDRS
	abstractimageeffectmanager.h
	abstractimageeffectmodel.h
	abstractimageeffectwidget.h
	advancedeffectfilterwidget.h
	gammaeffectwidget.h
	imageeffectmodel.h
	imageenhancementeffectwidget.h
	imagepreviewwidget.h
	kpiprinter.h
	labeledsliderwidget.h
	sharpeneffectwidget.h
	kpprinterinfo.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_prnlib_SOURCE_DIR}/stprnlibexport.h.in"
               "${st_prnlib_BINARY_DIR}/stprnlibexport.h")
INCLUDE_DIRECTORIES( ${st_prnlib_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_prnlib SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_prnlib PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_prnlib ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} st_imagelib)

INSTALL(TARGETS st_prnlib DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
