INCLUDE(${st_kexiv2_SOURCE_DIR}/kexiv2.cmake)
	
INCLUDE_DIRECTORIES( ${KEXIV2_DEP_INCLUDE_DIR} )
ADD_DEFINITIONS( ${KEXIV2_DEP_DEFINITIONS} )
	
INCLUDE_DIRECTORIES( ${st_kexiv2_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_kexiv2_BINARY_DIR} )
SET( ST_KEXIV2_LIBRARIES st_kexiv2 )


