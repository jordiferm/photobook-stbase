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

#include "sconfigwidget.h"

#include <QtGui>

//#include "sappsettings.h"

SConfigWidget::SConfigWidget(QWidget* _Parent) : QWidget(_Parent)
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMinimumWidth(150);
	//contentsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	//contentsWidget->setSpacing(52);
	
	pagesWidget = new QStackedWidget;
	//contentsWidget->setCurrentRow(0);
	
	QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->addWidget(contentsWidget);
	horizontalLayout->addWidget(pagesWidget, 1);
	horizontalLayout->setStretchFactor(contentsWidget, 1);
	horizontalLayout->setStretchFactor(pagesWidget, 5);

	
	setWindowTitle(tr("Config Dialog"));
	connect(contentsWidget,
				SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
				this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

//! Un widget normal => No cridarà a save i load...
void SConfigWidget::addWidget(QWidget* _Widget, const QIcon& _Icon, const QString& _Label)
{	
	QWidget* NewPage = new QWidget(this);
	QVBoxLayout* NPLayout = new QVBoxLayout(NewPage); 
	NPLayout->setSpacing(0); 
	NPLayout->setMargin(0); 

	QLabel* PageLabel = new QLabel(_Label, NewPage); 
	QFont Font = PageLabel->font();
	Font.setBold(true);
	PageLabel->setFont(Font); 
	NPLayout->addWidget(PageLabel);
	
	QFrame* Line = new QFrame(NewPage);
	Line->setFrameShape(QFrame::HLine);	
	Line->setFrameShadow(QFrame::Sunken);
	NPLayout->addWidget(Line);

	NPLayout->addWidget(_Widget);
	
	pagesWidget->addWidget(NewPage);
	QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
	configButton->setIcon(_Icon);
	configButton->setText(_Label);
	configButton->setTextAlignment(Qt::AlignHCenter);
	configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	if (pagesWidget->count() == 1)
		contentsWidget->setCurrentRow(0);
}


void SConfigWidget::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;

	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}


