/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "stprogressindicator.h"
#include <QProgressBar>
#include <QApplication>


STProgressIndicator::STProgressIndicator(QProgressBar* _ProgressBar) : ProgressBar(_ProgressBar)
{
}

void STProgressIndicator::setRange(int _From, int _To)
{
	if (ProgressBar)
		ProgressBar->setRange(_From, _To);
}

void STProgressIndicator::start(const QString& _Message)
{
	if (ProgressBar)
		setValue(ProgressBar->minimum());
}

void STProgressIndicator::setValue(int _Value)
{
	if (ProgressBar)
	{
		ProgressBar->setValue(_Value);
		QApplication::processEvents();
	}
}

void STProgressIndicator::incValue()
{
	if (ProgressBar)
		setValue(ProgressBar->value() + 1);
}

void STProgressIndicator::stop()
{
	if (ProgressBar)
		setValue(ProgressBar->maximum());
}
