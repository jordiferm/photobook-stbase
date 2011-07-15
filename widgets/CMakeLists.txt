#this is just a basic CMakeLists.txt, for more information see the cmake manpage

# --------------------------------------------------------------------------
# General
PROJECT(st_widgets)

INCLUDE_DIRECTORIES( ${st_core_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${st_core_BINARY_DIR} )

# --------------------------------------------------------------------------
# Configure Qt

SET(QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})

# --------------------------------------------------------------------------
# Project files
SET(prj_SRCS
	sconfigwidget.cpp
	smappedwidget.cpp
	fileselectlineedit.cpp
	sactionbuttonsframe.cpp
	siconfactory.cpp
	smappablecombobox.cpp
	pitemeditorfactory.cpp
	smessagebox.cpp
	fsearchwidget.cpp
	fstdactions.cpp
	fabstractactioncontainer.cpp
	factiontableview.cpp
	fpixmapselector.cpp
	sgeometrysavedialog.cpp
	fdialog.cpp
	ftsdialog.cpp
	fsearchdialog.cpp
	fsearchdialogbase.cpp
	ftsmessagebox.cpp #Obsolete
	gkeyboard2.cpp
	omaintoolbar.cpp
	qxtgroupbox.cpp
	qxtprogresslabel.cpp
	sprocessstatuswidget.cpp
	sspinslider.cpp
	qpotmet.cpp
	stclickablelabel.cpp
	htmleditwidget.cpp
	qslide.cpp
	smappablelabel.cpp
	stcolorbutton.cpp
	stsizeandposdialog.cpp
	styearandmonthdialog.cpp
	smultiprocessstatuswidget.cpp
	stthumbnailview.cpp
	previewitemdelegate.cpp
	sprintersettings.cpp
	sprintersettingswidget.cpp
	kmultitabbar.cpp
	stsidebar.cpp
	ksqueezedtextlabel.cpp
	kseparator.cpp
	kvbox.cpp
	khbox.cpp
	plusminusitemdelegate.cpp
	tsinputdialog.cpp
	checkitemdelegate.cpp
	stfolderselector.cpp
	stbusyinfowidget.cpp
	qxtlabel.cpp
	qf_clickablelabel.cpp
	qf_navbar.cpp
	qf_navbargroup.cpp
	qf_navbaritem.cpp
	qf_selectablewidget.cpp
	sresetslider.cpp
	stprogressindicator.cpp
	stconcurrentprogressindicator.cpp
	stftpstatuswidget.cpp
	templatedownloaddialog.cpp
	sintvalueselector.cpp
	logindialog.cpp
	wwglobal_p.cpp
	qwwbuttonlineedit.cpp
	ocheckeddirmodel.cpp
	fintvalueselector.cpp
	qxtpushbutton.cpp
	qwwrichtextedit.cpp
	qwwcolorbutton.cpp
	colormodel.cpp
	sacceptcanceldialog.cpp
)

SET(prj_MOC_HDRS
#	stwidgetsexport.h
	sconfigwidget.h
	smappedwidget.h
	fileselectlineedit.h
	sactionbuttonsframe.h
	siconfactory.h
	smappablecombobox.h
	pitemeditorfactory.h
	smessagebox.h
	fsearchwidget.h
	fstdactions.h
	fabstractactioncontainer.h
	factiontableview.h
	fpixmapselector.h
	sgeometrysavedialog.h
	fdialog.h
	ftsdialog.h
	fsearchdialog.h
	fsearchdialogbase.h
	ftsmessagebox.h #Obsolete
	gkeyboard2.h
	keyboardw2.h
	omaintoolbar.h
	qxtgroupbox.h
	qxtprogresslabel.h
	sprocessstatuswidget.h
	sspinslider.h
	qpotmet.h
	stclickablelabel.h
	htmleditwidget.h
	qslide.h
	smappablelabel.h
	stcolorbutton.h
	stsizeandposdialog.h
	styearandmonthdialog.h
	smultiprocessstatuswidget.h
	stthumbnailview.h
	previewitemdelegate.h
	sprintersettings.h
	sprintersettingswidget.h
	kmultitabbar.h
	kmultitabbar_p.h
	stsidebar.h
	ksqueezedtextlabel.h
	kseparator.h
	kvbox.h
	khbox.h
	plusminusitemdelegate.h
	tsinputdialog.h
	checkitemdelegate.h
	stfolderselector.h
	stbusyinfowidget.h
	qxtlabel.h
	qf_clickablelabel.h
	qf_navbar.h
	qf_navbargroup.h
	qf_navbaritem.h
	qf_selectablewidget.h
	sresetslider.h
	stprogressindicator.h
	stconcurrentprogressindicator.h
	stftpstatuswidget.h
	templatedownloaddialog.h
	sintvalueselector.h
	logindialog.h
	wwglobal.h
	wwglobal_p.h
	qwwbuttonlineedit.h
	qwwbuttonlineedit_p.h
	ocheckeddirmodel.h
	fintvalueselector.h
	qxtpushbutton.h
	qwwrichtextedit.h
	colormodel.h
	sacceptcanceldialog.h
)

QT4_AUTOMOC(${prj_SRCS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
QT4_WRAP_CPP(prj_MOC_SRCS ${prj_MOC_HDRS})

#Resource files
SET(prj_RCCS ./icons/st-widgets-icons.qrc)
QT4_ADD_RESOURCES(prj_RCC_SRCS ${prj_RCCS})

# --------------------------------------------------------------------------
# Configuration files

CONFIGURE_FILE("${st_widgets_SOURCE_DIR}/stwidgetsexport.h.in"
               "${st_widgets_BINARY_DIR}/stwidgetsexport.h")
INCLUDE_DIRECTORIES( ${st_widgets_BINARY_DIR} )



# --------------------------------------------------------------------------
# Building

ADD_LIBRARY(st_widgets SHARED ${prj_SRCS} ${prj_MOC_SRCS} ${prj_RCC_SRCS})

SET_TARGET_PROPERTIES(st_widgets PROPERTIES VERSION ${STBASE_VERSION} SOVERSION ${STBASE_SO_VERSION})
TARGET_LINK_LIBRARIES(st_widgets ${QT_LIBRARIES} st_core)

INSTALL(TARGETS st_widgets DESTINATION ${STBASE_LIBRARY_OUTPUT_PATH})
