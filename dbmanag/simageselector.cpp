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

#include "simageselector.h"

//SImageSelector
#include <QLayout>
#include <QFileDialog>
#include <QBuffer>
#include <QToolButton>

// _________________________________________________________________________
//
// Class SImageSelector
// _________________________________________________________________________

SImageSelector::SImageSelector(QWidget* _Parent)
: QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this);
	MLayout->setMargin(2);
	MTButton = new QToolButton(this);
	MLayout->addWidget(MTButton);
	MRemoveButton = new QToolButton(this);
	MRemoveButton->setIcon(QIcon(":/st/editdelete.png"));
	MRemoveButton->setIconSize(QSize(32, 32));
	MLayout->addWidget(MRemoveButton);

	setFocusPolicy(Qt::StrongFocus);

	connect(MTButton, SIGNAL(clicked()), this, SLOT(selectImage()));
	connect(MRemoveButton, SIGNAL(clicked()), this, SLOT(removeImage()));
}

void SImageSelector::setPixmap(const QPixmap& _Pixmap)
{
	MPixmap = _Pixmap;
	if (!MPixmap.isNull())
	{
		MTButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
		QPixmap DisplPix;
		if (MPixmap.width() > 128 || MPixmap.height() > 128)
			DisplPix = MPixmap.scaled(128, 128, Qt::KeepAspectRatio);
		else
			DisplPix = MPixmap;

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

void SImageSelector::setImage(const QVariant& _Image)
{
	QByteArray ImageData = _Image.toByteArray();
	QImage Image;
	Image.loadFromData(ImageData);
	setPixmap(QPixmap::fromImage(Image));
}

QVariant SImageSelector::image() const
{
	QImage MImage = MPixmap.toImage();
	QByteArray Bytes;
	QBuffer Buffer(&Bytes);
	Buffer.open(QIODevice::WriteOnly);
	MImage.save(&Buffer, "PNG");

	return Bytes;
}

void SImageSelector::selectImage()
{
	QString FileName = QFileDialog::getOpenFileName(this, tr("Open File"),
												 "~",
												 tr("Images (*.png *.xpm *.jpg)"));
	if (!FileName.isEmpty())
	{
		setPixmap(QPixmap(FileName));
		setFocus(); //I think that windows handles focuspolicy in a diferent way.
	}
}

void SImageSelector::removeImage()
{
	setPixmap(QPixmap());
}


