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

#ifndef STBUSYINFOWIDGET_H
#define STBUSYINFOWIDGET_H

#include <QWidget>
#include "stwidgetsexport.h"

class QLabel;
class QPoint;
class ST_WIDGETS_EXPORT STBusyInfoWidget : public QWidget
{
	QLabel* InfoLabel;
	static STBusyInfoWidget* StaticMe;
	QPoint InitPos;

public:
	STBusyInfoWidget(QWidget* _Parent = 0);
	void setText(const QString& _Text);
	void show();
	static void show(const QString& _Text);
	static void hide();
};

#endif // STBUSYINFOWIDGET_H
