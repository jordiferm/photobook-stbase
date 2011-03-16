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

#ifndef ABSTRACTIMAGEEFFECTMODEL_H
#define ABSTRACTIMAGEEFFECTMODEL_H

#include <QStandardItemModel>
#include <QMap> 
#include "stprnlibexport.h"

/**
Base class for imageeffect models.
Key-Value Model class. We need to implement the store policy reimplementing setValue and value functions.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PRNLIB_EXPORT AbstractImageEffectModel : public QStandardItemModel
{
	Q_OBJECT
public:	
	class ImageEffectMStr
	{
		QVariant DefaultValue; 
		QVariant::Type Type;
		QString Label; 
		int Index;

	public: 
		ImageEffectMStr() {}
		ImageEffectMStr(const QVariant& _DefaultValue, QVariant::Type _Type, const QString& _Label)
			: DefaultValue(_DefaultValue), Type(_Type), Label(_Label) {}
		QVariant defaultValue() const { return DefaultValue; }
		QVariant::Type type() const { return Type; }
		QString label() const { return Label; }
		void setIndex(int _Index) { Index = _Index; }
		int index() const { return Index; }
	};
	typedef QMap<QString, ImageEffectMStr>	TKeyMap;

private:
	TKeyMap KeyMap;
	
public:
	AbstractImageEffectModel(QObject* _Parent = 0);
	void setColumn(const QString& _Key, ImageEffectMStr& _Value, bool _Insert = false);
	void fetch();
	QModelIndex index(const QString& _Key);
	bool saveData();
	void addKey(const QString& _KeyName, const QVariant& _DefaultValue, 
		QVariant::Type _Type, const QString& _Label);
	virtual void setValue(const QString& _Key, const QVariant& _Value) = 0;
	virtual QVariant value(const QString& _Key, const QVariant& _DefaultValue = QVariant()) = 0;


};

#endif
