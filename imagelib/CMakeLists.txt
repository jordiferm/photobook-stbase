# --------------------------------------------------------------------------
# General
PROJECT(st_imagelib)
cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_imagelib_SOURCE_DIR}/cursors )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_printkeeper_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_printkeeper_BINARY_DIR} )

# --------------------------------------------------------------------------
# Configure Qt
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# ImageLib Dependences: libexiv2 and imagemagick

# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/imagelib.cmake)

INCLUDE_DIRECTORIES( ${IMAGELIB_DEP_INCLUDE_DIR} )
ADD_DEFINITIONS( ${IMAGELIB_DEP_DEFINITIONS} )

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	exifmetadata.cpp
	imimage.cpp
	imageTools.cpp
	selectionInterface.cpp
	cursors.cpp
	histogramEditor.cpp
	histogramInterface.cpp	
	scaledPreviewInterface.cpp
	splitViewInterface.cpp
	clickableLabel.cpp
	dynamicSlider.cpp
	blackWhite.cpp
	sepia.cpp
	ilcolor.cpp
	contrast.cpp
	crop.cpp
	redEye.cpp
	tilt.cpp
	jpeg/jpegSize.cpp
	jpeg/jpegTools.cpp
	jpeg/jpegInternal.c
	stimage.cpp
	stimagetools.cpp
	sttemporaryimagefile.cpp
	stphotoindexbuilder.cpp
	bf_skey.c
	bf_ecb.c
	bf_enc.c
	bf_cfb64.c
	bf_ofb64.c
	qxtblowfish.cpp
	simplecrypt.cpp
)

SET(prj_MOC_HDRS
	exifmetadata.h
	imimage.h
	imageTools.h
	selectionInterface.h
	cursors.h
	histogramEditor.h
	histogramInterface.h
	scaledPreviewInterface.h
	splitViewInterface.h
	clickableLabel.h
	dynamicSlider.h
	manipulationOptions.h
	blackWhite.h
	sepia.h
	ilcolor.h
	contrast.h
	crop.h
	redEye.h
	redEye_internal.h
	tilt.h
	tilt_internal.h	
	jpeg/jpegInternal.h
	jpeg/jpegSize.h
	jpeg/jpegTools.h
	stimage.h
	stimagetools.h
	sttemporaryimagefile.h
	stphotoindexbuilder.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})


# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_imagelib_SOURCE_DIR}/stimagelibexport.h.in"
               "${st_imagelib_BINARY_DIR}/stimagelibexport.h")

INCLUDE_DIRECTORIES( ${st_imagelib_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_imagelib SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_imagelib PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_imagelib ${QT_LIBRARIES} ${IMAGELIB_DEP_LIBRARIES} ${PRINTKEEPER_LIBS} st_widgets )
INSTALL(TARGETS st_imagelib DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
