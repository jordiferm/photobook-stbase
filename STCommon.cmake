
INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/core )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )

SET( ST_LIBRARIES st_core )

IF (ST_USE_WIDGETS)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/widgets )
	INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_widgets )
ENDIF (ST_USE_WIDGETS)

IF (ST_USE_EMBEDDEDWIDGETS)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/embeddedwidgets )
	INCLUDE_DIRECTORIES( ${st_embeddedwidgets_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_embeddedwidgets )
ENDIF (ST_USE_EMBEDDEDWIDGETS)

IF (ST_USE_DATABASE)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/database )
	INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_database )
ENDIF (ST_USE_DATABASE)

IF (ST_USE_IMAGELIB)
	# Module include for libs that depends on other libs.
	INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)
	SET( ST_LIBRARIES ${ST_LIBRARIES} ${ST_IMAGELIB_LIBRARIES})
ENDIF (ST_USE_IMAGELIB)

IF (ST_USE_KEXIV2)
	# Module include for libs that depends on other libs.
	INCLUDE(${st_kexiv2_SOURCE_DIR}/stmodinclude.cmake)
	SET( ST_LIBRARIES ${ST_LIBRARIES} ${ST_KEXIV2_LIBRARIES})
ENDIF (ST_USE_KEXIV2)

IF (ST_USE_KDCRAW)
	INCLUDE_DIRECTORIES( ${st_kdcraw_SOURCE_DIR} )
	INCLUDE_DIRECTORIES( ${st_kdcraw_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_kdcraw )
ENDIF (ST_USE_KDCRAW)


IF (ST_USE_METADATA)
	INCLUDE_DIRECTORIES( ${st_metadata_SOURCE_DIR} )
	INCLUDE_DIRECTORIES( ${st_metadata_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_metadata )
ENDIF (ST_USE_METADATA)

IF (ST_USE_KPHOSCOS)
# 	# Module include for libs that depends on other libs.
# 	INCLUDE(${st-kphoscos_SOURCE_DIR}/stmodinclude.cmake)
	INCLUDE_DIRECTORIES( ${st_kphoscos_SOURCE_DIR} )
	INCLUDE_DIRECTORIES( ${st_kphoscos_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_kphoscos )
ENDIF (ST_USE_KPHOSCOS)

IF (ST_USE_DOM)
	INCLUDE_DIRECTORIES( ${st_dom_SOURCE_DIR} )
	INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_dom )
ENDIF (ST_USE_DOM)

IF (ST_USE_TICKETPRINTER)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/ticketprinter )
	INCLUDE_DIRECTORIES( ${st_ticketprinter_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_ticketprinter )
ENDIF (ST_USE_TICKETPRINTER)

IF (USE_PRINTKEEPER)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/printkeeper )
	INCLUDE_DIRECTORIES( ${st_printkeeper_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_printkeeper )
ENDIF (USE_PRINTKEEPER)

IF (ST_USE_DBMANAG)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/dbmanag )
	INCLUDE_DIRECTORIES( ${st_dbmanag_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_dbmanag )
ENDIF (ST_USE_DBMANAG)

IF (ST_USE_BURNING)
	INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/burning )
	INCLUDE_DIRECTORIES( ${st_burning_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_burning)
ENDIF (ST_USE_BURNING)

IF (ST_USE_SERIALPORT)
	IF (WIN32)
		ADD_DEFINITIONS ( -D_TTY_WIN_ )
	ELSE (WIN32)
		ADD_DEFINITIONS ( -D_TTY_POSIX_ )
	ENDIF (WIN32)

	INCLUDE_DIRECTORIES( ${st_serialport_SOURCE_DIR} )
	INCLUDE_DIRECTORIES( ${st_serialport_BINARY_DIR} )
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_serialport )
	
ENDIF (ST_USE_SERIALPORT)

IF (ST_USE_OPENRPT)
	INCLUDE_DIRECTORIES( 
		${st_openrpt_common_SOURCE_DIR} 
		${st_openrpt_common_BINARY_DIR} 
#		${st-base_SOURCE_DIR}/openrpt/OpenRPT/renderer
		${st_openrpt_renderer_SOURCE_DIR} 
		${st_openrpt_renderer_BINARY_DIR} 
		${st_openrpt_wrtembed_SOURCE_DIR} 
		${st_openrpt_wrtembed_BINARY_DIR} 
	)
	SET( ST_LIBRARIES ${ST_LIBRARIES} st_openrpt_common st_openrpt_renderer st_openrpt_wrtembed )
ENDIF (ST_USE_OPENRPT)

IF (ST_USE_DIGIKAM)
	INCLUDE(${st_digikam_SOURCE_DIR}/includedirs.cmake)

	SET( ST_LIBRARIES ${ST_LIBRARIES} st_digikam )
ENDIF (ST_USE_DIGIKAM)

IF (ST_USE_PHOTOTEMPLATES)
		INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/phototemplates )
		INCLUDE_DIRECTORIES( ${st_phototemplates_BINARY_DIR} )
		SET( ST_LIBRARIES ${ST_LIBRARIES} st_phototemplates )
ENDIF (ST_USE_PHOTOTEMPLATES)

IF (ST_USE_WIZARDS)
		INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/wizards )
		INCLUDE_DIRECTORIES( ${st_wizards_BINARY_DIR} )
		SET( ST_LIBRARIES ${ST_LIBRARIES} st_wizards )
ENDIF (ST_USE_WIZARDS)

IF (ST_USE_OOFFICE)
		INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/ooffice )
		INCLUDE_DIRECTORIES( ${st_ooffice_BINARY_DIR} )
		SET( ST_LIBRARIES ${ST_LIBRARIES} st_ooffice )
ENDIF (ST_USE_OOFFICE)

IF (ST_USE_TPOPSL)
		INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/tpopsl )
		INCLUDE_DIRECTORIES( ${st_tpopsl_BINARY_DIR} )
		SET( ST_LIBRARIES ${ST_LIBRARIES} st_tpopsl )
ENDIF (ST_USE_TPOPSL)

IF (ST_USE_OROMEL)
		INCLUDE_DIRECTORIES( ${st-base_SOURCE_DIR}/oromel )
		INCLUDE_DIRECTORIES( ${st_oromel_BINARY_DIR} )
		SET( ST_LIBRARIES ${ST_LIBRARIES} st_oromel )
ENDIF (ST_USE_OROMEL)
