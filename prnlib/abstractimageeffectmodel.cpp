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

#include "abstractimageeffectmodel.h"

AbstractImageEffectModel::AbstractImageEffectModel(QObject* _Parent)
: QStandardItemModel(_Parent)
{	
	insertRows(0,1);
}

void AbstractImageEffectModel::setColumn(const QString& _Key, ImageEffectMStr& _Value, bool _Insert)
{
	//int CColumn = KeyMap.keys().indexOf(_Key);
	int CColumn = 0;
	//qDebug(QString("Fetching column %1, index=%2").arg(it.key()).arg(CColumn).toLatin1());
	if (_Insert)
	{
		CColumn = columnCount();
		insertColumns(CColumn,1);
		_Value.setIndex(CColumn);
	}
	else 	
		CColumn = _Value.index();
	
	QVariant CVal = value(_Key, _Value.defaultValue());
	//qDebug(QString("Key: %1, Value: %2").arg(_Key).arg(CVal.toString()).toAscii());
	CVal.convert(_Value.type());

	setData(QStandardItemModel::index(0, CColumn), CVal);
	setHeaderData(CColumn, Qt::Horizontal, _Value.label());
}

void AbstractImageEffectModel::fetch()
{
	//qDebug("void AbstractImageEffectModel::fetch()");
	TKeyMap::iterator it; 
	for (it = KeyMap.begin(); it != KeyMap.end(); ++it )
	{
		setColumn(it.key(), it.value());
	}
}

QModelIndex AbstractImageEffectModel::index(const QString& _Key)
{
	int Column = 0;
	if (KeyMap.contains(_Key))
		Column = KeyMap[_Key].index();
				
	//qDebug(QString("index(%1) = %2").arg(_Key).arg(Column).toLatin1());
	return QStandardItemModel::index(0, Column);
}

bool AbstractImageEffectModel::saveData()
{
	//qDebug("AbstractImageEffectModel::submit()");
	TKeyMap::iterator it; 
	for (it = KeyMap.begin(); it != KeyMap.end(); ++it )
	{
		QModelIndex ValueIndex = QStandardItemModel::index(0, it.value().index());			
		//qDebug(QString("SettingValue %0, %1 ").arg(it.key()).arg(data(ValueIndex).toString()).toLatin1());
		setValue(it.key(), data(ValueIndex));
	}
	return true;
}

void AbstractImageEffectModel::addKey(const QString& _KeyName, const QVariant& _DefaultValue, 
	QVariant::Type _Type, const QString& _Label)
{
	ImageEffectMStr CurrValue(_DefaultValue, _Type, _Label);
	setColumn(_KeyName, CurrValue, true);
	KeyMap.insert(_KeyName, CurrValue);
	
	//insertColumns(index(_KeyName).column(),1);
}

