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

#ifndef SPTOOLBARLISTVIEW_H
#define SPTOOLBARLISTVIEW_H

#include <QWidget>
#include "stphototemplatesexport.h"

class QToolBar;
class STThumbnailView;
class QAbstractItemModel;
class QLabel;
class ST_PHOTOTEMPLATES_EXPORT SPToolbarListView : public QWidget
{
	Q_OBJECT

	QToolBar* ToolBar;
	STThumbnailView* ListView;
	QLabel* ItemCounterLabel;
	QString ItemCounterName;

public:
	SPToolbarListView(Qt::Orientation _Orientation, QWidget* _Parent = 0, bool _ToolBarOnTop = true);
	virtual void setModel(QAbstractItemModel* _Model);
	QToolBar* toolBar() { return ToolBar; }
	STThumbnailView* listView();
	void activateItemCounter(const QString& _ItemName);

private slots:
	void updateItemCounter();

};

#endif // SPTOOLBARLISTVIEW_H
