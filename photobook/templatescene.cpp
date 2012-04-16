/****************************************************************************
**
** Copyright (C) 2006-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** SoftTopia reserves all rights not expressly granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "templatescene.h"
#include <QColor> 
#include <QPixmap>
#include <QProgressBar> 
#include <QApplication> 
#include <QDir>
#include <QGraphicsSceneContextMenuEvent>
#include <QUrl>
#include <QTextDocument>
#include <QTextCursor>
#include <QDebug>

//-- Graphics
#include "updateitemevent.h"
#include "graphicsphotoitem.h"
#include "graphicspageitem.h"
#include "graphicstextitem.h"
#include "abstractgraphicsitem.h"
#include "graphicsclipartitem.h"
#include "graphicsmonthitem.h"
#include "graphicsitemoperation.h"

//-- Resources
#include "resource.h"
#include "resourcemimedata.h"

//-- Drag and Drop
#include "templatescenemimedata.h"

//-- Documents
#include "ddoc.h"

//-- Miscelanea
#include "collectioninfo.h"
#include "stimage.h"
#include "stutils.h"

using namespace SPhotoBook;

qreal TemplateScene::PageZValue = -1000000;

void TemplateScene::init()
{
	setBackgroundBrush(QColor("#DFE1BD"));
	connect(this, SIGNAL(imageDropped(QString,QString)), this, SLOT(slotCheckModifyAll(QString)));
	connect(this, SIGNAL(imageListDropped(QList<QUrl>)), this, SLOT(slotImageListDropped(QList<QUrl>)));
	PageItem = 0;
	ItemsMovable = false;
	ItemsResizable = true;
	ModifyAllFrames = false;
	AutoAdjustFrames = true;
	ExpandImagesToFillFrames = false;
	SnapToBounds = true;
	IgnoreExifRotation = false;
	HasChanges = false;
	FixedOutMargin = 0.0;
}

TemplateScene::TemplateScene(QObject* _Parent)
 : QGraphicsScene(_Parent)
{
	init();
}

TemplateScene::TemplateScene(const QSizeF& _PageSize, QObject* _Parent)
 : QGraphicsScene(_Parent)
{
	init();
	GraphicsPageItem* NewPageItem = new GraphicsPageItem(QRectF(0, 0, _PageSize.width(), _PageSize.height()));
	NewPageItem->setZValue(PageZValue);
	configureItem(NewPageItem);
	addItem(NewPageItem);
	setPageItem(NewPageItem);
}


void TemplateScene::copy(TemplateScene* _Other)
{
	//Make sure photoitems are inserted to PhotoItems list.
	//Use clone method in each item ? from virtual in AbstractGraphicsItem ?
	QDomDocument Doc;
	DataContext = _Other->DataContext;
/*	QString DestImageSourcePath;
	if (_Other->pageItem())
		DestImageSourcePath = _Other->pageItem()->imageSourcePath();*/

	loadElement(_Other->createElement(Doc));
}


QRectF TemplateScene::translatedRectF(const QRectF& _Rect, const QSizeF& _DestSize, const QSizeF& _SourceSize)
{
	qreal XPos = static_cast<int>((_Rect.x() * _DestSize.width()) / _SourceSize.width());
	qreal Width = static_cast<int>((_Rect.width() * _DestSize.width()) / _SourceSize.width());
	qreal YPos = static_cast<int>((_Rect.y() * _DestSize.height()) / _SourceSize.height());
	qreal Height = static_cast<int>((_Rect.height() * _DestSize.height()) / _SourceSize.height());
	return QRectF(XPos, YPos, Width, Height);
}

void TemplateScene::resize(const QSizeF& _NewSize)
{
	QSizeF CurrentSize = PageItem->rect().size();
	setSceneRect(translatedRectF(sceneRect(), _NewSize, CurrentSize));
	PageItem->setRect(sceneRect());
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
		{
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			{
				QRectF ItemRect = CItem->boundingRect();
				QRectF TransRect = translatedRectF(ItemRect, _NewSize, CurrentSize);
				CItem->setRect(TransRect);
			}
			else
			if (QGraphicsItem* CItem = qgraphicsitem_cast<QGraphicsItem*>(*it))
			{
				if (CItem != PageItem)
				{
					QRectF ItemRect(CItem->pos(), CItem->boundingRect().size());
					QRectF TransRect = translatedRectF(ItemRect, _NewSize, CurrentSize);
					CItem->setPos(TransRect.topLeft());
					CItem->scale(TransRect.width() / ItemRect.width(), TransRect.height() / ItemRect.height());
				}
			}
		}
	}
}

/*!
	Replaces the template and reuses current photoItems.	
*/
void TemplateScene::replaceTemplate(TemplateScene* _Scene)
{
	QFileInfoList OldIndexList;
	QList<GraphicsPhotoItem*> OldPhotoItems = photoItems();
	QList<GraphicsPhotoItem*>::iterator it;
	for (it = OldPhotoItems.begin(); it != OldPhotoItems.end(); ++it)
	{
		if ((*it)->hasImage() && (*it)->isVisible())
			OldIndexList.push_back((*it)->imageResource().fileInfo());
	}

	clear();
	copy(_Scene);
	getContextVariables();
	if (PageItem->hasImage())
		PageItem->loadImageSpawn();

	QList<GraphicsPhotoItem*> PhotoItems = photoItems();
	int CntCurrPhoto = 0;
	while (CntCurrPhoto < PhotoItems.count())
	{
		GraphicsPhotoItem* CurrPhotoItem = PhotoItems.at(CntCurrPhoto);
		if (OldIndexList.size() > CntCurrPhoto)
		{
			STDom::DImageDoc CImageDoc(OldIndexList[CntCurrPhoto].absoluteFilePath());
			CurrPhotoItem->setImage(CImageDoc);
			CurrPhotoItem->loadImageSpawn();
		}
		else
		{
			if (ModifyAllFrames && OldIndexList.size() > 0)
			{
				STDom::DImageDoc CImageDoc(OldIndexList[OldIndexList.size() -1].absoluteFilePath());
				CurrPhotoItem->setImage(CImageDoc);
				CurrPhotoItem->loadImageSpawn();
			}
		}
		
		CntCurrPhoto++;
	}
	modified();
}


void TemplateScene::setImageToSelectedItems(const QPixmap& _ThumbNail, const QString& _ImageFileName)
{
	QList<QGraphicsItem *> Items = selectedItems(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it);
		if (!CPhotoItem)
			CPhotoItem = qgraphicsitem_cast<GraphicsPageItem*>(*it);
		if (CPhotoItem)
		{
			CPhotoItem->setThumbnail(_ThumbNail, _ImageFileName);
			CPhotoItem->loadImageSpawn();
			CPhotoItem->update();			
		}
	}
	modified();
}

void TemplateScene::setDummyImages(const QList<QImage>& _ImageList)
{
	if (!_ImageList.isEmpty()) //Defensive 
	{
		QList<QGraphicsItem *> Items = items(); 
		QList<QGraphicsItem *>::iterator it; 
		for (it = Items.begin(); it != Items.end(); ++it)
		{
			if (GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			{
				
				CPhotoItem->setImage(_ImageList[STUtils::random(0, _ImageList.count())], "");
				//CPhotoItem->loadImageSpawn();
				CPhotoItem->update();			
			}
		}
	}
}

void TemplateScene::setBackgroundImage(const QImage& _ThumbNail, const QString& _ImageFileName, bool _Encrypted)
{
	PageItem->setThumbnail(_ThumbNail, _ImageFileName);
	PageItem->setImageEncrypted(_Encrypted);
	PageItem->loadImageSpawn();
	PageItem->update();
}

bool TemplateScene::hasBackgroundImage() const
{
	return PageItem->hasImage();
}


QString TemplateScene::thumbnailImage(const QString& _ImgFile)
{
	return _ImgFile + ".thumb";
}

QImage TemplateScene::getThumbnail(const QString& _ImageFileName, bool _Encrypted, bool _CreateIfNotExist)
{
	QSize ThumbnailSize(600, 400);
	QString ThumbnailFileName = thumbnailImage(_ImageFileName);
	STImage ThumbnailPix(ThumbnailFileName);

	if (ThumbnailPix.isNull())
	{
		/*if (_Encrypted)
		{
			STImage ThumbnailImage = STImage(_ImageFileName);
			ThumbnailImage.blowFishDecode();
			ThumbnailPix = ThumbnailImage.scaled(ThumbnailSize, Qt::KeepAspectRatio);
		}
		else*/
		ThumbnailPix = STImage(_ImageFileName).scaled(ThumbnailSize, Qt::KeepAspectRatio);

		if (_CreateIfNotExist)
			ThumbnailPix.save(ThumbnailFileName, "PNG");
		//!!! save it as png and do the same with backgrounds !
	}
	return ThumbnailPix;
}

void TemplateScene::setBgBrush(const QBrush& _Brush)
{
	//PageItem->unloadImage();	
	PageItem->setBrush(_Brush);
	modified();
}

QBrush TemplateScene::bgBrush() const
{
	return PageItem->brush();
}

void TemplateScene::clear()
{
	PageItem = 0;
	PhotoItems.clear();
	#if QT_VERSION >= 0x040400
		QGraphicsScene::clear(); 
	#else 
		QList<QGraphicsItem *> AllItems = items();
		QList<QGraphicsItem *>::iterator it;
		for (it = AllItems.begin(); it != AllItems.end(); ++it)
		{
			removeItem(*it);
			delete (*it);
		}
	#endif   
}

//TODO: Make it O(1)
QList<GraphicsPhotoItem *> TemplateScene::photoItems() const
{
/*	QList<GraphicsPhotoItem *> Res;
	QList<QGraphicsItem *> AllItems = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		if (GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			Res.push_back(CPhotoItem);
	}	
	
	return Res;*/
	return PhotoItems;
}

QRectF TemplateScene::photoItemsBoundingRect(GraphicsPhotoItem* _ExcludeMe) const
{
	QRectF Res;

	if (FixedOutMargin > 0)
		Res = sceneRect().adjusted(FixedOutMargin,FixedOutMargin, -FixedOutMargin, -FixedOutMargin);
	else
	{
		QList<QGraphicsItem *> AllItems = items();
		QList<QGraphicsItem *>::iterator it;
		for (it = AllItems.begin(); it != AllItems.end(); ++it)
		{
			if ( GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			{
				if (CPhotoItem != _ExcludeMe)
					Res |= CPhotoItem->sceneBoundingRect();
			}
		}
	}

	return Res;
}


QList<GraphicsMonthItem*> TemplateScene::monthItems() const
{
	QList<GraphicsMonthItem *> Res;
	QList<QGraphicsItem *> AllItems = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		if ( GraphicsMonthItem* CPhotoItem = qgraphicsitem_cast<GraphicsMonthItem*>(*it))
			Res.push_back(CPhotoItem);
	}

	return Res;
}

void TemplateScene::loadHiResImages(GraphicsPhotoItem* _Item)
{
	if (_Item->hasImage()) ///!!!!!!!!!!!!!!!!!!!!!! No hauria pas de produir-se amb els templates de calendaris...
	{
		_Item->setImageMode(GraphicsPhotoItem::HiResImageMode);
		_Item->unloadImage();
		//_Item->waitForImageLoaded();
	}
}

void TemplateScene::loadHiResImages(QProgressBar* _Progress)
{
	//If there is page background image: 
	if (PageItem)
		loadHiResImages(PageItem);
	
	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	QList<GraphicsPhotoItem *>::iterator it;
	if (_Progress)
	{
		//_Progress->setRange(0, PhotoItems.size());
		_Progress->setRange(0, -1);
		_Progress->setValue(0);
		QApplication::processEvents();
	}
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		loadHiResImages(*it);
// 		(*it)->setImageMode(GraphicsPhotoItem::HiResImageMode);
// 		(*it)->unloadImage();
// 		(*it)->waitForImageLoaded();
		
		if (_Progress)
		{
			_Progress->setValue(_Progress->value() + 1);
			QApplication::processEvents();
		}
	}
	if (_Progress)
	{
		_Progress->setValue(PhotoItems.size());
		QApplication::processEvents();
	}
}

void TemplateScene::unloadImage(GraphicsPhotoItem* _Item)
{
	_Item->setImageMode(GraphicsPhotoItem::LowResImageMode);
	_Item->unloadImage();
}
		
void TemplateScene::unloadImages()
{
	if (PageItem)
		unloadImage(PageItem);

	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	QList<GraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		unloadImage(*it);
	}
}

void TemplateScene::clearImages()
{
	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	QList<GraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		(*it)->clearImage();
	}
}


void TemplateScene::updateImage(GraphicsPhotoItem* _Item)
{
	_Item->loadImageSpawn();
	_Item->update();
}
		
void TemplateScene::updateImages()
{
	if (PageItem)
		updateImage(PageItem); 

	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	QList<GraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		updateImage(*it); 
	}
}

void TemplateScene::setEmptyFramesVisible(bool _Value)
{
	QList<GraphicsPhotoItem*> OldPhotoItems = photoItems();
	QList<GraphicsPhotoItem*>::iterator it;
	for (it = OldPhotoItems.begin(); it != OldPhotoItems.end(); ++it)
	{
		if (!(*it)->hasImage())
			(*it)->setVisible(_Value); 
	}
}

void TemplateScene::setWarningsVisible(bool _Visible)
{
	if (PageItem) //Defensive
		PageItem->setWarningVisible(_Visible);

	QList<QGraphicsItem *>::iterator it;
	QList<QGraphicsItem *> AllItems = items();
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			CItem->setWarningVisible(_Visible);
	}
}

/*!
	Prepares scene to print. Unselects all items ...
*/
void TemplateScene::prepareForPrint()
{
	if (PageItem) //Defensive 
		setSceneRect(PageItem->boundingRect());
	
	QList<QGraphicsItem *>::iterator it;
	QList<QGraphicsItem *> AllItems = items();
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		(*it)->setSelected(false);

	}	
}

void TemplateScene::addRandomPhotoFrame()
{
	QRectF NewTemplateRect;
	if (PhotoItems.size() > 0)
	{
		int Index = qrand() % PhotoItems.size();
		GraphicsPhotoItem* SelItem = PhotoItems[Index];
		NewTemplateRect = SelItem->rect();
	}
	else
		NewTemplateRect = QRectF(0, 0, 46, 35);

	GraphicsPhotoItem* PhotoItem = new GraphicsPhotoItem;
	PhotoItem->setZValue(topZValue() + 1);
	PhotoItem->setRect(NewTemplateRect);
	addPhotoItem(PhotoItem);

	//TODO: Update it !
/*	STPhotoLayoutTemplate::TFrameList FrameList = Template.frames();

	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{		
		if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			FrameList.push_back(STPhotoLayoutTemplate::Frame(CItem->rect()));
	}
	bool Found = false; 
	GraphicsPhotoItem* PhotoItem = 0;
	if (!FrameList.isEmpty())
	{
		int NTemplate = qrand() % FrameList.size();
		STPhotoLayoutTemplate::Frame CFrame = FrameList[NTemplate];
		int NInspectedItems = 1;
		while (!Found && NInspectedItems <= FrameList.size())
		{
			Found = !CFrame.isTextFrame();
			if (!Found)
			{
				NTemplate = (NTemplate + 1) % FrameList.size();
				CFrame = FrameList[NTemplate];
				NInspectedItems++;
			}
		}
		if (Found)
		{
			CFrame.moveTopLeft(QPointF(10,10));
			PhotoItem = new GraphicsPhotoItem(CFrame);
		}
	}
	if (!Found)
	{
		PhotoItem = new GraphicsPhotoItem;
		PhotoItem->setRect(10, 10, 46, 35); 
	}	
	if (PhotoItem) //Defensive 
	{
		PhotoItem->setZValue(topZValue() + 1);
		addPhotoItem(PhotoItem);
		//PhotoItem->setPanningEnabled(false);
	}*/
}

void TemplateScene::addPhotoItem(GraphicsPhotoItem* _PhotoItem)
{
	_PhotoItem->setPanningEnabled(!ItemsMovable);
	_PhotoItem->setMultiSelection(ModifyAllFrames);
	_PhotoItem->setAutoAdjustFramesToImages(AutoAdjustFrames);
	_PhotoItem->setExpandImagesToFillFrames(ExpandImagesToFillFrames);
	configureItem(_PhotoItem); 
	connect(_PhotoItem, SIGNAL(mousePanning(const QPointF&)), this, SLOT(panSelectedPhotoItems(const QPointF&)));
	connect(_PhotoItem, SIGNAL(imageDropped(const QString&, const QString&)), this, SIGNAL(imageDropped(const QString&, const QString&)));
	connect(_PhotoItem, SIGNAL(imageListDropped(QList<QUrl>)), this, SIGNAL(imageListDropped(QList<QUrl>)));
	connect(_PhotoItem, SIGNAL(imageRemoved(const QString&, const QString&)), this, SIGNAL(imageRemoved(const QString&, const QString&)));
	PhotoItems.push_back(_PhotoItem);
	QGraphicsScene::addItem(_PhotoItem);
	modified();
}

void TemplateScene::setPageItem(GraphicsPageItem* _PageItem)
{
	PageItem = _PageItem; 
	if (_PageItem) //Defensive 
	{
		connect(_PageItem, SIGNAL(imageDropped(const QString&, const QString&)), this, SIGNAL(imageDropped(const QString&, const QString&)));
		connect(_PageItem, SIGNAL(imageListDropped(QList<QUrl>)), this, SIGNAL(imageListDropped(QList<QUrl>)));
		connect(_PageItem, SIGNAL(imageRemoved(const QString&, const QString&)), this, SIGNAL(imageRemoved(const QString&, const QString&)));
		setSceneRect(PageItem->boundingRect());
	}
}

void TemplateScene::deletePageItem()
{
	if (PageItem)
		delete PageItem; 
	PageItem = 0; 
}


QGraphicsItem* TemplateScene::addClipartItem(const QString& _FileName)
{
	GraphicsClipartItem* NewItem = new GraphicsClipartItem(Resource(_FileName, Resource::TypeClipart));
	configureItem(NewItem); 
	//NewItem->scaleToWidth(75); 
	NewItem->scaleToHeight(75);
	addItemOnTop(NewItem);	

	modified();
	return NewItem;
}

GraphicsTextItem* TemplateScene::addTextFrame(const QString& _Html)
{
	GraphicsTextItem* NewFrame = new GraphicsTextItem;
	NewFrame->setHtml(_Html);
	NewFrame->setFont(TextFrameFont);
	NewFrame->setPos(QPoint(0, 0));
	NewFrame->setMovable(true); //The text items are always movable.
	configureItem(NewFrame); 
	addItemOnTop(NewFrame);	
	//NewFrame->adjustSize();
	//NewFrame->setTextWidth(-1);
	modified();
	return NewFrame;
}

void TemplateScene::addSubTittleTextFrame(int _FrameIndex)
{
	if (PhotoItems.size() > _FrameIndex)
	{
		GraphicsPhotoItem* CItem = PhotoItems[_FrameIndex];

		GraphicsTextItem* NewText = addTextFrame("TEXT TEXT TEXT TEXT");
		NewText->setAlignment(Qt::AlignCenter);
		QRectF CItemRect = CItem->boundingRect();
		double TextFrameMarginY = 2;
		double TextFrameMarginX = 15;
		NewText->setPos(CItemRect.x() + TextFrameMarginX, CItemRect.y() + CItemRect.height() + TextFrameMarginY);
	}
/*	double TextFrameHeight = 15;
	double TextFrameMarginX = 15;
	double TextFrameMarginY = 2;
	Frame& CFrame = FrameList[_FrameIndex];
	CFrame.setHeight(CFrame.height() - TextFrameHeight);

	Frame NewFrame(QRectF(CFrame.x() + TextFrameMarginX, CFrame.y() + CFrame.height() + TextFrameMarginY, CFrame.width() - TextFrameMarginX * 2, TextFrameHeight - TextFrameMarginY * 2));
	NewFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeText);
	QString Text;
	for (int Vfor = 0; Vfor < static_cast<int>(NewFrame.width() / 20); Vfor++)
	{
		Text = Text + QObject::tr("TEXT ");
	}
	NewFrame.setText(Text);
	NewFrame.setFont(QFont("Arial",16));
	addFrame(NewFrame);*/
}

GraphicsMonthItem* TemplateScene::createMonthTextFrameItem()
{
	GraphicsMonthItem* NewItem = new GraphicsMonthItem;
	NewItem->setMovable(true);
	configureItem(NewItem); 
	addItemOnTop(NewItem);
	modified();
	return NewItem;
}

//@Revision Mark
QGraphicsItem* TemplateScene::addElement(QDomElement& _Element, const QString& _LoadDir)
{
	AbstractGraphicsItem* CItem;
	QGraphicsItem* GrItem = 0;
	if (_Element.tagName().toLower() ==  GraphicsPhotoItem::tagName())
	{
		GraphicsPhotoItem* NPItem = new GraphicsPhotoItem;
		NPItem->loadElement(_Element, _LoadDir);
		NPItem->setZValue(_Element.attribute("zvalue").toDouble());
		addPhotoItem(NPItem );
		GrItem = NPItem;  
	}
	else 
	{
		CItem = 0; 
		GrItem = 0; 
		if (_Element.tagName().toLower() ==  GraphicsClipartItem::tagName() )
		{
			GraphicsClipartItem* NItem = new GraphicsClipartItem;
			configureItem(NItem);
			CItem = NItem;
			GrItem = NItem;
		}
		else
		if (_Element.tagName().toLower() ==  GraphicsTextItem::tagName())
		{
			GraphicsTextItem* NItem = new GraphicsTextItem;
			configureItem(NItem); 
			CItem = NItem; 
			GrItem = NItem;
		}
		else
		if (_Element.tagName().toLower() ==  GraphicsMonthItem::tagName())
		{
			GraphicsMonthItem* NItem = new GraphicsMonthItem;
			configureItem(NItem);
			CItem = NItem;
			GrItem = NItem;
		}

		if (CItem && GrItem)
		{
			CItem->loadElement(_Element, _LoadDir);
			GrItem->setZValue(_Element.attribute("zvalue").toDouble());
			addItem(GrItem); 
		}
	}
	return GrItem; 
}

void TemplateScene::loadElement(const QDomElement& _SceneElement, const QString& _LoadDir)
{
	deletePageItem();

	ModifyAllFrames = _SceneElement.attribute("modifyallframes", "0").toLower() == "1";

	//Search first the Page item
	QDomNode CNode = _SceneElement.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() ==  GraphicsPageItem::tagName() )
			{
				GraphicsPageItem* NewPageItem = new GraphicsPageItem;
				NewPageItem->loadElement(CEl, _LoadDir);
				NewPageItem->setZValue(PageZValue);
				configureItem(NewPageItem); 
				addItem(NewPageItem);
				setPageItem(NewPageItem); 
			}
		}
		CNode = CNode.nextSibling();
	}
	Assert(PageItem, Error("Could not load Scene Page Item")); 
	
	CNode = _SceneElement.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
			addElement(CEl, _LoadDir);
		 
		CNode = CNode.nextSibling();
	}	
}

QDomElement TemplateScene::createElement(QDomDocument& _Doc, const QString& _StoreDir)
{
	QDomElement SceneEl = _Doc.createElement("scene");
	SceneEl.setAttribute("modifyallframes", ModifyAllFrames);

	//PageItem
	if (PageItem) //Defensive
		SceneEl.appendChild(PageItem->createElement(_Doc, _StoreDir));

	//All the items
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
		{
			AbstractGraphicsItem* CItem = dynamic_cast<AbstractGraphicsItem*>(*it);
			if (CItem && CItem != PageItem)//FIXME it crash for non STAbstractGraphicsItems !!!.
			{
				QDomElement NewEl = CItem->createElement(_Doc, _StoreDir);
				NewEl.setAttribute("zvalue",(*it)->zValue());
					SceneEl.appendChild(NewEl); 
			}
		}
	}
	return SceneEl; 
}

qreal TemplateScene::topZValue() const
{
	qreal Res = 0; 
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->zValue() > Res)
			Res = (*it)->zValue();
	}
	return Res + 1; 	
}

qreal TemplateScene::bottomZValue() const
{
	qreal Res = 0; 
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it) != PageItem && (*it)->zValue() < Res)
			Res = (*it)->zValue();
	}
	return Res - 1; 	
}

void TemplateScene::configureItem(AbstractGraphicsItem* _Item)
{
	_Item->setResizeAllowed(ItemsResizable);
}

void TemplateScene::addItemOnTop(QGraphicsItem* _Item)
{
	_Item->setZValue(topZValue());
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_Item))
		addPhotoItem(CItem);
	else
		addItem(_Item);
	modified();
}

QGraphicsItem* TemplateScene::currentItem() const
{
	QGraphicsItem* Res = 0; 
	//Res = focusItem();
	if (!Res && selectedItems().size() > 0)
		Res = selectedItems().first();

	if (!Res)
		Res = focusItem();

	return Res; 
}

void TemplateScene::applyResource(const SPhotoBook::Resource& _Resource)
{
	switch (_Resource.type())
	{
		case Resource::TypeBackground :
			//TODO Encrypted Resources
			setBackgroundImage(QImage(), _Resource.fileInfo().absoluteFilePath(), _Resource.isEncrypted());
		break;
		case Resource::TypeClipart :
			addClipartItem(_Resource.fileInfo().absoluteFilePath());
		break;
		case Resource::TypeFrame :
		case Resource::TypeFrameMask :
		{
			QGraphicsItem* CItem = currentItem();
			if (CItem)
			{
				if (GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(CItem))
					CPhotoItem->setResource(_Resource);
			}
		}
		break;
	}
}

void TemplateScene::selectNone()
{
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		(*it)->setSelected(false); 
	}
}

void TemplateScene::selectAll()
{
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		(*it)->setSelected(true); 
	}
}

void TemplateScene::setAllControlsVisible(bool _Value)
{
	QFileInfoList OldIndexList; 
	QList<GraphicsPhotoItem*> OldPhotoItems = photoItems();
	QList<GraphicsPhotoItem*>::iterator it;
	for (it = OldPhotoItems.begin(); it != OldPhotoItems.end(); ++it)
	{
		(*it)->setControlsVisible(_Value); 
	}
}

void TemplateScene::selectAllByType(int _Type)
{
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->type() == _Type)
			(*it)->setSelected(true);
	}
}

void TemplateScene::shrinkFramesBy(double _Amount)
{
	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	QList<GraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		(*it)->shrink(_Amount);
	}

}

void TemplateScene::splitXFrame(int _FrameIndex)
{
	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	if (PhotoItems.size() > _FrameIndex)
	{
		GraphicsPhotoItem* CItem = PhotoItems[_FrameIndex];
		QRectF ItemRect = CItem->rect();
		CItem->setRect(ItemRect.x(), ItemRect.y(), ItemRect.width() / 2, ItemRect.height());

		GraphicsPhotoItem* NewItem = new GraphicsPhotoItem;
		NewItem->setRect(ItemRect.x() + ItemRect.width() / 2, ItemRect.y(), ItemRect.width() / 2, ItemRect.height());
		addPhotoItem(NewItem);
	}
}

void TemplateScene::splitYFrame(int _FrameIndex)
{
	QList<GraphicsPhotoItem *> PhotoItems = photoItems();
	if (PhotoItems.size() > _FrameIndex)
	{
		GraphicsPhotoItem* CItem = PhotoItems[_FrameIndex];
		QRectF ItemRect = CItem->rect();
		CItem->setRect(ItemRect.x(), ItemRect.y(), ItemRect.width(), ItemRect.height() / 2);

		GraphicsPhotoItem* NewItem = new GraphicsPhotoItem;
		NewItem->setRect(ItemRect.x(), ItemRect.y() + ItemRect.height() / 2, ItemRect.width(), ItemRect.height() / 2);
		addPhotoItem(NewItem);
	}
}



QList<QGraphicsItem *> TemplateScene::selectedItemsOfType(int _Type)
{
	QList<QGraphicsItem *> Res; 
	QList<QGraphicsItem *> Items = selectedItems(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->type() == _Type)
			Res.push_back(*it);
	}
	return Res; 	
}


//! \return true if photobook currently contains image with MD5Sum _ImageMD5Sum.
bool TemplateScene::containsImage(const QString& _ImageMD5Sum)
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it = Items.begin(); 
	bool Found = false; 
	while (it != Items.end() && !Found)
	{
		if ((*it)->isVisible())	
		{
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
				Found = CItem->imageMD5Sum() == _ImageMD5Sum; 
			if (GraphicsPageItem* CItem = qgraphicsitem_cast<GraphicsPageItem*>(*it))
				Found = CItem->imageMD5Sum() == _ImageMD5Sum; 
		}
		++it; 
	}
	return Found; 
}

//! \return true if photobook contains some photoitem with no image.
bool TemplateScene::hasEmptyPhotoItems() const
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it = Items.begin(); 
	bool Found = false; 
	while (it != Items.end() && !Found)
	{
		if ((*it)->isVisible())	
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
				Found = !CItem->hasImage(); 
	
		++it; 
	}
	return Found; 
}

//! return num of visible photoitems.
int TemplateScene::numPhotoItems() const
{
	int Res = 0;
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible() && (*it) != PageItem)
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
				++Res;

	}
	return Res;
}

int TemplateScene::numMonthItems() const
{
	int Res = 0;
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
			if (GraphicsMonthItem* CItem = qgraphicsitem_cast<GraphicsMonthItem*>(*it))
				++Res;

	}
	return Res;
}

int TemplateScene::numEmptyPhotoItems() const
{
	int Res = 0;
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
				if (!CItem->hasImage())
					++Res;
	}
	return Res;
}

int TemplateScene::numLandscapeFrames() const
{
	int Res = 0;

	QList<GraphicsPhotoItem*> PhotoItems = photoItems();
	QList<GraphicsPhotoItem*>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		if ((*it)->boundingRect().width() > (*it)->boundingRect().height())
			Res++;
	}
	return Res;
}

bool TemplateScene::hasSameFrames(const TemplateScene* _Other) const
{
	bool Diferent = _Other->numPhotoItems() != numPhotoItems();
	QList<GraphicsPhotoItem*> PhotoItems = photoItems();
	QList<GraphicsPhotoItem*>::iterator it = PhotoItems.begin();
	while (!Diferent && it != PhotoItems.end())
	{
		//Look for a frame line *it
		bool Found = false;
		QList<GraphicsPhotoItem*> OtherPhotoItems = _Other->photoItems();
		QList<GraphicsPhotoItem*>::iterator otherit = OtherPhotoItems.begin();
		while (!Found && otherit != OtherPhotoItems.end())
		{
			QRectF Rect = (*it)->rect();
			QRectF OtherRect =  (*otherit)->rect();
			Found =  (qAbs(Rect.x() - OtherRect.x()) < 1) &&
					 (qAbs(Rect.y() - OtherRect.y()) < 1) &&
					 (qAbs(Rect.width() - OtherRect.width()) < 1) &&
					 (qAbs(Rect.height() - OtherRect.height()) < 1);
			++otherit;
		}
		Diferent = !Found;
		++it;
	}
	return !Diferent;
}

int TemplateScene::biggestFrameIndex()
{
	int Res = -1;
	double CurrentArea = 0;
	int ItemIndex = 0;
	QList<GraphicsPhotoItem*> ItemList = photoItems();
	while (ItemIndex < ItemList.size())
	{
		QRectF CurrBRect = ItemList[ItemIndex]->boundingRect();
		if (CurrBRect.width() * CurrBRect.height() > CurrentArea)
		{
			CurrentArea = CurrBRect.width() * CurrBRect.height();
			Res = ItemIndex;
		}

		++ItemIndex;
	}
	return Res;
}

bool TemplateScene::operator==(const TemplateScene& _Other) const
{
	return hasSameFrames(&_Other);
}

bool TemplateScene::operator<(const TemplateScene& _Other) const
{
	return numPhotoItems() < _Other.numPhotoItems();
}

bool TemplateScene::operator>(const TemplateScene& _Other) const
{
	return !operator<(_Other);
}

bool TemplateScene::operator<(int _Value) const
{
	return numPhotoItems() < _Value;
}

bool TemplateScene::operator>(int _Value) const
{
	return !operator<(_Value);
}


void TemplateScene::selectFirstEmptyPhotoItem()
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it = Items.begin(); 
	bool Found = false; 
	while (it != Items.end() && !Found)
	{
		if ((*it)->isVisible())	
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
				Found = !CItem->hasImage(); 
		if (Found)
			(*it)->setSelected(true); 	
		++it; 
	}
}

void TemplateScene::modified()
{
	HasChanges = true;
}

void TemplateScene::clearChanges()
{
	HasChanges = false;
}

void TemplateScene::autoFillImages(const QFileInfoList& _Images)
{
	QList<QUrl> UrlList;
	QFileInfoList::const_iterator it;
	for (it = _Images.begin(); it != _Images.end(); ++it)
	{
		UrlList.push_back(QUrl::fromLocalFile(it->absoluteFilePath()));
	}
	slotImageListDropped(UrlList);
}

void TemplateScene::getContextVariables()
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if (GraphicsTextItem* CItem = qgraphicsitem_cast<GraphicsTextItem*>(*it))
		{
			CItem->replaceTextVariables(DataContext);
		}
	}
}

void TemplateScene::setDataContext(const TemplateDataContext& _DataContext)
{
	DataContext = _DataContext;
}

TemplateDataContext& TemplateScene::dataContext()
{
	return DataContext;
}

/*!
	\return a list of saved images.
*/
QStringList TemplateScene::storePhotoItemImages(const CollectionInfo& _CInfo, bool _OnlyDesignImages)
{
	QStringList StoredFiles; 
	if (PageItem)
		StoredFiles +=  PageItem->saveResources(_CInfo.photoBookPath());//storePhotoItemImage(PageItem, _CInfo);

	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{	
		if ((*it)->isVisible())	
		{
			if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
				StoredFiles += CItem->saveResources(_CInfo.photoBookPath(), !_OnlyDesignImages);
			else 
			if (GraphicsClipartItem* CItem = qgraphicsitem_cast<GraphicsClipartItem*>(*it))
				StoredFiles += CItem->saveResources(_CInfo.photoBookPath());
		}
	}
	return StoredFiles; 
}

// void TemplateScene::performSelectedItemsOp(GraphicsItemOperation* _Operation)
// {
// 	QList<QGraphicsItem *> Items = selectedItems();
// 	QList<QGraphicsItem *>::iterator it; 
// 	for (it = Items.begin(); it != Items.end(); ++it)
// 	{
// 		_Operation->performOp(*it);
// 		(*it)->update();
// 	}
// }

void TemplateScene::setItemsResizable(bool _Resizable)
{
	ItemsResizable = _Resizable; 
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if (AbstractGraphicsItem* CItem = dynamic_cast<AbstractGraphicsItem*>(*it))
			CItem->setResizeAllowed(_Resizable);
	}
}


void TemplateScene::setItemsMovable(bool _Movable)
{
	ItemsMovable = _Movable; 
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		//TODO Unificar-ho amb setMovable per una base comú ?
		if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			CItem->setPanningEnabled(!_Movable);
// 		else 
// 		if (GraphicsSimpleTextItem* CItem = qgraphicsitem_cast<GraphicsSimpleTextItem*>(*it))
// 			CItem->setFlag(QGraphicsItem::ItemIsMovable, _Movable);
// 		else 
// 		if (GraphicsTextItem* CItem = qgraphicsitem_cast<GraphicsTextItem*>(*it))
// 			CItem->setMovable(_Movable);
	}
}


bool TemplateScene::event(QEvent* _Event)
{
	
	if (_Event->type() == UpdateItemEvent::UpdateItemEventType)
	{
		UpdateItemEvent* CEvent = static_cast<UpdateItemEvent*>(_Event);
		if (!CEvent->item().isNull())
			CEvent->item()->update();
		return true;
	}
	return QGraphicsScene::event(_Event);
}

void TemplateScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _Event)
{
	emit doubleClicked();
	QGraphicsScene::mouseDoubleClickEvent(_Event); 
}

void TemplateScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _Event)
{
	emit clicked();
	QGraphicsScene::mouseReleaseEvent(_Event); 
}

void TemplateScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* _ContextMenuEvent)
{
	QGraphicsItem* Item = itemAt(_ContextMenuEvent->scenePos());
	
	if (Item)
		emit itemContextMenu(Item, _ContextMenuEvent->screenPos());
}

// Drag and Drop
void TemplateScene::dropEvent(QGraphicsSceneDragDropEvent* _Event )
{
	bool Propagate = true;
	;
	if (const SPhotoBook::TemplateSceneMimeData* SMData = qobject_cast<const SPhotoBook::TemplateSceneMimeData*>(_Event->mimeData()))
	{
		emit templateDropped(this, SMData->getTemplateScene());
		Propagate = false;
	}
	else
	if (const SPhotoBook::ResourceMimeData* MData = qobject_cast<const SPhotoBook::ResourceMimeData*>(_Event->mimeData()))
	{
		if (MData->resource().type() == Resource::TypeClipart || MData->resource().type() == Resource::TypeBackground)
		{
			Propagate = false;
			applyResource(MData->resource());
		}
	}
	else
	if (_Event->mimeData()->hasUrls())
	{
		QList<QUrl> Urls = _Event->mimeData()->urls();

		QList<QUrl>::iterator it = Urls.begin();
		while (it != Urls.end())
		{
			QUrl CUrl = *it;
			if (!CUrl.isEmpty() && CUrl.isValid())
			{
				//Only local file uris are supported.
				QString FilePath = CUrl.toLocalFile();
				if (!FilePath.isEmpty())
				{
					QFileInfo FilePathInfo(FilePath);
					if (SPhotoBook::Resource::isResource(FilePathInfo))
					{
						SPhotoBook::Resource CRes(FilePathInfo);
						applyResource(CRes);
						Propagate = false;
					}
				}
			}
			++it;
		}

	}
	if (Propagate)
		QGraphicsScene::dropEvent(_Event);
	_Event->accept();
}


void TemplateScene::panSelectedPhotoItems(const QPointF& _Point)
{
	//if (!ItemsResizable)
	//{
	QList<QGraphicsItem *> Items = selectedItems();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		//TODO Unificar-ho amb setMovable per una base comú ?
		if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it))
			CItem->panImage(_Point); 
	}
	//}
}

void TemplateScene::slotCheckModifyAll(const QString& _ImagePath)
{
	if (ModifyAllFrames)
	{
		selectAllByType(GraphicsPhotoItem::Type);
		QList<QGraphicsItem *> Items = selectedItems();
		QList<QGraphicsItem *>::iterator it;

		STDom::DImageDoc Doc(_ImagePath);
		for (it = Items.begin(); it != Items.end(); ++it)
		{
			GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(*it);
			if (CPhotoItem)
			{
				CPhotoItem->setImage(Doc);
				CPhotoItem->loadImageSpawn();
			}
		}

	}
}

void TemplateScene::slotImageListDropped(const QList<QUrl>& _Urls)
{
	selectAllByType(GraphicsPhotoItem::Type);
	QList<QGraphicsItem *> Items = selectedItems();
	bool EmptyItemsAvailable = true;
	bool ItemFound = true;
	QList<QUrl>::const_iterator it = _Urls.begin();
	int CntItem = 0;
	while (ItemFound && it != _Urls.end())
	{
		//Only local file uris are supported.
		QString ImagePath = it->toLocalFile();
		if (!ImagePath.isEmpty())
		{
			STDom::DImageDoc Doc(ImagePath);
			ItemFound = false;
			//Look for an item
			while (CntItem < Items.size() && !ItemFound)
			{
				GraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Items[CntItem]);
				if (CPhotoItem)
				{
					if (!CPhotoItem->hasImage() || !EmptyItemsAvailable)
					{
						CPhotoItem->setImage(Doc);
						CPhotoItem->loadImageSpawn();
						ItemFound = true;
						emit imageDropped(ImagePath, STImage::hashString(ImagePath));
					}
				}
				CntItem++;
				if (!ItemFound && EmptyItemsAvailable && CntItem == Items.size())
				{
					EmptyItemsAvailable = false;
					CntItem = 0;
				}
			}
		}
		++it;
	}
}

//Related non member functions.
bool SPhotoBook::operator<(int _Value, const TemplateScene& _Other)
{
	return _Other > _Value;
}

