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

#include "imageenhancementeffectwidget.h"
#include <QLayout> 
#include <QGroupBox> 
#include <QCheckBox> 
#include <QDataWidgetMapper>

#include "imimage.h"
#include "abstractimageeffectmodel.h"
#include "labeledsliderwidget.h"

ImageEnhancementEffectManager::ImageEnhancementEffectManager(AbstractImageEffectModel* _Model, QObject* _Parent) 
	: AbstractImageEffectManager(_Model, _Parent)
{
	_Model->addKey("enheffects/apply", false, QVariant::Bool, tr("Apply Enhancements ?"));
	_Model->addKey("enheffects/normalize", true, QVariant::Bool, tr("Normalize"));
	_Model->addKey("enheffects/despeckle", false, QVariant::Bool, tr("Despeckle"));
	_Model->addKey("enheffects/enhance", false, QVariant::Bool, tr("Enhance"));
	_Model->addKey("enheffects/equalize", false, QVariant::Bool, tr("Equalize"));
	_Model->addKey("enheffects/applycontrast", false, QVariant::Bool, tr("Apply contrast ?"));
	_Model->addKey("enheffects/contrast", 1, QVariant::Int, tr("Contrast"));
	_Model->addKey("enheffects/applysigmoidalcontrast", false, QVariant::Bool, tr("Apply contrast ?"));
	_Model->addKey("enheffects/sigmoidalcontrast", 4, QVariant::Int, tr("Contrast"));
	_Model->addKey("enheffects/sigmoidalmidpoint", 50, QVariant::Int, tr("MidPoint"));
}

bool ImageEnhancementEffectManager::anyEffectToApply() const
{
	return (Model->data(Model->index("enheffects/apply")).toBool());
}

void ImageEnhancementEffectManager::applyEffects(IMImage& _SourceImage)
{
	//IMImage Image(_SourceImage);
	if (Model->data(Model->index("enheffects/apply")).toBool())
	{
		qDebug("Applying Enhancement");

		if (Model->data(Model->index("enheffects/normalize")).toBool())
			_SourceImage.normalize();
		
		if (Model->data(Model->index("enheffects/despeckle")).toBool())
			_SourceImage.despeckle();
		
		if (Model->data(Model->index("enheffects/enhance")).toBool())
			_SourceImage.enhance();
		
		if (Model->data(Model->index("enheffects/equalize")).toBool())
			_SourceImage.equalize();
	
		if (Model->data(Model->index("enheffects/applycontrast")).toBool())
		{
			for (int Vfor = 0; Vfor < static_cast<unsigned int>(Model->data(Model->index("enheffects/contrast")).toInt()); Vfor++)
			{
				_SourceImage.contrast(1);
			}
		}
		if (Model->data(Model->index("enheffects/applysigmoidalcontrast")).toBool())
		{
			int Sharpen = 1;
			_SourceImage.sigmoidalContrast(Sharpen, Model->data(Model->index("enheffects/sigmoidalcontrast")).toInt(), 65535 * static_cast<double>(Model->data(Model->index("enheffects/sigmoidalmidpoint")).toInt()) / 100);
		}
		
	}
	//return Image.image();
}


ImageEnhancementEffectWidget::ImageEnhancementEffectWidget(AbstractImageEffectModel* _Model, QWidget* _Parent)
 : AbstractImageEffectWidget(_Model, _Parent)
{
	setEManager(new ImageEnhancementEffectManager(_Model, this));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	GBEnEffects = new QGroupBox(tr("Apply image enhancement effects"), this);
	MLayout->addWidget(GBEnEffects);
	GBEnEffects->setCheckable(true);
	Mapper->addMapping(GBEnEffects, _Model->index("enheffects/apply").column());
	connect(GBEnEffects, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	QVBoxLayout* GBEnEffectsLayout = new QVBoxLayout(GBEnEffects);
	
	GBEnEffectsLayout->addWidget(newCheck("enheffects/normalize", this));
	GBEnEffectsLayout->addWidget(newCheck("enheffects/despeckle", this));
	GBEnEffectsLayout->addWidget(newCheck("enheffects/enhance", this));
	GBEnEffectsLayout->addWidget(newCheck("enheffects/equalize", this));
	
	QGroupBox* GBContrast = new QGroupBox(Model->headerData(
			Model->index("enheffects/applycontrast").column(), Qt::Horizontal).toString(), this);
	GBEnEffectsLayout->addWidget(GBContrast);
	GBContrast->setCheckable(true);
	connect(GBContrast, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	Mapper->addMapping(GBContrast, Model->index("enheffects/applycontrast").column());
	
	QVBoxLayout* GBContrastLayout = new QVBoxLayout(GBContrast);
	GBContrastLayout->addWidget(newSlider("enheffects/contrast", 0, 20, this));
	
	
	QGroupBox* GBSigContrast = new QGroupBox(Model->headerData(
			Model->index("enheffects/applysigmoidalcontrast").column(), Qt::Horizontal).toString(), this);
	GBEnEffectsLayout->addWidget(GBSigContrast);
	GBSigContrast->setCheckable(true);
	connect(GBSigContrast, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	Mapper->addMapping(GBSigContrast, Model->index("enheffects/applysigmoidalcontrast").column());
	
	QVBoxLayout* GBSigContrastLayout = new QVBoxLayout(GBSigContrast);
	GBSigContrastLayout->addWidget(newSlider("enheffects/sigmoidalcontrast", 0, 20, this));
	GBSigContrastLayout->addWidget(newSlider("enheffects/sigmoidalmidpoint", 0, 100, this));
	
	GBEnEffectsLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
	
	Mapper->toFirst();
}

ImageEnhancementEffectWidget::~ImageEnhancementEffectWidget()
{
}

