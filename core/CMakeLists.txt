#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_core)


# --------------------------------------------------------------------------
# Configure Qt

INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	flowlayout.cpp
	sappsettings.cpp
	stlog.cpp
	stutils.cpp
	sapplication.cpp
	sterrorstack.cpp
	stpaintutils.cpp
	stlocalestring.cpp
	strecentfiles.cpp
	blowfish.cpp
	appversion.cpp
	updatemanager.cpp
)

SET(prj_MOC_HDRS
	flowlayout.h
	sappsettings.h
	sterror.h
	stlog.h
	stutils.h
	sapplication.h
	qxtpimpl.h
	sterrorstack.h
	stpaintutils.h
	stlocalestring.h
	strecentfiles.h
	qxtnamespace.h
	blowfish.h
	appversion.h
	updatemanager.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_core_SOURCE_DIR}/stcoreexport.h.in"
               "${st_core_BINARY_DIR}/stcoreexport.h")
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )

# --------------------------------------------------------------------------
# Building

#ADD_LIBRARY(st_core ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

ADD_LIBRARY(st_core SHARED ${prj_SRCS} ${prj_MOC_SRCS})

SET_TARGET_PROPERTIES(st_core PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_core ${QT_LIBRARIES})

INSTALL(TARGETS st_core DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
