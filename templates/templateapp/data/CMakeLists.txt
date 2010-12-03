#Data files for templatename

#--- Desktop related files ---

IF(WIN32 AND NOT UNIX)
	INSTALL(FILES 
		${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/images/appicon.ico
	DESTINATION bin)
ELSE(WIN32 AND NOT UNIX)
	IF(NOT APPLE)
		INSTALL(FILES
			${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/images/appicon.png
		DESTINATION share/icons/hicolor/64x64/apps/)
		INSTALL(DIRECTORY ${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/kde4/applications DESTINATION share)
		INSTALL(DIRECTORY ${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/kde4/menu DESTINATION share)
		INSTALL(FILES
			${${CMAKE_PROJECT_NAME}_SOURCE_DIR}/scripts/runtemplateapp
		DESTINATION bin PERMISSIONS WORLD_EXECUTE WORLD_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
						  GROUP_EXECUTE GROUP_READ)
	ENDIF(NOT APPLE)
ENDIF(WIN32 AND NOT UNIX)

#INSTALL(FILES
#	${${CMAKE_PACKAGE_NAME}_SOURCE_DIR}/templateapp_style.txt
#DESTINATION bin)

# ---------------- Translations --------------------

install(FILES ${QM_FILES} DESTINATION ${${CMAKE_PROJECT_NAME}bin_dest_dir})

#SET(QMFILES_PATH "../../translations")
#FILE(GLOB qmfiles "${QMFILES_PATH}/${CMAKE_PROJECT_NAME}_*.qm")
#INSTALL(FILES ${qmfiles} DESTINATION ${${CMAKE_PROJECT_NAME}bin_dest_dir})

#In apple we need to install qt system translations
IF (APPLE)
	SET ( QTLIBS_PATH "/Developer/Applications/Qt/" )
	SET ( SYS_TRANSLATIONS_DESTPATH "${${CMAKE_PROJECT_NAME}bin_dest_dir}/../translations/" )
	INSTALL(FILES ${QTLIBS_PATH}/translations/qt_es.qm DESTINATION ${SYS_TRANSLATIONS_DESTPATH})
	INSTALL(FILES ${QTLIBS_PATH}/translations/qt_fr.qm DESTINATION ${SYS_TRANSLATIONS_DESTPATH})
ENDIF(APPLE)
