#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_burning)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR})
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )

# --------------------------------------------------------------------------
# Configure Qt

INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	stcdrecord.cpp
	stmkisofs.cpp
	stburncddialog.cpp
	stburningsettings.cpp
	stburningsettingswidget.cpp
)

SET(prj_MOC_HDRS
	stcdrecord.h
	stmkisofs.h
	stburncddialog.h
	stburningsettings.h
	stburningsettingswidget.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
SET(prj_RCCS ./icons/st-burning-icons.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_burning_SOURCE_DIR}/stburningexport.h.in"
               "${st_burning_BINARY_DIR}/stburningexport.h")
INCLUDE_DIRECTORIES( ${st_burning_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_burning SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_burning PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_burning ${QT_LIBRARIES} st_core st_widgets )

INSTALL(TARGETS st_burning DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
