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

#include "advancedeffectfilterwidget.h"
#include <QLayout> 
#include <QGroupBox> 
#include <QCheckBox> 
#include <QDataWidgetMapper>

#include "imimage.h"
#include "abstractimageeffectmodel.h"
#include "labeledsliderwidget.h"

AdvancedEffectFilterManager::AdvancedEffectFilterManager(AbstractImageEffectModel* _Model, QObject* _Parent) 
	: AbstractImageEffectManager(_Model, _Parent)
{
	_Model->addKey("adveffects/apply", false, QVariant::Bool, tr("Apply advanced filters ?"));
	_Model->addKey("adveffects/applymedianfilter", false, QVariant::Bool, tr("Median filter"));
	_Model->addKey("adveffects/medianfilter", 1, QVariant::Int, tr("Value"));
	
	_Model->addKey("adveffects/applymodulatefilter", false, QVariant::Bool, tr("Modulate filter"));
	_Model->addKey("adveffects/modulatebrightness", 100, QVariant::Int, tr("Brightness"));
	_Model->addKey("adveffects/modulatesaturation", 100, QVariant::Int, tr("Saturation"));
	_Model->addKey("adveffects/modulatehue", 100, QVariant::Int, tr("Hue"));
	
	_Model->addKey("adveffects/applyreducenoisefilter", false, QVariant::Bool, tr("Reduce Noise"));
	_Model->addKey("adveffects/reducenoise", 1, QVariant::Int, tr("Amount"));
// 	_Model->addKey("adveffects/applysigmoidalcontrastfilter", false, QVariant::Bool, tr("Sigmoidal Contrast"));
// 	_Model->addKey("adveffects/sigmoidalcontrastsharpen", 1, QVariant::Int, tr("Sharpen"));
// 	_Model->addKey("adveffects/sigmoidalcontrastcontrast", 1, QVariant::Int, tr("Contrast"));
}

bool AdvancedEffectFilterManager::anyEffectToApply() const
{
	return (Model->data(Model->index("adveffects/apply")).toBool());
}

void AdvancedEffectFilterManager::applyEffects(IMImage& _SourceImage)
{
	if (Model->data(Model->index("adveffects/apply")).toBool())
	{
		qDebug("Applying Advanced Effect");
		
		if (Model->data(Model->index("adveffects/applymedianfilter")).toBool())
			_SourceImage.medianFilter(static_cast<double>(Model->data(Model->index("adveffects/applymedianfilter")).toInt()) / 100);
		
		if (Model->data(Model->index("adveffects/applymodulatefilter")).toBool())
		{
			_SourceImage.modulate(Model->data(Model->index("adveffects/modulatebrightness")).toInt(), 
							  Model->data(Model->index("adveffects/modulatesaturation")).toInt(), 
							  Model->data(Model->index("adveffects/modulatehue")).toInt());
		
		}
		if (Model->data(Model->index("adveffects/applyreducenoisefilter")).toBool())
			_SourceImage.medianFilter(static_cast<double>(Model->data(Model->index("adveffects/reducenoise")).toInt()) / 100);
	
// 		if (Model->data(Model->index("adveffects/applysigmoidalcontrastfilter")).toBool())
// 		{
// 			Image.sigmoidalContrast(Model->data(Model->index("adveffects/sigmoidalcontrastsharpen")).toInt(), 
// 							  static_cast<double>(Model->data(Model->index("adveffects/sigmoidalcontrastcontrast")).toInt()) / 100);
// 		}	
	}
}


AdvancedEffectFilterWidget::AdvancedEffectFilterWidget(AbstractImageEffectModel* _Model, QWidget* _Parent): AbstractImageEffectWidget(_Model, _Parent)
{
	setEManager(new AdvancedEffectFilterManager(_Model, this));
	
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	
	QGroupBox* MainGB = newGroupBox("adveffects/apply", this);
	MLayout->addWidget(MainGB);
	QVBoxLayout* MainGBLayout = new QVBoxLayout(MainGB);
	
	QGroupBox* MedianFilterGB = newGroupBox("adveffects/applymedianfilter", MainGB);
	MainGBLayout->addWidget(MedianFilterGB);
	QVBoxLayout* MedianFilterGBLayout = new QVBoxLayout(MedianFilterGB);
	MedianFilterGBLayout->addWidget(newSlider("adveffects/medianfilter", 0, 255, this));
	
	
	QGroupBox* ModulateFilterGB = newGroupBox("adveffects/applymodulatefilter", MainGB);
	MainGBLayout->addWidget(ModulateFilterGB);
	QVBoxLayout* ModulateFilterGBLayout = new QVBoxLayout(ModulateFilterGB);
	ModulateFilterGBLayout->addWidget(newSlider("adveffects/modulatebrightness", 0, 255, this));
	ModulateFilterGBLayout->addWidget(newSlider("adveffects/modulatesaturation", 0, 255, this));
	ModulateFilterGBLayout->addWidget(newSlider("adveffects/modulatehue", 0, 255, this));
	
	QGroupBox* ReduceNoiseFilterGB = newGroupBox("adveffects/applyreducenoisefilter", MainGB);
	MainGBLayout->addWidget(ReduceNoiseFilterGB);
	QVBoxLayout* ReduceNoiseFilterGBLayout = new QVBoxLayout(ReduceNoiseFilterGB);
	ReduceNoiseFilterGBLayout->addWidget(newSlider("adveffects/reducenoise", 0, 255, this));
	
	MainGBLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));	
	
// 	QGroupBox* SMContrastFilterGB = newGroupBox("adveffects/applysigmoidalcontrastfilter", MainGB);
// 	MainGBLayout->addWidget(SMContrastFilterGB);
// 	QVBoxLayout* SMContrastFilterGBLayout = new QVBoxLayout(SMContrastFilterGB);
// 	SMContrastFilterGBLayout->addWidget(newSlider("adveffects/sigmoidalcontrastsharpen", 0, 255, this));
// 	SMContrastFilterGBLayout->addWidget(newSlider("adveffects/sigmoidalcontrastcontrast", 0, 1000, this));	
	Mapper->toFirst();	
}


