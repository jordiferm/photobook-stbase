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

#ifndef FPIXMAPSELECTOR_H
#define FPIXMAPSELECTOR_H

#include <QWidget>
#include <QPixmap>
#include "stwidgetsexport.h"

/**
Selector for pixmaps.

@author Shadow
*/
class QToolButton;
class ST_WIDGETS_EXPORT FPixmapSelector : public QWidget
{
Q_OBJECT 
	
Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap USER true)
	
	QPixmap MPixmap;
	QToolButton* MTButton;
	QToolButton* MRemoveButton;
	QString PixmapFileName;
			
public: 
	FPixmapSelector(QWidget* _Parent = 0);
	void setPixmap(const QPixmap& _Pixmap);
	QPixmap pixmap() const;
	void setPixmapFileName(const QString& _FileName);
	QString pixmapFileName() const { return PixmapFileName; }

protected slots: 
	void selectImage();
	void removeImage();

};

#endif
