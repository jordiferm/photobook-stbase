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

#include "stbusyinfowidget.h"
#include <QLayout>
#include <QLabel>
#include <QDesktopWidget>

STBusyInfoWidget* STBusyInfoWidget::StaticMe = 0;

STBusyInfoWidget::STBusyInfoWidget(QWidget* _Parent) : QWidget(_Parent, Qt::Popup)
{
	QVBoxLayout* MainLayout = new QVBoxLayout(this);
	InfoLabel = new QLabel(this);
	InfoLabel->setAlignment(Qt::AlignHCenter);
	MainLayout->addWidget(InfoLabel);

	QLabel* IconLabel = new QLabel(this);
	IconLabel->setPixmap(QPixmap(":/st/hourglass.png"));
	IconLabel->setAlignment(Qt::AlignCenter);
	MainLayout->addWidget(IconLabel);

	QDesktopWidget Desktop;
	InitPos = Desktop.screenGeometry(Desktop.screenNumber(_Parent)).center();
}

void STBusyInfoWidget::setText(const QString& _Text)
{
	InfoLabel->setText(_Text);
}

void STBusyInfoWidget::show()
{
	QWidget::show();
	move(QPoint(InitPos.x() - width() / 2, InitPos.y() - height() / 2 ));
}

void STBusyInfoWidget::show(const QString& _Text)
{
	if (!StaticMe)
		StaticMe = new STBusyInfoWidget;
	StaticMe->setText(_Text);
	StaticMe->show();
}

void STBusyInfoWidget::hide()
{
	if (StaticMe) //Defensive
		StaticMe->QWidget::hide();
}
