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
#ifndef ODIGIKAMTOOLSWIDGET_H
#define ODIGIKAMTOOLSWIDGET_H

#include <QWidget>
#include "editortool.h"
#include "storomelexport.h"

/**
Widget to handle digikam tools.

	@author
*/
class QHBoxLayout;
class QToolBar;
class QLabel;
class ST_OROMEL_EXPORT ODigikamToolsWidget : public QWidget
{
	Q_OBJECT

	QHBoxLayout* MLayout;
	QLabel* MLabel;
	Digikam::EditorTool* CurrTool;

public:
	ODigikamToolsWidget(const QString& _HelpText, QWidget *parent = 0);
	~ODigikamToolsWidget();
	virtual void setupActions(QToolBar* _ToolBar) = 0;
	void unloadTool(Digikam::EditorTool* _Tool);
	void loadTool(Digikam::EditorTool* _Tool);

private slots: 
	void closeCurrentTool();
	void slotOkClicked();

signals: 
	void toolActivated(Digikam::EditorTool* _Tool); 
	void toolClosed(Digikam::EditorTool* _Tool); 
	void imageChanged(const QString& _Changes); 

};

#endif
