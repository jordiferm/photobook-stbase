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
#include "stfolderselector.h"
#include <QLayout> 
#include <QDirModel> 
#include <QTreeView> 
#include <QDialogButtonBox> 
#include <QPushButton> 

#include "siconfactory.h" 

//_____________________________________________________________________________
//
// Class STFolderSelector
//_____________________________________________________________________________

STFSIconProvider::STFSIconProvider()
{}
	
QIcon STFSIconProvider::icon(IconType _Type) const
{
	return QIcon(":/st/folder.png"); 
}

QIcon STFSIconProvider::icon ( const QFileInfo & info ) const
{
	return QIcon(":/st/folder.png"); 
}

//_____________________________________________________________________________
//
// Class STFSDirModel
//_____________________________________________________________________________


STFSDirModel::STFSDirModel(QObject* _Parent) : QDirModel(_Parent) 
{}

QVariant STFSDirModel::data(const QModelIndex& _Index, int _Role) const
{
	if (_Role == Qt::DecorationRole)
		return iconProvider()->icon(QFileInfo(filePath(_Index))).pixmap(QSize(64, 64));
	else 
		return QDirModel::data(_Index, _Role); 
}


//_____________________________________________________________________________
//
// Class STFolderSelector
//_____________________________________________________________________________

STFolderSelector::STFolderSelector(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	TView = new QTreeView(this); 
	TView->setHeaderHidden(true);

	DirModel = new STFSDirModel(this); 
	IconProvider = new STFSIconProvider; 
	DirModel->setIconProvider(IconProvider); 
	DirModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot ); 
	TView->setModel(DirModel); 
	
	MLayout->addWidget(TView); 

	QDialogButtonBox* Buttons = new QDialogButtonBox(this);

	QPushButton* ButOk = Buttons->addButton(tr("Ok"), QDialogButtonBox::AcceptRole);
	ButOk->setIcon(stIcon(SIconFactory::Accept)); 

	QPushButton* ButCancel = Buttons->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
	ButCancel->setIcon(stIcon(SIconFactory::Cancel)); 
	
	MLayout->addWidget(Buttons);
	connect(Buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(Buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

STFolderSelector::~STFolderSelector()
{
	delete IconProvider; 
}

QSize STFolderSelector::sizeHint () const
{
	return QSize(600, 600); 
}

QString STFolderSelector::selectedDir() const 
{
	return DirModel->filePath(TView->currentIndex()); 
}

void STFolderSelector::setDirectory(const QString& _Dir)
{
	qDebug(_Dir.toLatin1()); 
	TView->setRootIndex(DirModel->index(_Dir));
	for (int Vfor = 1; Vfor < DirModel->columnCount(); Vfor++)
		TView->hideColumn(Vfor); 

	TView->expandToDepth(2); 
}

QString STFolderSelector::getExistingDirectory(QWidget* _Parent, const QString& _Caption, const QString& _Dir)
{
	QString Res; 
	STFolderSelector* Selector = new STFolderSelector(_Parent); 
	Selector->setDirectory(_Dir); 
	if (Selector->exec() == QDialog::Accepted)
		Res = Selector->selectedDir();
	
	delete Selector; 
	return Res;
}


