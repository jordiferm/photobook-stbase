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
#ifndef OEDITORACTIONSWIDGET_H
#define OEDITORACTIONSWIDGET_H

#include <QWidget>
#include "storomelexport.h"

/**
Actions for image editor.

	@author
*/
class OPhotoCollectionImageModel; 
class OImageNavigatorWidget; 
class QModelIndex; 
class QUndoStack; 
class QAction;
class ST_OROMEL_EXPORT OEditorActionsWidget : public QWidget
{
	Q_OBJECT
			
	OImageNavigatorWidget* ImageNavWidget;
	QAction* SaveAct;
	QAction* OriginalAct;
	QAction* AcceptAct;
	
public:
	OEditorActionsWidget(QUndoStack* _UndoStack, QWidget *parent = 0);
	~OEditorActionsWidget();
	OImageNavigatorWidget* imageNavigator() { return ImageNavWidget; }
	void init();
	void setOriginalEnabled(bool _Enabled);
	void setOriginalVisible(bool _Visible); 
	void setAcceptVisible(bool _Visible);
	
signals: 
	void saveClicked();
	void saveAsClicked();
	void openClicked(); 
	void revertOriginalClicked();
	void backClicked();
	void histogramClicked(); 
	void acceptClicked();

public slots:
	void disableOriginalAct();
	void undoIndexChanged(int _Index);
};

#endif
