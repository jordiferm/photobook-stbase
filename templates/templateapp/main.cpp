/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** SoftTopia reserves all rights not expressly granted herein.
**
** SoftTopia (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

//-- Settings and miscelanea
//#include <QSettings>
//#include <QPixmapCache>

// -- Styles
#include <QPlastiqueStyle>
#include <QTextStream>
#ifdef Q_OS_MAC
#include <QMacStyle>
#endif

// -- Styles

#include "sapplication.h"
#include "mainwindow.h"
#include "stlog.h"
#include "cursors.h"
//#include "appsettings.h"
#include "stimage.h"
#include "cursors.h"

void setStyleSheet()
{
#ifdef Q_OS_MAC
	QFile StyleFile(QCoreApplication::applicationDirPath() + "/templateapp_style_mac.txt");
#else
	QFile StyleFile(QCoreApplication::applicationDirPath() + "/templateapp_style.txt");
#endif
	if (!StyleFile.open(QIODevice::ReadOnly | QIODevice::Text))
		qWarning(QApplication::tr("Could not find %1 file").arg(StyleFile.fileName()).toLatin1());
	
	QTextStream In(&StyleFile);
	QString StyleStr = In.readAll();
	qApp->setStyleSheet(StyleStr); 
}

int main (int argc, char **argv)
{
	SApplication App(argc, argv, "TemplateApp", PRJ_VERSION_MAJOR, PRJ_VERSION_MINOR, PRJ_VERSION_PATCH, QString(" - (%2)").arg(RELEASE_CODENAME));
#ifdef Q_OS_MAC
	App.setStyle(new QMacStyle);
#else
	App.setStyle(new QPlastiqueStyle);
#endif
	App.setWindowIcon(QIcon(":/appicon.png"));
	setStyleSheet();
	loadCursors();
	
	//AppSettings Settings;
	SApplication::setLocaleFilePrefix("templateapp");
	//SApplication::setLocale(AppSettings::mainLocale());

	STLog::setIdentity(App.applicationName());
	App.showSplashScreen(QPixmap(":/splash.png"));
	App.showSplashMessage(QApplication::tr("Initializing ..."), Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
	
	// Initializations (Miscelanea)
	//QPixmapCache::setCacheLimit(1024*96);
	//STAbstractGraphicsItem::setGridValue(5);
	//STImage::setDefaultEncryptionKey("SPiscool");
	
	App.showSplashMessage(QApplication::tr("Creating main window..."), Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
	
	MainWindow MainW;
	MainW.show();
	stLog << ISTLogIO::Crit << ISTLogIO::L2 << "Up and running." << ISTLogIO::Endl; 
	App.showSplashMessage("Welcome to " + App.fullApplicationName(), Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
	App.finishSplashDelayed(&MainW);

	QStringList Arguments = App.arguments(); 

	return App.exec();
}

