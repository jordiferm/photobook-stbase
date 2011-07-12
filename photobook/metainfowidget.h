/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
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
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef METAINFOWIDGET_H
#define METAINFOWIDGET_H
#include "stphotobookexport.h"

#include <QWidget>

class QToolBar;
namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT MetaInfoWidget : public QWidget
{
    Q_OBJECT

	QToolBar* ToolBar;

	void createActions();

public:
    explicit MetaInfoWidget(QWidget *parent = 0);

signals:

public slots:

};
}

#endif // METAINFOWIDGET_H
