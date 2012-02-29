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

#ifndef THUMBNAILSIZEWIDGET_H
#define THUMBNAILSIZEWIDGET_H
#include "stwidgetsexport.h"

#include <QWidget>

class QSlider;
class ST_WIDGETS_EXPORT ThumbnailSizeWidget : public QWidget
{
	Q_OBJECT

	QSlider* Slider;

public:
	ThumbnailSizeWidget(QWidget* _Parent = 0);
	void setValue(int _Value);

signals:
	void valueChanged(int);
};

#endif // SPTHUMBNAILSIZEWIDGET_H
