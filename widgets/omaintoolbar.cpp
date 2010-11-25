/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
**
** This file is part of starblitz Foto Suite.
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
#include "omaintoolbar.h"
#include <QLabel> 
#include <QDesktopServices> 
#include <QUrl>

#include <QApplication>
#include <QFile> 

OMainToolBar::OMainToolBar(QWidget *parent)
 : QToolBar(parent)
{
	QLabel* StarblitzLab = new QLabel(this); 
	//StarblitzLab->setPixmap(QPixmap(":/logo_starblitz.png"));
	StarblitzLab->setText("<a href=http://www.starblitz-k.com><img src=:/st/logo_starblitz.png /></a>");
	QString PublisherLogo = QApplication::applicationDirPath() + "/publisherlogo.png"; 
	if (QFile::exists(PublisherLogo))
	{
		StarblitzLab->setPixmap(QPixmap(PublisherLogo));
		StarblitzLab->setMaximumHeight(70);
	}

	connect(StarblitzLab, SIGNAL(linkActivated(const QString&)), this, SLOT(openLink(const QString& )));
	addWidget(StarblitzLab);
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

