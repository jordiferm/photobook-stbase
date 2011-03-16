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

#include "sharpeneffectwidget.h"
#include <QLayout>
#include <QGroupBox>
#include <QLabel> 
#include <QSlider> 
#include <QDataWidgetMapper>
#include <QLineEdit>

#include "imimage.h"
#include "abstractimageeffectmodel.h"

SharpenEffectFilterManager::SharpenEffectFilterManager(AbstractImageEffectModel* _Model, QObject* _Parent) 
	: AbstractImageEffectManager(_Model, _Parent)
{
	_Model->addKey("sharpen/apply", false, QVariant::Bool, tr("Apply sharpen?"));
	_Model->addKey("sharpen/radius", 0, QVariant::Int, tr("Sharpen Radius"));
	_Model->addKey("sharpen/sigma", 20, QVariant::Int, tr("Sharpen Sigma"));
}

bool SharpenEffectFilterManager::anyEffectToApply() const
{
	return (Model->data(Model->index("sharpen/apply")).toBool());
}

void SharpenEffectFilterManager::applyEffects(IMImage& _SourceImage)
{
	if (Model->data(Model->index("sharpen/apply")).toBool())
	{
		qDebug("Applying Sharpen");
		double Radius = static_cast<double>(Model->data(Model->index("sharpen/radius")).toInt()) / 100;
		double Sigma = static_cast<double>(Model->data(Model->index("sharpen/sigma")).toInt()) / 100;
		_SourceImage.sharpen(Radius, Sigma);
	}
}


SharpenEffectWidget::SharpenEffectWidget(AbstractImageEffectModel* _Model, QWidget* _Parent): AbstractImageEffectWidget(_Model, _Parent)
{
	setEManager(new SharpenEffectFilterManager(_Model, this));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	GBSharpen = new QGroupBox(tr("Apply sharpen image effect"), this);
	MLayout->addWidget(GBSharpen);
	GBSharpen->setCheckable(true);
	Mapper->addMapping(GBSharpen, _Model->index("sharpen/apply").column());
	
	connect(GBSharpen, SIGNAL(clicked()), this, SIGNAL(valueChanged()));
	QVBoxLayout* GBSharpenLayout = new QVBoxLayout(GBSharpen);
	
	RadiusLabel = new QLabel(GBSharpen);
	GBSharpenLayout->addWidget(RadiusLabel);
	SRadius = new QSlider(GBSharpen);
	Mapper->addMapping(SRadius, _Model->index("sharpen/radius").column());
	connect(SRadius, SIGNAL(sliderReleased()), this, SIGNAL(valueChanged()));
	connect(SRadius, SIGNAL(valueChanged(int )), this, SLOT(setRadius(int )));
	SRadius->setRange(0,5000);
	SRadius->setOrientation(Qt::Horizontal);
	GBSharpenLayout->addWidget(SRadius); 
	
	SigmaLabel = new QLabel(GBSharpen);
	
	GBSharpenLayout->addWidget(SigmaLabel);
	SSigma = new QSlider(GBSharpen);
	Mapper->addMapping(SSigma, _Model->index("sharpen/sigma").column());
	connect(SSigma, SIGNAL(sliderReleased()), this, SIGNAL(valueChanged()));
	connect(SSigma, SIGNAL(valueChanged(int )), this, SLOT(setSigma(int )));
	SSigma->setOrientation(Qt::Horizontal);
	SSigma->setRange(20,1000);
	GBSharpenLayout->addWidget(SSigma); 
	
	setRadius(0);
	setSigma(20);
	
	GBSharpenLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));	
	
	Mapper->toFirst();
}

void SharpenEffectWidget::setRadius(int /*_Value*/)
{
	double Radius = static_cast<double>(SRadius->value()) / 100;
	RadiusLabel->setText(QString(tr("Radius: %1").arg(Radius, 0, 'f', 3)));
}

void SharpenEffectWidget::setSigma(int /*_Value*/)
{
	double Sigma = static_cast<double>(SSigma->value()) / 100;
	SigmaLabel->setText(QString(tr("Sigma: %1").arg(Sigma, 0, 'f', 3)));
}

