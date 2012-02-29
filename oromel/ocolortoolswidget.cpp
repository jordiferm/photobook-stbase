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
#include "ocolortoolswidget.h"

#include <QAction> 
#include <QToolBar>

#include "whitebalancetool.h" 
#include "adjustcurvestool.h" 
#include "adjustlevelstool.h" 
#include "bwsepiatool.h" 
#include "channelmixertool.h" 
#include "colorfxtool.h"
#include "autocorrectiontool.h"
#include "bcgtool.h" 
#include "hsltool.h" 
#include "rgbtool.h" 

using namespace Digikam;

OColorToolsWidget::OColorToolsWidget(QWidget *parent) 
 : ODigikamToolsWidget(tr("<h1>Color correction tools</h1><p>Here you can adjust the color aspects of your images like white balance, color curves, constrast, .... </p> <p>Please select one of the options on the top.</p> <center><img src=:/st/oromel/colortools.png> </img></center>"), parent)
{
}

OColorToolsWidget::~OColorToolsWidget()
{
}

void OColorToolsWidget::setupActions(QToolBar* _ToolBar)
{
	AutoCorrectionAct = new QAction(QIcon(":/st/oromel/autocorrection.png"), tr("Auto-Correction"), this);
	AutoCorrectionAct->setToolTip(tr("Color auto-correction."));
	connect(AutoCorrectionAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(AutoCorrectionAct);

	CurvesAdjustAct = new QAction(QIcon(":/st/oromel/adjustcurves.png"), tr("Curves Adjust"), this);
	CurvesAdjustAct->setToolTip(tr("Adjust color curves."));
	connect(CurvesAdjustAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(CurvesAdjustAct);

	LevelsAdjustAct = new QAction(QIcon(":/st/oromel/adjustlevels.png"), tr("Levels Adjust"), this);
	LevelsAdjustAct->setToolTip(tr("Adjust color levels."));
	connect(LevelsAdjustAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(LevelsAdjustAct);
	
	ChannelMixerAct = new QAction(QIcon(":/st/oromel/channelmixer.png"), tr("Channel Mixer"), this);
	ChannelMixerAct->setToolTip(tr("Color channels mixer."));
	connect(ChannelMixerAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(ChannelMixerAct);

	WhiteBalanceAct = new QAction(QIcon(":/st/oromel/whitebalance.png"), tr("White Balance"), this);
	WhiteBalanceAct->setToolTip(tr("White Balance adjustments."));
	connect(WhiteBalanceAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(WhiteBalanceAct);

	BWSepiaAct = new QAction(QIcon(":/st/oromel/blackandwhite.png"), tr("Black and White"), this);
	BWSepiaAct->setToolTip(tr("Black and white and sepia effects."));
	connect(BWSepiaAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(BWSepiaAct);

	ColorFxAct = new QAction(QIcon(":/st/oromel/colorfx.png"), tr("Color Effects"), this);
	ColorFxAct->setToolTip(tr("Special color effects."));
	connect(ColorFxAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(ColorFxAct);

	BCGAct = new QAction(QIcon(":/st/oromel/contrast.png"), tr("Brightness/Contrast/Gamma"), this);
	BCGAct->setToolTip(tr("Brightness, Contrast and Gamma adjustment."));
	connect(BCGAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(BCGAct);

	HSLAct = new QAction(QIcon(":/st/oromel/adjusthsl.png"), tr("Hue/Saturation/Lightness"), this);
	HSLAct->setToolTip(tr("Hue, Saturation and Lightness adjustment."));
	connect(HSLAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(HSLAct);

	RGBAct = new QAction(QIcon(":/st/oromel/adjustrgb.png"), tr("Color Balance"), this);
	RGBAct->setToolTip(tr("Color balance adjustment."));
	connect(RGBAct, SIGNAL(triggered()), this, SLOT(slotDoToolAction()));	
	_ToolBar->addAction(RGBAct);


}


void OColorToolsWidget::slotDoToolAction()
{
	if (sender() == WhiteBalanceAct)
		loadTool(new DigikamWhiteBalanceImagesPlugin::WhiteBalanceTool(this));
	else if (sender() == LevelsAdjustAct)
		loadTool(new DigikamAdjustLevelsImagesPlugin::AdjustLevelsTool(this));
	else if (sender() == CurvesAdjustAct)
		loadTool(new DigikamAdjustCurvesImagesPlugin::AdjustCurvesTool(this));
	else if (sender() == ChannelMixerAct)
		loadTool(new DigikamChannelMixerImagesPlugin::ChannelMixerTool(this));
	else if (sender() == BWSepiaAct)
		loadTool(new DigikamImagesPluginCore::BWSepiaTool(this)); 
	else if (sender() == ColorFxAct)
		loadTool(new DigikamColorFXImagesPlugin::ColorFXTool(this)); 
	else if (sender() == AutoCorrectionAct)
		loadTool(new DigikamImagesPluginCore::AutoCorrectionTool(this)); 
	else if (sender() == BCGAct)
		loadTool(new DigikamImagesPluginCore::BCGTool(this)); 
	else if (sender() == HSLAct)  
		loadTool(new DigikamImagesPluginCore::HSLTool(this)); 
	else if (sender() == RGBAct)  
		loadTool(new DigikamImagesPluginCore::RGBTool(this)); 


}


