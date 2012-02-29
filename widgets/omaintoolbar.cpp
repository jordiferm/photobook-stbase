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
#include "omaintoolbar.h"
#include <QLabel> 
#include <QDesktopServices> 
#include <QUrl>

#include <QApplication>
#include <QFile> 

OMainToolBar::OMainToolBar(QWidget *parent)
 : QToolBar(parent)
{
	QLabel* AiliImageLab = new QLabel(this);
	//AiliImageLab->setPixmap(QPixmap(":/logo_Aili Image.png"));
	AiliImageLab->setText("<a href=http://www.ailiimage.com><img src=:/st/logo_ailiimage.png /></a>");
	QString PublisherLogo = QApplication::applicationDirPath() + "/publisherlogo.png"; 
	if (QFile::exists(PublisherLogo))
	{
		AiliImageLab->setPixmap(QPixmap(PublisherLogo));
		AiliImageLab->setMaximumHeight(70);
	}

	connect(AiliImageLab, SIGNAL(linkActivated(const QString&)), this, SLOT(openLink(const QString& )));
	addWidget(AiliImageLab);
	addSeparator();
	
	setIconSize(QSize(64, 64)); 
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
}


OMainToolBar::~OMainToolBar()
{
}

void OMainToolBar::openLink(const QString& _Link)
{
	QDesktopServices::openUrl(QUrl(_Link));
}

