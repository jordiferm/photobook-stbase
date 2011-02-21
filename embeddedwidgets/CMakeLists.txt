#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_embeddedwidgets)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSVG TRUE)
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	qtsvgpixmapcache.cpp
	qtsvgtoggleswitch.cpp
	qtscrollwheel.cpp
	stmonthselector.cpp
	qtbasicdialgauge.cpp
	qtsvgdialgauge.cpp
	stnumqualitygauge.cpp
)

SET(prj_MOC_HDRS
	qtsvgpixmapcache.h
	qtsvgtoggleswitch.h
	qtscrollwheel.h
	stmonthselector.h
	qtbasicdialgauge.h
	qtsvgdialgauge.h
	stnumqualitygauge.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
SET(prj_RCCS
	./skins/beryl_svgtoggleswitch.qrc
	./skins/beryl_scrollwheel.qrc
	./skins/amperemeter_svgdialgauge.qrc
	./skins/tachometer_svgdialgauge.qrc
	./skins/thermometer_svgdialgauge.qrc
)


QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_embeddedwidgets_SOURCE_DIR}/stembeddedwidgetsexport.h.in"
			   "${st_embeddedwidgets_BINARY_DIR}/stembeddedwidgetsexport.h")
INCLUDE_DIRECTORIES( ${st_embeddedwidgets_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_embeddedwidgets SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_embeddedwidgets PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_embeddedwidgets ${QT_LIBRARIES} st_core)

INSTALL(TARGETS st_embeddedwidgets DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
