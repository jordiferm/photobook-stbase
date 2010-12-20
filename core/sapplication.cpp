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

#include "sapplication.h"
#include <QTranslator> 
#include <QSplashScreen> 
#include <QTimer> 
#include <QLibraryInfo> 
#include <QDir>
#include <QSettings>
#include <QProcess>
#include "stlog.h"
#include "updatemanager.h"
#include "appversion.h"
	

QString SApplication::Version = "";
int SApplication::Major = 0;
int SApplication::Minor = 0;
int SApplication::Release = 0;
QString SApplication::AppName = "NoName"; 
QString SApplication::CurrLocale = "";
QString SApplication::AppLocaleFilePrefix = "";
QTranslator* SApplication::SysTranslator = 0;
QList<SApplication::SAppLocaleDef> SApplication::LocaleList; 
QStringList SApplication::LocaleIds; 
QString SApplication::UserSharedPath = "";
QString SApplication::DBFilesPath = QDir::homePath() + "/.stdatabase";
bool SApplication::BadClosed = false;


void SApplication::init(const QString& _AppName)
{
	Splash = 0;
	SplashFinishTimer = 0;
	SplashMainWindow = 0;
	
	QCoreApplication::setOrganizationName("Starblitz");
	QCoreApplication::setOrganizationDomain("starblitz-k.com");
	QCoreApplication::setApplicationName(_AppName);
}

/*!
	Per defecte posa OrganizationName i OrganizationDomain a S3os. 
	Si volem canviar-los només cal cridar als mètodes pertinents a QCoreApplication un 
	cop instanciada SApplication.
	Concatena _AppName i _AppVersion i l'assigna a ApplicationName. 
	Tinquem en compte que QSettings utilitza aquest valors per a l'ordenació de Settings.
*/

SApplication::SApplication(int & argc, char ** argv, const QString& _AppName, 
	const QString& _AppVersion, bool GUIenabled) : QApplication(argc, argv, GUIenabled)
{
	Version = _AppVersion;
	AppVersion Vers(_AppVersion);
	Major = Vers.major();
	Minor = Vers.minor();
	Release = Vers.release();

	AppName = _AppName;
	init(_AppName + "-" + _AppVersion);
}

SApplication::SApplication(int & argc, char ** argv, const QString& _AppName,
		unsigned int _Major,
		unsigned int _Minor, 
		unsigned int _Release,
		const QString& _ReleaseModifier,
		 bool GUIenabled)  : QApplication(argc, argv, GUIenabled)
{
	Version = QString::number(_Major) + "." + QString::number(_Minor) + "." + QString::number(_Release) + " " + _ReleaseModifier;
	Major = _Major;
	Minor = _Minor;
	Release = _Release;
	AppName = _AppName;
	init(_AppName + QString::number(_Major) + "." + QString::number(_Minor));
}

QString SApplication::version()
{
	return Version;
}

/*!
	Creates and shows splashscreen.
*/
void SApplication::showSplashScreen(const QPixmap& _SplashPix)
{
	Splash = new QSplashScreen(_SplashPix); 
	Splash->show();
}

/*!
	You have to call showSplashScreen previously. If you haven't this method does nothing.
*/
void SApplication::showSplashMessage(const QString& _Message, int _Alignment, const QColor& _Color)
{
	if (Splash) //Defensive
	{
		Splash->showMessage(_Message, _Alignment, _Color);
		processEvents(); 
	}
}

void SApplication::finishSplashDelayed(QWidget* _MainWin, int _MSecs)
{
	if (!SplashFinishTimer)
	{
		SplashFinishTimer = new QTimer(this);
		connect(SplashFinishTimer, SIGNAL(timeout()), this, SLOT(finishSplash()));
	}
	SplashMainWindow = _MainWin;
	SplashFinishTimer->start(_MSecs);
}

QString SApplication::fullApplicationName()
{
	return QCoreApplication::organizationName() + " - " + AppName + " - " + version(); 
}

void SApplication::installTranslator(const QString& _LocaleFilePath)
{
	QTranslator* Translator = new QTranslator(qApp); 
	if (!Translator->load(_LocaleFilePath))
		stLog << ISTLogIO::L1 << QString("Failed to load translation : %1 !").arg(_LocaleFilePath) << ISTLogIO::Endl; 
	QCoreApplication::installTranslator(Translator);

}

void SApplication::installStTranslator(const QString& _Locale)
{
	#ifndef Q_OS_WIN32
		QString LocalePath = ST_LOCALE_PATH;
		if (!LocalePath.isEmpty())
			installTranslator(QCoreApplication::applicationDirPath() + "/" + LocalePath + "/st_" + _Locale);
		else 
			installTranslator(QCoreApplication::applicationDirPath() + "/st_" + _Locale);
	#else 
		installTranslator(QCoreApplication::applicationDirPath() + "/" + "st_" + _Locale);
	#endif 
	
}

void SApplication::installSystemTranslator(const QString& _Locale)
{
	QString Locale; 
	if (_Locale == "en") 
		Locale = "untranslated"; 
	else 
		Locale = _Locale; 

	if (!SysTranslator)
	{
		SysTranslator = new QTranslator(qApp); 
		QCoreApplication::installTranslator(SysTranslator);
	}
	if (!SysTranslator->load("qt_" + Locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
	{
		stLog << ISTLogIO::L1 << QString("Failed to load system translation for locale : %1 at %2!").arg(_Locale).arg(QLibraryInfo::location(QLibraryInfo::TranslationsPath)) << ISTLogIO::Endl;
	}
	
}

void SApplication::installAppTranslator(const QString& _Locale)
{
	if (!AppLocaleFilePrefix.isEmpty())
		installTranslator(applicationDirPath() + "/" + AppLocaleFilePrefix + "_" + _Locale);
}

void SApplication::setLocale(const QString& _Locale)
{
	//Remove all installed translators
	QList<QTranslator*> TranslatorList = qApp->findChildren<QTranslator*>();
	QList<QTranslator*>::iterator it; 
	for (it = TranslatorList.begin(); it != TranslatorList.end(); ++it)
	{
		removeTranslator(*it); 
	}

	CurrLocale = _Locale; 
	
	installAppTranslator(_Locale); 
	installStTranslator(_Locale); 
	installSystemTranslator(_Locale); 
}

void SApplication::addLocale(const QString& _Locale, const QString& _Description)
{
	SAppLocaleDef LDef; 
	LDef.Id = _Locale; 
	LDef.Description = _Description; 
	LocaleList.push_back(LDef); 
	LocaleIds.push_back(_Locale); 
}

QString SApplication::dbFilesPath()
{
	return DBFilesPath;
}

void SApplication::setDbFilesPath(const QString& _Value)
{
	DBFilesPath = _Value;
}

QString SApplication::appUserSharedPath(const QString& _AppPath)
{
	QString Res;
	#ifdef Q_OS_WIN32
		Res = _AppPath + "/../share/st";
	#else
		//In macosx bundle we first look for bundle publishers:
		#ifdef Q_OS_MAC
				QString BundlePath = _AppPath + "/share/st";
				if (QFile::exists(BundlePath))
						Res = BundlePath;
				else
						Res = "/usr/share/st";
		#else
				Res = "/usr/share/st";
		#endif
	#endif
	return Res;
}

bool SApplication::isBadClosed()
{
	return BadClosed;
}


void SApplication::checkForUpdates(const QString& _Host, int _Port, const QString& _User, const QString& _Password,
							const QString& _RemoteDir)
{

	UpdateManager UManager(_Host, _Port, _User, _Password, _RemoteDir);
	UManager.setAppVersion(Major, Minor, Release);
	UManager.checkForUpdates();
}


int SApplication::exec()
{
	QSettings Settings;
	int Res;
	BadClosed = Settings.value("badclose", false).toBool();
	Settings.setValue("badclose", true);
	Res = QApplication::exec();
	Settings.setValue("badclose", false);
	return Res;
}

QString SApplication::userSharedPath()
{
	QString Res = UserSharedPath;
	if (Res.isEmpty())
	{
		Res = appUserSharedPath(QCoreApplication::applicationDirPath());
	}
	return Res ;
}


void SApplication::finishSplash()
{
	if (Splash && SplashMainWindow) //Defensive
	{
		Splash->finish(SplashMainWindow);
		delete Splash; 
		Splash = 0;
		emit splashFinished(); 
	}
}
