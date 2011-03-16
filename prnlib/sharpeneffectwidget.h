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

#ifndef SHARPENEFFECTWIDGET_H
#define SHARPENEFFECTWIDGET_H

#include "abstractimageeffectwidget.h"
#include "abstractimageeffectmanager.h"
#include "stprnlibexport.h"

class ST_PRNLIB_EXPORT SharpenEffectFilterManager : public AbstractImageEffectManager
{
Q_OBJECT	
		
public:
	SharpenEffectFilterManager(AbstractImageEffectModel* _Model, QObject* _Parent);
	virtual bool anyEffectToApply() const;
	virtual void applyEffects(IMImage& _SourceImage);
};

/**
	@author Jordi Fern?dez <shadow@softwarelliure.com>
*/
class QGroupBox;
class QLabel;
class QSlider;
class ST_PRNLIB_EXPORT SharpenEffectWidget : public AbstractImageEffectWidget
{
Q_OBJECT
	QGroupBox* GBSharpen;
	QLabel* RadiusLabel;
	QLabel* SigmaLabel;
	QSlider* SSigma;
	QSlider* SRadius;
	
public:
	SharpenEffectWidget(AbstractImageEffectModel* _Model, QWidget* _Parent);
	
protected slots:
	void setRadius(int _Value);
	void setSigma(int _Value);
};

#endif
