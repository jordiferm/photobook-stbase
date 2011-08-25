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

#include "documentviewwidget.h"
#include <QLabel>
#include <QLayout>
#include <QStackedLayout>
#include <QApplication>
#include <QToolButton> 
#include <QPaintEvent> 
#include <QUrl>

#include "documentpageview.h"
#include "stclickablelabel.h"
#include "graphicsphotoitem.h"
#include "graphicspageitem.h"

#include "document.h"

using namespace SPhotoBook;

QToolButton* DocumentViewWidget::createActionButton(QWidget* _Parent, const QString& _IconFile, const QString& _Text)
{
	QToolButton* ButRes = new QToolButton(_Parent); 
	ButRes->setIcon(QIcon(_IconFile));
	ButRes->setIconSize(QSize(48, 48));
	ButRes->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ButRes->setMaximumWidth(100);
	ButRes->setText(_Text);
	return ButRes;
}


void DocumentViewWidget::adjustViewToScene(DocumentPageView* _View, QGraphicsScene* _Scene)
{
	//TODO: See what a hell is going on here !
	QRectF FitRect = _Scene->sceneRect(); 
	//FitRect.setSize(QSizeF(FitRect.width() / 22, FitRect.height() / 22));
	_View->fitInView( FitRect, Qt::KeepAspectRatio);
}

DocumentViewWidget::DocumentViewWidget(QWidget* _Parent)
 : QWidget(_Parent), PBDocument(0) /*CurrentPhotoW(0),*/ /*CurrentPageW(0),*/
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


void DocumentViewWidget::clear()
{
	QList<DocumentPageView*> PageList = findChildren<DocumentPageView*>();
	QList<DocumentPageView*>::iterator it;

	for (it = PageList.begin(); it != PageList.end(); ++it)
	{
		StkLayout->removeWidget(*it);
		delete (*it);
	}
}

void DocumentViewWidget::reloadPages()
{
	clear();
	PageList Pages = PBDocument->pages();
	PageList::iterator it;
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		DocumentPageView* NAlbumPage = new DocumentPageView(*it, this);
		if (it == Pages.begin())
			NAlbumPage->setMarginRects(PBDocument->metaInfo().coverMarginRects());
		else
		{
			NAlbumPage->setMarginRects(PBDocument->metaInfo().coverMarginRects());
			//NAlbumPage->setDrawTwoPagesEffect(_PBDocument->PBDocumentTemplate().numPages() > 1);
		}
		StkLayout->addWidget(NAlbumPage);
	}
}

void DocumentViewWidget::setPBDocument(const Document* _PBDocument)
{
	PBDocument = _PBDocument;
	reloadPages();
	firstPage();
	emit photoBookInfoChanged();
}


/*!
  \return _Scene index or -1 if _Scene isn't here.
 */
int DocumentViewWidget::sceneIndex(QGraphicsScene* _Scene)
{
	int Res = -1;
	int CurrentIndex = 0;
	bool Found = false;
	while (CurrentIndex < StkLayout->count() && !Found)
	{
		Found = qobject_cast<DocumentPageView*>(StkLayout->widget(CurrentIndex))->scene() == _Scene;
		if (!Found)
			CurrentIndex ++;
	}
	if (Found)
		Res = CurrentIndex;
	return Res;
}

void DocumentViewWidget::setCurrentPage(int _PageIndex)
{
	int OldPageIndex = StkLayout->currentIndex(); 
	StkLayout->setCurrentIndex(_PageIndex);
	adjustViewToScene(currentPage(), currentScene());		
	if (currentScene())
	{
		if (currentScene()->modifyAllFrames())
			currentScene()->selectAllByType(GraphicsPhotoItem::Type);
		else
		{
			if (currentScene()->hasEmptyPhotoItems())
				currentScene()->selectFirstEmptyPhotoItem();
		}
	}
	QApplication::processEvents();
	emit pageChanged(OldPageIndex, _PageIndex);
}


bool DocumentViewWidget::showingCover() const
{
	return StkLayout->currentIndex() == 0;
}

int DocumentViewWidget::numPages() const
{
	return StkLayout->count();
}

int DocumentViewWidget::currentPageIndex()
{
	return StkLayout->currentIndex();
}


//! \returns The Current page or null if no page is showed.
DocumentPageView* DocumentViewWidget::currentPage() const
{
	DocumentPageView* Res = 0;
	int CIndex = StkLayout->currentIndex();
	if (CIndex >= 0 && CIndex < StkLayout->count() )	
		Res = qobject_cast<DocumentPageView*>(StkLayout->widget(CIndex));
	return Res;
}

TemplateScene* DocumentViewWidget::currentScene() const
{
	TemplateScene* Res = 0;
	DocumentPageView* CPage = currentPage();
	if (CPage) //Defensive
		Res = qobject_cast<TemplateScene*>(CPage->scene());
	return Res;
}

QGraphicsItem* DocumentViewWidget::currentItem() const
{
	QGraphicsItem* Res = 0; 
	if (TemplateScene* CScene = currentScene())
		Res = CScene->currentItem();
	return Res;
}

//! Returns GraphicsPhotoItem if currentItem is GraphicsPhotoItem otherwise returns false.
GraphicsPhotoItem* DocumentViewWidget::currentPhotoItem() const
{
	GraphicsPhotoItem* Res = 0;
	QGraphicsItem* CItem = currentItem();
	if (CItem)
	{
		Res = qgraphicsitem_cast<GraphicsPhotoItem*>(CItem);
		if (!Res)
			Res = qgraphicsitem_cast<GraphicsPageItem*>(CItem);
	}
	return Res;
}

GraphicsPageItem* DocumentViewWidget::currentPageItem() const
{
	GraphicsPageItem* Res = 0;
	if (TemplateScene* Scene = currentScene())
		Res = Scene->pageItem();
	return Res;
}

QString DocumentViewWidget::currentPageInfo()
{
	QString Res;
	if (!PBDocument)
		return Res;
	if (!PBDocument->covers().isEmpty())
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

QString DocumentViewWidget::pageBoundariesInfo()
{
	return tr("From %1 to %2 Pages").arg(PBDocument->metaInfo().minPages()).arg(PBDocument->metaInfo().maxPages());
}

QString DocumentViewWidget::templateInfo()
{
	QString Res;
	if (PBDocument)
	{
		QSizeF DocSize = PBDocument->metaInfo().size();
		QString TemplateDescription = QString("%1 (%2x%3 mm)").arg(PBDocument->metaInfo().name()).arg(
					DocSize.width()).arg(DocSize.height());
		Res = QString("<a href=\"%1\">%2</a>").arg(PBDocument->metaInfo().infoUrl().toString()).arg(TemplateDescription);
	}
	return Res;
}


void DocumentViewWidget::firstPage()
{
	if (StkLayout->count() > 0)  //Defensiva
	{
		setCurrentPage(0);
	}
}

void DocumentViewWidget::nextPage()
{
	int CIndex = StkLayout->currentIndex();
	if ( CIndex <  StkLayout->count() - 1)
	{
		setCurrentPage(CIndex + 1);
	}
}


void DocumentViewWidget::previousPage()
{
	int CIndex = StkLayout->currentIndex();
	if (CIndex > 0)	
		setCurrentPage(CIndex - 1);
}

void DocumentViewWidget::lastPage()
{
	if (StkLayout->count() > 0)  //Defensiva
	{
		setCurrentPage(StkLayout->count() - 1);
	}
}

void DocumentViewWidget::showPage(int _Page)
{
	if (_Page >=0 && _Page < StkLayout->count())
		setCurrentPage(_Page);
}


void DocumentViewWidget::selectCurrentPageBackground()
{
	if (TemplateScene* CScene = currentScene())
	{
		CScene->clearSelection();
		CScene->pageItem()->setSelected(true); 
	}
}

