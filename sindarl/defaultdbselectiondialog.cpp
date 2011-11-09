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

#include "defaultdbselectiondialog.h"
#include <QDialogButtonBox>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "siconfactory.h"

void DefaultDbSelectionDialog::updateDescription(int _Index)
{
	DescriptionLabel->setText(QString("<small>%1</small>").arg(DDBList[_Index].description()));
}

void DefaultDbSelectionDialog::selectDatabase(int _Index)
{
	updateDescription(_Index);
	SelectedDatabase = DDBList[_Index];
}

DefaultDbSelectionDialog::DefaultDbSelectionDialog(QWidget *parent) :
    QDialog(parent)
{
	DDBList = SindarDatabase::getDefaultDatabases();

	setWindowTitle(tr("Publisher default database creation"));
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	QLabel* MLabel = new QLabel(tr("<h2>Please, select one of the following options:</h2>"));
	MLayout->addWidget(MLabel);

	QHBoxLayout* DBSelectLayout = new QHBoxLayout;
	MLayout->addLayout(DBSelectLayout);

	DBSelectLayout->addWidget(new QLabel("Database type:"));

	QComboBox* DBTypeCombo = new QComboBox(this);
	DBSelectLayout->addWidget(DBTypeCombo);
	for (int Vfor = 0; Vfor < DDBList.size(); Vfor++)
	{
		DBTypeCombo->insertItem(Vfor, DDBList[Vfor].name());
	}
	connect(DBTypeCombo, SIGNAL(activated(int)), this, SLOT(slotComboIndexActivated(int)));

	DescriptionLabel = new QLabel(this);
	MLayout->addWidget(DescriptionLabel);

	QDialogButtonBox* ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	ButtonBox->button(QDialogButtonBox::Ok)->setIcon(stIcon(SIconFactory::Accept));
	MLayout->addWidget(ButtonBox);
	connect(ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));

	if (DDBList.size() > 0)
	{
		DBTypeCombo->setCurrentIndex(0);
		selectDatabase(0);
	}
}

void DefaultDbSelectionDialog::slotComboIndexActivated(int _Index)
{
	selectDatabase(_Index);
}

