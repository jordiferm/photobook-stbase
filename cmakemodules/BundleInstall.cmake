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
	#ADD_SUBDIRECTORY(data)

#Main app target
	INSTALL(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)

ELSE(WIN32)

#Bundles on Mac an Unix like systems

    SET(plugin_dest_dir bin)
    SET(qtconf_dest_dir bin)
	SET(${CMAKE_PROJECT_NAME}bin_dest_dir bin)
	SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_PROJECT_NAME}")
    IF(APPLE)
	  SET(plugin_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/MacOS)
	  SET(qtconf_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/Resources)
	  SET(${CMAKE_PROJECT_NAME}bin_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/MacOS)
	  SET(APPS "\${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}.app")
    ENDIF(APPLE)
    IF(WIN32)
	  SET(APPS "\${CMAKE_INSTALL_PREFIX}/bin/${CMAKE_PROJECT_NAME}.exe")
    ENDIF(WIN32)

    #--------------------------------------------------------------------------------
	# Install the ${CMAKE_PROJECT_NAME} application, on Apple, the bundle is at the root of the
    # install tree, and on other platforms it'll go into the bin directory.
	INSTALL(TARGETS ${CMAKE_PROJECT_NAME}
	    BUNDLE DESTINATION . COMPONENT Runtime
	    RUNTIME DESTINATION bin COMPONENT Runtime
	    )


	IF (APPLE)
		#--------------------------------------------------------------------------------
		# Install needed Qt plugins by copying directories from the qt installation
		# One can cull what gets copied by using 'REGEX "..." EXCLUDE'
	#	SET(MY_PLUGINS_DIR "../installer/macosx/plugins")
	#	INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
	#	INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/codecs" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
	#	INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/sqldrivers" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
#		INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
#		INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/codecs" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
#		INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/sqldrivers" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

		#--------------------------------------------------------------------------------
		# install a qt.conf file
		# this inserts some cmake code into the install script to write the file
		INSTALL(CODE "
			file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]
		Plugins = MacOS/plugins\")
			" COMPONENT Runtime)


		#--------------------------------------------------------------------------------
		# Use BundleUtilities to get all other dependencies for the application to work.
		# It takes a bundle or executable along with possible plugins and inspects it
		# for dependencies.  If they are not system dependencies, they are copied.

		# directories to look for dependencies
		SET(DIRS "${QT_LIBRARY_DIRS}")

		# Now the work of copying dependencies into the bundle/package
		# The quotes are escaped and variables to use at install time have their $ escaped
		# An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
		# Note that the image plugins depend on QtSvg and QtXml, and it got those copied
		# over.
		#include(BundleUtilities)
		#verify_app("/Users/jordifernandez/desenvol/starblitz-k-soft/sysfolderprint/build/sysfolderprint.app")
		#fixup_bundle("/Users/jordifernandez/desenvol/starblitz-k-soft/sysfolderprint/build/sysfolderprint.app" "${QTPLUGINS}" "${DIRS}")
		INSTALL(CODE "
			file(GLOB_RECURSE QTPLUGINS
			  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
			set(QTPLUGINS \"\")
			include(BundleUtilities)
			fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
			" COMPONENT Runtime)

		SET(MY_PLUGINS_DIR "../installer/macosx/plugins")
		INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
		INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/codecs" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)
		INSTALL(DIRECTORY "${MY_PLUGINS_DIR}/sqldrivers" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

		#QTWebKit needs phonon
		#TODO: See why BundleUtilities does not install this dependence.
		INSTALL(FILES "/Library/Frameworks/phonon.framework/Versions/4/phonon" DESTINATION
			"${CMAKE_PROJECT_NAME}.app/Contents/Frameworks/phonon.framework/Versions/4/" COMPONENT Runtime)

	    set(CPACK_BINARY_DRAGNDROP ON)
    ENDIF (APPLE)

ENDIF(WIN32)
