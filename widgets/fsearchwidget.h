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
#ifndef FSEARCHWIDGET_H
#define FSEARCHWIDGET_H

#include <QDialog>
#include <QModelIndex>
#include <QLineEdit>
#include "stwidgetsexport.h"

class ST_WIDGETS_EXPORT QFSLineEdit : public QLineEdit 
{
Q_OBJECT 
public: 
	QFSLineEdit(QWidget* _Parent = 0);

protected: 
	void keyPressEvent(QKeyEvent * _Event);

signals: 
	void keyUp(); 
	void keyDown(); 
	void pageUp(); 
	void pageDown(); 
	void keyRight(); 
	void keyLeft(); 
};

/**
Dialeg per a cerques basades en model/View
Pot servir com a classe base de molts tipus de cerca.

@author Shadow
*/
class QAbstractItemModel;
class QAbstractItemView; 
class QLineEdit;
class QLabel;
class QFrame;
class ST_WIDGETS_EXPORT FSearchWidget : public QWidget
{
Q_OBJECT


private: 
	QAbstractItemModel* Model;
	QAbstractItemView* View;
	QModelIndex SearchStart;
	int SearchRole; 
	int SearchHits;
	Qt::MatchFlags SearchMatchFlags;
	QLabel* LabSearch;
	QFrame* LEditFrame;

protected:
	QLineEdit* LEditSearch; 
		
private: 
	void createControls(QAbstractItemView* _View = 0);

public:
	FSearchWidget(QAbstractTableModel* _Model, QWidget* _Parent = 0);
	FSearchWidget(QAbstractItemView* _View, QAbstractItemModel* _Model, QWidget* _Parent = 0);
	void setSearchDefaults();
	void setSearchStart(const QModelIndex& _Start);
	QModelIndex searchStart() const;
	void setSearchRole(int _Role);
	int searchRole() const;
	void setSearchHits(int _Hits);
	int searchHits() const;
	void setSearchMatchFlags(Qt::MatchFlags _Flags);
	Qt::MatchFlags searchMatchFlags() const;
	void setSearchLabel(const QString& _Label);
	void setEditable(bool _Editable);
	bool editable() const;

	QAbstractItemView* view() const {return View;}
	QAbstractItemModel* model() const {return Model;}
	QLineEdit* lineEditSearch() const {return LEditSearch;}
	QFrame* searchFrame() const {return LEditFrame;}

protected:
	void showEvent( QShowEvent * event );

public slots: 
	virtual void search(const QString& _SearchText);
	void showSearchInput(bool _Value); 

protected slots: 
	virtual void scrollDown();
	virtual void scrollUp();
	virtual void selectFirst();
	virtual void selectNext();
	virtual void selectPrevious();
	virtual void nextSearchColumn();
	virtual void previousSearchColumn();
};

#endif
