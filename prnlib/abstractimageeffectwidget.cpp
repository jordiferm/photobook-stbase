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

#include "abstractimageeffectwidget.h"
#include <QDataWidgetMapper>
#include <QGroupBox>
#include <QCheckBox> 
#include <QDataWidgetMapper>
#include <QSlider>

#include "imimage.h"
#include "abstractimageeffectmodel.h"
#include "labeledsliderwidget.h"

QGroupBox* AbstractImageEffectWidget::newGroupBox(const QString& _KeyName, QWidget* _Parent)
{
	QGroupBox* NGroupBox = new QGroupBox(Model->headerData(
			Model->index(_KeyName).column(), Qt::Horizontal).toString(), _Parent);
	NGroupBox->setCheckable(true);
	connect(NGroupBox, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	Mapper->addMapping(NGroupBox, Model->index(_KeyName).column());
	return NGroupBox;
}

QCheckBox* AbstractImageEffectWidget::newCheck(const QString& _KeyName, QWidget* _Parent)
{
	QCheckBox* NewCheckBox = new QCheckBox(Model->headerData(
			Model->index(_KeyName).column(), Qt::Horizontal).toString(), _Parent);
	connect(NewCheckBox, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	Mapper->addMapping(NewCheckBox, Model->index(_KeyName).column());
	return NewCheckBox;
}

LabeledSliderWidget* AbstractImageEffectWidget::newSlider(const QString& _KeyName, int _MinValue, int _MaxValue, QWidget* _Parent)
{
	LabeledSliderWidget* SContrast = new LabeledSliderWidget(Model->headerData(
			Model->index(_KeyName).column(), Qt::Horizontal).toString(), _MinValue, _MaxValue, _Parent);
	Mapper->addMapping(SContrast->slider(), Model->index(_KeyName).column());
	connect(SContrast, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
	return SContrast; 
}

AbstractImageEffectWidget::AbstractImageEffectWidget(AbstractImageEffectModel* _Model, QWidget *parent)
 : QWidget(parent), Model(_Model), EManager(0)
{
	Mapper = new QDataWidgetMapper(this);
	Mapper->setModel(_Model);
	Mapper->toFirst();
}


AbstractImageEffectWidget::~AbstractImageEffectWidget()
{
}

void AbstractImageEffectWidget::applyEffects(IMImage& _SourceImage)
{
	if (EManager)
	{
		if (anyEffectToApply())
			EManager->applyEffects(_SourceImage);
	}
}

bool AbstractImageEffectWidget::anyEffectToApply() const
{
	if (isVisible())
	{
 		if (!Mapper->submit())
			qDebug("Submit failed !");
	}
	return EManager->anyEffectToApply();
}
