
# Once loaded this will define
#  IMAGELIB_DEP_INCLUDE_DIR - imagelib deps include dir
#  IMAGELIB_DEP_DEFINITIONS - imagelib deps definitions
#  IMAGELIB_DEP_LIBRARIES - imagelib deps libraries

# --------------------------------------------------------------------------
# LibExiv2

INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/exiv2.cmake)

SET( IMAGELIB_DEP_INCLUDE_DIR ${EXIV2_INCLUDE_DIR} )
SET( IMAGELIB_DEP_INCLUDE_DIR ${IMAGELIB_DEP_INCLUDE_DIR} ${EXIV2_INCLUDE_DIR}/exiv2 )
SET( IMAGELIB_DEP_DEFINITIONS ${EXIV2_DEFINITIONS})
SET( IMAGELIB_DEP_LIBRARIES ${EXIV2_LIBRARIES} )


# --------------------------------------------------------------------------
# LibMagick and LibMagick++

OPTION(OLD_IMAGEMAGICK
  "Imagemagick old packages (Ubuntu 8.04 and older)"
  OFF)


IF(OLD_IMAGEMAGICK) #Deprecated, this part is here for compatibility with old linux distros.
	INCLUDE(${st_SOURCE_DIR}/cmakemodules/FindMagick.cmake)
	
	# LibMagick
	IF(NOT MAGICK_FOUND)
	MESSAGE(SEND_ERROR "ImageMagick library not found.")
	ENDIF(NOT MAGICK_FOUND)
	
	SET( IMAGELIB_DEP_INCLUDE_DIR ${IMAGELIB_DEP_INCLUDE_DIR} ${MAGICK_INCLUDE_DIR} )
	
	IF ( WIN32 )
			SET(IMAGELIB_DEP_LIBRARIES ${IMAGELIB_DEP_LIBRARIES} Magick++ ${MAGICK_LIBRARIES} gdi32 m )
	ELSE ( WIN32 )
			SET(IMAGELIB_DEP_LIBRARIES ${IMAGELIB_DEP_LIBRARIES} ${MAGICK_LIBRARIES} Magick++ )
	ENDIF ( WIN32 )
	
	# LibMagick++
	IF(NOT MAGICK++_FOUND)
	MESSAGE(SEND_ERROR "ImageMagick++ library not found.")
	ENDIF(NOT MAGICK++_FOUND)
	
	SET( IMAGELIB_DEP_INCLUDE_DIR ${IMAGELIB_DEP_INCLUDE_DIR} ${MAGICK++_INCLUDE_DIR} )
	SET( IMAGELIB_DEP_LIBRARIES ${IMAGELIB_DEP_LIBRARIES} ${MAGICK++_LIBRARIES} )

ELSE(OLD_IMAGEMAGICK)
	# New LibMagick++
	FIND_PACKAGE(ImageMagick COMPONENTS Magick++)
	
	IF(NOT ImageMagick_Magick++_FOUND)
	MESSAGE(SEND_ERROR "ImageMagick++ library not found.")
	ENDIF(NOT ImageMagick_Magick++_FOUND)
	
	SET( IMAGELIB_DEP_INCLUDE_DIR ${IMAGELIB_DEP_INCLUDE_DIR} ${ImageMagick_Magick++_INCLUDE_DIR} )
	
	IF ( WIN32 )
			SET(IMAGELIB_DEP_LIBRARIES ${IMAGELIB_DEP_LIBRARIES} ${ImageMagick_Magick++_LIBRARY} gdi32 m )
	ELSE ( WIN32 )
			SET(IMAGELIB_DEP_LIBRARIES ${IMAGELIB_DEP_LIBRARIES} ${ImageMagick_Magick++_LIBRARY} )
	ENDIF ( WIN32 )
ENDIF(OLD_IMAGEMAGICK)

# --------------------------------------------------------------------------
# LibJPEG

# Lib JPEG
IF(NOT JPEG_FOUND)
#  MESSAGE(SEND_ERROR "JPEG library not found.")
	FIND_PACKAGE(JPEG REQUIRED)
	SET( IMAGELIB_DEP_INCLUDE_DIR ${IMAGELIB_DEP_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} )
	SET( IMAGELIB_DEP_LIBRARIES ${IMAGELIB_DEP_LIBRARIES} ${JPEG_LIBRARIES} )
ENDIF(NOT JPEG_FOUND)


