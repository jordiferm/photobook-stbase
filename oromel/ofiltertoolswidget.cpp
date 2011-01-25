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
#include "ofiltertoolswidget.h"
#include <QAction> 
#include <QToolBar> 

#include "oilpainttool.h" 
#include "raindroptool.h" 
#include "blurfxtool.h" 
#include "charcoaltool.h" 
#include "distortionfxtool.h"


OFilterToolsWidget::OFilterToolsWidget(QWidget* parent)
: ODigikamToolsWidget(tr("<h1>Filter tools</h1><p>Here you can apply filters to your images.</p> <p>Please select filters on the top.</p> <center><img src=:/st/oromel/filter.png> </img></center>"), parent)
{
}

OFilterToolsWidget::~OFilterToolsWidget()
{
}

void OFilterToolsWidget::setupActions(QToolBar* _ToolBar)
{
	OilPaintAct = new QAction(QIcon(":/st/oromel/oilpaint.png"), tr("Oil Paint"), this);
	OilPaintAct->setToolTip(tr("Oil paint filter."));
	connect(OilPaintAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(OilPaintAct);

	RainDropAct = new QAction(QIcon(":/st/oromel/raindrop.png"), tr("Raindrops"), this);
	RainDropAct->setToolTip(tr("Raindrops filter."));
	connect(RainDropAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(RainDropAct);

	BlurEffectsAct = new QAction(QIcon(":/st/oromel/blurfx.png"), tr("Blur effects"), this);
	BlurEffectsAct->setToolTip(tr("Blur effects filter."));
	connect(BlurEffectsAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(BlurEffectsAct);

	CharcoalAct = new QAction(QIcon(":/st/oromel/charcoaltool.png"), tr("Charcoal drawing"), this);
	CharcoalAct->setToolTip(tr("Charcoal drawing filter."));
	connect(CharcoalAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(CharcoalAct);

	DistortionAct = new QAction(QIcon(":/st/oromel/distortionfx.png"), tr("Distortion effects"), this);
	DistortionAct->setToolTip(tr("Distortion effects filter."));
	connect(DistortionAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(DistortionAct);
}

void OFilterToolsWidget::slotDoToolAction()
{
	if (sender() == OilPaintAct)
		loadTool(new DigikamOilPaintImagesPlugin::OilPaintTool(this));
	if (sender() == RainDropAct)
		loadTool(new DigikamRainDropImagesPlugin::RainDropTool(this)); 
	if (sender() == BlurEffectsAct)
		loadTool(new DigikamBlurFXImagesPlugin::BlurFXTool(this)); 
	if (sender() == CharcoalAct)
		loadTool(new DigikamCharcoalImagesPlugin::CharcoalTool(this));
	if (sender() == DistortionAct)
		loadTool(new DigikamDistortionFXImagesPlugin::DistortionFXTool(this)); 
}


