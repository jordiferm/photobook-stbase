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
#ifndef ADDREMOVEWIDGET_H
#define ADDREMOVEWIDGET_H

#include <QWidget>
#include "sttpopslexport.h"

/**
	Wiget with 2 toolButtons, one for add and on for remove.
	@author
*/
class QToolButton; 
class ST_TPOPSL_EXPORT AddRemoveWidget : public QWidget
{
	Q_OBJECT
	QToolButton* AddItemsBut;
	QToolButton* RemoveItemsBut; 
	
public:
	AddRemoveWidget(QWidget* parent = 0, Qt::Orientation _Orientation = Qt::Vertical);
	~AddRemoveWidget();
	void setAddLabel(const QString& _Label); 
	void setRemoveLabel(const QString& _Label); 

signals: 
	void addClicked(); 
	void removeClicked(); 
};

#endif
