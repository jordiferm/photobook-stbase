#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_kdcraw)


# --------------------------------------------------------------------------
# Configure Qt

#SET(QT_USE_QTXML TRUE) 
INCLUDE(${QT_USE_FILE})


# Modules
#INCLUDE(${st_kdcraw_SOURCE_DIR}/kdcraw.cmake)

INCLUDE_DIRECTORIES( ${kdcraw_DEP_INCLUDE_DIR} )
ADD_DEFINITIONS( ${kdcraw_DEP_DEFINITIONS} )


# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	rnuminput.cpp
	rcombobox.cpp
)

SET(prj_MOC_HDRS
	rnuminput.h
	rcombobox.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
SET(prj_RCCS ./icons/st-kdcraw-icons.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_kdcraw_SOURCE_DIR}/stkdcrawexport.h.in"
               "${st_kdcraw_BINARY_DIR}/stkdcrawexport.h")
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake 
#					${CMAKE_CURRENT_BINARY_DIR}/version.h)

INCLUDE_DIRECTORIES( ${st_kdcraw_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_kdcraw SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_kdcraw PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})

#TARGET_LINK_LIBRARIES(st_kdcraw ${QT_LIBRARIES}  ${kdcraw_DEP_LIBRARIES} st_core)
TARGET_LINK_LIBRARIES(st_kdcraw ${QT_LIBRARIES} st_core)

INSTALL(TARGETS st_kdcraw DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
