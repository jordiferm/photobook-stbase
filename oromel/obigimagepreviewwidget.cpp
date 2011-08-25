/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "obigimagepreviewwidget.h"
#include <QLayout>
#include <QLabel>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <QDir>
#include <QDebug>
#include <QApplication>

#include "stthumbnailview.h"
#include "siconfactory.h"
#include "ddocmodel.h"
#include "stimagetools.h"
#include "dimagedoc.h"
#include "stimage.h"
#include "doccheckedproxymodel.h"


void OBigImagePreviewWidget::updateIncludeState()
{
	QModelIndex CurrentIndex = LVImages->currentIndex();
	STDom::DDoc* Doc =  doc(CurrentIndex);
	if (Doc) // Defensive
	{
		if (CheckedDocModel->isChecked(CurrentIndex))
		{
			TBInclude->setText(tr("Exclude"));
			TBInclude->setIcon(stIcon(SIconFactory::EditDelete));
		}
		else
		{
			TBInclude->setText(tr("Include"));
			TBInclude->setIcon(stIcon(SIconFactory::EditAdd));
		}
	}
}

void OBigImagePreviewWidget::setCurrentIndex(const QModelIndex& _Index)
{
	LVImages->setCurrentIndex(_Index);
	LVImages->scrollTo(_Index, QAbstractItemView::PositionAtCenter);
	slotShowImage(_Index);
}

OBigImagePreviewWidget::OBigImagePreviewWidget(QWidget* _Parent) : QWidget(_Parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setSpacing(0);
	MLayout->setMargin(1);
	QToolBar* MainToolBar = new QToolBar(this);
	MainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	MLayout->addWidget(MainToolBar);

	QAction* BackAction = new QAction(QIcon(":/st/oromel/back.png"), tr("Back"), this);
	BackAction->setShortcut(Qt::Key_Escape);
	connect(BackAction, SIGNAL(triggered()), this, SIGNAL(closeRequest()));
	MainToolBar->addAction(BackAction);

	QAction* IncludeAction = new QAction(QIcon(":/st/oromel/include.png"), tr("Include"), this);
	IncludeAction->setShortcut(Qt::Key_Down);
	connect(IncludeAction, SIGNAL(triggered()), this, SLOT(slotIncludeCurrentImage()));
	MainToolBar->addAction(IncludeAction);

	QHBoxLayout* IconsLayout = new QHBoxLayout;
	MLayout->addLayout(IconsLayout);

	QToolButton* TBRewind = new QToolButton(this);
	TBRewind->setShortcut(Qt::Key_Left);
	TBRewind->setIcon(stIcon(SIconFactory::Rew));
	connect(TBRewind, SIGNAL(clicked()), this, SLOT(slotGoBackward()));
	IconsLayout->addWidget(TBRewind);

	LVImages = new STThumbnailView(this);
	LVImages->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	LVImages->setViewMode(QListView::IconMode);
	LVImages->setWrapping(false);
	LVImages->setMinimumHeight(125);
	LVImages->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(LVImages, SIGNAL(clicked(QModelIndex)), this, SLOT(slotShowImage(QModelIndex)));
	IconsLayout->addWidget(LVImages);

	DocModel = new STDom::DDocModel(this);
	CheckedDocModel = new SPhotoBook::CheckedProxyModel(this, QImage(":/photobook/checked.png"));
	CheckedDocModel->setSourceModel(DocModel);
	CheckedDocModel->setThumbnailSize(QSize(80, 80));
	LVImages->setModel(CheckedDocModel);

	QToolButton* TBForward = new QToolButton(this);
	TBForward->setShortcut(Qt::Key_Right);
	TBForward->setIcon(stIcon(SIconFactory::Fwd));
	connect(TBForward, SIGNAL(clicked()), this, SLOT(slotGoForward()));
	IconsLayout->addWidget(TBForward);

	ImageLabel = new QLabel(this);
	ImageLabel->setAlignment(Qt::AlignCenter);
	QGridLayout* LabelLayout = new QGridLayout;
	MLayout->addLayout(LabelLayout);
	LabelLayout->addWidget(ImageLabel, 0, 0);

	TBInclude = new QToolButton(this);
	TBInclude->setIconSize(QSize(64, 64));
	TBInclude->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	connect(TBInclude, SIGNAL(clicked()), this, SLOT(slotIncludeCurrentImage()));
	TBInclude->setText(tr("Include"));
	LabelLayout->addWidget(TBInclude, 0, 0);
	LabelLayout->setAlignment(TBInclude, Qt::AlignHCenter | Qt::AlignBottom);

}

void OBigImagePreviewWidget::setLabel(const QString& _Label)
{

}

void OBigImagePreviewWidget::clear()
{
	ImageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	ImageLabel->clear();
}

void OBigImagePreviewWidget::setDir(const QDir& _Dir)
{
	DocModel->clear();
	DocModel->setDocs(STImageTools::recursiveImagesEntryInfoList(_Dir.absolutePath(), false));
	setLabel(_Dir.dirName());
	if (DocModel->rowCount() > 0)
		setCurrentIndex(CheckedDocModel->index(0, 0));
}

STDom::DDoc* OBigImagePreviewWidget::doc(const QModelIndex& _Index)
{
	STDom::DDoc* Res = 0;
	if (_Index.isValid())
		Res = DocModel->doc(CheckedDocModel->mapToSource(_Index));
	return Res;
}

void OBigImagePreviewWidget::slotShowImage(const QModelIndex& _Index)
{
	STDom::DDoc* Doc =  doc(_Index);
	if (Doc) // Defensive
	{
		if (Doc->type() == STDom::DImageDoc::Type) //Defensive
		{
			STImage Image;
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			if (Image.load(Doc->fileInfo().absoluteFilePath()))
			{
				ImageLabel->setPixmap(QPixmap::fromImage(Image.scaled(ImageLabel->size(), Qt::KeepAspectRatio)));
				ImageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
				updateIncludeState();
			}
			QApplication::restoreOverrideCursor();
		}
	}
}

void OBigImagePreviewWidget::slotIncludeCurrentImage()
{
	QModelIndex CurrentIndex = LVImages->currentIndex();
	STDom::DDoc* Doc =  doc(CurrentIndex);
	if (Doc) // Defensive
	{
		if (CheckedDocModel->isChecked(CurrentIndex))
		{
			CheckedDocModel->setChecked(CurrentIndex, false);
			emit excludeImage(Doc->fileInfo());
		}
		else
		{
			CheckedDocModel->setChecked(CurrentIndex, true);
			emit includeImage(Doc->fileInfo());
		}
		updateIncludeState();
	}
}

void OBigImagePreviewWidget::slotGoForward()
{
	QModelIndex CurrentIndex = LVImages->currentIndex();
	if (CurrentIndex.row() + 1 < CheckedDocModel->rowCount())
		setCurrentIndex(CheckedDocModel->index(CurrentIndex.row() + 1, CurrentIndex.column()));
}

void OBigImagePreviewWidget::slotGoBackward()
{
	QModelIndex CurrentIndex = LVImages->currentIndex();
	if (CurrentIndex.row() > 0)
		setCurrentIndex(CheckedDocModel->index(CurrentIndex.row() - 1, CurrentIndex.column()));
}
