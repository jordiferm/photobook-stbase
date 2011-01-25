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
#ifndef OEDITORBOTTOMACTIONSWIDGET_H
#define OEDITORBOTTOMACTIONSWIDGET_H

#include <QFrame>
#include "storomelexport.h"

/**
Actions located to bottom of image editor.

	@author
*/
class QUndoStack;
class QUndoView;
class QComboBox; 
class ST_OROMEL_EXPORT OEditorBottomActionsWidget : public QFrame
{
	Q_OBJECT
	QUndoStack* UndoStack;
	QUndoView* MUndoView;
	QComboBox* ZoomCombo; 
	bool ResettingZoom;

public:
	OEditorBottomActionsWidget(QUndoStack* _UndoStack, QWidget *parent = 0);
	~OEditorBottomActionsWidget();
	void resetZoom(); 
	void setZoomValue(double _Val);
	
public slots:
	void showUndoView();
	void zoomComboChanged(int );
	void zoomMag();
	void zoomMin(); 
 
signals:
	void zoomValueChanged(double ); 
	void zoomFitToWindow(); 
	void zoomFitToSelection();
};

#endif
