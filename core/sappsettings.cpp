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

#include "sappsettings.h"
#include <QStringList>
#include <QRegExp> 
// _________________________________________________________________________*/
//
//	Class SAppSettings
// _________________________________________________________________________*/


SAppSettings::TKeyMap SAppSettings::KeyMap;
SAppSettingsModel* SAppSettings::SettingsModel = 0;

void SAppSettings::init()
{
	if (!SettingsModel)
		SettingsModel = new SAppSettingsModel(*this);
}

SAppSettings::SAppSettings(QObject* _Parent) : QSettings(_Parent)
{
	init();
}

SAppSettings::SAppSettings(const QString& _Organization, const QString& _Application , QObject* _Parent)
	: QSettings(_Organization, _Application, _Parent)
{
	init();
}

void SAppSettings::addKey(const QString& _KeyName, const QVariant& _DefaultValue, 
	QVariant::Type _Type, const QString& _Label, const QString& _Description)
{
	KeyMap.insert(_KeyName, SettingStr(_DefaultValue, _Type, _Label, _Description));
}

bool SAppSettings::keySaved()
{
	return !KeyMap.isEmpty();
}

bool SAppSettings::keySaved(const QString& _KeyName)
{
	return KeyMap.contains(_KeyName); 
}


QString SAppSettings::label(QString _Key ) const
{
	SAppSettings::SettingStr CurrVal = KeyMap.value(_Key); 
	return CurrVal.Label;
}

QString SAppSettings::description( QString _Key ) const
{
	SAppSettings::SettingStr CurrVal = KeyMap.value(_Key); 
	return CurrVal.Description;
}

QVariant::Type SAppSettings::typeOf(const QString& _Key ) const
{
	SAppSettings::SettingStr CurrVal = KeyMap.value(_Key); 
	return CurrVal.Type;
}

QVariant SAppSettings::defaultValue( QString _Key ) const
{
	SAppSettings::SettingStr CurrVal = KeyMap.value(_Key); 
	return CurrVal.DefaultValue;
}

QVariant SAppSettings::value(QString _Key ) const
{
	return QSettings::value(_Key, defaultValue(_Key));
}


void SAppSettings::saveDefaults()
{
	SAppSettings::TKeyMap::iterator it; 
	for (it = KeyMap.begin(); it != KeyMap.end(); ++it)
	{
		if (!contains( it.key() ))
			setValue(it.key(), it.value().DefaultValue);
	}
}

QStringList SAppSettings::keys() const
{
	return KeyMap.keys();
}

SAppSettingsModel* SAppSettings::model()
{
	return SettingsModel;
}

bool SAppSettings::validSettings() const
{
	return true; 
}


// _________________________________________________________________________*/
//
//	Class SAppSettingsModel
// _________________________________________________________________________*/


SAppSettingsModel::SAppSettingsModel(SAppSettings& _Settings, QObject * _Parent)
: QStandardItemModel(_Parent), Settings(_Settings)
{	
	insertRows(0,1);
}

void SAppSettingsModel::fetch()
{
	QStringList Keys = Settings.keys();
	for ( int i = 0; i < Keys.count(); i++ )
	{
		insertColumns(i,1);
		QVariant CVal = Settings.QSettings::value(Keys[i], Settings.defaultValue(Keys[i]));
		CVal.convert(Settings.typeOf(Keys[i]));

		setData(QStandardItemModel::index(0, i), CVal);
		setHeaderData(i, Qt::Horizontal, Settings.label(Keys[i]));
	}
}

QModelIndex SAppSettingsModel::index(const QString& _Key)
{
	QStringList Keys = Settings.keys();
	//qDebug(QString("Index of %1 : %2").arg(_Key).arg(Keys.indexOf(_Key)).toLatin1());
	//return QStandardItemModel::index(0, Keys.indexOf(QRegExp("^" + _Key + "$")));
	return QStandardItemModel::index(0, Keys.indexOf(_Key));
}

bool SAppSettingsModel::submit()
{
	QStringList Keys = Settings.keys();
	for (int i = 0; i < Keys.count(); i ++)
	{
		QModelIndex ValueIndex = QStandardItemModel::index(0, i);			
		//qDebug(QString("SettingValue %0, %1 ").arg(Keys[i]).arg(data(ValueIndex).toString()).toLatin1());
		Settings.setValue(Keys[i], data(ValueIndex));
	}
	return true;
}
