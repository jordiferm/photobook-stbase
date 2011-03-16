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

#ifndef ABSTRACTIMAGEEFFECTWIDGET_H
#define ABSTRACTIMAGEEFFECTWIDGET_H

#include <QWidget>
#include "abstractimageeffectmanager.h"
#include "stprnlibexport.h"

/**
Base class for all image effect widgets.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class AbstractImageEffectModel;
class QDataWidgetMapper;
class QCheckBox;
class LabeledSliderWidget;
class QGroupBox;
class AbstractImageEffectManager;
class ST_PRNLIB_EXPORT AbstractImageEffectWidget : public QWidget
{
	Q_OBJECT
	
protected:
	AbstractImageEffectModel* Model;
	QDataWidgetMapper* Mapper;
	AbstractImageEffectManager* EManager;
	
	QGroupBox* newGroupBox(const QString& _KeyName, QWidget* _Parent);
	QCheckBox* newCheck(const QString& _KeyName, QWidget* _Parent = 0);
	LabeledSliderWidget* newSlider(const QString& _KeyName, int _MinValue, int _MaxValue, QWidget* _Parent = 0);

	protected: 
	void setEManager(AbstractImageEffectManager* _EManager) { EManager = _EManager;}
	
public:
	AbstractImageEffectWidget(AbstractImageEffectModel* _Model , QWidget* _Parent = 0);
	virtual ~AbstractImageEffectWidget();
	virtual void applyEffects(IMImage& _SourceImage);
	bool anyEffectToApply() const;
	
signals:
	void valueChanged();
};

#endif
