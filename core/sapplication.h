/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Starblitz reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SAPPLICATION_H
#define SAPPLICATION_H
#include <QApplication> 
#include <QColor>
#include <QList> 
#include "stcoreexport.h"


/**
Applicació de flam, QApplication + versió.
Demana per a construir-se el nom de l'aplicació i la versió. 
 
 
@author Shadow
*/
#define sApp static_cast<SApplication*>(qApp)

class QSplashScreen; 
class QTranslator;
class ST_CORE_EXPORT SApplication : public QApplication
{
Q_OBJECT

public:
	struct SAppLocaleDef
	{
		QString Id; 
		QString Description;
	};
	typedef QList<SAppLocaleDef> TAppLocaleList;

private:
	static int Major, Minor, Release;
	static QString Version;
	static QString AppName; 
	static QString CurrLocale;
	static QString AppLocaleFilePrefix;
	QSplashScreen* Splash; 
	QTimer* SplashFinishTimer;
	QWidget* SplashMainWindow;
	static QTranslator* SysTranslator;
	static TAppLocaleList LocaleList; 
	static QStringList LocaleIds; 
	static QString UserSharedPath;
	static QString DBFilesPath;
	static bool BadClosed;
	
	void init(const QString& _AppName);
	

public:
	SApplication(int & argc, char ** argv, const QString& _AppName, const QString& _AppVersion, bool GUIenabled = true);
	//! Es recomana ja que manté els settings per les diferents releases de cada minor.
	SApplication(int & argc, char ** argv, const QString& _AppName, 
		unsigned int _Major = 0,
		unsigned int _Minor = 0, 
		unsigned int _Release = 0,
		const QString& _ReleaseModifier = "",
		 bool GUIenabled = true);
	void showSplashScreen(const QPixmap& _SplashPix);
	void showSplashMessage(const QString & message, int alignment = Qt::AlignLeft, const QColor & color = Qt::black);
	void finishSplashDelayed(QWidget* _MainWin, int _MSecs = 1000);
	static QString version();
	//! Nom i versió de l'aplicació.
	static QString fullApplicationName(); 
	static void installTranslator(const QString& _LocaleFilePath);
	static void installStTranslator(const QString& _Locale);
	static void installSystemTranslator(const QString& _Locale);
	static void setLocaleFilePrefix(const QString& _Prefix) { AppLocaleFilePrefix = _Prefix; } 
	static void installAppTranslator(const QString& _Locale);
	static void setLocale(const QString& _Locale); 
	static QString currentLocale() { return CurrLocale; }
	static void addLocale(const QString& _Locale, const QString& _Description);
	static TAppLocaleList locales() { return LocaleList; }
	static QStringList localeIds() { return LocaleIds; }

	static int major() { return Major; }
	static int minor() { return Minor; }
	static int release() { return Release; }


	//App default paths
	//!Path to store database files.
	static QString dbFilesPath();
	static void setDbFilesPath(const QString& _Value);
	static QString appUserSharedPath(const QString& _AppPath);
	static QString userSharedPath();
	static bool isBadClosed();

	//! Runs updatenotifier app detached.
	static void checkForUpdates(const QString& _Host, int _Port, const QString& _User, const QString& _Password,
								const QString& _RemoteDir);

	int exec();


private slots: 
	void finishSplash();

signals:
	void splashFinished();
};

#endif
