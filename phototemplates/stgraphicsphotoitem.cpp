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

#include "stgraphicsphotoitem.h"

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
#include <QDebug>

//Drag And Drop
#include <QMimeData> 
#include <QUrl>
#include "stmaskmimedata.h"
#include "stframemimedata.h"


//Controls 
#include <QToolBar> 
#include <QAction> 
#include <QGraphicsProxyWidget> 

//Others
#include <QGraphicsColorizeEffect>
#include <QGraphicsBlurEffect>

#include "stupdateitemevent.h"
#include "stimage.h"
#include "imageTools.h" 
#include "dimagedoc.h"
//#include "kphoto.h"
//#include "ophotocollection.h" // For OPhotoCollectionImage.

#include "stgraphicsitemmodifier.h"


// _________________________________________________________________________*/
//
//   Class STGraphicsPhotoItemLoadThread
// _________________________________________________________________________*/


QMutex STGraphicsPhotoItemLoadThread::Mutex;
	
STGraphicsPhotoItemLoadThread::STGraphicsPhotoItemLoadThread(STGraphicsPhotoItem* _RelatedItem) : CurrItem(_RelatedItem)
{}


void STGraphicsPhotoItemLoadThread::run()
{
	setPriority(QThread::LowPriority);
	Mutex.lock();
	if (!CurrItem.isNull()) //Defensive
		CurrItem->loadImage();
	Mutex.unlock();
	if (!CurrItem.isNull()) //Defensive
		QApplication::postEvent(CurrItem->scene(), new STUpdateItemEvent(CurrItem));
}

void STGraphicsPhotoItemLoadThread::getPreview()
{
	if (isRunning())
		wait(1500);
	start();
}


// _________________________________________________________________________*/
//
//   Class STGraphicsPhotoItem
// _________________________________________________________________________*/

QBrush STGraphicsPhotoItem::SelectedBrush(QColor(255,255,255, 150), Qt::Dense6Pattern);


QSize STGraphicsPhotoItem::MaxLowResImageSize(100, 100); 
int STGraphicsPhotoItem::LowResImageDPIs = 72;
QString STGraphicsPhotoItem::LowResWarningImage(":/phototemplates/dialog-warning.png");
int STGraphicsPhotoItem::LowResMinDpis = 100;
bool STGraphicsPhotoItem::TouchInterface = false;



QSize STGraphicsPhotoItem::imageWindowDpis(const QSizeF& _ItemRectInmm, const QSize& _ImageClipSize, const QSize& _PaintedImageSize, const QSize& _OriginalImageSize)
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

void STGraphicsPhotoItem::createPaintedImage(const Qt::TransformationMode&  _CTransformMode, const QRect& _MaxResRect)
{
	PaintedImage = CurrImage.transformed(ImageMatrix, _CTransformMode);
	PaintedImage = PaintedImage.scaled(QSize(_MaxResRect.width() * CurrScale, _MaxResRect.height() * CurrScale), AspectRatioMode, _CTransformMode);
	//If Mask
	if (CurrScale < 1.0 && !MaskImage.isNull())
		PaintedImage.setAlphaChannel(MaskImage.scaled(PaintedImage.size()));
}

void STGraphicsPhotoItem::updateToolTip()
{
	//QPointF Pos = rect().topLeft() + scenePos();
	QPointF Pos = rectPos();
	QSizeF Size = boundingRect().size();
	STAbstractGraphicsItem::updateToolTip(QRectF(Pos, Size));
}

void STGraphicsPhotoItem::updateCursor()
{
	if (PanningEnabled)
		setCursor(Qt::OpenHandCursor); 
	else 
		setCursor(Qt::SizeAllCursor);
}

void STGraphicsPhotoItem::createControls()
{
	/*QToolBar* TBControls = new QToolBar; 
	TBControls->setObjectName("SceneItemToolBar"); 

	PanAct = new QAction(QIcon(":/phototemplates/thumbtack_red_unplug_64.png"), tr("Pan"), this); 
	connect(PanAct, SIGNAL(toggled( bool )), this, SLOT(setPanningEnabled(bool))); 
	PanAct->setCheckable(true); 
	TBControls->addAction(PanAct);


	QAction* ZoomInAct = new QAction(QIcon(":/phototemplates/viewmag+.png"), tr("Zoom+"), this); 
	connect(ZoomInAct, SIGNAL(triggered( bool )), this, SLOT(slotZoomImageIn())); 
	TBControls->addAction(ZoomInAct);

	QAction* ZoomOutAct = new QAction(QIcon(":/phototemplates/viewmag-.png"), tr("Zoom-"), this); 
	connect(ZoomOutAct, SIGNAL(triggered( bool )), this, SLOT(slotZoomImageOut())); 
	TBControls->addAction(ZoomOutAct);

	QAction* RotateAct = new QAction(QIcon(":/phototemplates/rotate_ccw.png"), tr("Rotate"), this); 
	connect(RotateAct, SIGNAL(triggered( bool )), this, SLOT(slotRotateImage())); 
	TBControls->addAction(RotateAct);

	QAction* FitAct = new QAction(QIcon(":/phototemplates/fittoframe.png"), tr("Fit"), this); 
	connect(FitAct, SIGNAL(triggered( bool )), this, SLOT(slotFitImage())); 
	TBControls->addAction(FitAct);


	QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this); 
	proxy->setWidget(TBControls); 
	proxy->setVisible(false);
	double Scale = (100 + 46 - 20) / proxy->geometry().width();
	proxy->scale( Scale, Scale);
//	proxy->setPos(rectPos());
//	proxy->setVisible(false);
//	if (proxy->geometry().width() > rect().width() * 2)
//	{
//		double Scale1 = (rect().height() / 6) / proxy->geometry().height();
//		double Scale2 = (rect().width() * 1.5) / proxy->geometry().width();
//		double Scale = qMax(Scale1, Scale2);
//		proxy->scale( Scale, Scale);
//	}

	setControlsWidget(proxy);
	layoutControlWidget();*/
}


void STGraphicsPhotoItem::layoutControlWidget()
{
	if (MControlGWidget) //Defensive
	{
		QPointF MyPos = rectPos() - pos();
		MyPos.setY(MyPos.y() - (MControlGWidget->geometry().height() - 10));
		MControlGWidget->setPos(MyPos);
	}
}

void STGraphicsPhotoItem::init()
{
	PanAct = 0; 
	AspectRatioMode = Qt::KeepAspectRatioByExpanding; 
	ShadowDepth = 0;
	CurrScale = 1.0;
	Opacity = 1.0; 
	ImageModified = false; 
	PanningEnabled = false; 
	ImageLoaded = false; 
	WarningVisible = true;
	setShowNoImageMessage(true);
	ImageEncrypted = false;
	IgnoreExifRotation = false;
	OrientationChanged = false;

	QColor EmptyFrameColor("#FFD95C"); 
	EmptyFrameColor.setAlpha(150);
	//EmptyFrameColor.setAlpha(0);
	setBrush(QBrush(EmptyFrameColor, Qt::SolidPattern));

	#if QT_VERSION >= 0x040600
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemClipsToShape | QGraphicsItem::ItemUsesExtendedStyleOption);
	#else
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemClipsToShape);
	#endif
	MLoadThread = new STGraphicsPhotoItemLoadThread(this);
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

	QGraphicsColorizeEffect* ColEffect = new QGraphicsColorizeEffect(this);
	QGraphicsBlurEffect* BlurEffect = new QGraphicsBlurEffect(this);
	ColEffect->setColor(Qt::red);
	//setGraphicsEffect(ColEffect);
	//setGraphicsEffect(BlurEffect);
}

void STGraphicsPhotoItem::checkForImageOrientation()
{
	if (!AutoAdjustFramesToImages)
	{
		QRect CurrImageRect = ImageMatrix.mapRect(CurrImage.rect());
		if ( AspectRatioMode == Qt::KeepAspectRatio && ((CurrImageRect.width() > CurrImageRect.height() && rect().width() < rect().height()) ||
				(CurrImageRect.height() > CurrImageRect.width() && rect().height() < rect().width())))
			rotateImage(90);
	}
}

QPointF STGraphicsPhotoItem::insideSceneRect(const QPointF& _Point)
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



STGraphicsPhotoItem::STGraphicsPhotoItem(const STPhotoLayoutTemplate::Frame& _Frame, QGraphicsItem* _Parent) : QGraphicsRectItem(_Parent), Moving(false), STAbstractGraphicsItem(this)
{
	init();
	setRect(_Frame);
	updateToolTip();
	createStandardCorners();
	createControls(); 
	setControlsVisible(false);
}

STGraphicsPhotoItem::STGraphicsPhotoItem(QGraphicsItem* _Parent) : QGraphicsRectItem(_Parent), STAbstractGraphicsItem(this)
{
	init();
	updateToolTip();
	createStandardCorners();
	createControls(); 
	setControlsVisible(false);
}

STGraphicsPhotoItem::~STGraphicsPhotoItem()
{
	if (MLoadThread->isRunning())
		MLoadThread->wait();
	delete MLoadThread;
}

void STGraphicsPhotoItem::adjustRectToImage()
{
	if (!PaintedImage.isNull())
		adjustRectToImage(PaintedImage.size());
}

void STGraphicsPhotoItem::adjustRectToImage(const QSize& _ImageSize)
{
	//Adjust itemRect to Image.
	double ImageRatio = static_cast<double>(_ImageSize.width()) / static_cast<double>(_ImageSize.height());
	QRectF FrameRect = rect();
	QRectF AdjustedRect = FrameRect;
	qDebug() << "OrientationChanged:" << OrientationChanged;
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

void STGraphicsPhotoItem::setAutoAdjustFramesToImages(bool _Value)
{
	AutoAdjustFramesToImages = _Value;
	if (_Value)
		setAspectRatioMode(Qt::KeepAspectRatio);
	//else
	//	setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
}

void STGraphicsPhotoItem::setImage(STDom::DImageDoc& _Image)
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
			setImage(Thumbnail, _Image.fileInfo().absoluteFilePath());
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
		setImage(Thumbnail, _Image.fileInfo().absoluteFilePath());

	update();
}

void  STGraphicsPhotoItem::setDoc(STDom::DDoc* _Doc)
{
	if (STDom::DImageDoc* CImageDoc = static_cast<STDom::DImageDoc*>(_Doc))
		setImage(*CImageDoc);
	else
		setImage(_Doc->thumbnail(), _Doc->fileInfo().absoluteFilePath());
}

void STGraphicsPhotoItem::setImage(const QPixmap& _ThumbNail, const QString& _ImageFileName)
{
	//CurrImage = QImage(_ImageFileName);
	ImageLoaded = false;
	Opacity = 1;
	PanningPoint = QPoint(0, 0);
	bool ChangingImage = !CurrImage.isNull();
	CurrImage = _ThumbNail.toImage();
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


QString STGraphicsPhotoItem::alphaChannelFileName() const 
{
	QString Res = ""; 
	if (hasAlphaChannel())
	{
		STImage SMImage = MaskImage; 
		Res = "mask_" + SMImage.hashString() + ".png"; 
	}
	return Res; 
}

QString STGraphicsPhotoItem::frameImageFileName() const
{
	QString Res = "";
	if (hasFrameImage())
	{
		STImage SFImage = FrameImage;
		Res = "frame_" + SFImage.hashString() + ".png";
	}
	return Res;
}

void STGraphicsPhotoItem::setAlphaChannel(const QImage& _AlphaChannel)
{
	MaskImage = _AlphaChannel; 

	QBrush CurrBrush = brush(); 
	QColor EmptyFrameColor = brush().color();
	if (MaskImage.isNull())
		EmptyFrameColor.setAlpha(150);
	else
		EmptyFrameColor.setAlpha(0);
	CurrBrush.setColor(EmptyFrameColor); 
	setBrush(CurrBrush);

	update();
	modified();
}

void STGraphicsPhotoItem::setFrameImage(const QImage& _FrameImage)
{
	FrameImage = _FrameImage;
	update();
}

QImage STGraphicsPhotoItem::originalPaintedImage()
{
	createPaintedImage(Qt::SmoothTransformation, LastMaxResRect);
	return PaintedImage;
}


void STGraphicsPhotoItem::updatePaintedImage(const QImage& _Image)
{
	PaintedImage = _Image;
	update();
}

void STGraphicsPhotoItem::setImage(const QImage& _Image, const QString& _ImageFileName)
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

void STGraphicsPhotoItem::loadImage()
{
	if (!CurrImageFileName.isEmpty())
	{
		//!FIXME What happens if load fails ???

		CurrImage = STImage(CurrImageFileName);
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
void STGraphicsPhotoItem::loadImageSpawn()
{
	STDom::DDocFormat PhotoFormat(rect().size());
	QSize PixelSize = PhotoFormat.pixelSize(LowResImageDPIs); 
	if ((PixelSize.width() > MaxLowResImageSize.width() || PixelSize.height() > MaxLowResImageSize.height()) || ImageMode == HiResImageMode || CurrImage.isNull())
	{
		if (!imageLoaded() && !MLoadThread->isRunning())
			MLoadThread->getPreview();
	}
}

void STGraphicsPhotoItem::waitForImageLoaded()
{
	loadImageSpawn();
	MLoadThread->wait();
}

void STGraphicsPhotoItem::unloadImage()
{
	CurrImage = QImage();
	PaintedImage = QImage();
	ImageLoaded = false;
	BigImageSize.setWidth(0);
	BigImageSize.setHeight(0);
}

void STGraphicsPhotoItem::clearImage()
{
	unloadImage(); 
	CurrImageFileName = ""; 
	ImageMD5Sum = ""; 
	setBrush(QBrush(EmptyFrameColor, Qt::SolidPattern));
}

void STGraphicsPhotoItem::rotateImage(qreal _Degrees)
{
	ImageMatrix.rotate(_Degrees);
	ImageModified = true;
	update();
	modified();
}

void STGraphicsPhotoItem::scaleImage(qreal _Scale)
{
	CurrScale *= _Scale;
	setImageScale(CurrScale);
	modified();
}

void STGraphicsPhotoItem::setImageScale(qreal _Scale)
{
	CurrScale = _Scale; 
	ImageModified = true;

	update();
	modified();
}

qreal STGraphicsPhotoItem::imageScale()
{
	return CurrScale; 
}

qreal STGraphicsPhotoItem::fitInImageScale()
{
	qreal Res = 1; 
	//We calculate it by the largest side of image
	if (PaintedImage.width() > PaintedImage.height())
		Res = (static_cast<qreal>(LastMaxResRect.width()) ) / (static_cast<qreal>(PaintedImage.width()) / CurrScale); 
	else 
		Res = static_cast<qreal>(LastMaxResRect.height()) / (static_cast<qreal>(PaintedImage.height()) / CurrScale); 
	
	return Res; 
}

void STGraphicsPhotoItem::setImageFileName(const QString& _ImageFileName)
{
	if (!CurrImageFileName.isEmpty() && !ImageMD5Sum.isEmpty())
		emit imageRemoved(CurrImageFileName, ImageMD5Sum);
	CurrImageFileName = _ImageFileName; 
	if (!_ImageFileName.isEmpty())
	ImageMD5Sum = STImage::hashString(_ImageFileName); 	
	modified();
}

void STGraphicsPhotoItem::setImageSourcePath(const QString& _ImagesSourcePath)
{
	ImagesSourcePath = _ImagesSourcePath;
}

void STGraphicsPhotoItem::loadElement(QDomElement& _Element)
{
	QDomNode CNode = _Element.firstChild();
	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			STAbstractGraphicsItem* CItem;
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
		if (!ImageFilePath.contains("/") && !ImageFilePath.contains("\\"))
			ImageFilePath = ImagesSourcePath + "/" + ImageFilePath;


		QFileInfo ImgFInfo(ImageFilePath);
		if (ImgFInfo.exists() && ImgFInfo.isFile()) //Is not only a dir and it exists.
		{
			CImage.loadThumbnail(ImageFilePath);
			setImage(QPixmap::fromImage(CImage), ImageFilePath);
			if (ImageEncrypted)
				loadImageSpawn();
		}
	}
	setTransform(STAbstractGraphicsItem::loadTransformElement(_Element)); 
	setPos(_Element.attribute("x", "0").toDouble(), _Element.attribute("y", "0").toDouble());
	CurrScale = _Element.attribute("scale", "1.0").toDouble(); 
	ShadowDepth = _Element.attribute("shadowdepth", "0").toDouble(); 
	Opacity = _Element.attribute("opacity", "1.0").toDouble(); 
	PanningPoint.setX(_Element.attribute("panningx", "0").toDouble());
	PanningPoint.setY(_Element.attribute("panningy", "0").toDouble());
	
	//Mask Image.
	QString MaskSrc = _Element.attribute("mask_src", "");
	if (!MaskSrc.isEmpty())
	{
		MaskSrc = ImagesSourcePath + "/" + MaskSrc; 
		setAlphaChannel(QImage(MaskSrc)); 
	}

	QString FrameSrc = _Element.attribute("frame_src", "");
	if (!FrameSrc.isEmpty())
	{
		FrameSrc = ImagesSourcePath + "/" + FrameSrc;
		setFrameImage(QImage(FrameSrc));
	}

	STAbstractGraphicsItem::loadEffectElements(this,  _Element);

	updateToolTip(); 
	layoutControlWidget();
}

QDomElement STGraphicsPhotoItem::createElement(QDomDocument& _Doc)
{
	QDomElement MElement = _Doc.createElement(tagName());
	if (!imageFileName().isEmpty())
	{
		QFileInfo ImageFile(imageFileName());
		if (ImageFile.absoluteFilePath().contains(ImagesSourcePath))
			MElement.setAttribute("src", ImageFile.fileName());
		else
		{
			MElement.setAttribute("src", ImageFile.absoluteFilePath());
		}
	}
	if (hasAlphaChannel())
		MElement.setAttribute("mask_src", alphaChannelFileName()); 
	if (hasFrameImage())
		MElement.setAttribute("frame_src", frameImageFileName());

	MElement.setAttribute("shadowdepth", ShadowDepth); 
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

	MElement.appendChild(STAbstractGraphicsItem::createTransformElement(this, _Doc));
	//Effects
	STAbstractGraphicsItem::appendEffectElements(MElement, this, _Doc);

	return MElement; 
}

void STGraphicsPhotoItem::setOpacity(qreal _Value)
{
	Opacity = _Value; 
	update(); 
	modified();
}

void STGraphicsPhotoItem::setShadowDepth(qreal _Value)
{
	ShadowDepth = _Value;
	update(); 
	modified();
}

bool STGraphicsPhotoItem::encryptedByFileName(const QString& _FilePath)
{
	return QFileInfo(_FilePath).baseName().endsWith("_cyph");
}

QRectF STGraphicsPhotoItem::boundingRect() const
{
	QRectF Res = QGraphicsRectItem::boundingRect(); 
	Res.setWidth( Res.width() + ShadowDepth ); 
	Res.setHeight( Res.height() + ShadowDepth ); 
	return Res;
}

/*! 
	\return position of item rect in page coordinates.
*/

QPointF STGraphicsPhotoItem::rectPos() const
{
	return rect().topLeft() + scenePos();
}

void STGraphicsPhotoItem::setRectPos(const QPointF& _Pos)
{
	QRectF newItemRect = QRectF(QPoint(0, 0), rect().size());
	setRect(newItemRect);
	setPos(_Pos);
}

void STGraphicsPhotoItem::setLowResWarning(const QString& _WarningImage, int _MinDpis)
{
	LowResWarningImage = _WarningImage;
	LowResMinDpis = _MinDpis;
}


void STGraphicsPhotoItem::setRectPos(qreal _X, qreal _Y)
{
	setRectPos(QPointF(_X, _Y)); 
}


void STGraphicsPhotoItem::paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* )
{

	_P->setClipRect(_Option->exposedRect);
	if (ShadowDepth == 0)
		QGraphicsRectItem::paint(_P, _Option); //To draw borders and empty frames...

	_P->setOpacity(Opacity);
	if (ShadowDepth != 0)
	{
		QRectF ShadowRect = boundingRect(); 
		ShadowRect.setLeft(ShadowRect.left() + ShadowDepth); 
		ShadowRect.setTop(ShadowRect.top() + ShadowDepth); 
		ShadowRect.setWidth(ShadowRect.width() - ShadowDepth); 
		ShadowRect.setHeight(ShadowRect.height() - ShadowDepth); 
		_P->setPen(QPen(Qt::NoPen)); 
		for (int Vfor = 1; Vfor < 6; Vfor ++)
		{
			ShadowRect.setWidth(ShadowRect.width() - 0.4); 
			ShadowRect.setHeight(ShadowRect.height() - 0.4); 
			ShadowRect.setLeft(ShadowRect.left() + 0.2); 
			ShadowRect.setTop(ShadowRect.top() + 0.2); 
			_P->setBrush(QBrush(QColor(0, 0, 0, 20 * Vfor)));
			_P->drawRoundRect(ShadowRect, 2, 2);
		}
	}

	if (ImageMode == HiResImageMode)
		waitForImageLoaded();

	if (!CurrImage.isNull())
	{
		QRectF ItemRect = rect();
		ItemRect.setWidth(ItemRect.width() - ShadowDepth);
		ItemRect.setHeight(ItemRect.height() - ShadowDepth);
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
			QColor EmptyFrameColor = brush().color();
			EmptyFrameColor.setAlpha(150);
			_P->fillRect(rect(), QBrush(EmptyFrameColor));
		}
		if (ShowNoImageMessage)
		{
			_P->setFont(QFont("Arial Black", 10));
			_P->setPen(QPen(Qt::black));
			_P->drawText(rect(), Qt::AlignCenter | Qt::TextWordWrap, tr("Drop images here"));
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

void STGraphicsPhotoItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * _Event )
{
	Moving = false;
	if (TouchInterface && MultiSelection)
		_Event->setModifiers(_Event->modifiers() | Qt::ControlModifier);
	QGraphicsRectItem::mouseReleaseEvent(_Event);
}

void STGraphicsPhotoItem::mousePressEvent(QGraphicsSceneMouseEvent* _Event)
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

void STGraphicsPhotoItem::mouseMoveEvent(QGraphicsSceneMouseEvent* _Event)
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


QVariant STGraphicsPhotoItem::itemChange(GraphicsItemChange change, const QVariant &value)
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
	if (change == ItemPositionChange /*&& scene()*/) 
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
			//We only move if the initial movement is greater than Threshold
			QSizeF Threshold(10, 10); 
			if ((qAbs(newPos.x() - pos().x()) < Threshold.width()) && (qAbs(newPos.y() - pos().y()) < Threshold.height()) && !Moving)
				return pos();
			else 
				Moving = true;
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
void STGraphicsPhotoItem::dropEvent(QGraphicsSceneDragDropEvent* _Event )
{
	if (const STMaskMimeData* MaskMimeData = qobject_cast<const STMaskMimeData*>(_Event->mimeData()))
	{
		setAlphaChannel(QImage(MaskMimeData->maskFilePath()));
	}
	else
	if (const STFrameMimeData* FrameMimeData = qobject_cast<const STFrameMimeData*>(_Event->mimeData()))
	{
		setFrameImage(QImage(FrameMimeData->frameFilePath()));
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


void STGraphicsPhotoItem::panImage(const QPointF& _PanningPoint)
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


void STGraphicsPhotoItem::setPanningEnabled(bool _Enabled)
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


void STGraphicsPhotoItem::slotZoomImageIn()
{
	scaleImage(1.1);
}

void STGraphicsPhotoItem::slotZoomImageOut()
{
	scaleImage(1.0/1.1);
}

void STGraphicsPhotoItem::slotRotateImage()
{
	rotateImage(); 
}

void STGraphicsPhotoItem::slotFitImage()
{
	setImageScale(fitInImageScale());
}

