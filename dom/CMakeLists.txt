# --------------------------------------------------------------------------
# General
PROJECT(st_dom)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_database_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_database_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_metadata_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_metadata_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_ticketprinter_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_ticketprinter_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${st_dom_BINARY_DIR} )

# --------------------------------------------------------------------------
# Modules
INCLUDE(${st_imagelib_SOURCE_DIR}/stmodinclude.cmake)

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTSQL TRUE) 
SET(QT_USE_QTXML TRUE) 
SET(QT_USE_QTNETWORK TRUE) 
INCLUDE(${QT_USE_FILE})


# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	xmlorder.cpp
	ddoc.cpp
	ddoclist.cpp
	ddocfactory.cpp
	dimagedoc.cpp
	publisherdatabase.cpp
	ddocmodel.cpp
	printjob.cpp
	printjobmodel.cpp
	orderdatabase.cpp
	ticketdatabase.cpp
	stxmlpublishersettings.cpp
	stcollectionpublishermodel.cpp
	stftpordertransfer.cpp
	stftpordertransferdialog.cpp
	printjobprinter.cpp
	ordertransfermodel.cpp
	publisher.cpp
	publisherinfo.cpp
	iddesctablemodel.cpp
	collectionpoint.cpp
	paymenttype.cpp
	shippingmethod.cpp
)

SET(prj_MOC_HDRS
	xmlorder.h
	ddoc.h
	ddoclist.h
	ddocfactory.h
	dimagedoc.h
	publisherdatabase.h
	ddocmodel.h
	printjob.h
	printjobmodel.h
	orderdatabase.h
	ticketdatabase.h
	stxmlpublishersettings.h
	stcollectionpublishermodel.h
	stftpordertransfer.h
	stftpordertransferdialog.h
	printjobprinter.h
	ordertransfermodel.h
	publisher.h
	publisherinfo.h
	iddesctablemodel.h
	collectionpoint.h
	paymenttype.h
	shippingmethod.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})


# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_dom_SOURCE_DIR}/stdomexport.h.in"
			   "${st_dom_BINARY_DIR}/stdomexport.h")

# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_dom SHARED ${prj_SRCS} ${prj_MOC_SRCS})
SET_TARGET_PROPERTIES(st_dom PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})

TARGET_LINK_LIBRARIES(st_dom ${QT_LIBRARIES} st_database st_metadata st_ticketprinter st_widgets ${ST_IMAGELIB_LIBRARIES} ${PRINTKEEPER_LIBS} st_dom)

INSTALL(TARGETS st_dom DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
