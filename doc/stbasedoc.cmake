set (DOC_OUTPUT_PATH ${CMAKE_PROJECT_NAME}-${MODULE_NAME}-api)
# check if doxygen is even installed
find_package(Doxygen)
if (DOXYGEN_FOUND STREQUAL "NO")
	message(FATAL_ERROR "Doxygen not found. Please get a copy http://www.doxygen.org")
endif (DOXYGEN_FOUND STREQUAL "NO")

# prepare doxygen configuration file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# add doxygen as target
add_custom_target(doxygen-${MODULE_NAME} ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# cleanup $build/api-doc on "make clean"
set_property(DIRECTORY APPEND PROPERTY
		 ADDITIONAL_MAKE_CLEAN_FILES api-doc)

# add doxygen as dependency to doc-target
get_target_property(DOC_TARGET doc TYPE)
if(NOT DOC_TARGET)
	add_custom_target(doc)
endif()
add_dependencies(doc doxygen-${MODULE_NAME})

if (STBASE_INSTALL_DOCS)
	# install HTML API documentation and manual pages
	set(DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}-${VERSION}")

	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DOC_OUTPUT_PATH}/html
			 DESTINATION ${DOC_PATH}
		   )
endif (STBASE_INSTALL_DOCS)

