#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_openrpt_common)

# To locate ui generated .h files.
INCLUDE_DIRECTORIES( ${st_openrpt_common_BINARY_DIR} 
							${st_openrpt_common_SOURCE_DIR})


# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE) 
SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QT3SUPPORT TRUE) 
INCLUDE(${QT_USE_FILE})

ADD_DEFINITIONS ( -DMAKELIB )

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	dbtools.cpp
	login.cpp
	loginOptions.cpp
	parameter.cpp
	quuencode.cpp
	xsqlquery.cpp
	xvariant.cpp
	reportpageoptions.cpp
	parsexmlutils.cpp
	querysource.cpp
	memdbloader.cpp
	languageoptions.cpp
	parameteredit.cpp
	paramlistedit.cpp
	newvariant.cpp
	booledit.cpp
	intedit.cpp
	doubleedit.cpp
	stringedit.cpp
	listedit.cpp
	qlistboxvariant.cpp
	builtinSqlFunctions.cpp
)

SET(prj_MOC_HDRS
	dbtools.h
	header.h
	login.h
	loginOptions.h
	parameter.h
	quuencode.h
	xsqlquery.h
	xvariant.h
	reportpageoptions.h
	parsexmlutils.h
	querysource.h
	memdbloader.h
	languageoptions.h
	parameteredit.h
	paramlistedit.h
	newvariant.h
	booledit.h
	intedit.h
	doubleedit.h
	stringedit.h
	listedit.h
	qlistboxvariant.h
	builtinSqlFunctions.h
)

QT4_AUTOMOC(${prj_SRCS})

QT4_WRAP_UI(prj_UI_BUILT_SOURCES
  login.ui loginOptions.ui parameteredit.ui paramlistedit.ui newvariant.ui booledit.ui intedit.ui doubleedit.ui stringedit.ui listedit.ui
)

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
SET(prj_RCCS ../OpenRPT/images/OpenRPTCommon.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_openrpt_common_SOURCE_DIR}/stopenrptcommonexport.h.in"
               "${st_openrpt_common_BINARY_DIR}/stopenrptcommonexport.h")
INCLUDE_DIRECTORIES( ${st_openrpt_common_BINARY_DIR} )


# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_openrpt_common SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_UI_BUILT_SOURCES} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_openrpt_common PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_openrpt_common ${QT_LIBRARIES})

INSTALL(TARGETS st_openrpt_common DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
