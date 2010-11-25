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
#ifndef STCOLORBUTTON_H
#define STCOLORBUTTON_H

#include <QPushButton>
#include <QColor> 
#include "stwidgetsexport.h"

/**
Button to select a color.

	@author
*/
class ST_WIDGETS_EXPORT STColorButton : public QPushButton
{
	Q_OBJECT
	Q_PROPERTY(QString colorStr READ colorStr WRITE setColorStr USER true)			
	QColor Color; 

public:
	STColorButton(QWidget* parent);
	void setColor(const QColor& _Color); 
	QColor color() const; 
	QString colorStr() const; 
	void setColorStr(const QString& _ColorStr);
public slots:
	void chooseColor(); 
signals: 
	void valueChanged(const QString& _ColorName); 
};

#endif
