#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_kexiv2)

# =======================================================
# Informations to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0
# 0.1.1 => 0.2.0
# 0.1.2 => 1.0.1
# 0.1.3 => 1.0.1
# 0.1.4 => 2.0.2
# 0.1.5 => 2.1.1
# 0.1.6 => 3.0.0
# 0.1.7 => 4.0.1
# 0.1.8 => 5.0.0
# ... here we can have new releases from KDE3 branch
# 0.2.0 => 6.0.0     (released with KDE 4.1.0)
# 0.3.0 => 7.0.0     (released with KDE 4.1.2)
# 0.4.0 => 7.1.0
# 0.5.0 => 7.2.0     (Released with KDE 4.2.0)

# Library API version
SET(KEXIV2_LIB_MAJOR_VERSION "0")
SET(KEXIV2_LIB_MINOR_VERSION "5")
SET(KEXIV2_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(KEXIV2_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual.html#Updating-version-info
SET(KEXIV2_LIB_SO_CUR_VERSION "7")
SET(KEXIV2_LIB_SO_REV_VERSION "2")
SET(KEXIV2_LIB_SO_AGE_VERSION "0")

# =======================================================
# Set env. variables accordinly.

SET(KEXIV2_LIB_VERSION_STRING "${KEXIV2_LIB_MAJOR_VERSION}.${KEXIV2_LIB_MINOR_VERSION}.${KEXIV2_LIB_PATCH_VERSION}${KEXIV2_LIB_SUFFIX_VERSION}")
SET(KEXIV2_LIB_VERSION_ID "0x0${KEXIV2_LIB_MAJOR_VERSION}0${KEXIV2_LIB_MINOR_VERSION}0${KEXIV2_LIB_PATCH_VERSION}")

SET(KEXIV2_LIB_SO_VERSION_STRING "${KEXIV2_LIB_SO_CUR_VERSION}.${KEXIV2_LIB_SO_REV_VERSION}.${KEXIV2_LIB_SO_AGE_VERSION}")

# =======================================================


INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )



# --------------------------------------------------------------------------
# Configure Qt

#SET(QT_USE_QTXML TRUE) 
INCLUDE(${QT_USE_FILE})


# Modules
INCLUDE(${st_kexiv2_SOURCE_DIR}/kexiv2.cmake)

INCLUDE_DIRECTORIES( ${KEXIV2_DEP_INCLUDE_DIR} )
ADD_DEFINITIONS( ${KEXIV2_DEP_DEFINITIONS} )


# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	kexiv2comments.cpp
	kexiv2.cpp
	kexiv2exif.cpp
	kexiv2gps.cpp
	kexiv2image.cpp
	kexiv2iptc.cpp
	kexiv2private.cpp
	kexiv2xmp.cpp
)

SET(prj_MOC_HDRS
	kexiv2.h
	kexiv2private.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
#SET(prj_RCCS ./icons/st-kexiv2-icons.qrc)
#QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_kexiv2_SOURCE_DIR}/stkexiv2export.h.in"
               "${st_kexiv2_BINARY_DIR}/stkexiv2export.h")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake 
					${CMAKE_CURRENT_BINARY_DIR}/version.h)

INCLUDE_DIRECTORIES( ${st_kexiv2_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_kexiv2 SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_kexiv2 PROPERTIES VERSION ${KEXIV2_LIB_SO_VERSION_STRING} 
                             SOVERSION ${KEXIV2_LIB_SO_CUR_VERSION})
TARGET_LINK_LIBRARIES(st_kexiv2 ${QT_LIBRARIES}  ${KEXIV2_DEP_LIBRARIES} st_core)

INSTALL(TARGETS st_kexiv2 DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
