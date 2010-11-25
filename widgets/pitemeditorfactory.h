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

#ifndef PITEMEDITORFACTORY_H
#define PITEMEDITORFACTORY_H

#include <QItemEditorFactory>
#include <QComboBox>
#include <QStringList>
#include "stwidgetsexport.h"

/**
QComboBox amb la propietat contents.
 
@author Shadow
*/


class ST_WIDGETS_EXPORT PComboBox : public QComboBox
{
	Q_OBJECT 
	
	Q_PROPERTY(QStringList contents READ contents WRITE setContents)

public:
	explicit PComboBox(QWidget *parent = 0) : QComboBox(parent)
	{}
	
	void setContents(const QStringList& _StringList)
	{
		addItems(_StringList);
		setCurrentIndex(0);
	}

	QStringList contents() const
	{
		return QStringList(currentText());
	}
};



/**
Editor factory per a solventar el problema dels tipus StringList.
 
@author Shadow
*/
class ST_WIDGETS_EXPORT PItemEditorFactory : public QItemEditorFactory
{
	
public:
	inline PItemEditorFactory() {}
	QWidget * createEditor ( QVariant::Type type, QWidget * parent ) const;
	QByteArray valuePropertyName(QVariant::Type) const;
};

#endif
