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
#ifndef STSIZEANDPOSDIALOG_H
#define STSIZEANDPOSDIALOG_H

#include <QDialog>
#include "stwidgetsexport.h"

/**
Dialog to specify position and size.

	@author
*/
class QDoubleSpinBox;
class QLabel;
class ST_WIDGETS_EXPORT STSizeAndPosDialog : public QDialog
{
Q_OBJECT
	QDoubleSpinBox* SPValueX;
	QDoubleSpinBox* SPValueY;
	QDoubleSpinBox* SPValueWidth;
	QDoubleSpinBox* SPValueHeight;
	QLabel* LabelX;
	QLabel* LabelY;
	QLabel* LabelWidth;
	QLabel* LabelHeight;

public:
	STSizeAndPosDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	void setSize(const QSizeF& _Size); 
	QSizeF size() const; 
	void setPosition(const QPointF& _Position); 
	QPointF position() const; 
	void setPosVisible(bool _Visible);
	void setSizeVisible(bool _Visible);
};

#endif
