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

#ifndef SAPPSETTINGS_H
#define SAPPSETTINGS_H

#include <QStandardItemModel>
#include <QSettings>
#include "stcoreexport.h"


/**
Settings basats en ModelView.

@author Shadow
*/
class SAppSettingsModel;
class ST_CORE_EXPORT SAppSettings : public QSettings
{
Q_OBJECT
public:
	class SettingStr
	{
		QVariant DefaultValue; 
		QVariant::Type Type;
		QString Label; 
		QString Description; 

	public: 
		SettingStr() {}
		SettingStr(const QVariant& _DefaultValue, QVariant::Type _Type, const QString& _Label, const QString& _Description)
			: DefaultValue(_DefaultValue), Type(_Type), Label(_Label), Description(_Description) {}
		friend class SAppSettings;
	};

	typedef QMap<QString, SettingStr>	TKeyMap;

protected:
	static TKeyMap KeyMap;
	static SAppSettingsModel* SettingsModel;

private:
	void createModel();
	void init();

public:
	SAppSettings(QObject* _Parent = 0);
	SAppSettings(const QString& _Organization, const QString& _Application = QString(), QObject* _Parent = 0);
	void addKey(const QString& _KeyName, const QVariant& _DefaultValue, 
		QVariant::Type _Type, const QString& _Label, const QString& _Description);
	bool keySaved();
	bool keySaved(const QString& _KeyName);
	QString label( QString _key ) const;
	QString description( QString _key ) const;
	QVariant::Type typeOf(const QString& _key ) const;
	QVariant defaultValue( QString _key ) const;
	//! Value from settings, not from the model.
	QVariant value(QString _key ) const;
	void saveDefaults();
	QStringList keys() const;
	static SAppSettingsModel* model();
	virtual bool validSettings() const;
};

typedef SAppSettings::TKeyMap TSAppSettingsKeyMap;


/**
Model per a settings.
 
@author Shadow
*/

class ST_CORE_EXPORT SAppSettingsModel: public QStandardItemModel
{
	Q_OBJECT
	
	SAppSettings& Settings;

public:
	SAppSettingsModel(SAppSettings& _Settings, QObject * parent = 0);
	void fetch();
	QModelIndex index(const QString& _Key);
	virtual bool submit ();
};


#endif
