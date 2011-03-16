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

#include "gammaeffectwidget.h"
#include <QLayout> 
#include <QGroupBox> 
#include <QCheckBox> 
#include <QDataWidgetMapper>

#include "imimage.h"
#include "abstractimageeffectmodel.h"
#include "labeledsliderwidget.h"

GammaEffectFilterManager::GammaEffectFilterManager(AbstractImageEffectModel* _Model, QObject* _Parent) 
	: AbstractImageEffectManager(_Model, _Parent)
{
	_Model->addKey("gammaeffects/apply", false, QVariant::Bool, tr("Apply Color correction ?"));
	_Model->addKey("gammaeffects/gamma", 100, QVariant::Int, tr("Gamma"));
	_Model->addKey("gammaeffects/red", 100, QVariant::Int, tr("Red"));
	_Model->addKey("gammaeffects/green", 100, QVariant::Int, tr("Green"));
	_Model->addKey("gammaeffects/blue", 100, QVariant::Int, tr("Blue"));
}

bool GammaEffectFilterManager::anyEffectToApply() const
{
	return (Model->data(Model->index("gammaeffects/apply")).toBool());
}

void GammaEffectFilterManager::applyEffects(IMImage& _SourceImage)
{
	if (Model->data(Model->index("gammaeffects/apply")).toBool())
	{
		// Gamma Modifiers
		_SourceImage.gamma(static_cast<double>(Model->data(Model->index("gammaeffects/gamma")).toInt()) / 100);
		
		//qDebug("Applying gamma");
		double Red = static_cast<double>(Model->data(Model->index("gammaeffects/red")).toInt()) / 100;
		double Green = static_cast<double>(Model->data(Model->index("gammaeffects/green")).toInt()) / 100;
		double Blue = static_cast<double>(Model->data(Model->index("gammaeffects/blue")).toInt()) / 100;
		_SourceImage.gamma(Red, Green, Blue);
	}
}

GammaEffectWidget::GammaEffectWidget(AbstractImageEffectModel* _Model, QWidget* _Parent)
 : AbstractImageEffectWidget(_Model, _Parent)
{
	setEManager(new GammaEffectFilterManager(_Model, this));
	
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	
	QGroupBox* GBGamma = new QGroupBox(Model->headerData(
			Model->index("gammaeffects/apply").column(), Qt::Horizontal).toString(), this);
	MLayout->addWidget(GBGamma);
	GBGamma->setCheckable(true);
	connect(GBGamma, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	Mapper->addMapping(GBGamma, Model->index("gammaeffects/apply").column());
	
	QVBoxLayout* GBGammaLayout = new QVBoxLayout(GBGamma);
	GBGammaLayout->addWidget(newSlider("gammaeffects/gamma", 0, 255, this));
	GBGammaLayout->addWidget(newSlider("gammaeffects/red", 0, 255, this));
	GBGammaLayout->addWidget(newSlider("gammaeffects/green", 0, 255, this));
	GBGammaLayout->addWidget(newSlider("gammaeffects/blue", 0, 255, this));
	
	GBGammaLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));	
	
	Mapper->toFirst();
}

