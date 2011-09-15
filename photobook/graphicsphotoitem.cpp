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

#include "graphicsphotoitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QFileInfo> 
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent> 
#include <QGraphicsWidget> 
#include <QFontMetrics>
#include <QImage>
#include <QDir>
#include <QDomNode>
#include <QDebug>

//Drag And Drop
#include <QMimeData> 
#include <QUrl>
#include "resourcemimedata.h"


//Controls 
#include <QToolBar> 
#include <QAction> 
#include <QGraphicsProxyWidget> 


#include "updateitemevent.h"
#include "stimage.h"
#include "imageTools.h" 
#include "dimagedoc.h"
//#include "kphoto.h"
//#include "ophotocollection.h" // For OPhotoCollectionImage.

#include "graphicsitemmodifier.h"

using namespace SPhotoBook;

// _________________________________________________________________________*/
//
//   Class GraphicsPhotoItemLoadThread
// _________________________________________________________________________*/


QMutex GraphicsPhotoItemLoadThread::Mutex;
	
GraphicsPhotoItemLoadThread::GraphicsPhotoItemLoadThread(GraphicsPhotoItem* _RelatedItem) : CurrItem(_RelatedItem)
{}


void GraphicsPhotoItemLoadThread::run()
{
	setPriority(QThread::LowPriority);
	Mutex.lock();
	if (!CurrItem.isNull()) //Defensive
		CurrItem->loadImage();
	Mutex.unlock();
	if (!CurrItem.isNull()) //Defensive
		QApplication::postEvent(CurrItem->scene(), new UpdateItemEvent(CurrItem));
}

void GraphicsPhotoItemLoadThread::getPreview()
{
	if (isRunning())
		wait(1500);
	start();
}


// _________________________________________________________________________*/
//
//   Class GraphicsPhotoItem
// _________________________________________________________________________*/

QBrush GraphicsPhotoItem::SelectedBrush(QColor(255,255,255, 150), Qt::Dense6Pattern);


QSize GraphicsPhotoItem::MaxLowResImageSize(100, 100);
int GraphicsPhotoItem::LowResImageDPIs = 72;
QString GraphicsPhotoItem::LowResWarningImage(":/phototemplates/dialog-warning.png");
int GraphicsPhotoItem::LowResMinDpis = 100;
bool GraphicsPhotoItem::TouchInterface = false;
bool GraphicsPhotoItem::ThresholdMoving = true;
bool GraphicsPhotoItem::OldThresholdMoving;
bool GraphicsPhotoItem::OldSnapToGrid;
bool GraphicsPhotoItem::DefaultNoImageMessage = false;

QSize GraphicsPhotoItem::imageWindowDpis(const QSizeF& _ItemRectInmm, const QSize& _ImageClipSize, const QSize& _PaintedImageSize, const QSize& _OriginalImageSize)
{
	QSize Res;

	if (_PaintedImageSize.width() > 0 && _PaintedImageSize.height() > 0)
	{
		//Calculate clipRect in original image size;
		double ClipRectZoomX = static_cast<double>(_OriginalImageSize.width()) / _PaintedImageSize.width();
		double ClipRectZoomY = static_cast<double>(_OriginalImageSize.height()) / _PaintedImageSize.height();
		STDom::DDocFormat Format(_ItemRectInmm.width(), _ItemRectInmm.height());
		QSize BigClipRect(_ImageClipSize.width() * ClipRectZoomX, _ImageClipSize.height() * ClipRectZoomY);

		Res = Format.dpis(BigClipRect);
	}
	return Res;
}

void GraphicsPhotoItem::createPaintedImage(const Qt::TransformationMode&  _CTransformMode, const QRect& _MaxResRect)
{
	PaintedImage = CurrImage.transformed(ImageMatrix, _CTransformMode);
	PaintedImage = PaintedImage.scaled(QSize(_MaxResRect.width() * CurrScale, _MaxResRect.height() * CurrScale), AspectRatioMode, _CTransformMode);
	//If Mask
	if (CurrScale < 1.0 && !MaskImage.isNull())
		PaintedImage.setAlphaChannel(MaskImage.scaled(PaintedImage.size()));
}

void GraphicsPhotoItem::updateToolTip()
{
	//QPointF Pos = rect().topLeft() + scenePos();
	QPointF Pos = rectPos();
	QSizeF Size = boundingRect().size();
	AbstractGraphicsItem::updateToolTip(QRectF(Pos, Size));
}

void GraphicsPhotoItem::updateCursor()
{
	if (PanningEnabled)
		setCursor(Qt::OpenHandCursor); 
	else 
		setCursor(Qt::SizeAllCursor);
}

void GraphicsPhotoItem::init()
{
	PanAct = 0; 
	AspectRatioMode = Qt::KeepAspectRatioByExpanding; 
	CurrScale = 1.0;
	Opacity = 1.0; 
	ImageModified = false; 
	PanningEnabled = false; 
	ImageLoaded = false; 
	WarningVisible = true;
	setShowNoImageMessage(DefaultNoImageMessage);
	ImageEncrypted = false;
	IgnoreExifRotation = false;
	OrientationChanged = false;

	//QColor EmptyFrameColor("#FFD95C");
	EmptyFrameColor = QColor("#BBBBBB");
	EmptyFrameColor.setAlpha(150);
	//EmptyFrameColor.setAlpha(0);
	setBrush(QBrush(EmptyFrameColor, Qt::SolidPattern));

	#if QT_VERSION >= 0x040600
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemClipsToShape | QGraphicsItem::ItemUsesExtendedStyleOption);
	#else
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemClipsToShape);
	#endif
	MLoadThread = new GraphicsPhotoItemLoadThread(this);
	setPanningEnabled(true);
	setImageMode(LowResImageMode);
	QPen Pen = pen();
	//Pen.setJoinStyle(Qt::MiterJoin);
	//Pen.setCapStyle(Qt::SquareCap);
	Pen.setWidth(0);
	Pen.setStyle(Qt::NoPen);
	
	setPen(Pen);
	setAcceptDrops(true); 
	RestoreZValue = false;
	setAutoAdjustFramesToImages(true);

}

QPointF GraphicsPhotoItem::insideSceneRect(const QPointF& _Point)
{

	QPointF Res = _Point;
	QRectF rect = scene()->sceneRect();
	if (!rect.contains(Res))
	{
		// Keep the item inside the scene rect.
		Res.setX(qMin(rect.right(), qMax(Res.x(), rect.left())));
		Res.setY(qMin(rect.bottom(), qMax(Res.y(), rect.top())));
	}
	return Res;
}

void GraphicsPhotoItem::checkForImageOrientation()
{
	if (!AutoAdjustFramesToImages)
	{
		QRect CurrImageRect = ImageMatrix.mapRect(CurrImage.rect());
		if ( AspectRatioMode == Qt::KeepAspectRatio && ((CurrImageRect.width() > CurrImageRect.height() && rect().width() < rect().height()) ||
				(CurrImageRect.height() > CurrImageRect.width() && rect().height() < rect().width())))
			rotateImage(90);
	}
}


GraphicsPhotoItem::GraphicsPhotoItem(QGraphicsItem* _Parent) : QGraphicsRectItem(_Parent), AbstractGraphicsItem(this)
{
	init();
	updateToolTip();
	createStandardCorners();
	setControlsVisible(false);
}

GraphicsPhotoItem::~GraphicsPhotoItem()
{
	if (MLoadThread->isRunning())
		MLoadThread->wait();
	delete MLoadThread;
}

AbstractGraphicsItem* GraphicsPhotoItem::clone() const
{
	GraphicsPhotoItem* Res = new GraphicsPhotoItem;
	QDomDocument Doc;
	Res->loadElement(createElement(Doc));
	return Res;
}

QStringList GraphicsPhotoItem::saveResources(const QDir& _StoreDir, bool _SaveImageRes)
{
	qDebug() << "------ QStringList GraphicsPhotoItem::saveResources(const QDir& _StoreDir, bool _SaveImageRes)";
	QStringList Res;
	if (!ImageResource.isNull() && _SaveImageRes)
		Res << ImageResource.save(_StoreDir);
	if (!FrameResource.isNull())
		Res << FrameResource.save(_StoreDir); //This will save frameMask also
	else
		if (!MaskResource.isNull())
			Res << MaskResource.save(_StoreDir);
	return Res;
}


void GraphicsPhotoItem::setResource(const Resource& _Resource)
{
	if (_Resource.type() == Resource::TypeFrameMask)
	{
		setAlphaChannel(_Resource);
	}
	else
	if (_Resource.type() == Resource::TypeFrame)
	{
		setFrameResource(_Resource);
	}
}

void GraphicsPhotoItem::setAlphaChannel(const Resource& _Resource, const QImage& _AlphaChannel)
{
	if (!_Resource.isNull())
		MaskImage = _AlphaChannel;
	else
		MaskImage = QImage();

	MaskResource = _Resource;

	QBrush CurrBrush = brush();
	QColor EFColor = EmptyFrameColor;
	if (MaskImage.isNull())
		EFColor.setAlpha(150);
	else
		EFColor.setAlpha(0);
	CurrBrush.setColor(EFColor);
	setBrush(CurrBrush);

	update();
	modified();
}

void GraphicsPhotoItem::setAlphaChannel(const Resource& _Resource)
{
	setAlphaChannel(_Resource, QImage(_Resource.fileInfo().absoluteFilePath()));
}

void GraphicsPhotoItem::setFrameResource(const Resource& _Resource)
{
	FrameResource = _Resource;
	if (!_Resource.isNull())
	{
		QString FrameFilePath = _Resource.fileInfo().absoluteFilePath();
		if (!QFile::exists(FrameFilePath))
			return;

		FrameImage = QImage(FrameFilePath);

		if (FrameImage.isNull())
			return;

		QRectF BRect = boundingRect();
		QMatrix Matrix;
		if ((BRect.width() > BRect.height() && FrameImage.width() < FrameImage.height())
			|| (BRect.width() < BRect.height() && FrameImage.width() > FrameImage.height()))
		{
			Matrix.rotate(90);
			FrameImage = FrameImage.transformed(Matrix);
		}

		QFileInfo FrameMaskFile = Resource::frameMaskFile(_Resource);
		if (FrameMaskFile.exists())
			setAlphaChannel(Resource(FrameMaskFile), QImage(FrameMaskFile.absoluteFilePath()).transformed(Matrix));
		else
			setAlphaChannel(Resource());//No Alpha Channel
	}
	else
	{
		setAlphaChannel(Resource());
		FrameImage = QImage();
	}

	update();
}

QImage GraphicsPhotoItem::originalPaintedImage()
{
	createPaintedImage(Qt::SmoothTransformation, LastMaxResRect);
	return PaintedImage;
}

void GraphicsPhotoItem::updatePaintedImage(const QImage& _Image)
{
	PaintedImage = _Image;
	update();
}

void GraphicsPhotoItem::setImage(const QImage& _Image, const QString& _ImageFileName)
{
	ImageLoaded = true;
	if (!CurrImage.isNull())
		ImageModified = true;
	setImageFileName(_ImageFileName);
	CurrImage = _Image;
	LastMaxResRect = QRect();
	checkForImageOrientation();
	update();
	updateCursor();
}

void GraphicsPhotoItem::setImageFileName(const QString& _ImageFileName)
{
	if (!ImageResource.isNull() && !ImageMD5Sum.isEmpty())
		emit imageRemoved(ImageResource.fileInfo().absoluteFilePath(), ImageMD5Sum);
	if (_ImageFileName.isEmpty())
		ImageResource = Resource();
	else
	{
		ImageResource = Resource(QFileInfo(_ImageFileName), Resource::TypeImage);
		ImageMD5Sum = STImage::hashString(_ImageFileName);
	}
	modified();
}

void GraphicsPhotoItem::setImage(STDom::DImageDoc& _Image)
{
	QPixmap Thumbnail = _Image.thumbnail();
	bool ImageAssigned = false;
	if (!IgnoreExifRotation)
	{
		STDom::DImageDoc::CInfo ImageInfo = _Image.getInfo();
		if (!ImageInfo.isNull())
		{
			ImageMatrix.reset();
			QTransform Transform;
			switch (ImageInfo.orientation())
			{
				case ExifMetadata::Orientation_Right_Bottom:
				case ExifMetadata::Orientation_Left_Bottom :
				{
					rotateImage(-90);
					Transform.rotate(90);
				}
				break;

				case ExifMetadata::Orientation_Left_Top :
				case ExifMetadata::Orientation_Right_Top :
				{
					rotateImage(90);
					Transform.rotate(-90);
				}
				break;

				case ExifMetadata::Orientation_Bottom_Left :
				case ExifMetadata::Orientation_Bottom_Right :
				{
					rotateImage(-180);
					Transform.rotate(180);
				}
			}
			// Thumbnail is already rotated.
			Thumbnail = Thumbnail.transformed(Transform, Qt::SmoothTransformation);
			setThumbnail(Thumbnail, _Image.fileInfo().absoluteFilePath());
			if (AutoAdjustFramesToImages)
			{
				QSize ImgSize;
				if (!ImageInfo.size().isNull())
					ImgSize = ImageInfo.size();
				else
					ImgSize = Thumbnail.size();
				if (!ImgSize.isNull())
					adjustRectToImage(ImgSize);
			}
			ImageAssigned = true;
		}
		else
			qDebug("Image Info is Null :( .....");
	}
	if (!ImageAssigned)
		setThumbnail(Thumbnail, _Image.fileInfo().absoluteFilePath());

	update();
}

//! Obsolete, provided for compatibility reasons.
void GraphicsPhotoItem::setThumbnail(const QPixmap& _ThumbNail, const QString& _ImageFileName)
{
	setThumbnail(_ThumbNail.toImage(), _ImageFileName);
}

//@Revision Mark
void GraphicsPhotoItem::setThumbnail(const QImage& _ThumbNail, const QString& _ImageFileName)
{
	//CurrImage = QImage(_ImageFileName);
	ImageLoaded = false;
	Opacity = 1;
	PanningPoint = QPoint(0, 0);
	bool ChangingImage = !CurrImage.isNull();
	CurrImage = _ThumbNail;
	setImageFileName(_ImageFileName);
	setBrush(QBrush(Qt::NoBrush));
	checkForImageOrientation();

	//loadImageSpawn();
	//If item is selected its GraphicsView must be visible.
	//if (ChangingImage || isSelected())
	if (ChangingImage || isVisible())
	{
		//ImageModified = true;
		//update();
		//loadImageSpawn();
	}
	updateCursor();
}

void  GraphicsPhotoItem::setDoc(STDom::DDoc* _Doc)
{
	if (STDom::DImageDoc* CImageDoc = static_cast<STDom::DImageDoc*>(_Doc))
		setImage(*CImageDoc);
	else
		setThumbnail(_Doc->thumbnail(), _Doc->fileInfo().absoluteFilePath());
}

void GraphicsPhotoItem::loadImage()
{
	if (!ImageResource.isNull())
	{
		//!FIXME What happens if load fails ???

		CurrImage = STImage(ImageResource.fileInfo().absoluteFilePath());
		if (ImageEncrypted)
			CurrImage.blowFishDecode();

		BigImageSize = CurrImage.size();
		//!TODO put the 800 in settings.
		if (ImageMode == LowResImageMode && !CurrImage.isNull())
			CurrImage =	CurrImage.scaledToWidth(qMin(CurrImage.width(), qMin(800, mmToPixels(rect().width(), LowResImageDPIs))));

		LastMaxResRect = QRect();
		ImageLoaded = true;
		checkForImageOrientation();
	}
	//else
	//	qWarning("Trying to load PhotoItem without image: rect width: %f", rect().width());

}

//! Loads current image in background using a thread
void GraphicsPhotoItem::loadImageSpawn()
{
	STDom::DDocFormat PhotoFormat(rect().size());
	QSize PixelSize = PhotoFormat.pixelSize(LowResImageDPIs);
	if ((PixelSize.width() > MaxLowResImageSize.width() || PixelSize.height() > MaxLowResImageSize.height()) || ImageMode == HiResImageMode || CurrImage.isNull())
	{
		if (!imageLoaded() && !MLoadThread->isRunning())
			MLoadThread->getPreview();
	}
}

void GraphicsPhotoItem::waitForImageLoaded()
{
	loadImageSpawn();
	MLoadThread->wait();
}

void GraphicsPhotoItem::unloadImage()
{
	CurrImage = QImage();
	PaintedImage = QImage();
	ImageLoaded = false;
	BigImageSize.setWidth(0);
	BigImageSize.setHeight(0);
}

void GraphicsPhotoItem::clearImage()
{
	unloadImage();
	ImageResource = Resource();
	ImageMD5Sum = "";
	setBrush(QBrush(EmptyFrameColor, Qt::SolidPattern));
}

void GraphicsPhotoItem::adjustRectToImage()
{
	if (!PaintedImage.isNull())
	{
		OrientationChanged = false;
		adjustRectToImage(PaintedImage.size());
	}
}

void GraphicsPhotoItem::adjustRectToImage(const QSize& _ImageSize)
{
	//Adjust itemRect to Image.
	double ImageRatio = static_cast<double>(_ImageSize.width()) / static_cast<double>(_ImageSize.height());
	QRectF FrameRect = rect();
	QRectF AdjustedRect = FrameRect;
	if (ImageRatio > 1) //=> Width > height => Adjust rect height
	{
		double NewHeight = FrameRect.width() / ImageRatio;
		if (NewHeight < FrameRect.height() && !OrientationChanged)
		{
			AdjustedRect.setHeight(NewHeight);
			AdjustedRect.moveTop(FrameRect.y() + ((FrameRect.height() - AdjustedRect.height()) / 2));
		}
		else
		{
			AdjustedRect.setWidth(FrameRect.height() * ImageRatio);
			AdjustedRect.moveLeft(FrameRect.x() + ((FrameRect.width() - AdjustedRect.width()) / 2));
		}

	}
	else
	{
		double NewWidth = FrameRect.height() * ImageRatio;
		if (NewWidth < FrameRect.width() && !OrientationChanged)
		{
			AdjustedRect.setWidth(NewWidth);
			AdjustedRect.moveLeft(FrameRect.x() + ((FrameRect.width() - AdjustedRect.width()) / 2));
		}
		else
		{
			AdjustedRect.setHeight(FrameRect.width() / ImageRatio);
			AdjustedRect.moveTop(FrameRect.y() + ((FrameRect.height() - AdjustedRect.height()) / 2));
		}
	}

	if ((FrameRect.width() > FrameRect.height() && AdjustedRect.width() <= AdjustedRect.height())
		|| (FrameRect.width() <= FrameRect.height() && AdjustedRect.width() > AdjustedRect.height()))
		OrientationChanged = !OrientationChanged;

	setRect(AdjustedRect);

	setSelected(false);
	modified();
}

void GraphicsPhotoItem::rotateImage(qreal _Degrees)
{
	ImageMatrix.rotate(_Degrees);
	ImageModified = true;
	update();
	modified();
}

void GraphicsPhotoItem::scaleImage(qreal _Scale)
{
	CurrScale *= _Scale;
	setImageScale(CurrScale);
	modified();
}

void GraphicsPhotoItem::setImageScale(qreal _Scale)
{
	CurrScale = _Scale;
	ImageModified = true;

	update();
	modified();
}

qreal GraphicsPhotoItem::imageScale()
{
	return CurrScale;
}

qreal GraphicsPhotoItem::fitInImageScale()
{
	qreal Res = 1;
	//We calculate it by the largest side of image
	if (PaintedImage.width() > PaintedImage.height())
		Res = (static_cast<qreal>(LastMaxResRect.width()) ) / (static_cast<qreal>(PaintedImage.width()) / CurrScale);
	else
		Res = static_cast<qreal>(LastMaxResRect.height()) / (static_cast<qreal>(PaintedImage.height()) / CurrScale);

	return Res;
}

void GraphicsPhotoItem::shrink(double _Amount)
{
	QRectF CurrRect = rect();
	setRect(CurrRect.x() + _Amount, CurrRect.y() + _Amount, CurrRect.width() - _Amount * 2, CurrRect.height() - _Amount * 2);
}

void GraphicsPhotoItem::setOpacity(qreal _Value)
{
	Opacity = _Value;
	update();
	modified();
}


void GraphicsPhotoItem::loadElement(const QDomElement& _Element, const QString& _LoadDir)
{
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			AbstractGraphicsItem* CItem;
			if (CEl.tagName().toLower() ==  "rect" )
			{
				QRect MRect(CEl.attribute("x", "0").toDouble(),
								CEl.attribute("y", "0").toDouble(),
								CEl.attribute("width", "0").toDouble(),
								CEl.attribute("height", "0").toDouble());
				setRect(MRect);
			}
			else
			if (CEl.tagName().toLower() ==  "pen" )
			{
				QPen Pen = pen();
				Pen.setColor(CEl.attribute("color", "#000000"));
				Pen.setStyle(static_cast<Qt::PenStyle>(CEl.attribute("style", "0").toInt()));
				Pen.setWidth(CEl.attribute("width", "0").toInt());
				Pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(CEl.attribute("joinstyle", "0").toInt()));
				setPen(Pen);
			}
			else
			if (CEl.tagName().toLower() ==  "matrix" )
			{
				ImageMatrix.setMatrix(
								CEl.attribute("m11", "0").toDouble(),
								CEl.attribute("m12", "0").toDouble(),
								CEl.attribute("m21", "0").toDouble(),
								CEl.attribute("m22", "0").toDouble(),
								CEl.attribute("dx", "0").toDouble(),
								CEl.attribute("dy", "0").toDouble());
			}
		}
		CNode = CNode.nextSibling();
	}
	setImageEncrypted(_Element.attribute("encrypted", "false").toLower() == "true");
	STImage CImage;
	QString ImageFilePath = _Element.attribute("src");

	if (!ImageFilePath.isEmpty())
	{
		QFileInfo ImgFInfo = Resource::fileInfoFromXmlSrc(ImageFilePath, _LoadDir);
		if (ImgFInfo.exists() && ImgFInfo.isFile()) //Is not only a dir and it exists.
		{
			CImage.loadThumbnail(ImgFInfo.absoluteFilePath());
			setThumbnail(CImage, ImgFInfo.absoluteFilePath());
			if (ImageEncrypted)
				loadImageSpawn();
		}
	}
	else
		ImageResource = Resource();

	setTransform(AbstractGraphicsItem::loadTransformElement(_Element));
	setPos(_Element.attribute("x", "0").toDouble(), _Element.attribute("y", "0").toDouble());
	CurrScale = _Element.attribute("scale", "1.0").toDouble();
	Opacity = _Element.attribute("opacity", "1.0").toDouble();
	PanningPoint.setX(_Element.attribute("panningx", "0").toDouble());
	PanningPoint.setY(_Element.attribute("panningy", "0").toDouble());

	//Load Frame and masks
	setFrameResource(Resource::resourceFromXmlSrc(_Element.attribute("frame_src", ""), _LoadDir));

	if (FrameResource.isNull()) //Frame and mask are exclusives
		setAlphaChannel(Resource::Resource::resourceFromXmlSrc(_Element.attribute("mask_src", ""), _LoadDir));

	AbstractGraphicsItem::loadEffectElements(this,  _Element);

	updateToolTip();
}

QDomElement GraphicsPhotoItem::createElement(QDomDocument& _Doc, const QString& _StoreDir) const
{
	QDomElement MElement = _Doc.createElement(tagName());
	if (!ImageResource.isNull())
		MElement.setAttribute("src", Resource::fileInfoToXmlSrc(ImageResource.fileInfo(), _StoreDir));

	if (!FrameResource.isNull())
		MElement.setAttribute("frame_src", Resource::fileInfoToXmlSrc(FrameResource.fileInfo(), _StoreDir));
	else
		if (!MaskResource.isNull())
			MElement.setAttribute("mask_src", Resource::fileInfoToXmlSrc(MaskResource.fileInfo(), _StoreDir));

	MElement.setAttribute("scale", CurrScale);
	MElement.setAttribute("x", x());
	MElement.setAttribute("y", y());
	MElement.setAttribute("panningx", PanningPoint.x());
	MElement.setAttribute("panningy", PanningPoint.y());
	MElement.setAttribute("encrypted", ImageEncrypted ? "true" : "false");
	MElement.setAttribute("opacity", Opacity);
	QDomElement RectEl = _Doc.createElement("rect");
	RectEl.setAttribute("x", rect().x());
	RectEl.setAttribute("y", rect().y());
	RectEl.setAttribute("width", rect().width());
	RectEl.setAttribute("height", rect().height());
	MElement.appendChild(RectEl);

	QDomElement MatrixEl = _Doc.createElement("matrix");
	MatrixEl.setAttribute("m11", ImageMatrix.m11());
	MatrixEl.setAttribute("m12", ImageMatrix.m12());
	MatrixEl.setAttribute("m21", ImageMatrix.m21());
	MatrixEl.setAttribute("m22", ImageMatrix.m22());
	MatrixEl.setAttribute("dx", ImageMatrix.dx());
	MatrixEl.setAttribute("dy", ImageMatrix.dy());
	MElement.appendChild(MatrixEl);

	QDomElement PenEl = _Doc.createElement("pen");
	PenEl.setAttribute("color", pen().color().name());
	PenEl.setAttribute("style", pen().style());
	PenEl.setAttribute("width", pen().width());
	PenEl.setAttribute("joinstyle", pen().joinStyle());
	MElement.appendChild(PenEl);

	MElement.appendChild(AbstractGraphicsItem::createTransformElement(this, _Doc));
	//Effects
	AbstractGraphicsItem::appendEffectElements(MElement, this, _Doc);

	return MElement;
}

void GraphicsPhotoItem::setAutoAdjustFramesToImages(bool _Value)
{
	AutoAdjustFramesToImages = _Value;
	if (_Value)
		setAspectRatioMode(Qt::KeepAspectRatio);
	//else
	//	setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
}

bool GraphicsPhotoItem::encryptedByFileName(const QString& _FilePath)
{
	return QFileInfo(_FilePath).baseName().endsWith("_cyph");
}

void GraphicsPhotoItem::disableMoveConstraints()
{
	OldSnapToGrid = snapToGrid();
	setSnapToGrid(false);
	OldThresholdMoving = thresholdMoving();
	setThresholdMoving(false);
}

void GraphicsPhotoItem::restoreMoveConstraints()
{
	setSnapToGrid(OldSnapToGrid);
	setThresholdMoving(OldThresholdMoving);
}

void GraphicsPhotoItem::setLowResWarning(const QString& _WarningImage, int _MinDpis)
{
	LowResWarningImage = _WarningImage;
	LowResMinDpis = _MinDpis;
}

/*!
	\return position of item rect in page coordinates.
*/

QPointF GraphicsPhotoItem::rectPos() const
{
	return rect().topLeft() + scenePos();
}

void GraphicsPhotoItem::setRectPos(const QPointF& _Pos)
{
	QRectF newItemRect = QRectF(QPoint(0, 0), rect().size());
	setRect(newItemRect);
	setPos(_Pos);
}

void GraphicsPhotoItem::setRectPos(qreal _X, qreal _Y)
{
	setRectPos(QPointF(_X, _Y));
}

void GraphicsPhotoItem::paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* )
{

	_P->setClipRect(_Option->exposedRect);
	QGraphicsRectItem::paint(_P, _Option); //To draw borders and empty frames...

	_P->setOpacity(Opacity);

	if (ImageMode == HiResImageMode)
		waitForImageLoaded();

	if (!CurrImage.isNull())
	{
		QRectF ItemRect = rect();

		//Real size rect to fit in.
		QMatrix OMatrix = _Option->matrix;
		//QMatrix OMatrix = QMatrix(); //_Option->matrix;
		//OMatrix.scale(LevelOfDetailFromTransform(_P->worldTransform()), LevelOfDetailFromTransform(_P->worldTransform()) );
		if (Modifier->rotation(Qt::ZAxis) != 0)
			OMatrix.rotate( -Modifier->rotation(Qt::ZAxis));
		QRect MaxResRect = OMatrix.mapRect(ItemRect).toRect();

		//qDebug("_Option->exposedRect: %f, %f, %f, %f", _Option->exposedRect.x(), _Option->exposedRect.y(), _Option->exposedRect.width(), _Option->exposedRect.height());
		//qDebug("MaxResRect: %d, %d, %d, %d", MaxResRect.x(), MaxResRect.y(), MaxResRect.width(), MaxResRect.height());
		//qDebug("Event Pos: %f, %f", Pos.x(), Pos.y());
		#if QT_VERSION >= 0x040600
		qreal LevelOfDetail = _Option->levelOfDetailFromTransform(_P->worldTransform());
		#else
		qreal LevelOfDetail = _Option->levelOfDetail;
		#endif
		if (PaintedImage.isNull() || LastMaxResRect.size() != MaxResRect.size() || (ImageModified))
		{
			createPaintedImage(Qt::SmoothTransformation,  MaxResRect);

			LastMaxResRect = MaxResRect;
			ImageModified = false;
		}

		qreal PosImageX = qMax(qMin(-PanningPoint.x() * LevelOfDetail, static_cast<qreal>(PaintedImage.width() - MaxResRect.width())), 0.0);
		qreal PosImageY = qMax(qMin(-PanningPoint.y() * LevelOfDetail, static_cast<qreal>(PaintedImage.height() - MaxResRect.height())), 0.0);
		QRectF ClipRect(PosImageX , PosImageY, MaxResRect.width(), MaxResRect.height() );

		if (PaintedImage.width() < MaxResRect.width())
		{
			qreal ImageWidth = PaintedImage.width() *  (1 / LevelOfDetail);
			PanningPoint.setX(qMin(qMax(PanningPoint.x(), 0.0), ItemRect.width() - ImageWidth));
			ItemRect.moveCenter(QPoint(ItemRect.center().x() + PanningPoint.x(), ItemRect.center().y()));
		}
		else
		{//Image scaling could left a panningpoint with a bad scale factor. Lets fit it in the limits.
			if (-PanningPoint.x() * LevelOfDetail > static_cast<qreal>(PaintedImage.width() - MaxResRect.width()))
				PanningPoint.setX(- (PaintedImage.width() - MaxResRect.width()) / LevelOfDetail);
		}

		if (PaintedImage.height() < MaxResRect.height())
		{
			qreal ImageHeight = PaintedImage.height() *  (1 / LevelOfDetail);
			PanningPoint.setY(qMin(qMax(PanningPoint.y(), 0.0), ItemRect.height() - ImageHeight));
			ItemRect.moveCenter(QPoint(ItemRect.center().x(), ItemRect.center().y() + PanningPoint.y()));
		}
		else
		{//Image scaling could left a panningpoint with a bad scale factor. Lets fit it in the limits.
			if (-PanningPoint.y() * LevelOfDetail > static_cast<qreal>(PaintedImage.height() - MaxResRect.height()))
				PanningPoint.setY(- (PaintedImage.height() - MaxResRect.height()) / LevelOfDetail);
		}
		if (MaskImage.isNull() && ImageMode != HiResImageMode)
			_P->drawImage(ItemRect, PaintedImage, ClipRect);
		else
		{ //In HiResImageMode there's a problem for example drawing to a PDF printer se we always yse BuffImage to solve it.
			QImage BuffImage(ClipRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
			QPainter Painter(&BuffImage);
			Painter.drawImage(BuffImage.rect(), PaintedImage, ClipRect);
			Painter.end();
			if (!MaskImage.isNull())
				BuffImage.setAlphaChannel(MaskImage.scaled(BuffImage.size()));
			_P->drawImage(ItemRect, BuffImage);
		}
		QSize Dpis = imageWindowDpis(ItemRect.size(), ClipRect.size().toSize(), PaintedImage.size(), BigImageSize);

		//Low Res warning
		if ( Dpis.width() > 0 && Dpis.width() < LowResMinDpis && ImageMode != HiResImageMode && WarningVisible)
		{
			QPixmap Pix(LowResWarningImage);
			QRectF TargetRect(QPointF(ItemRect.topRight().x() - 20, ItemRect.topRight().y()),
							  QPointF(ItemRect.topRight().x(), ItemRect.topRight().y() + 20));
			_P->drawPixmap(TargetRect,Pix, Pix.rect());
			QPen Pen(Qt::black);
			Pen.setCosmetic(true);
			_P->setPen(Pen);
			QFont MFont = _P->font();
			MFont.setPixelSize(8);
			_P->setFont(MFont);
			QRectF TextRect = TargetRect;
			QString LowResText = tr("Low resolution (%1 Dpis)").arg(Dpis.width());
			TextRect.adjust(-(qMin(_P->fontMetrics().boundingRect(LowResText).width() + 2, ItemRect.toRect().width() - 20)), 4, 0, 0);
			TextRect.setHeight(ItemRect.height());
			_P->drawText(TextRect, LowResText);
			Pen.setColor(Qt::white);
			_P->setPen(Pen);
			TextRect.adjust(1, 1, 0, 0);
			_P->drawText(TextRect, LowResText);
		}
		//Frame Image
		if (!FrameImage.isNull())
		{
			_P->drawImage(rect(), FrameImage);
		}
	}
	else //We could not see frames with mask image and not image.
	{
		if (hasAlphaChannel())
		{
			QColor EFColor = EmptyFrameColor;
			QImage Img(rect().size().toSize(), QImage::Format_ARGB32_Premultiplied);
			//QImage Img(rect().size().toSize(), QImage::Format_ARGB32);
			Img.fill(EFColor.rgb());
			if (!MaskImage.isNull())
				Img.setAlphaChannel(MaskImage.scaled(Img.size()));
			_P->drawImage(rect(), Img);

			//_P->fillRect(rect(), QBrush(EmptyFrameColor));
		}
		if (ShowNoImageMessage)
		{
			_P->setFont(QFont("Arial Black", 10));
			_P->setPen(QPen(Qt::black));
			_P->drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, tr("Drop images here"));
		}
		if (!FrameImage.isNull())
		{
			_P->drawImage(rect(), FrameImage);
		}

	}

	#ifdef DEMO_MODE
	//If Demo
	if (!CurrImage.isNull())
	{
		QString DemoString = "-- DEMO MODE --";
		_P->save();
		QPen Pen;
		_P->setFont(QFont("Arial Black", 10));
		QRect BRect = _P->fontMetrics().boundingRect(DemoString);
		QRectF TextRect = QRectF(rect().left(), rect().top(), BRect.width(), BRect.height());
		//_P->drawText(TextRect, DemoString);
		QRectF OrigRect = TextRect;
		//for (int VforX = 0; VforX < 1; VforX++)
		while (TextRect.left() < rect().right())
		{
			TextRect.moveTop(OrigRect.top());
			//for (int VforY = 0; VforY < 50; VforY++)
			int CntY = 0;
			while (TextRect.top() < rect().bottom())
			{
				if (CntY++ % 2)
					Pen.setColor(QColor(255, 255, 255, 46));
				else
					Pen.setColor(QColor(0, 0, 0, 46));
				_P->setPen(Pen);
				_P->drawText(TextRect, DemoString);
				TextRect.moveTop(TextRect.bottom() + 3);
			}
			TextRect.moveLeft(TextRect.right() + 3);
		}
		_P->restore();
	}
	#endif

	if (ImageMode == HiResImageMode)
		unloadImage();

//  	else
//  	{
//  		_P->fillRect(rect(), QBrush(QColor(255,255,255))); //The item paint already do it.
//  	}

	if (isSelected())
	{
		QRectF ItemRect = rect();
		int SelMarkWidth = 15;

		_P->save();
		_P->setOpacity(1);

		QPen Pen;
		Pen.setCosmetic(true);
		Pen.setColor(Qt::red);
		Pen.setCapStyle(Qt::SquareCap);
		Pen.setStyle(Qt::SolidLine);
		Pen.setWidth(8);
		_P->setPen(Pen);
		QVector<QLine> Lines;
		Lines.push_back(QLine(ItemRect.x(), ItemRect.y(), ItemRect.x() + SelMarkWidth, ItemRect.y()));
		Lines.push_back(QLine(ItemRect.x(), ItemRect.y(), ItemRect.x(), ItemRect.y() + SelMarkWidth));
		Lines.push_back(QLine(ItemRect.topRight().x(), ItemRect.y(), ItemRect.topRight().x() - SelMarkWidth, ItemRect.y()));
		Lines.push_back(QLine(ItemRect.topRight().x(), ItemRect.y(), ItemRect.topRight().x(), ItemRect.topRight().y() + SelMarkWidth));
		Lines.push_back(QLine(ItemRect.bottomLeft().x(), ItemRect.bottomLeft().y(), ItemRect.bottomLeft().x() + SelMarkWidth, ItemRect.bottomLeft().y()));
		Lines.push_back(QLine(ItemRect.bottomLeft().x(), ItemRect.bottomLeft().y(), ItemRect.bottomLeft().x(), ItemRect.bottomLeft().y() - SelMarkWidth));
		Lines.push_back(QLine(ItemRect.bottomRight().x(), ItemRect.bottomRight().y(), ItemRect.bottomRight().x() - SelMarkWidth, ItemRect.bottomRight().y()));
		Lines.push_back(QLine(ItemRect.bottomRight().x(), ItemRect.bottomRight().y(), ItemRect.bottomRight().x(), ItemRect.bottomRight().y() - SelMarkWidth));
		_P->drawLines(Lines);

		_P->restore();
	}

}

void GraphicsPhotoItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * _Event )
{
	Moving = false;
	if (TouchInterface && MultiSelection)
		_Event->setModifiers(_Event->modifiers() | Qt::ControlModifier);
	QGraphicsRectItem::mouseReleaseEvent(_Event);
}

void GraphicsPhotoItem::mousePressEvent(QGraphicsSceneMouseEvent* _Event)
{
	if (PanningEnabled)
	{
		_Event->accept();
		InitPanningPoint = PanningPoint;
	}
	MouseDownPosInt = getMouseIntersects(_Event->pos(), rect());

	if (TouchInterface && MultiSelection)
		_Event->setModifiers(_Event->modifiers() | Qt::ControlModifier);
	QGraphicsRectItem::mousePressEvent(_Event);
}

void GraphicsPhotoItem::mouseMoveEvent(QGraphicsSceneMouseEvent* _Event)
{
	if (PanningEnabled)
	{
		panImage(InitPanningPoint + (_Event->pos() - _Event->buttonDownPos(Qt::LeftButton)));
		emit mousePanning(PanningPoint);
	}
// 	if (MouseDownPosInt != MouseOnNoEdge && ResizeAllowed)
// 	{
// 		QRectF CurrRect = rect();
// 		if (MouseDownPosInt & MouseOnLeftEdge)
// 			CurrRect.setLeft(snapToGridValue(_Event->pos().x()));
// 		else
// 			if (MouseDownPosInt & MouseOnRightEdge)
// 				CurrRect.setRight(snapToGridValue(_Event->pos().x()));
// 		if (MouseDownPosInt & MouseOnTopEdge)
// 			CurrRect.setTop(snapToGridValue(_Event->pos().y()));
// 		else
// 			if (MouseDownPosInt & MouseOnBottomEdge)
// 				CurrRect.setBottom(snapToGridValue(_Event->pos().y()));
//
// 		if (CurrRect != rect())
// 		{
// 			setRect(CurrRect);
// 			updateToolTip();
// 		}
// 	}

	if (MouseDownPosInt == MouseOnNoEdge || PanningEnabled)
		QGraphicsRectItem::mouseMoveEvent(_Event);

// 	if (_Event->pos().x() > CurrRect.x() - 10 && _Event->pos().x() < CurrRect.x() + 10)
// 		setCursor(Qt::SizeHorCursor);
// 	else
// 		unsetCursor();
	//if (_Event->pos() )
}


QVariant GraphicsPhotoItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionHasChanged)
		updateToolTip();
	else
	if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
		setControlsVisible(isSelected());
//TODO: No retorna sol al valor anterior !.
// 		if (isSelected() )
// 		{
// 			qreal NZValue = zValue();
// 			if (controlCollided(NZValue))
// 			{
// 				OldZValue = zValue();
// 				RestoreZValue = true;
// 				setZValue(NZValue);
// 			}
// 		}
// 		else
// 		{
// 			if (RestoreZValue)
// 			{
// 				qDebug("Hola !!!!!!!!!!!!!");
// 				RestoreZValue = false;
// 				setZValue(OldZValue);
// 			}
// 		}
	}
	else
	if (change == ItemPositionChange)
	{
		// value is the new position.
		QPointF newPos = value.toPointF();
		//return insideSceneRect(newPos);
		//QRectF rect = scene()->sceneRect();
		if (SnapToGrid)
		{
			newPos.setX(snapToGridValue(newPos.x()));
			newPos.setY(snapToGridValue(newPos.y()));
			return newPos;
		}
		else
		{
			if (ThresholdMoving)
			{
				//We only move if the initial movement is greater than Threshold
				QSizeF Threshold(10, 10);
				if ((qAbs(newPos.x() - pos().x()) < Threshold.width()) && (qAbs(newPos.y() - pos().y()) < Threshold.height()) && !Moving)
					return pos();
				else
					Moving = true;
			}
		}

//          if (!rect.contains(newPos)) {
//              // Keep the item inside the scene rect.
//              newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
//              newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
//              return newPos;
//          }
	}
	return QGraphicsItem::itemChange(change, value);
}

// Drag and Drop
void GraphicsPhotoItem::dropEvent(QGraphicsSceneDragDropEvent* _Event )
{
	if (const ResourceMimeData* MMimeData = qobject_cast<const ResourceMimeData*>(_Event->mimeData()))
	{
		setResource(MMimeData->resource());
	}
	else
	{
		if (_Event->mimeData()->hasUrls())
		{
			if (_Event->mimeData()->urls().size() > 1)
				emit imageListDropped(_Event->mimeData()->urls());
			else
			{
				QList<QUrl> Urls = _Event->mimeData()->urls();

				QUrl FirstUrl = _Event->mimeData()->urls().first();
				if (!FirstUrl.isEmpty() && FirstUrl.isValid())
				{
					QString LocalPath = FirstUrl.toString();
					//Only local file uris are supported.
					QString ImagePath = FirstUrl.toLocalFile();
					if (!ImagePath.isEmpty())
					{
						STDom::DImageDoc Image(ImagePath);
						setImage(Image);
						if (encryptedByFileName(ImagePath))
							setImageEncrypted(true);
						loadImageSpawn();
						emit imageDropped(ImagePath, ImageMD5Sum);
					}
				}
			}
		}
	}
	_Event->accept();
}

void GraphicsPhotoItem::panImage(const QPointF& _PanningPoint)
{
	if (hasImage())
	{
		double DifX = (PanningPoint.x() * 10) - (_PanningPoint.x() * 10);
		double DifY = (PanningPoint.y() * 10) - (_PanningPoint.y() * 10);
		if (abs(DifX) > 1 || abs(DifY) > 1)
			modified();
	}
	PanningPoint = _PanningPoint;
	update();
}


void GraphicsPhotoItem::setPanningEnabled(bool _Enabled)
{
	if (_Enabled != PanningEnabled)
	{
		setFlag(QGraphicsItem::ItemIsMovable, !_Enabled);
		PanningEnabled = _Enabled;
		updateCursor();
	}
	if (PanAct)
	{
		PanAct->setChecked(_Enabled);
		if (_Enabled)
			PanAct->setIcon(QIcon(":/phototemplates/thumbtack_red_plug_64.png"));
		else
			PanAct->setIcon(QIcon(":/phototemplates/thumbtack_red_unplug_64.png"));
	}
}


void GraphicsPhotoItem::slotZoomImageIn()
{
	scaleImage(1.1);
}

void GraphicsPhotoItem::slotZoomImageOut()
{
	scaleImage(1.0/1.1);
}

void GraphicsPhotoItem::slotRotateImage()
{
	rotateImage();
}

void GraphicsPhotoItem::slotFitImage()
{
	setImageScale(fitInImageScale());
}

