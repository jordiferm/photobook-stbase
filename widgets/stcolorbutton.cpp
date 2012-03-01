/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "stcolorbutton.h"
#include <QColorDialog>

STColorButton::STColorButton(QWidget* parent): QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(chooseColor()));
	setObjectName("STColorButton");  
}

void STColorButton::setColor(const QColor& _Color)
{
	QPalette Pal = palette(); 
	Pal.setColor(QPalette::Button, _Color); 
	setPalette(Pal);

	setStyleSheet(QString(".STColorButton { background-color: %1; }").arg(_Color.name())); 

	Color = _Color; 
}

QColor STColorButton::color() const
{
	return Color; 
}

QString STColorButton::colorStr() const
{
	return Color.name(); 
}

void STColorButton::setColorStr(const QString& _ColorStr)
{
	setColor(QColor(_ColorStr)); 
}

void STColorButton::chooseColor()
{
	QColor SelCol = QColorDialog::getColor(Color, this);
	if (SelCol.isValid())
	{
		setColor(SelCol); 
		emit valueChanged(colorStr());
	}
}
