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
#ifndef SMAPPABLELABEL_H
#define SMAPPABLELABEL_H

#include <QLabel>
#include "stwidgetsexport.h"


/**
QLabel with text property suitable for QDataWidgetMapper.

	@author
*/
class ST_WIDGETS_EXPORT SMappableLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText USER true)

public:
	SMappableLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0);
	SMappableLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~SMappableLabel();

};

#endif
