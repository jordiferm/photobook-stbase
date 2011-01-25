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
#include "basicfixeswidget.h"
#include <QLayout> 
#include <QLabel> 
#include <QButtonGroup> 
#include <QGroupBox>
#include <QSlider>
#include <QPushButton> 

#include "sactionbuttonsframe.h"
#include "imageTools.h"

//_____________________________________________________________________________
// 
// Class ApplyEfectBasicWidget
//_____________________________________________________________________________

ApplyEfectBasicWidget::ApplyEfectBasicWidget(const QString& _HelpText, QWidget* _Parent, 
															const QString& _ActionText, const QIcon& _ActionIcon)
	: QWidget(_Parent)
{
	MLayout = new QVBoxLayout(this); 
	QLabel* LabHelp = new QLabel(_HelpText, this); 
	LabHelp->setWordWrap(true); 
	MLayout->addWidget(LabHelp);
	
	SActionButtonsFrame* ActButtons = new SActionButtonsFrame(this);
	QAction* ApplyAct = new QAction(_ActionIcon, _ActionText, this);
	ApplyAct->setToolTip(tr("Applies current effect."));
	connect(ApplyAct, SIGNAL(triggered()), this, SIGNAL(applyEffect()));	
	ActButtons->addAction(ApplyAct);
	
	MLayout->addWidget(ActButtons);
	MLayout->addItem(new QSpacerItem(0,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
	
}
	
void ApplyEfectBasicWidget::setMainWidget(QWidget* _MainWidget)
{
	MLayout->insertWidget(1, _MainWidget); 
}


//_____________________________________________________________________________
// 
// Class CropFixesWidget
//_____________________________________________________________________________

CropFixesWidget::CropFixesWidget(QWidget* _Parent) 
	: ApplyEfectBasicWidget(tr("Select a dimension bellow and then click and drag on the image to define the region you wish to crop. When you are shure click <b>Apply</b>."),  _Parent, tr("Crop"))
{
	QFrame* CropFrame = new QFrame(this); 
	QVBoxLayout* FrLayout = new QVBoxLayout(CropFrame);
	
	FrLayout->setMargin(0);
	QGroupBox* GBDimensions = new QGroupBox(tr("Selection standard proportion"), CropFrame);
	FrLayout->addWidget(GBDimensions); 
	RButtonGroup = new QButtonGroup(CropFrame); 
	QGridLayout* BGLayout = new QGridLayout(GBDimensions);
	
	
	BGLayout->addWidget(new QLabel(tr("Selection size"), GBDimensions), 0, 0, 1, 2); 
	
	QSlider* SlCropZoom = new QSlider(GBDimensions);
	SlCropZoom->setOrientation(Qt::Horizontal); 
	SlCropZoom->setRange(100, 250); 
	SlCropZoom->setTickPosition(QSlider::TicksBothSides); 
	connect(SlCropZoom, SIGNAL(valueChanged( int )), this, SLOT(zoomSliderChanged(int ))); 	
	BGLayout->addWidget(SlCropZoom, 1, 0, 1, 2); 
	
	
	QList<QSize> CropDSizes; 
	CropDSizes.push_back(QSize(4,6)); 
	CropDSizes.push_back(QSize(7,5)); 
	CropDSizes.push_back(QSize(6,8)); 
	CropDSizes.push_back(QSize(6,9)); 
	CropDSizes.push_back(QSize(8,10)); 
	CropDSizes.push_back(QSize(8,12)); 
	
	QList<QSize>::iterator it; 
	int Cnt = BGLayout->rowCount() * 2;
	for (it = CropDSizes.begin(); it != CropDSizes.end(); ++it)
	{
		DimensionRadioButton* NButton = new DimensionRadioButton(*it, GBDimensions);
		RButtonGroup->addButton(NButton); 
		BGLayout->addWidget(NButton, Cnt / 2, Cnt % 2); 
		Cnt++;
	}
	
	RotateButton = new QPushButton(GBDimensions);
	RotateButton->setIcon(QIcon(":/st/oromel/rotate.png"));
	RotateButton->setCheckable(true); 
	connect(RotateButton, SIGNAL(toggled( bool )), this, SLOT(rotateToggled(bool )));
	BGLayout->addWidget(RotateButton, Cnt / 2, Cnt % 2); 
	
	connect(RButtonGroup, SIGNAL(buttonClicked(QAbstractButton* )), this, SLOT(cropSizeClicked(QAbstractButton* )));
	setMainWidget(CropFrame);
}

//_____________________________________________________________________________
// 
// Class StraightenFixesWidget
//_____________________________________________________________________________


StraightenFixesWidget::StraightenFixesWidget(QWidget* _Parent )
	: ApplyEfectBasicWidget(tr("Press <b>Draw Rect</b> button and draw a line that follows a straight plane of the image."),  _Parent, tr("Draw Rect"))
{}


//_____________________________________________________________________________
// 
// Class CropFixesWidget
//_____________________________________________________________________________

void CropFixesWidget::rotateToggled(bool )
{
	if (RButtonGroup->checkedButton())
		cropSizeClicked(RButtonGroup->checkedButton());
}

void CropFixesWidget::cropSizeClicked(QAbstractButton* _Button)
{
	if (DimensionRadioButton* CRButton = qobject_cast<DimensionRadioButton*>(_Button) )
	{
		QSize CropSize = CRButton->dimension();
		QSizeF CropSizeInches = QSizeF(mmToInch(CropSize.width()), mmToInch(CropSize.height()));
		if (RotateButton->isChecked())
			CropSizeInches.transpose(); 
		
		emit cropSizeChanged(CropSizeInches);	
	}
}

void CropFixesWidget::zoomSliderChanged(int _Value)
{
	emit cropZoomChanged((static_cast<double>(_Value) / 100));
}


//_____________________________________________________________________________
// 
// Class TransformationsWidget
//_____________________________________________________________________________

TransformationsWidget::TransformationsWidget(QWidget* _Parent ) : QWidget(_Parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setMargin(0); 
	MLayout->setSpacing(0); 
	
	SActionButtonsFrame* ActButtons = new SActionButtonsFrame(this, SActionButtonsFrame::LayoutVertical);
	QAction* NewAction = new QAction(QIcon(":/st/oromel/rotateright.png"), tr("Rotate Right"), this);
	connect(NewAction, SIGNAL(triggered()), this, SIGNAL(rotateRight()));	
	ActButtons->addAction(NewAction);
	
	NewAction = new QAction(QIcon(":/st/oromel/rotateleft.png"), tr("Rotate Left"), this);
	connect(NewAction, SIGNAL(triggered()), this, SIGNAL(rotateLeft()));	
	ActButtons->addAction(NewAction);
	
	NewAction = new QAction(QIcon(":/st/oromel/flipx.png"), tr("Flip X"), this);
	connect(NewAction, SIGNAL(triggered()), this, SIGNAL(flipX()));	
	ActButtons->addAction(NewAction);
	
	NewAction = new QAction(QIcon(":/st/oromel/flipy.png"), tr("Flip Y"), this);
	connect(NewAction, SIGNAL(triggered()), this, SIGNAL(flipY()));	
	ActButtons->addAction(NewAction);	
	
	MLayout->addWidget(ActButtons);
	MLayout->addItem(new QSpacerItem(0,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));	
}



//_____________________________________________________________________________
// 
// Class RedEyeFixesWidget
//_____________________________________________________________________________

RedEyeFixesWidget::RedEyeFixesWidget(QWidget* _Parent )
	: ApplyEfectBasicWidget(tr("Click, hold, and drag the mouse around the eyes regions. Once you have selected it you can press <b>Apply</b> button. The picture then will be displayed with the red eye removed."),  _Parent)
{}


//_____________________________________________________________________________
// 
// Class BasicFixesWidget
//_____________________________________________________________________________

BasicFixesWidget::BasicFixesWidget(QWidget *parent)
 : QToolBox(parent)
{
	CropFixesWidget* CrWidget = new CropFixesWidget(this);
	addItem(CrWidget, QIcon(":/crop.png"), tr("Crop"));
	connect(CrWidget, SIGNAL(cropSizeChanged(const QSizeF&)), this, SIGNAL(cropSizeChanged(const QSizeF&)));
	connect(CrWidget, SIGNAL(applyEffect()), this, SIGNAL(applyCrop()));
	connect(CrWidget, SIGNAL(cropZoomChanged(double )), this, SIGNAL(cropZoomChanged(double )));
	
	StraightenFixesWidget* StFixesWidget = new StraightenFixesWidget(this);
	connect(StFixesWidget, SIGNAL(applyEffect()), this, SIGNAL(applyStraighten()));
	addItem(StFixesWidget, QIcon(":/straighten.png"), tr("Straighten"));
	
	TransformationsWidget* TrWidget = new TransformationsWidget(this); 
	connect(TrWidget, SIGNAL(rotateRight()), this, SIGNAL(rotateRight()));
	connect(TrWidget, SIGNAL(rotateLeft()), this, SIGNAL(rotateLeft()));
	connect(TrWidget, SIGNAL(flipX()), this, SIGNAL(flipX()));
	connect(TrWidget, SIGNAL(flipY()), this, SIGNAL(flipY()));
	addItem(TrWidget, QIcon(":/transform.png"), tr("Image transformations"));
	
	RedEyeFixesWidget* RedEyeWidg = new RedEyeFixesWidget(this);
	connect(RedEyeWidg, SIGNAL(applyEffect()), this, SIGNAL(applyRedEyes()));
	addItem(RedEyeWidg, QIcon(":/redeye.png"), tr("Red Eye correction"));
	
}


BasicFixesWidget::~BasicFixesWidget()
{
}


