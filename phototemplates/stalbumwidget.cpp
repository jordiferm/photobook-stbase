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

#include "stalbumwidget.h"
#include <QLabel>
#include <QLayout>
#include <QStackedLayout>
#include <QApplication>
#include <QToolButton> 
#include <QPaintEvent> 

#include "stalbumpagegraphicsview.h"
#include "stclickablelabel.h"

#include "sttemplatescene.h"
#include "stgraphicsphotoitem.h"
#include "stgraphicssimpletextitem.h"
#include "stgraphicspageitem.h"
#include "stgraphicstextitem.h"
#include "stphotobook.h"


QToolButton* STAlbumWidget::createActionButton(QWidget* _Parent, const QString& _IconFile, const QString& _Text)
{
	QToolButton* ButRes = new QToolButton(_Parent); 
	ButRes->setIcon(QIcon(_IconFile));
	ButRes->setIconSize(QSize(48, 48));
	ButRes->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ButRes->setMaximumWidth(100);
	ButRes->setText(_Text);
	return ButRes;
}


void STAlbumWidget::adjustViewToScene(STAlbumPageGraphicsView* _View, QGraphicsScene* _Scene)
{
	//TODO: Mirar qu� collons passa !
	QRectF FitRect = _Scene->sceneRect(); 
	//FitRect.setSize(QSizeF(FitRect.width() / 22, FitRect.height() / 22));
	_View->fitInView( FitRect, Qt::KeepAspectRatio);
}

STAlbumWidget::STAlbumWidget(QWidget* _Parent)
 : QWidget(_Parent), PhotoBook(0) /*CurrentPhotoW(0),*/ /*CurrentPageW(0),*/
{
	//setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	QVBoxLayout* MainLayout = new QVBoxLayout(this);
	MainLayout->setMargin(1);
	StkLayout = new QStackedLayout;
	MainLayout->addLayout(StkLayout);

 	QPalette Pal = palette();
 	Pal.setColor(QPalette::Background, QColor("#2E2E2E"));
 	setPalette(Pal);
 	setAutoFillBackground(true);
	//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//setFixedSize(825, 620);
}


void STAlbumWidget::clear()
{
	QList<STAlbumPageGraphicsView*> PageList = findChildren<STAlbumPageGraphicsView*>();
	QList<STAlbumPageGraphicsView*>::iterator it;

	for (it = PageList.begin(); it != PageList.end(); ++it)
	{
		StkLayout->removeWidget(*it);
		delete (*it);
	}
}

void STAlbumWidget::setPhotoBook(const STPhotoBook* _PhotoBook)
{
	PhotoBook = _PhotoBook;
	clear();
	STPhotoBook::TPagesList	Pages = _PhotoBook->pages(); 
	STPhotoBook::TPagesList::iterator it; 
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		STAlbumPageGraphicsView* NAlbumPage = new STAlbumPageGraphicsView(*it, this);
		if (it == Pages.begin())
			NAlbumPage->setMarginRects( _PhotoBook->coverMarginRects());
		else 
		{
			NAlbumPage->setMarginRects( _PhotoBook->pageMarginRects());
			NAlbumPage->setDrawTwoPagesEffect(_PhotoBook->photoBookTemplate().numPages() > 1);
		}
		//adjustViewToScene(NAlbumPage, *it);		
		StkLayout->addWidget(NAlbumPage);
	}
	firstPage();
	emit photoBookInfoChanged();
}

/*!
  \return _Scene index or -1 if _Scene isn't here.
 */
int STAlbumWidget::sceneIndex(QGraphicsScene* _Scene)
{
	int Res = -1;
	int CurrentIndex = 0;
	bool Found = false;
	while (CurrentIndex < StkLayout->count() && !Found)
	{
		Found = qobject_cast<STAlbumPageGraphicsView*>(StkLayout->widget(CurrentIndex))->scene() == _Scene;
		if (!Found)
			CurrentIndex ++;
	}
	if (Found)
		Res = CurrentIndex;
	return Res;
}

void STAlbumWidget::setCurrentPage(int _PageIndex)
{
	int OldPageIndex = StkLayout->currentIndex(); 
	StkLayout->setCurrentIndex(_PageIndex);
	adjustViewToScene(currentPage(), currentScene());		
	if (currentScene())
	{
		if (currentScene()->modifyAllFrames())
			currentScene()->selectAllByType(STGraphicsPhotoItem::Type);
		else
		{
			if (currentScene()->hasEmptyPhotoItems())
				currentScene()->selectFirstEmptyPhotoItem();
		}
	}
	QApplication::processEvents();
	emit pageChanged(OldPageIndex, _PageIndex);
}


bool STAlbumWidget::showingCover() const
{
	return StkLayout->currentIndex() == 0;
}

int STAlbumWidget::numPages() const
{
	return StkLayout->count();
}

int STAlbumWidget::currentPageIndex()
{
	return StkLayout->currentIndex();
}


//! \returns The Current page or null if no page is showed.
STAlbumPageGraphicsView* STAlbumWidget::currentPage() const
{
	STAlbumPageGraphicsView* Res = 0;
	int CIndex = StkLayout->currentIndex();
	if (CIndex >= 0 && CIndex < StkLayout->count() )	
		Res = qobject_cast<STAlbumPageGraphicsView*>(StkLayout->widget(CIndex));
	return Res;
}

STTemplateScene* STAlbumWidget::currentScene() const 
{
	STTemplateScene* Res = 0; 
	STAlbumPageGraphicsView* CPage = currentPage();
	if (CPage) //Defensive
		Res = qobject_cast<STTemplateScene*>(CPage->scene());
	return Res;
}

QGraphicsItem* STAlbumWidget::currentItem() const
{
	QGraphicsItem* Res = 0; 
	if (STTemplateScene* CScene = currentScene())
		Res = CScene->currentItem();
	return Res;
}

//! Returns STGraphicsPhotoItem if currentItem is STGraphicsPhotoItem otherwise returns false.
STGraphicsPhotoItem* STAlbumWidget::currentPhotoItem() const
{
	STGraphicsPhotoItem* Res = 0;
	QGraphicsItem* CItem = currentItem();
	if (CItem)
	{
		Res = qgraphicsitem_cast<STGraphicsPhotoItem*>(CItem);
		if (!Res)
			Res = qgraphicsitem_cast<STGraphicsPageItem*>(CItem);
	}
	return Res;
}

STGraphicsSimpleTextItem* STAlbumWidget::currentTextItem() const
{
	STGraphicsSimpleTextItem* Res = 0;
	QGraphicsItem* CItem = currentItem();
	if (CItem)
		Res = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem);
	return Res;
}

STGraphicsPageItem* STAlbumWidget::currentPageItem() const
{
	STGraphicsPageItem* Res = 0; 
	if (STTemplateScene* Scene = currentScene())
		Res = Scene->pageItem();
	return Res;
}

QString STAlbumWidget::currentPageInfo()
{
	QString Res;
	if (PhotoBook->photoBookTemplate().hasFirstPages())
	{
		if ( showingCover() )
			Res = tr("Model cover");
		else
			Res = QString(tr("Sheet %1 of %2 ")).arg(StkLayout->currentIndex()).arg(StkLayout->count() - 1);
	}
	else
		Res = QString(tr("Sheet %1 of %2 ")).arg(StkLayout->currentIndex() + 1).arg(StkLayout->count());
	return Res;
}

QString STAlbumWidget::pageBoundariesInfo()
{
	return tr("From %1 to %2 Pages").arg(PhotoBook->minPages()).arg(PhotoBook->maxPages());
}

QString STAlbumWidget::templateInfo()
{
	QString Res;
	if (PhotoBook)
		Res = QString("<a href=\"%1\">%2</a>").arg(PhotoBook->photoBookTemplate().infoUrl()).arg(PhotoBook->photoBookTemplate().description());
	return Res;
}


void STAlbumWidget::firstPage()
{
	if (StkLayout->count() > 0)  //Defensiva
	{
		setCurrentPage(0);
	}
}

void STAlbumWidget::nextPage()
{
	int CIndex = StkLayout->currentIndex();
	if ( CIndex <  StkLayout->count() - 1)
	{
		setCurrentPage(CIndex + 1);
	}
}


void STAlbumWidget::previousPage()
{
	int CIndex = StkLayout->currentIndex();
	if (CIndex > 0)	
		setCurrentPage(CIndex - 1);
}

void STAlbumWidget::lastPage()
{
	if (StkLayout->count() > 0)  //Defensiva
	{
		setCurrentPage(StkLayout->count() - 1);
	}
}

void STAlbumWidget::showPage(int _Page)
{
	if (_Page >=0 && _Page < StkLayout->count())
		setCurrentPage(_Page);
}


void STAlbumWidget::selectCurrentPageBackground()
{
	if (STTemplateScene* CScene = currentScene())
	{
		CScene->clearSelection();
		CScene->pageItem()->setSelected(true); 
	}
}

