#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_digikam)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_kdcraw_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_kdcraw_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/themeengine )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/dimg )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/dimg/loaders )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/dimg/filters )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/dmetadata )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/curves )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/levels )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/whitebalance )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/jpegutils )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/threadimageio )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/lprof )
INCLUDE_DIRECTORIES( ${st_digikam_SOURCE_DIR}/kde )

# Configuration 
OPTION(ST_QIMAGELOADERONLY
  "Use only QImage Reader"
  ON)

OPTION(ST_ICC_SUPPORT
  "ICC Support with lcms"
  OFF)

IF ( ST_ICC_SUPPORT)
	SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "ST_ICC_SUPPORT" )
	SET ( DIGIKAM_LPROF_SRCS
	./lprof/cmshull.cpp
	./lprof/cmslm.cpp
	./lprof/cmslnr.cpp
	./lprof/cmsmatn.cpp
	./lprof/cmsmkmsh.cpp
	./lprof/cmsmntr.cpp
	./lprof/cmsoutl.cpp
	./lprof/cmspcoll.cpp
	./lprof/cmsprf.cpp
	./lprof/cmsreg.cpp
	./lprof/cmsscn.cpp
	./lprof/cmssheet.cpp
	)

	SET ( DIGIKAM_ICC_SRCS
	iccprofileinfodlg.cpp
	iccprofilewidget.cpp
	colorcorrectiondlg.cpp
	cietonguewidget.cpp
	)

	SET ( DIGIKAM_ICC_HDRS
	iccprofileinfodlg.h
	iccprofilewidget.h
	colorcorrectiondlg.h
	cietonguewidget.h
	)
	
	INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/FindLCMS.cmake)
	SET( DIGIKAM_DEP_INCLUDE_DIR ${DIGIKAM_DEP_INCLUDE_DIR} ${LCMS_INCLUDE_DIR} )
	SET( DIGIKAM_DEP_LIBRARIES ${DIGIKAM_DEP_LIBRARIES} ${LCMS_LIBRARIES} )

ENDIF(ST_ICC_SUPPORT)


IF(ST_QIMAGELOADERONLY)
	SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "ST_QIMAGELOADERONLY" )
ELSE(ST_QIMAGELOADERONLY)
	SET ( DIGIKAM_IMG_LOADERS_SRCS
	./dimg/loaders/iccjpeg.c
	./dimg/loaders/jp2ksettings.cpp
	./dimg/loaders/jp2kloader.cpp
	./dimg/loaders/jpegloader.cpp
	./dimg/loaders/jpegsettings.cpp
	./dimg/loaders/pngloader.cpp
	./dimg/loaders/pngsettings.cpp
	./dimg/loaders/ppmloader.cpp
	./dimg/loaders/rawloader.cpp
	./dimg/loaders/tiffloader.cpp
	./dimg/loaders/tiffsettings.cpp
		)

	SET ( DIGIKAM_IMG_LOADERS_HDRS
	./dimg/loaders/jp2ksettings.h
	./dimg/loaders/jp2kloader.h
	./dimg/loaders/jpegloader.h
	./dimg/loaders/jpegsettings.h
	./dimg/loaders/pngloader.h
	./dimg/loaders/pngsettings.h
	./dimg/loaders/ppmloader.h
	./dimg/loaders/rawloader.h
	./dimg/loaders/tiffloader.h
	./dimg/loaders/tiffsettings.h
	)
ENDIF(ST_QIMAGELOADERONLY)

# --------------------------------------------------------------------------
#Libs 
# Lib JPEG
IF(NOT JPEG_FOUND)
#  MESSAGE(SEND_ERROR "JPEG library not found.")
	FIND_PACKAGE(JPEG REQUIRED)
	SET( DIGIKAM_DEP_INCLUDE_DIR ${DIGIKAM_DEP_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} )
	SET( DIGIKAM_DEP_LIBRARIES ${DIGIKAM_DEP_LIBRARIES} ${JPEG_LIBRARIES} )
ENDIF(NOT JPEG_FOUND)

INCLUDE_DIRECTORIES(${DIGIKAM_DEP_INCLUDE_DIR})

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_kexiv2_SOURCE_DIR}/stmodinclude.cmake)

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QT3SUPPORT TRUE)
INCLUDE(${QT_USE_FILE})



# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	./themeengine/texture.cpp
	./themeengine/theme.cpp
	./themeengine/themeengine.cpp
	./curves/imagecurves.cpp
	./levels/imagelevels.cpp
	./whitebalance/whitebalance.cpp
	./dimg/dcolor.cpp
	./dimg/dcolorcomposer.cpp
	./dimg/dimg.cpp
	./dimg/dimgscale.cpp
	./dimg/loaders/dimgloader.cpp
	./dimg/loaders/qimageloader.cpp
	${DIGIKAM_IMG_LOADERS_SRCS}
	${DIGIKAM_LPROF_SRCS}
	./dimg/filters/bcgmodifier.cpp
	./dimg/filters/colormodifier.cpp
	./dimg/filters/dimggaussianblur.cpp
	./dimg/filters/dimgimagefilters.cpp
	./dimg/filters/dimgsharpen.cpp
	./dimg/filters/dimgthreadedfilter.cpp
	./dimg/filters/hslmodifier.cpp
	./dimg/filters/icctransform.cpp
	./dmetadata/dmetadata.cpp
	./dmetadata/geodetictools.cpp
	./dmetadata/photoinfocontainer.cpp
	./jpegutils/jpegutils.cpp
	./jpegutils/jpegwin.cpp
	./jpegutils/transupp.cpp
	./threadimageio/loadingcache.cpp
	./threadimageio/loadingcacheinterface.cpp
	./threadimageio/loadingdescription.cpp
	./threadimageio/loadsavetask.cpp
	./threadimageio/loadsavethread.cpp
	./threadimageio/managedloadsavethread.cpp
	./threadimageio/previewloadthread.cpp
	./threadimageio/previewtask.cpp
	./threadimageio/sharedloadsavethread.cpp
	./threadimageio/thumbnailbasic.cpp
	./threadimageio/thumbnailcreator.cpp
	./threadimageio/thumbnailloadthread.cpp
	./threadimageio/thumbnailtask.cpp
	./kde/kxyselector.cpp
	./kde/khuesaturationselect.cpp
	imagehistogram.cpp
	histogramwidget.cpp
	histogrambox.cpp
	colorgradientwidget.cpp
	editortoolsettings.cpp
	editortool.cpp
	imagepaniconwidget.cpp
	paniconwidget.cpp
	imageguidewidget.cpp
	imagepanelwidget.cpp
	imagewidget.cpp
	editortooliface.cpp
	imageiface.cpp
	imageregionwidget.cpp
	previewwidget.cpp
	dimginterface.cpp
	undomanager.cpp
	${DIGIKAM_ICC_SRCS}
	undoaction.cpp
	undocache.cpp
	metadatawidget.cpp
	metadatalistview.cpp
	metadatalistviewitem.cpp
	mdkeylistviewitem.cpp
	searchtextbar.cpp
	curveswidget.cpp
	hspreviewwidget.cpp
	dgradientslider.cpp
	whitebalancetool.cpp
	adjustcurvestool.cpp
	bwsepiatool.cpp
	adjustlevelstool.cpp
	channelmixertool.cpp
	colorfxtool.cpp
	autocorrectiontool.cpp
	bcgtool.cpp
	hsltool.cpp
	rgbtool.cpp
	oilpaint.cpp
	oilpainttool.cpp
	raindrop.cpp
	raindroptool.cpp
	blurfx.cpp
	blurfxtool.cpp
	charcoal.cpp
	charcoaltool.cpp
	distortionfx.cpp
	distortionfxtool.cpp
	kpopupframe.cpp
	drubberband.cpp
	canvas.cpp
)

SET(prj_MOC_HDRS
	./themeengine/texture.h
	./themeengine/theme.h
	./themeengine/themeengine.h
	./curves/imagecurves.h
	./levels/imagelevels.h
	./whitebalance/whitebalance.h
	./dimg/dcolor.h
	./dimg/dcolorcomposer.h
	./dimg/dimg.h
	./dimg/loaders/dimgloader.h
	${DIGIKAM_IMG_LOADERS_HDRS}
	./dimg/loaders/qimageloader.h
	./dimg/filters/bcgmodifier.h
	./dimg/filters/colormodifier.h
	./dimg/filters/dimggaussianblur.h
	./dimg/filters/dimgimagefilters.h
	./dimg/filters/dimgsharpen.h
	./dimg/filters/dimgthreadedfilter.h
	./dimg/filters/hslmodifier.h
	./dimg/filters/icctransform.h
	./dmetadata/dmetadata.h
	./dmetadata/geodetictools.h
	./dmetadata/photoinfocontainer.h
	./jpegutils/jpegutils.h
	./jpegutils/jpegwin.h
	./jpegutils/transupp.h
	./threadimageio/loadingcache.h
	./threadimageio/loadingcacheinterface.h
	./threadimageio/loadingdescription.h
	./threadimageio/loadsavetask.h
	./threadimageio/loadsavethread.h
	./threadimageio/managedloadsavethread.h
	./threadimageio/previewloadthread.h
	./threadimageio/previewtask.h
	./threadimageio/sharedloadsavethread.h
	./threadimageio/thumbnailcreator.h
	./threadimageio/thumbnailloadthread.h
	./threadimageio/thumbnailtask.h
	./kde/kxyselector.h
	./kde/khuesaturationselect.h
	imagehistogram.h
	histogramwidget.h
	histogrambox.h
	colorgradientwidget.h
	editortoolsettings.h
	editortool.h
	imagepaniconwidget.h
	paniconwidget.h
	imageguidewidget.h
	imagepanelwidget.h
	imagewidget.h
	editortooliface.h
	imageiface.h
	imageregionwidget.h
	previewwidget.h
	dimginterface.h
	undomanager.h
	${DIGIKAM_ICC_HDRS}
	undoaction.h
	undocache.h
	metadatawidget.h
	metadatalistview.h
	metadatalistviewitem.h
	mdkeylistviewitem.h
	searchtextbar.h
	curveswidget.h
	hspreviewwidget.h
	dgradientslider.h
	whitebalancetool.h
	adjustcurvestool.h
	bwsepiatool.h
	adjustlevelstool.h
	channelmixertool.h
	colorfxtool.h
	autocorrectiontool.h
	bcgtool.h
	hsltool.h
	rgbtool.h
	oilpaint.h
	oilpainttool.h
	raindrop.h
	raindroptool.h
	blurfx.h
	blurfxtool.h
	charcoal.h
	charcoaltool.h
	distortionfx.h
	distortionfxtool.h
	kpopupframe.h
	drubberband.h
	canvas.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
SET(prj_RCCS ./icons/st-digikam-icons.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_digikam_SOURCE_DIR}/stdigikamexport.h.in"
               "${st_digikam_BINARY_DIR}/stdigikamexport.h")
INCLUDE_DIRECTORIES( ${st_digikam_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_digikam SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_digikam PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_digikam ${QT_LIBRARIES} ${ST_KEXIV2_LIBRARIES} ${DIGIKAM_DEP_LIBRARIES} st_core st_widgets st_kdcraw)

INSTALL(TARGETS st_digikam DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
