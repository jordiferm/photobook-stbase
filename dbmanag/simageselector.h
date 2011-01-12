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

#ifndef SIMAGESELECTOR_H
#define SIMAGESELECTOR_H

#include <QWidget>
#include <QPixmap>
#include <QVariant>

#include "stdbmanagexport.h"

/**
Selector for images.

@author Shadow
*/
class QToolButton;
class ST_DBMANAG_EXPORT SImageSelector : public QWidget
{
Q_OBJECT

Q_PROPERTY(QVariant image READ image WRITE setImage USER true)

	QPixmap MPixmap;
	QToolButton* MTButton;
	QToolButton* MRemoveButton;

public:
	SImageSelector(QWidget* _Parent = 0);
	void setPixmap(const QPixmap& _Pixmap);
	void setImage(const QVariant& _Image);
	QVariant image() const;

protected slots:
	void selectImage();
	void removeImage();

};

#endif // SIMAGESELECTOR_H
