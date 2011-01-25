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
#ifndef STPHOTOBOOKINFOWIDGET_H
#define STPHOTOBOOKINFOWIDGET_H

#include <QWidget>
#include "stphototemplatesexport.h"

/**
Widget to display information about current PhotoBook.

	@author
*/
class STPhotoBook; 
class QLabel; 
class ST_PHOTOTEMPLATES_EXPORT STPhotoBookInfoWidget : public QWidget
{
	Q_OBJECT
	const STPhotoBook* MPhotoBook; 
	QLabel* InfoLabel; 

public:
	STPhotoBookInfoWidget(QWidget *parent = 0);
	void setPhotoBook(const STPhotoBook* _PhotoBook);
public slots:
	void updateInfo(); 
};

#endif
