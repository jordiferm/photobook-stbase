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
#ifndef SSPINSLIDER_H
#define SSPINSLIDER_H

#include <QWidget>
#include "stwidgetsexport.h"

/**
QSlider and QSpinBox working together.

	@author
*/
class QSpinBox; 
class QSlider; 
class ST_WIDGETS_EXPORT SSpinSlider : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(int value READ value WRITE setValue USER true)
						
	QSpinBox* MSpinBox; 
	QSlider* MSlider; 
		
public:
	SSpinSlider ( QWidget *parent = 0 );
	~SSpinSlider();
	void setRange(int _Min, int _Max);
	int value() const; 
	void setValue(int _Value); 
	void setSuffix(const QString& _Suffix); 
	void setPrefix(const QString& _Prefix); 

signals: 
	void valueChanged(int ); 	
};

#endif
