# ----------------------- Installation -------------------------

IF(WIN32)
#Manually install dependences in windows
#Qt libs
    SET ( WINQTLIBS_PATH "../installer/windows/files/libs/qt" )
    FILE(GLOB qtlibfiles "${WINQTLIBS_PATH}/*.dll")
    INSTALL(FILES ${qtlibfiles} DESTINATION bin)
    FILE(GLOB qtconffiles "${WINQTLIBS_PATH}/*.conf")
    INSTALL(FILES ${qtconffiles} DESTINATION bin)
    INSTALL(DIRECTORY ${WINQTLIBS_PATH}/plugins DESTINATION bin)
    INSTALL(DIRECTORY ${WINQTLIBS_PATH}/translations DESTINATION bin)

#Other Libs
    SET ( WINDEPLIBS_PATH "../installer/windows/files/libs" )
    FILE(GLOB deplibfiles "${WINDEPLIBS_PATH}/*.dll")
    INSTALL(FILES ${deplibfiles} DESTINATION bin)

#Data, translations, icons and app files.
	SET(${CMAKE_PROJECT_NAME}bin_dest_dir bin)

#Main app target
	INSTALL(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${${CMAKE_PROJECT_NAME}bin_dest_dir})
ELSE(WIN32)

#Bundles on Mac an Unix like systems

	SET(${CMAKE_PROJECT_NAME}bin_dest_dir bin/${ST_APP_NAME})
	IF (UNIX AND NOT APPLE)
		SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_PROJECT_NAME}")
		INSTALL(TARGETS ${CMAKE_PROJECT_NAME}
			BUNDLE DESTINATION ./${ST_APP_NAME} COMPONENT Runtime
			RUNTIME DESTINATION ${${CMAKE_PROJECT_NAME}bin_dest_dir} COMPONENT Runtime
			)
	ENDIF (UNIX AND NOT APPLE)


	IF (APPLE)
		SET(plugin_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/MacOS)
		SET(qtconf_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/Resources)
		SET(${CMAKE_PROJECT_NAME}bin_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/MacOS)
		SET(APPS "\${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}.app")
		#	SET(APP_BUNDLE_NAME ${CMAKE_PROJECT_NAME}.app)
		#	SET(BUNDLE_INSTALL_TARGET "\${CMAKE_INSTALL_PREFIX}/${APP_BUNDLE_NAME}")
		INSTALL(TARGETS ${CMAKE_PROJECT_NAME} BUNDLE DESTINATION . COMPONENT Runtime)

		# install a qt.conf file
		INSTALL(CODE "
			file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]
		Plugins = MacOS/plugins\")
			" COMPONENT Runtime)

		# directories to look for dependencies
		SET(DIRS "${QT_LIBRARY_DIRS}")

		INSTALL(CODE "
			file(GLOB_RECURSE QTPLUGINS
			  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#			set(QTPLUGINS \"\")
			include(BundleUtilities)
			fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
			" COMPONENT Runtime)

		SET(MY_PLUGINS_DIR "../installer/macosx/plugins")
		INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
		INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/codecs" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
		INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/sqldrivers" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

	    set(CPACK_BINARY_DRAGNDROP ON)
    ENDIF (APPLE)

ENDIF(WIN32)
