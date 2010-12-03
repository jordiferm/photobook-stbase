#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# ----------------------- Configuration ---------------------
PROJECT(templateapp)
cmake_minimum_required(VERSION 2.8)


# -----------------------    Version    ---------------------
set (PRJ_VERSION_MAJOR "1")
set (PRJ_VERSION_MINOR "0")
set (PRJ_VERSION_PATCH "0")

SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "PRJ_VERSION_MAJOR=${PRJ_VERSION_MAJOR}")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "PRJ_VERSION_MINOR=${PRJ_VERSION_MINOR}")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "PRJ_VERSION_PATCH=${PRJ_VERSION_PATCH}")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "RELEASE_CODENAME=\"Take The Time\"")


#--------------------- Include ST Lib -----------------------
SET(ST_APP_NAME ${CMAKE_PROJECT_NAME})
SET(ST_USE_WIDGETS TRUE)
SET(ST_USE_IMAGELIB TRUE)
ADD_SUBDIRECTORY(stbase)
INCLUDE(${st-base_SOURCE_DIR}/STCommon.cmake)


#--------------------- Check For Qt -----------------------
SET(QT_REQUIRED TRUE)
SET(QT_MIN_VERSION "4.2.2")
FIND_PACKAGE(Qt4)

IF(NOT QT4_FOUND)
  MESSAGE(SEND_ERROR "Qt ${QT_MIN_VERSION} or greater not found.  Please check the QT_QMAKE_EXECUTABLE variable.")
ELSE(NOT QT4_FOUND)

INCLUDE(${QT_USE_FILE})


# We dont want console in windows (Not Checked)
IF(WIN32)
 set(CMAKE_EXE_LINKER_FLAGS "-mwindows" CACHE STRING "Dont use windows console" FORCE)
ENDIF(WIN32)

# ----------------------- Project Files ----------------------
# -- Resource files
SET(prj_RCCS ./images/images.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})


# -- Source files
SET(prj_SRCS
	main.cpp
	mainwindow.cpp
)

SET(prj_MOC_HDRS
	mainwindow.h
)


# -- Qt Meta Object Compiler
QT4_AUTOMOC(${prj_SRCS})
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})


# -- Apple Icons
IF(APPLE)
  SET(MACOSX_BUNDLE_ICON_FILE appicon.icns)
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/images/appicon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  SET(prj_SRCS ${prj_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/images/appicon.icns)
ENDIF(APPLE)

# -- Translations

file (GLOB TRANSLATIONS_FILES translations/*.ts)
qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})

# to update ts files run: make update_ts
SET(PRJ_LANGUAGES "ca es en")
SET(TRANSLATE_PATHS ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/stbasetranslations.cmake)

# ----------------------- Target and Installation -------------------------

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE WIN32 ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS} ${QM_FILES})

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${QT_LIBRARIES} ${ST_LIBRARIES})

INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/fixmacbundle.cmake)
INCLUDE(${st-base_SOURCE_DIR}/cmakemodules/BundleInstall.cmake)

ADD_SUBDIRECTORY(data)

INCLUDE(CPack.cmake)

ENDIF(NOT QT4_FOUND)
