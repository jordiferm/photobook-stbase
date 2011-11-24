/****************************************************************************
**
** Copyright (C) 2006-2008 Softtopia. All rights reserved.
**
** This file is part of Softtopia Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Softtopia reserves all rights not expressly granted herein.
**
** Softtopia (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "strecteditwidget.h"
#include <QLayout>
#include <QDoubleSpinBox>
#include <QLabel>

STRectEditWidget::STRectEditWidget(QWidget *parent) :
    QWidget(parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);

	LabelTop = new QLabel(tr("Top:"), this);
	MLayout->addWidget(LabelTop);
	SPValueTop = new QDoubleSpinBox(this);
	SPValueTop->setRange(-10000, 10000);
	MLayout->addWidget(SPValueTop);

	LabelLeft = new QLabel(tr("Left:"), this);
	MLayout->addWidget(LabelLeft);
	SPValueLeft = new QDoubleSpinBox(this);
	SPValueLeft->setRange(-10000, 10000);
	MLayout->addWidget(SPValueLeft);

	LabelBottom = new QLabel(tr("Bottom:"), this);
	MLayout->addWidget(LabelBottom);
	SPValueBottom = new QDoubleSpinBox(this);
	SPValueBottom->setRange(-10000, 10000);
	MLayout->addWidget(SPValueBottom);

	LabelRight = new QLabel(tr("Right:"), this);
	MLayout->addWidget(LabelRight);
	SPValueRight = new QDoubleSpinBox(this);
	SPValueRight->setRange(-10000, 10000);
	MLayout->addWidget(SPValueRight);
}

void STRectEditWidget::setEditedRect(const QRectF& _Rect)
{
	SPValueLeft->setValue(_Rect.left());
	SPValueRight->setValue(_Rect.right());
	SPValueTop->setValue(_Rect.top());
	SPValueBottom->setValue(_Rect.bottom());
}

QRectF STRectEditWidget::editedRect() const
{
	QRectF Res;
	Res.setTop(SPValueTop->value());
	Res.setLeft(SPValueLeft->value());
	Res.setBottom(SPValueBottom->value());
	Res.setRight(SPValueRight->value());
	return Res;
}

void STRectEditWidget::setSuffix(const QString& _Suffix)
{
	SPValueTop->setSuffix(_Suffix);
	SPValueLeft->setSuffix(_Suffix);
	SPValueBottom->setSuffix(_Suffix);
	SPValueRight->setSuffix(_Suffix);
}
