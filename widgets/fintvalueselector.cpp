/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "fintvalueselector.h"
#include <QLayout>
#include <QComboBox>

// _________________________________________________________________________*/
//
//	Class FIntValueComboBox
// _________________________________________________________________________*/

FIntValueSelector::FIntValueSelector(QWidget* _Parent) : QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	MLayout->setMargin(0);
	MLayout->setSpacing(0);
	Combo = new QComboBox(this);
	MLayout->addWidget(Combo);
	setFocusPolicy(Qt::StrongFocus);
	Combo->setFocusPolicy(Qt::NoFocus);
	connect(Combo, SIGNAL(currentIndexChanged(int )), this, SIGNAL(selectedValueChanged()));
}

QVariant FIntValueSelector::value() const
{
	//qDebug("FIntValueSelector::value()");
	return Combo->itemData(Combo->currentIndex());
}

void FIntValueSelector::setValue(const QVariant& _Value)
{
	emit(selectedValueChanged());
	Combo->setCurrentIndex(Combo->findData(_Value));
}

void FIntValueSelector::addItem(const QString& _Text, const QVariant& _Value)
{
	Combo->addItem(_Text, _Value);
}

