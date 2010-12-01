if (APPLE)
	# prepare doxygen configuration file
	configure_file(${st-base_SOURCE_DIR}/cmakemodules/fix_stlibs_bundle.sh.in ${CMAKE_CURRENT_BINARY_DIR}/fix_stlibs_bundle.sh)

	# add doxygen as target
	add_custom_target(fixmacbundle ${CMAKE_CURRENT_BINARY_DIR}/fix_stlibs_bundle.sh ${CMAKE_CURRENT_BINARY_DIR}/fix_stlibs_bundle.sh ${CMAKE_CURRENT_BINARY_DIR})

	# cleanup on "make clean"
	set_property(DIRECTORY APPEND PROPERTY
			 ADDITIONAL_MAKE_CLEAN_FILES fixmacbundle)
ENDIF(APPLE)
