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

#include "fpixmapselector.h"
#include <QLayout> 
#include <QFileDialog> 
#include <QToolButton> 
#include "siconfactory.h"

FPixmapSelector::FPixmapSelector(QWidget* _Parent)
: QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	MLayout->setMargin(2);
	MTButton = new QToolButton(this);
	MLayout->addWidget(MTButton);
	MRemoveButton = new QToolButton(this);
	MRemoveButton->setIcon(stIcon(SIconFactory::EditDelete));
	MRemoveButton->setIconSize(QSize(32, 32));
	MLayout->addWidget(MRemoveButton);
	
	setFocusPolicy(Qt::StrongFocus);
	
	connect(MTButton, SIGNAL(clicked()), this, SLOT(selectImage()));
	connect(MRemoveButton, SIGNAL(clicked()), this, SLOT(removeImage()));
}

void FPixmapSelector::setPixmap(const QPixmap& _Pixmap)
{
	MPixmap = _Pixmap;
	if (!_Pixmap.isNull())
	{
		MTButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
		QPixmap DisplPix;
		if (_Pixmap.width() > 128 || _Pixmap.height() > 128)
			DisplPix = _Pixmap.scaled(128, 128, Qt::KeepAspectRatio);
		else 
			DisplPix = _Pixmap; 
		
		MTButton->setIcon(QIcon(DisplPix));
		MTButton->setIconSize(QSize(qMax(DisplPix.size().width(), 32),qMax(DisplPix.size().height(), 32) ));
		MRemoveButton->setVisible(true);
	}
	else 
	{
		MTButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
		MTButton->setText(tr("Click to select an image."));
		MRemoveButton->setVisible(false);
	}
}

QPixmap FPixmapSelector::pixmap() const
{
	return MPixmap;
}


void FPixmapSelector::setPixmapFileName(const QString& _FileName)
{
	setPixmap(QPixmap(_FileName));
	PixmapFileName = _FileName;
}

void FPixmapSelector::selectImage()
{
	QString FileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 "~",
                                                 tr("Images (*.png *.xpm *.jpg)"));
	if (!FileName.isEmpty())
	{
		setPixmapFileName(FileName);
		setFocus(); //I think that windows handles focuspolicy in a diferent way.
	}
}

void FPixmapSelector::removeImage()
{
	setPixmap(QPixmap());
}

