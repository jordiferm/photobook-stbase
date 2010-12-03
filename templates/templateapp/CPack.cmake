# -- For CPack --
SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Here goes the project description")
SET(CPACK_PACKAGE_VENDOR "Starblitz")

SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${PRJ_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PRJ_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PRJ_VERSION_PATCH}")
SET(CPACK_PACKAGE_EXECUTABLES ""${CMAKE_PROJECT_NAME};"${CMAKE_PROJECT_NAME}")

# ---------------------------  WIN 32 -----------------------------

IF(WIN32 AND NOT UNIX)
	SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cpacktemplates" ${CMAKE_MODULE_PATH})
	# There is a bug in NSI that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backlasshes.
	SET(CPACK_PACKAGE_ICON "${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/../installer/windows\\\\logo_starblitz.bmp")
	SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
	#SET(CPACK_NSIS_INSTALLED_ICON_NAME "${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/../installer/windows/files/ico\\\\sysfolderprint.ico")
	SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} sysfolderprint")
	SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.starblitz-k.com")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.starblitz-k.com")
	SET(CPACK_NSIS_CONTACT "jordif.starblitz@gmail.com")
	SET(CPACK_NSIS_MODIFY_PATH OFF)

	set(CPACK_NSIS_CREATE_ICONS "
			CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${CPACK_PACKAGE_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${CMAKE_PROJECT_NAME}.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\${CMAKE_PROJECT_NAME}.ico\\\"
			CreateShortCut \\\"$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${CMAKE_PROJECT_NAME}.exe\\\"  \\\"\\\" \\\"$INSTDIR\\\\bin\\\\${CMAKE_PROJECT_NAME}.ico\\\"
			CreateShortCut \\\"$QUICKLAUNCH\\\\${CPACK_PACKAGE_NAME}.lnk\\\" \\\"$INSTDIR\\\\${CMAKE_PROJECT_NAME}.exe\\\"  \\\"\\\" \\\"$INSTDIR\\\\bin\\\\${CMAKE_PROJECT_NAME}.ico\\\"
	")
	set(CPACK_NSIS_DELETE_ICONS "
			Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\${CPACK_PACKAGE_NAME}.lnk\\\"
			Delete \\\"$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk\\\"
			Delete \\\"$QUICKLAUNCH\\\\${CPACK_PACKAGE_NAME}.lnk\\\"
	")

# ---------------------------  GNU/Linux -----------------------------
ELSE(WIN32 AND NOT UNIX)
	IF(APPLE)
		SET(CPACK_GENERATOR "DragNDrop")
		SET(CPACK_OSX_PACKAGE_VERSION "10.6")
	ELSE(APPLE)
		#SET(CPACK_GENERATOR "TGZ;TBZ2;STGZ;RPM;DEB")
		SET(CPACK_GENERATOR "DEB")
		SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libqt4-core (>=4.3.2), libqt4-gui (>=4.3.2), libqt4-sql (>=4.3.2), libqt4-qt3support (>=4.3.2), liblcms1, libmagick++1 ,libexiv2-5")
		#For Debian
		SET(CPACK_DEBIAN_PACKAGE_NAME "starblitz-k-sysfolderprint")
		SET(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
		SET(CPACK_PACKAGE_CONTACT "jordif.starblitz@gmail.com")
		SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "jordif.starblitz@gmail.com")
		SET(CPACK_STRIP_FILES "bin/sysfolderprint")
		SET(CPACK_SOURCE_STRIP_FILES "")
	ENDIF(APPLE)
ENDIF(WIN32 AND NOT UNIX)

INCLUDE(CPack)

