# Once loaded this will define
#  KEXIV2_DEP_INCLUDE_DIR - imagelib deps include dir
#  KEXIV2_DEP_DEFINITIONS - imagelib deps definitions
#  KEXIV2_DEP_LIBRARIES - imagelib deps libraries

# --------------------------------------------------------------------------
# LibExiv2

INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/exiv2.cmake)

SET( KEXIV2_DEP_INCLUDE_DIR ${EXIV2_INCLUDE_DIR} )
IF(WIN32)
	SET( KEXIV2_DEP_INCLUDE_DIR ${KEXIV2_DEP_INCLUDE_DIR} ${EXIV2_INCLUDE_DIR} )
ELSE(WIN32)
	SET( KEXIV2_DEP_INCLUDE_DIR ${KEXIV2_DEP_INCLUDE_DIR} ${EXIV2_INCLUDE_DIR}/exiv2 )
ENDIF(WIN32)
SET( KEXIV2_DEP_DEFINITIONS ${EXIV2_DEFINITIONS})
SET( KEXIV2_DEP_LIBRARIES ${EXIV2_LIBRARIES} )
