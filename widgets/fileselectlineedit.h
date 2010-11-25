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
#ifndef FILESELECTLINEEDIT_H
#define FILESELECTLINEEDIT_H
#include <QWidget>
#include "stwidgetsexport.h"

/**
Widget with a line edit and a tool button that launches an open file dialog. This widget is ideal for data forms that have fields with path values.

	@author
*/
class QLineEdit;
class ST_WIDGETS_EXPORT FileSelectLineEdit : public QWidget
{
	Q_OBJECT 
	Q_PROPERTY(QString text READ text WRITE setText USER true)			
			
public:
	enum EnOpenMode
	{
		GetExistingDirectory, 
		GetOpenFileName, 
		GetSaveFileName
	};

private:
	EnOpenMode OpenMode;
	QLineEdit* LEFile;
	QString Filter;
	QString InitDir;

public:
	FileSelectLineEdit(EnOpenMode _OpenMode, QWidget* _Parent = 0);
	void setFilter(const QString& _Filter);
	void setInitDir(const QString& _Dir);
	void setText(const QString& _Text); 
	QString text() const; 
	QLineEdit* lineEdit() { return LEFile; }

public slots:
	void getFile();	
};

#endif
