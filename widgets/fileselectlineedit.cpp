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
#include "fileselectlineedit.h"
#include <QLayout> 
#include <QLineEdit> 
#include <QToolButton> 
#include <QFileDialog>
#include "siconfactory.h" 

FileSelectLineEdit::FileSelectLineEdit(EnOpenMode _OpenMode, QWidget* _Parent) : QWidget(_Parent), OpenMode(_OpenMode)
{
	QHBoxLayout* MainLayout = new QHBoxLayout(this); 

	LEFile = new QLineEdit(this); 
	MainLayout->addWidget(LEFile); 
	MainLayout->setSpacing(1);
	MainLayout->setMargin(0);

	QToolButton* ButSelect = new QToolButton(this); 
	ButSelect->setIcon(SIconFactory::defaultFactory()->icon(SIconFactory::FileOpen));
	ButSelect->setIconSize(QSize(22,22));
	MainLayout->addWidget(ButSelect);
	connect(ButSelect, SIGNAL(clicked()), this, SLOT(getFile()));
	setFilter("*.*");
	setText("");
	setInitDir("~/");
}


void FileSelectLineEdit::setFilter(const QString& _Filter)
{
	Filter = _Filter;
}

void FileSelectLineEdit::setInitDir(const QString& _Dir)
{
	InitDir = _Dir; 
}

void FileSelectLineEdit::setText(const QString& _Text)
{
	LEFile->setText(_Text); 
}

QString FileSelectLineEdit::text() const
{
	return LEFile->text();
}


void FileSelectLineEdit::getFile()
{
	QString FileName = "";
	switch(OpenMode)
	{
		case GetExistingDirectory:
			FileName = QFileDialog::getExistingDirectory(this, tr("Select folder"), InitDir);		
		break;
		case GetOpenFileName:
			FileName = QFileDialog::getOpenFileName(this, tr("Select file"), InitDir, Filter) ;		
		break;
		case GetSaveFileName:
			FileName = QFileDialog::getSaveFileName(this, tr("Select file"), InitDir, Filter) ;		
		break;
	}
	if (FileName != "")
	{
		setText(FileName);
		LEFile->setFocus();
	}
}
