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

#include "sttemplatescene.h"
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


#include "stgraphicsphotoitem.h"
#include "stgraphicssimpletextitem.h"
#include "stgraphicspageitem.h"
#include "stupdateitemevent.h"
#include "stgraphicstextitem.h"
#include "stabstractgraphicsitem.h"
#include "stgraphicsclipartitem.h"
#include "stphotobookcollectioninfo.h"
#include "stimage.h"
#include "stgraphicsitemoperation.h"
#include "stgraphicssuperimposeitem.h"
#include "stgraphicsmonthitem.h" 
#include "stutils.h" 

//Drag and Drop
#include "sttemplatemimedata.h"

#include "ddoc.h"


qreal STTemplateScene::PageZValue = -1000000;
qreal STTemplateScene::SImposeZValue = 10000;

/*!
	\return returns a list of saved filenames.
*/
QStringList STTemplateScene::storePhotoItemImage(STGraphicsPhotoItem* _CItem, const STPhotobookCollectionInfo& _CInfo, bool _OnlyDesignImages)
{
	QStringList Res; 
	if (!_CItem->imageFileName().trimmed().isEmpty())
	{
		if (!_OnlyDesignImages)
		{
			QString StoredImageName = _CInfo.imageFilePath(_CItem->imageFileName());
			Res.push_back(StoredImageName);
			if (!QFile::exists(StoredImageName))
				Assert(QFile::copy(_CItem->imageFileName(), StoredImageName), Error(QString(tr("Error storing image '%1' -> '%2'")).arg(_CItem->imageFileName()).arg(StoredImageName)));
			_CItem->setImageFileName(StoredImageName);
			_CItem->setImageSourcePath(QFileInfo(StoredImageName).absolutePath());
		}

		//Store mask image if it does not exist.
		if (_CItem->hasAlphaChannel())
		{
			QString MaskImageFileName = _CInfo.photoBookPath() + "/" + _CItem->alphaChannelFileName(); 
			QImage MaskImage = _CItem->alphaChannel(); 
			//qDebug(QString("Saving Mask %1").arg(MaskImageFileName).toLatin1());
			Res.push_back(MaskImageFileName); 
			if (!QFile::exists(MaskImageFileName))
				Assert(MaskImage.save(MaskImageFileName), 
					Error(QString(tr("Error storing mask image '%1'")).arg(MaskImageFileName)));
		}
		if (_CItem->hasFrameImage())
		{
			QString FrameImageFileName = _CInfo.photoBookPath() + "/" + _CItem->frameImageFileName();
			QImage FrameImage = _CItem->frameImage();
			Res.push_back(FrameImageFileName);
			if (!QFile::exists(FrameImageFileName))
				Assert(FrameImage.save(FrameImageFileName),
					Error(QString(tr("Error storing frame image '%1'")).arg(FrameImageFileName)));
		}
	}
	return Res; 
}

/*!
	\return saved filename
*/
QString STTemplateScene::storeClipartItemFile(STGraphicsClipartItem* _CItem, const STPhotobookCollectionInfo& _CInfo)
{
	QString Res; 
	if (!_CItem->fileName().trimmed().isEmpty())
	{
		QString StoredImageName = _CInfo.imageFilePath(_CItem->fileName());
		Res = StoredImageName; 
		if (!QFile::exists(StoredImageName))
			Assert(QFile::copy(_CItem->fileName(), StoredImageName), Error(QString(tr("Error storing image '%1' -> '%2'")).arg(_CItem->fileName()).arg(StoredImageName)));
		_CItem->setFileName(StoredImageName); 
	}
	return Res; 
}



STTemplateScene::STTemplateScene(QObject* _Parent)
 : QGraphicsScene(_Parent), PageItem(0), ItemsMovable(false), ItemsResizable(true), SImposeItem(0), ModifyAllFrames(false),
	RenderBaseSize(0, 0), AutoAdjustFrames(true), IgnoreExifRotation(false), HasChanges(false)
{
	setBackgroundBrush(QColor("#DFE1BD"));
	connect(this, SIGNAL(imageDropped(QString,QString)), this, SLOT(slotCheckModifyAll(QString)));
	connect(this, SIGNAL(imageListDropped(QList<QUrl>)), this, SLOT(slotImageListDropped(QList<QUrl>)));
}

void STTemplateScene::loadPageTemplate(const STPhotoLayoutTemplate& _Template)
{
	Template = _Template;
	ModifyAllFrames = _Template.modifyAllFrames();
	RenderBaseSize = _Template.baseSize();

	clear();
	STPhotoLayoutTemplate::TFrameList::const_iterator it;
	STPhotoLayoutTemplate::TFrameList FrameList = _Template.frames();
	setSceneRect(QRectF(QPointF(0, 0), _Template.size()));
	
	// Page Item
	STGraphicsPageItem* NewPageItem = new STGraphicsPageItem(sceneRect());
	NewPageItem->setBrush(Qt::white);
	configureItem(NewPageItem); 
	addItem(NewPageItem);
	setPageItem(NewPageItem);

	NewPageItem->setZValue(PageZValue);
	
	//SuperImpose item 
	// -- SuperImpose Image --
	QString SImposeImgFile = _Template.superImposeImageFilePath();
	if (!SImposeImgFile.isEmpty())
	{
		setSuperImposeImage(SImposeImgFile, _Template.encrypted());
	}
	
	
	//Frames 
	for ( it = FrameList.begin(); it != FrameList.end(); ++it )
	{
		switch(it->frameType())
		{
/*			case STPhotoLayoutTemplate::Frame::TypeText :
			{
				TextFrameFont = it->font();
				STGraphicsSimpleTextItem* STItem = new STGraphicsSimpleTextItem(*it);
				STItem->setZValue(it->zValue()); 

				configureItem(STItem); 
				//addItemOnTop(STItem);
				addItem(STItem); 
			}
			break;*/
			case STPhotoLayoutTemplate::Frame::TypeText :
			case STPhotoLayoutTemplate::Frame::TypeRitchText:
			{
				STGraphicsTextItem* STItem = new STGraphicsTextItem(*it);
				STItem->setZValue(it->zValue()); 
				configureItem(STItem); 
				addItem(STItem); 
			}
			break;
			case STPhotoLayoutTemplate::Frame::TypeCalMonth :
			{
				STGraphicsMonthItem* STItem = new STGraphicsMonthItem(*it);
				STItem->setTextColor(it->textColor()); 
				STItem->setFont(it->font());
				STItem->setMonth(it->month(), it->year()); 
				STItem->setHolidays(_Template.holidays(it->year()));
				STItem->setHtml(STItem->getMonthHtml()); 
				STItem->setZValue(it->zValue()); 
				configureItem(STItem); 
				STItem->scaleToSize(it->size());
				//addItemOnTop(STItem);
				addItem(STItem); 
			}
			break;
			case STPhotoLayoutTemplate::Frame::TypeClipart : 
			{
				STGraphicsClipartItem* STItem = new STGraphicsClipartItem(QDir(Template.templateFilePath()).absoluteFilePath(it->clipartFileName()));
				STItem->setZValue(it->zValue());
				configureItem(STItem);
				if (it->width() > 0)
					STItem->scaleToWidth(it->width());
				else
					if (it->height() > 0)
						STItem->scaleToHeight(it->height());
				STItem->setPos(it->topLeft());
				//addItemOnTop(STItem);
				addItem(STItem);
			}	
			break; 
			default :
			{
				STGraphicsPhotoItem* PhotoItem = new STGraphicsPhotoItem(*it);
				PhotoItem->setAspectRatioMode(Template.aspectRatioMode()); 
				PhotoItem->setAutoAdjustFramesToImages(AutoAdjustFrames);
				PhotoItem->setIgnoreExifRotation(IgnoreExifRotation);
				//It must be set before mask
				PhotoItem->setFrameImage(QDir(Template.templateFilePath()).absoluteFilePath(it->frameFileName()));

				QImage MaskImage = it->maskImage(QDir(Template.templateFilePath())); 
				if (!MaskImage.isNull())
					PhotoItem->setAlphaChannel(MaskImage);
				//QImage FrameImage = it->frameImage(QDir(Template.templateFilePath()));
				//if (!FrameImage.isNull())
				//	PhotoItem->setFrameImage(FrameImage);

				PhotoItem->setZValue(it->zValue()); 
				configureItem(PhotoItem); 
				if (it->borderSize() > 0)
				{
					QPen Pen(it->borderColor()); 
					Pen.setWidth(it->borderSize()); 
					Pen.setStyle(Qt::SolidLine);
					Pen.setJoinStyle(Qt::MiterJoin);
					PhotoItem->setPen(Pen);
				}
				addPhotoItem(PhotoItem);
			}
		}
	}
	//Background
	if (_Template.backgroundcolor().isValid())
		setBgBrush(QBrush(_Template.backgroundcolor()));
	
	QString BGImgFile = _Template.backgroundImageAbsoluteFilePath();
	if (!BGImgFile.isEmpty())
	{
		QImage ThumbnailPix = getThumbnail(BGImgFile, Template.encrypted(), true);
		setBackgroundImage(ThumbnailPix, BGImgFile, Template.encrypted());
	}
	modified();
}


STPhotoLayoutTemplate STTemplateScene::getPageTemplate() const
{
	STPhotoLayoutTemplate Res; 
	if (PageItem)
	{
		Res.setSize(PageItem->boundingRect().size());
		Res.setBackgroundColor(PageItem->brush().color()); 
		if (!PageItem->imageFileName().isEmpty())
			Res.setBackgroundImageFile(PageItem->imageFileName());
	}
	QList<QGraphicsItem *> AllItems = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		if ((*it)->isVisible()) //Removed items exists but are not visible
		{
			if (STGraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			{
				STPhotoLayoutTemplate::Frame CFrame(QRectF(CPhotoItem->rect().topLeft() + CPhotoItem->scenePos(), CPhotoItem->boundingRect().size())); 
				CFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypePhoto);
				CFrame.setBorderColor(CPhotoItem->pen().color()); 
				CFrame.setBorderSize(CPhotoItem->pen().width());
				CFrame.setZValue(CPhotoItem->zValue()); 
				if (CPhotoItem->hasFrameImage())
					CFrame.setFrame(CPhotoItem->frameImageFileName(), CPhotoItem->frameImage());
				if (CPhotoItem->hasAlphaChannel())
					CFrame.setMask(CPhotoItem->alphaChannelFileName(), CPhotoItem->alphaChannel());
				Res.addFrame(CFrame);
			}
			else 
			{
				STPhotoLayoutTemplate::Frame CFrame(QRectF((*it)->pos(), (*it)->transform().mapRect((*it)->boundingRect()).size()));
				CFrame.setZValue((*it)->zValue()); 
					if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(*it))
					{
						//Extract rotation from item transformation if too hard, so lets negotiate it in a simple way.
						if (CItem->transform().isRotating())
						{
							CFrame = QRectF(CItem->pos(), CItem->boundingRect().size()); 
							CFrame.setRotationAngle(90);	
						}
						CFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeText);
						CFrame.setText(CItem->text());
						//CFrame.setTextColor(CItem->pen().color());
						CFrame.setTextColor(CItem->brush().color());  
						CFrame.setFont(CItem->font()); 
						CFrame.setBorderColor(CItem->borderPen().color()); 
						if (CItem->borderPen().style() != Qt::NoPen)
							CFrame.setBorderSize(CItem->borderPen().width());
						else
							CFrame.setBorderSize(0);
						Res.addFrame(CFrame); 
					}
				else 
					if (STGraphicsTextItem* CItem = qgraphicsitem_cast<STGraphicsTextItem*>(*it))
					{
						//Extract rotation from item transformation if too hard, so lets negotiate it in a simple way.
						if (CItem->transform().isRotating())
						{
							CFrame = QRectF(CItem->pos(), CItem->boundingRect().size()); 
							CFrame.setRotationAngle(90);	
						}
						CFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeRitchText);
						CFrame.setText(CItem->toHtml());
						CFrame.setFont(CItem->font()); 
						Res.addFrame(CFrame); 
					}
				else 
					if (STGraphicsMonthItem* CItem = qgraphicsitem_cast<STGraphicsMonthItem*>(*it))
					{
						CFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeCalMonth);
						CFrame.setMonth(CItem->month()); 
						CFrame.setFont(CItem->font()); 
						Res.addFrame(CFrame); 
					}
				else 
					if (STGraphicsClipartItem* CItem = qgraphicsitem_cast<STGraphicsClipartItem*>(*it))
					{
						CFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeClipart);
						CFrame.setClipartFileName(CItem->fileName());
						Res.addFrame(CFrame);
					}
			}
		}
	}	
	Res.setEncrypted(false);
	return Res; 
}


/*!
	Replaces the template and reuses current photoItems.	
*/
void STTemplateScene::replaceTemplate(const STPhotoLayoutTemplate& _Template)
{
	QFileInfoList OldIndexList; 
	QList<STGraphicsPhotoItem*> OldPhotoItems = photoItems();
	QList<STGraphicsPhotoItem*>::iterator it;
	for (it = OldPhotoItems.begin(); it != OldPhotoItems.end(); ++it)
	{
		if ((*it)->hasImage() && (*it)->isVisible())
			OldIndexList.push_back(QFileInfo((*it)->imageFileName()));
	}
	loadPageTemplate(_Template); 

	QList<STGraphicsPhotoItem*> PhotoItems = photoItems();
	int CntCurrPhoto = 0;
	//while (InsertedPhotos < TotalPhotos && CntCurrPhoto < PhWidgets.count())
	while (CntCurrPhoto < PhotoItems.count())
	{
		STGraphicsPhotoItem* CurrPhotoItem = PhotoItems.at(CntCurrPhoto);
		//!!!!! No podem treballar amb indexs perque el model canvia !!!!!!
		if (OldIndexList.size() > CntCurrPhoto)
		{
			STImage Img;

			STDom::DImageDoc CImageDoc(OldIndexList[CntCurrPhoto].absoluteFilePath());
			//Img.loadThumbnail(OldIndexList[CntCurrPhoto].absoluteFilePath());
			//CurrPhotoItem->setImage(QPixmap::fromImage(Img), OldIndexList[CntCurrPhoto].absoluteFilePath());
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
//			CurrPhotoItem->setIndex(static_cast<KPhotoListModel*>(OldIndexList[CntCurrPhoto].model()), OldIndexList[CntCurrPhoto]);
		
		CntCurrPhoto++;
	}
	modified();
}


void STTemplateScene::setImageToSelectedItems(const QPixmap& _ThumbNail, const QString& _ImageFileName)
{
	QList<QGraphicsItem *> Items = selectedItems(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		STGraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it); 
		if (!CPhotoItem)
			CPhotoItem = qgraphicsitem_cast<STGraphicsPageItem*>(*it); 
		if (CPhotoItem)
		{
			CPhotoItem->setThumbnail(_ThumbNail, _ImageFileName);
			CPhotoItem->loadImageSpawn();
			CPhotoItem->update();			
		}
	}
	modified();
}

void STTemplateScene::setDummyImages(const QList<QImage>& _ImageList)
{
	if (!_ImageList.isEmpty()) //Defensive 
	{
		QList<QGraphicsItem *> Items = items(); 
		QList<QGraphicsItem *>::iterator it; 
		for (it = Items.begin(); it != Items.end(); ++it)
		{
			if (STGraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			{
				
				CPhotoItem->setImage(_ImageList[STUtils::random(0, _ImageList.count())], "");
				//CPhotoItem->loadImageSpawn();
				CPhotoItem->update();			
			}
		}
	}
}

void STTemplateScene::setBackgroundImage(const QImage& _ThumbNail, const QString& _ImageFileName, bool _Encrypted)
{
	PageItem->setThumbnail(_ThumbNail, _ImageFileName);
	PageItem->setImageEncrypted(_Encrypted);
	PageItem->loadImageSpawn();
	PageItem->update();
}

bool STTemplateScene::hasBackgroundImage() const
{
	return PageItem->hasImage();
}

QImage STTemplateScene::getThumbnail(const QString& _ImageFileName, bool _Encrypted, bool _CreateIfNotExist)
{
	QSize ThumbnailSize(600, 400);
	QString ThumbnailFileName = STPhotoLayoutTemplate::thumbnailImage(_ImageFileName);
	QImage ThumbnailPix(ThumbnailFileName);

	if (ThumbnailPix.isNull())
	{
		if (_Encrypted)
		{
			STImage ThumbnailImage = STImage(_ImageFileName);
			ThumbnailImage.blowFishDecode();
			ThumbnailPix = ThumbnailImage.scaled(ThumbnailSize, Qt::KeepAspectRatio);
		}
		else
			ThumbnailPix = QImage(_ImageFileName).scaled(ThumbnailSize, Qt::KeepAspectRatio);

		if (_CreateIfNotExist)
			ThumbnailPix.save(ThumbnailFileName, "PNG");
		//!!! save it as png and do the same with backgrounds !
	}
	return ThumbnailPix;
}

void STTemplateScene::setSuperImposeImage(const QString& _ImageFileName, bool _Encrypted)
{
	QImage ThumbnailPix = getThumbnail(_ImageFileName, _Encrypted, true);
	if (!SImposeItem)
	{

		SImposeItem = new STGraphicsSuperImposeItem(sceneRect());
	//QGraphicsItem* PageItem = addRect(sceneRect(), QPen(), Qt::white); //For page background color.
		configureItem(SImposeItem); 
		addItem(SImposeItem);
	}
	SImposeItem->setZValue(SImposeZValue);
	SImposeItem->setImageEncrypted(_Encrypted);
	SImposeItem->setThumbnail(ThumbnailPix, _ImageFileName);
	//SImposeItem->loadImageSpawn();
	//SImposeItem->update();
}

void STTemplateScene::setBgBrush(const QBrush& _Brush)
{
	//PageItem->unloadImage();	
	PageItem->setBrush(_Brush);
	modified();
}

QBrush STTemplateScene::bgBrush() const
{
	return PageItem->brush();
}

void STTemplateScene::clear()
{
	PageItem = 0;
	SImposeItem = 0; 
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

QList<STGraphicsPhotoItem *> STTemplateScene::photoItems() const 
{
	QList<STGraphicsPhotoItem *> Res; 
	QList<QGraphicsItem *> AllItems = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		if (STGraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			Res.push_back(CPhotoItem);
	}	
	
	return Res;
}

void STTemplateScene::loadHiResImages(STGraphicsPhotoItem* _Item)
{
	if (!_Item->imageFileName().isEmpty()) ///!!!!!!!!!!!!!!!!!!!!!! No hauria pas de produir-se amb els templates de calendaris...
	{
		_Item->setImageMode(STGraphicsPhotoItem::HiResImageMode);
		_Item->unloadImage();
		//_Item->waitForImageLoaded();
	}
}

void STTemplateScene::loadHiResImages(QProgressBar* _Progress)
{
	//If there is page background image: 
	if (PageItem)
		loadHiResImages(PageItem);
	if (SImposeItem)
		loadHiResImages(SImposeItem);
	
	QList<STGraphicsPhotoItem *> PhotoItems = photoItems(); 
	QList<STGraphicsPhotoItem *>::iterator it;
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
// 		(*it)->setImageMode(STGraphicsPhotoItem::HiResImageMode);
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

void STTemplateScene::unloadImage(STGraphicsPhotoItem* _Item)
{
	_Item->setImageMode(STGraphicsPhotoItem::LowResImageMode);
	_Item->unloadImage();
}
		
void STTemplateScene::unloadImages()
{
	if (PageItem)
		unloadImage(PageItem);
	if (SImposeItem)
		unloadImage(SImposeItem); 

	QList<STGraphicsPhotoItem *> PhotoItems = photoItems(); 
	QList<STGraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		unloadImage(*it);
	}
}

void STTemplateScene::clearImages()
{
	QList<STGraphicsPhotoItem *> PhotoItems = photoItems();
	QList<STGraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		(*it)->clearImage();
	}
}


void STTemplateScene::updateImage(STGraphicsPhotoItem* _Item)
{
	_Item->loadImageSpawn();
	_Item->update();
}
		
void STTemplateScene::updateImages()
{
	if (PageItem)
		updateImage(PageItem); 
	if (SImposeItem)
		updateImage(SImposeItem); 

	QList<STGraphicsPhotoItem *> PhotoItems = photoItems(); 
	QList<STGraphicsPhotoItem *>::iterator it;
	for (it = PhotoItems.begin(); it != PhotoItems.end(); ++it)
	{
		updateImage(*it); 
	}
}

void STTemplateScene::setEmptyFramesVisible(bool _Value)
{
	QList<STGraphicsPhotoItem*> OldPhotoItems = photoItems();
	QList<STGraphicsPhotoItem*>::iterator it;
	for (it = OldPhotoItems.begin(); it != OldPhotoItems.end(); ++it)
	{
		if (!(*it)->hasImage())
			(*it)->setVisible(_Value); 
	}
}

void STTemplateScene::setWarningsVisible(bool _Visible)
{
	if (PageItem) //Defensive
		PageItem->setWarningVisible(_Visible);

	QList<QGraphicsItem *>::iterator it;
	QList<QGraphicsItem *> AllItems = items();
	for (it = AllItems.begin(); it != AllItems.end(); ++it)
	{
		if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			CItem->setWarningVisible(_Visible);
	}
}

/*!
	Prepares scene to print. Unselects all items ...
*/
void STTemplateScene::prepareForPrint()
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

void STTemplateScene::addRandomPhotoFrame()
{
	STPhotoLayoutTemplate::TFrameList FrameList = Template.frames();

	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{		
		if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			FrameList.push_back(STPhotoLayoutTemplate::Frame(CItem->rect()));
	}
	bool Found = false; 
	STGraphicsPhotoItem* PhotoItem = 0; 
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
			PhotoItem = new STGraphicsPhotoItem(CFrame);
		}
	}
	if (!Found)
	{
		PhotoItem = new STGraphicsPhotoItem;
		PhotoItem->setRect(10, 10, 46, 35); 
	}	
	if (PhotoItem) //Defensive 
	{
		PhotoItem->setZValue(topZValue() + 1);
		addPhotoItem(PhotoItem);
		//PhotoItem->setPanningEnabled(false);
	}
}

void STTemplateScene::addPhotoItem(STGraphicsPhotoItem* _PhotoItem)
{
	_PhotoItem->setPanningEnabled(!ItemsMovable);
	_PhotoItem->setMultiSelection(Template.modifyAllFrames());
	configureItem(_PhotoItem); 
	connect(_PhotoItem, SIGNAL(mousePanning(const QPointF&)), this, SLOT(panSelectedPhotoItems(const QPointF&)));
	connect(_PhotoItem, SIGNAL(imageDropped(const QString&, const QString&)), this, SIGNAL(imageDropped(const QString&, const QString&)));
	connect(_PhotoItem, SIGNAL(imageListDropped(QList<QUrl>)), this, SIGNAL(imageListDropped(QList<QUrl>)));
	connect(_PhotoItem, SIGNAL(imageRemoved(const QString&, const QString&)), this, SIGNAL(imageRemoved(const QString&, const QString&)));

	QGraphicsScene::addItem(_PhotoItem);
	modified();
}

void STTemplateScene::setPageItem(STGraphicsPageItem* _PageItem)
{
	PageItem = _PageItem; 
	if (_PageItem) //Defensive 
	{
		connect(_PageItem, SIGNAL(imageDropped(const QString&, const QString&)), this, SIGNAL(imageDropped(const QString&, const QString&)));
		connect(_PageItem, SIGNAL(imageListDropped(QList<QUrl>)), this, SIGNAL(imageListDropped(QList<QUrl>)));
		connect(_PageItem, SIGNAL(imageRemoved(const QString&, const QString&)), this, SIGNAL(imageRemoved(const QString&, const QString&)));
	}
}

void STTemplateScene::deletePageItem()
{
	if (PageItem)
		delete PageItem; 
	PageItem = 0; 
}

QGraphicsItem* STTemplateScene::addSimpleTextFrame(const QString& _Text, const QFont& _Font)
{
	STPhotoLayoutTemplate::Frame TFrame(QRectF(10, 10, 0, 0));
	TFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeText); 
	TFrame.setText(_Text);
	TFrame.setFont(_Font); 
	STGraphicsSimpleTextItem* NewFrame = new STGraphicsSimpleTextItem(TFrame);
	//NewFrame->setFlag(QGraphicsItem::ItemIsMovable, ItemsMovable);
	NewFrame->setFlag(QGraphicsItem::ItemIsMovable, true); //The text is always movable.
	configureItem(NewFrame); 
	addItemOnTop(NewFrame);	
	modified();
	return NewFrame;
}

QGraphicsItem* STTemplateScene::addClipartItem(const QString& _FileName)
{
	STGraphicsClipartItem* NewItem = new STGraphicsClipartItem(_FileName);
	configureItem(NewItem); 
	//NewItem->scaleToWidth(75); 
	NewItem->scaleToHeight(75); 
	addItemOnTop(NewItem);	
	modified();
	return NewItem;
}

QGraphicsItem* STTemplateScene::addTextFrame(const QString& _Html)
{
	STPhotoLayoutTemplate::Frame TFrame(QRectF(0, 0, 0, 0));
	TFrame.setFrameType(STPhotoLayoutTemplate::Frame::TypeText); 
	TFrame.setText(_Html);
	TFrame.setFont(TextFrameFont); 
	STGraphicsTextItem* NewFrame = new STGraphicsTextItem(TFrame);
	//NewFrame->setMovable(ItemsMovable);
	NewFrame->setMovable(true); //The text items are always movable.
	configureItem(NewFrame); 
	addItemOnTop(NewFrame);	
	NewFrame->adjustSize();
	modified();
	return NewFrame;
}

STGraphicsMonthItem* STTemplateScene::createMonthTextFrameItem()
{
	STGraphicsMonthItem* NewItem = new STGraphicsMonthItem;
	NewItem->setMovable(true);
	configureItem(NewItem); 
	addItemOnTop(NewItem);
	modified();
	return NewItem;
}

QGraphicsItem* STTemplateScene::addElement(const QString& _ImageSourcePath, QDomElement& _Element)
{
	STAbstractGraphicsItem* CItem;
	QGraphicsItem* GrItem = 0;
	if (_Element.tagName().toLower() ==  STGraphicsPhotoItem::tagName())
	{
		STGraphicsPhotoItem* NPItem = new STGraphicsPhotoItem;
		NPItem->setImageSourcePath(_ImageSourcePath);
		NPItem->loadElement(_Element);
		NPItem->setZValue(_Element.attribute("zvalue").toDouble());
		addPhotoItem(NPItem );
		GrItem = NPItem;  
	}
	else 
	{
		CItem = 0; 
		GrItem = 0; 
		if (_Element.tagName().toLower() ==  STGraphicsSimpleTextItem::tagName() )
		{
/*			STGraphicsSimpleTextItem* NItem = new STGraphicsSimpleTextItem;
			configureItem(NItem); 
			CItem = NItem; 
			GrItem = NItem;*/
			STGraphicsSimpleTextItem STItem;
			STItem.loadElement(_Element);

			STGraphicsTextItem* NItem = new STGraphicsTextItem;
			NItem->document()->setPlainText(STItem.text());
			//NItem->document()->setCharFormat(STItem.textCharFormat());

			QTextCursor cursor(NItem->document());
			cursor.select(QTextCursor::Document);
			cursor.mergeCharFormat(STItem.textCharFormat());
			configureItem(NItem);
			NItem->setPos(STItem.pos());
			NItem->setZValue(_Element.attribute("zvalue").toDouble());
			addItem(NItem);
			//CItem = NItem;
			//GrItem = NItem;

		}
		else
		if (_Element.tagName().toLower() ==  STGraphicsClipartItem::tagName() )
		{
			//FIXME _ImageSourcePath + "/" + _Element.attribute("src")-> El load de la imatge hauria d'estar a l'item.
			QString ClipartFileName = _ImageSourcePath + "/" + _Element.attribute("src"); 
			
			if (QFile::exists(ClipartFileName))
			{
				QFileInfo FInfo(ClipartFileName); 
				if (FInfo.isFile())
				{
					STGraphicsClipartItem* NItem = new STGraphicsClipartItem(ClipartFileName);
					configureItem(NItem); 
					CItem = NItem; 
					GrItem = NItem;
				}
				else 
					qWarning(QString("Clipart filename is not a file: %1 ").arg(ClipartFileName).toLatin1()); 
			}
			else 
				qWarning(QString("Clipart filename Not Found: %1 ").arg(ClipartFileName).toLatin1()); 
		}
		else
		if (_Element.tagName().toLower() ==  STGraphicsTextItem::tagName())
		{
			STGraphicsTextItem* NItem = new STGraphicsTextItem;
			configureItem(NItem); 
			CItem = NItem; 
			GrItem = NItem;
		}
		else
		if (_Element.tagName().toLower() == STGraphicsSuperImposeItem::tagName())
		{
			if (SImposeItem)
			{
				removeItem(SImposeItem);
				delete SImposeItem;
			}
			SImposeItem = new STGraphicsSuperImposeItem(sceneRect());
			SImposeItem->setImageSourcePath(_ImageSourcePath);
			SImposeItem->loadElement(_Element);
			SImposeItem->setZValue(_Element.attribute("zvalue").toDouble());
			addItem(SImposeItem);
			GrItem = SImposeItem;
		}
		
		if (CItem && GrItem)
		{
			CItem->loadElement(_Element);
			GrItem->setZValue(_Element.attribute("zvalue").toDouble());
			addItem(GrItem); 
		}
	}
	return GrItem; 
}

void STTemplateScene::loadElement(const QString& _ImageSourcePath, QDomElement& _SceneElement)
{
	deletePageItem(); 
	
	ModifyAllFrames = _SceneElement.attribute("modifyallframes", "0").toLower() == "1";
	RenderBaseSize.setWidth(_SceneElement.attribute("basewidth","0").toDouble());
	RenderBaseSize.setHeight(_SceneElement.attribute("baseheight","0").toDouble());

	//Search first the Page item
	QDomNode CNode = _SceneElement.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() ==  STGraphicsPageItem::tagName() )
			{
				STGraphicsPageItem* NewPageItem = new STGraphicsPageItem;
				NewPageItem->setImageSourcePath(_ImageSourcePath);
				NewPageItem->loadElement(CEl); 
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
			addElement(_ImageSourcePath, CEl);
		 
		CNode = CNode.nextSibling();
	}	
}

QDomElement STTemplateScene::createElement(QDomDocument& _Doc) 
{
	QDomElement SceneEl = _Doc.createElement("scene");
	SceneEl.setAttribute("modifyallframes", ModifyAllFrames);
	SceneEl.setAttribute("basewidth", RenderBaseSize.width());
	SceneEl.setAttribute("baseheight", RenderBaseSize.height());

	//PageItem
	SceneEl.appendChild(PageItem->createElement(_Doc));

	//All the items 	
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
		{
			STAbstractGraphicsItem* CItem = dynamic_cast<STAbstractGraphicsItem*>(*it);
			if (CItem && CItem != PageItem)//FIXME it crash for non STAbstractGraphicsItems !!!.
			{
				QDomElement NewEl = CItem->createElement(_Doc);
				NewEl.setAttribute("zvalue",(*it)->zValue());
					SceneEl.appendChild(NewEl); 
			}
		}
	}
	return SceneEl; 
}

qreal STTemplateScene::topZValue() const
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

qreal STTemplateScene::bottomZValue() const
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

void STTemplateScene::configureItem(STAbstractGraphicsItem* _Item)
{
	_Item->setResizeAllowed(ItemsResizable);
}

void STTemplateScene::addItemOnTop(QGraphicsItem* _Item)
{
	_Item->setZValue(topZValue());
	addItem(_Item);
	modified();
}

QGraphicsItem* STTemplateScene::currentItem() const
{
	QGraphicsItem* Res = 0; 
	//Res = focusItem();
	if (!Res && selectedItems().size() > 0)
		Res = selectedItems().first();

	if (!Res)
		Res = focusItem();

	return Res; 
}

void STTemplateScene::selectNone()
{
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		(*it)->setSelected(false); 
	}
}

void STTemplateScene::selectAll()
{
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		(*it)->setSelected(true); 
	}
}

void STTemplateScene::setAllControlsVisible(bool _Value)
{
	QFileInfoList OldIndexList; 
	QList<STGraphicsPhotoItem*> OldPhotoItems = photoItems();
	QList<STGraphicsPhotoItem*>::iterator it;
	for (it = OldPhotoItems.begin(); it != OldPhotoItems.end(); ++it)
	{
		(*it)->setControlsVisible(_Value); 
	}
}

void STTemplateScene::selectAllByType(int _Type)
{
	QList<QGraphicsItem *> Items = items(); 
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->type() == _Type)
			(*it)->setSelected(true);
	}
}

QList<QGraphicsItem *> STTemplateScene::selectedItemsOfType(int _Type)
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
bool STTemplateScene::containsImage(const QString& _ImageMD5Sum)
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it = Items.begin(); 
	bool Found = false; 
	while (it != Items.end() && !Found)
	{
		if ((*it)->isVisible())	
		{
			if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
				Found = CItem->imageMD5Sum() == _ImageMD5Sum; 
			if (STGraphicsPageItem* CItem = qgraphicsitem_cast<STGraphicsPageItem*>(*it))
				Found = CItem->imageMD5Sum() == _ImageMD5Sum; 
		}
		++it; 
	}
	return Found; 
}

//! \return true if photobook contains some photoitem with no image.
bool STTemplateScene::hasEmptyPhotoItems() const
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it = Items.begin(); 
	bool Found = false; 
	while (it != Items.end() && !Found)
	{
		if ((*it)->isVisible())	
			if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
				Found = !CItem->hasImage(); 
	
		++it; 
	}
	return Found; 
}

int STTemplateScene::numPhotoItems() const
{
	int Res = 0;
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
			if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
				++Res;

	}
	return Res;
}

int STTemplateScene::numEmptyPhotoItems() const
{
	int Res = 0;
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it;
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if ((*it)->isVisible())
			if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
				if (!CItem->hasImage())
					++Res;
	}
	return Res;
}

void STTemplateScene::selectFirstEmptyPhotoItem()
{
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it = Items.begin(); 
	bool Found = false; 
	while (it != Items.end() && !Found)
	{
		if ((*it)->isVisible())	
			if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
				Found = !CItem->hasImage(); 
		if (Found)
			(*it)->setSelected(true); 	
		++it; 
	}
}

void STTemplateScene::modified()
{
	HasChanges = true;
}

void STTemplateScene::clearChanges()
{
	HasChanges = false;
}

void STTemplateScene::autoFillImages(const QFileInfoList& _Images)
{
	QList<QUrl> UrlList;
	QFileInfoList::const_iterator it;
	for (it = _Images.begin(); it != _Images.end(); ++it)
	{
		UrlList.push_back(QUrl::fromLocalFile(it->absoluteFilePath()));
	}
	slotImageListDropped(UrlList);
}


/*!
	\return a list of saved images.
*/
QStringList STTemplateScene::storePhotoItemImages(const STPhotobookCollectionInfo& _CInfo, bool _OnlyDesignImages)
{
	QStringList StoredFiles; 
	if (PageItem)
		StoredFiles += storePhotoItemImage(PageItem, _CInfo); 
	if (SImposeItem)
		StoredFiles += storePhotoItemImage(SImposeItem, _CInfo);

	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{	
		if ((*it)->isVisible())	
		{
			if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			{
				StoredFiles += storePhotoItemImage(CItem, _CInfo, _OnlyDesignImages);
			}
			else 
			if (STGraphicsClipartItem* CItem = qgraphicsitem_cast<STGraphicsClipartItem*>(*it))
				StoredFiles += storeClipartItemFile(CItem, _CInfo); 
		}
	}
	return StoredFiles; 
}

// void STTemplateScene::performSelectedItemsOp(STGraphicsItemOperation* _Operation)
// {
// 	QList<QGraphicsItem *> Items = selectedItems();
// 	QList<QGraphicsItem *>::iterator it; 
// 	for (it = Items.begin(); it != Items.end(); ++it)
// 	{
// 		_Operation->performOp(*it);
// 		(*it)->update();
// 	}
// }

void STTemplateScene::setItemsResizable(bool _Resizable)
{
	ItemsResizable = _Resizable; 
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		if (STAbstractGraphicsItem* CItem = dynamic_cast<STAbstractGraphicsItem*>(*it))
			CItem->setResizeAllowed(_Resizable);
	}
}


void STTemplateScene::setItemsMovable(bool _Movable)
{
	ItemsMovable = _Movable; 
	QList<QGraphicsItem *> Items = items();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		//TODO Unificar-ho amb setMovable per una base comú ?
		if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			CItem->setPanningEnabled(!_Movable);
// 		else 
// 		if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(*it))
// 			CItem->setFlag(QGraphicsItem::ItemIsMovable, _Movable);
// 		else 
// 		if (STGraphicsTextItem* CItem = qgraphicsitem_cast<STGraphicsTextItem*>(*it))
// 			CItem->setMovable(_Movable);
	}
}


bool STTemplateScene::event(QEvent* _Event)
{
	
	if (_Event->type() == STUpdateItemEvent::STUpdateItemEventType)
	{
		STUpdateItemEvent* CEvent = static_cast<STUpdateItemEvent*>(_Event);
		if (!CEvent->item().isNull())
			CEvent->item()->update();
		return true;
	}
	return QGraphicsScene::event(_Event);
}

void STTemplateScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _Event)
{
	emit doubleClicked();
	QGraphicsScene::mouseDoubleClickEvent(_Event); 
}

void STTemplateScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _Event)
{
	emit clicked();
	QGraphicsScene::mouseReleaseEvent(_Event); 
}

void STTemplateScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* _ContextMenuEvent)
{
	QGraphicsItem* Item = itemAt(_ContextMenuEvent->scenePos());
	
	if (Item)
		emit itemContextMenu(Item, _ContextMenuEvent->screenPos());
}

// Drag and Drop
void STTemplateScene::dropEvent(QGraphicsSceneDragDropEvent* _Event )
{
	bool Propagate = true;
	const STTemplateMimeData* MimeData = qobject_cast<const STTemplateMimeData*>(_Event->mimeData());
	if (MimeData)
	{
		emit templateDropped(this, MimeData->getTemplate());
		Propagate = false;
		//replaceTemplate(MimeData->getTemplate());
	}
	else
	if (_Event->mimeData()->hasUrls())
	{
		QList<QUrl> Urls = _Event->mimeData()->urls();

		QUrl FirstUrl = _Event->mimeData()->urls().first();
		if (!FirstUrl.isEmpty() && FirstUrl.isValid())
		{
			QString LocalPath = FirstUrl.toString();
			//Only local file uris are supported.
			QString FilePath = FirstUrl.toLocalFile();
			if (!FilePath.isEmpty())
			{
				QFileInfo FilePathInfo(FilePath);
				if (FilePathInfo.completeSuffix().toLower().contains("svg"))
				{
					emit clipartDropped(FilePath, _Event->scenePos());
					Propagate = false;
				}
			}
		}
	}
	if (Propagate)
		QGraphicsScene::dropEvent(_Event);
	_Event->accept();
}


void STTemplateScene::panSelectedPhotoItems(const QPointF& _Point)
{
	//if (!ItemsResizable)
	//{
	QList<QGraphicsItem *> Items = selectedItems();
	QList<QGraphicsItem *>::iterator it; 
	for (it = Items.begin(); it != Items.end(); ++it)
	{
		//TODO Unificar-ho amb setMovable per una base comú ?
		if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it))
			CItem->panImage(_Point); 
	}
	//}
}

void STTemplateScene::slotCheckModifyAll(const QString& _ImagePath)
{
	if (ModifyAllFrames)
	{
		selectAllByType(STGraphicsPhotoItem::Type);
		QList<QGraphicsItem *> Items = selectedItems();
		QList<QGraphicsItem *>::iterator it;

		STDom::DImageDoc Doc(_ImagePath);
		for (it = Items.begin(); it != Items.end(); ++it)
		{
			STGraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(*it);
			if (CPhotoItem)
			{
				CPhotoItem->setImage(Doc);
				CPhotoItem->loadImageSpawn();
			}
		}

	}
}

void STTemplateScene::slotImageListDropped(const QList<QUrl>& _Urls)
{
	selectAllByType(STGraphicsPhotoItem::Type);
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
				STGraphicsPhotoItem* CPhotoItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Items[CntItem]);
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
