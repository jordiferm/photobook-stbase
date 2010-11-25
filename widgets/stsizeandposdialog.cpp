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
#include "stsizeandposdialog.h"
#include <QDialogButtonBox>
#include <QLayout> 
#include <QLabel> 
#include <QLineEdit> 
#include <QPushButton> 
#include <QDoubleSpinBox> 

#include "siconfactory.h"

STSizeAndPosDialog::STSizeAndPosDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	QFrame* ValuesFrame = new QFrame(this); 
	MLayout->addWidget(ValuesFrame); 
	QGridLayout* FrLayout = new QGridLayout(ValuesFrame); 

	FrLayout->addWidget(new QLabel(tr("X:"), ValuesFrame), 0, 0); 
	SPValueX = new QDoubleSpinBox(ValuesFrame);
	SPValueX->setRange(-10000, 10000); 
	FrLayout->addWidget(SPValueX, 0, 1); 
	
	FrLayout->addWidget(new QLabel(tr("Y:"), ValuesFrame), 0, 2); 
	SPValueY = new QDoubleSpinBox(ValuesFrame);
	SPValueY->setRange(-10000, 10000); 
	FrLayout->addWidget(SPValueY, 0, 3); 

	FrLayout->addWidget(new QLabel(tr("Width:"), ValuesFrame), 1, 0); 
	SPValueWidth = new QDoubleSpinBox(ValuesFrame);
	SPValueWidth->setRange(-10000, 10000); 
	FrLayout->addWidget(SPValueWidth, 1, 1); 

	FrLayout->addWidget(new QLabel(tr("Height:"), ValuesFrame), 1, 2); 
	SPValueHeight = new QDoubleSpinBox(ValuesFrame);
	SPValueHeight->setRange(-10000, 10000); 
	FrLayout->addWidget(SPValueHeight, 1, 3); 

	QDialogButtonBox* Buttons = new QDialogButtonBox(this);

	QPushButton* ButOk = Buttons->addButton(tr("Ok"), QDialogButtonBox::AcceptRole);
	ButOk->setIcon(stIcon(SIconFactory::Accept)); 

	QPushButton* ButCancel = Buttons->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
	ButCancel->setIcon(stIcon(SIconFactory::Cancel)); 
	
	MLayout->addWidget(Buttons);
	connect(Buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(Buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void STSizeAndPosDialog::setSize(const QSizeF& _Size)
{
	SPValueWidth->setValue(_Size.width()); 
	SPValueHeight->setValue(_Size.height()); 	
}

QSizeF STSizeAndPosDialog::size() const
{
	return QSizeF(SPValueWidth->value(), SPValueHeight->value());
}

void STSizeAndPosDialog::setPosition(const QPointF& _Position)
{
	SPValueX->setValue(_Position.x()); 
	SPValueY->setValue(_Position.y()); 
}

QPointF STSizeAndPosDialog::position() const 
{
	return QPointF(SPValueX->value(), SPValueY->value());
}
