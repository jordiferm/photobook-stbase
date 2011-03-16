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

#ifndef LABELEDSLIDERWIDGET_H
#define LABELEDSLIDERWIDGET_H

#include <QWidget>
#include "stprnlibexport.h"

/**
	@author Jordi Fern?dez <shadow@softwarelliure.com>
*/
class QLabel;
class QSlider;
class ST_PRNLIB_EXPORT LabeledSliderWidget : public QWidget
{
	Q_OBJECT
	
	QLabel* MLabel;
	QSlider* MSlider;
	QString Title;
	
public:
	LabeledSliderWidget(const QString& _Title, int _MinValue, int _MaxValue, QWidget *parent = 0);
	~LabeledSliderWidget();
	QSlider* slider();
	
protected slots:	
	void setLabelValue(int _Value);
	
signals: 
	void valueChanged();
};

#endif
