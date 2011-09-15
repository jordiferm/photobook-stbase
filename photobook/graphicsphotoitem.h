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

#ifndef GRAPHICSPHOTOITEM_H
#define GRAPHICSPHOTOITEM_H

#include <QGraphicsRectItem>
#include <QImage> 
#include <QThread>
#include <QMutex> 
#include <QMatrix> 
#include <QBrush> 
#include <QPointer>

#include "abstractgraphicsitem.h"
#include "stphotobookexport.h"
#include "stimage.h"
#include "dimagedoc.h"
#include "resource.h"

/**
	Thread for load GraphicsPhotoItem images in background.
*/
class QGraphicsSceneMouseEvent;
class QAction;

namespace SPhotoBook
{

class GraphicsPhotoItem;
class Resource;

class ST_PHOTOBOOK_EXPORT GraphicsPhotoItemLoadThread : public QThread
{
Q_OBJECT
		
	QPointer<GraphicsPhotoItem> CurrItem;
	static QMutex Mutex;
	bool Cancelled;

public:
	GraphicsPhotoItemLoadThread(GraphicsPhotoItem* _RelatedItem);
	void run(); 

public slots: 
	void getPreview();
	
};

/**
	GraphicsItem that represents a frame with an image.
*/
class ST_PHOTOBOOK_EXPORT  GraphicsPhotoItem : public QObject, public QGraphicsRectItem, public AbstractGraphicsItem
{
	Q_OBJECT 

public:	
	enum { Type = UserType + 1 };
	enum EnImageResMode 
	{
		LowResImageMode,
		HiResImageMode
	};
	
private:
	static QString LowResWarningImage;
	static int LowResMinDpis;
	static bool TouchInterface;
	static bool OldSnapToGrid;
	static bool ThresholdMoving;
	static bool OldThresholdMoving;
	static bool DefaultNoImageMessage;

	//-- Loaded Resources
	STImage CurrImage;
	QImage PaintedImage;
	QImage MaskImage;
	QImage FrameImage;

	//-- Resources
	Resource FrameResource;
	Resource ImageResource;
	Resource MaskResource;

	//-- Internal
	QRect LastMaxResRect;
	GraphicsPhotoItemLoadThread* MLoadThread;
	QMatrix ImageMatrix;
	qreal CurrScale;
	qreal ImageModified;
	QPointF PanningPoint;
	QPointF InitPanningPoint;
	EnImageResMode ImageMode;
	QString ImagesSourcePath; 
	qreal Opacity; 
	QString ImageMD5Sum; 
	QColor EmptyFrameColor;
	static QSize MaxLowResImageSize;
	static QBrush SelectedBrush;
	static int LowResImageDPIs; 
	bool Moving; 
	bool RestoreZValue; 
	qreal OldZValue; 
	Qt::AspectRatioMode AspectRatioMode; 
	QAction* PanAct;
	QSize BigImageSize;
	bool WarningVisible;
	bool ShowNoImageMessage;
	bool MultiSelection;
	bool ImageEncrypted;
	bool AutoAdjustFramesToImages;
	bool IgnoreExifRotation;
	bool OrientationChanged;
	
protected:	
	bool ImageLoaded;
	bool PanningEnabled;
	QSize imageWindowDpis(const QSizeF& _ItemRectInmm, const QSize& _ImageClipSize, const QSize& _PaintedImageSize, const QSize& _OriginalImageSize);
	void createPaintedImage(const Qt::TransformationMode&  _CTransformMode, const QRect& _MaxResRect);
	void updateToolTip();
	virtual void updateCursor();

private:	
	void init();
	QPointF insideSceneRect(const QPointF& _Point);
	void checkForImageOrientation(); 

public:
	GraphicsPhotoItem(QGraphicsItem* _Parent = 0);
	~GraphicsPhotoItem();
	AbstractGraphicsItem* clone() const;
	int type() const { return Type; }

	// --- Resources
	QStringList saveResources(const QDir& _StoreDir, bool _SaveImageRes = true);
	void setResource(const Resource& _Resource);

	// --- Alpha channel ---
	void setAlphaChannel(const Resource& _Resource, const QImage& _AlphaChannel); //Preloaded
	bool hasAlphaChannel() const { return !MaskResource.isNull(); }
	void setAlphaChannel(const Resource& _AlphaChannel);
	Resource maskResource() const { return MaskResource; }

	// --- Frame image ---
	bool hasFrameImage() const { return !FrameResource.isNull(); }
	void setFrameResource(const Resource& _Resource);
	Resource frameResource() const { return FrameResource; }

	//-- Image
	//! OnScreen Image
	QImage paintedImage() const { return PaintedImage; }
	//! On Screen Image generated from CurrentImage.
	QImage originalPaintedImage();
	void updatePaintedImage(const QImage& _Image);
	//! Sets preloaded image.
	void setImage(const QImage& _Image, const QString& _ImageFileName);
	//! Sets only the filename without memory and thumbnail images changes.
	void setImageFileName(const QString& _ImageFileName);
	void setImage(STDom::DImageDoc& _Image);
	void setThumbnail(const QPixmap& _ThumbNail, const QString& _ImageFileName);
	void setThumbnail(const QImage& _ThumbNail, const QString& _ImageFileName);
	void setDoc(STDom::DDoc* _Doc);
	void loadImage();
	//! returns true if big image is loaded.
	bool imageLoaded() const { return ImageLoaded; }
	void loadImageSpawn();
	void waitForImageLoaded();
	void unloadImage();
	//! Unsets the item image file.
	void clearImage();
	QString imageMD5Sum() const { return ImageMD5Sum; }
	bool hasImage() const { return !ImageResource.isNull(); }
	void setImageMode(EnImageResMode _ImageMode) { ImageMode = _ImageMode; }
	Resource imageResource() const { return ImageResource; }


	//-- Manipulation
	void adjustRectToImage();
	void adjustRectToImage(const QSize& _ImageSize);
	//! Rotates image in counterclockwise degrees.
	void rotateImage(qreal _Degrees = -90);
	void scaleImage(qreal _Scale);
	void setImageScale(qreal _Scale);
	qreal imageScale();
	qreal fitInImageScale();
	void shrink(double _Amount);
	void setOpacity(qreal _Value);
	qreal opacity() const { return Opacity; }

	//-- Xml
	//! If _LoadDir is empty (default) all paths are handled as absolute, else all paths are relative to _LoadDir.
	void loadElement(const QDomElement& _Element, const QString& _LoadDir = "");
	//! If Storedir is empty (default) all paths are stored absolute, else path are stored relative to _StoreDir.
	QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "") const;
	static QString tagName() { return "photoitem"; }


	//-- Configuration
	void setAutoAdjustFramesToImages(bool _Value);
	void setIgnoreExifRotation(bool _Value) { IgnoreExifRotation = _Value; }
	void setAspectRatioMode(Qt::AspectRatioMode _Value) { AspectRatioMode = _Value; }
	Qt::AspectRatioMode aspectRatioMode() const { return AspectRatioMode; }
	void setWarningVisible(bool _Value) { WarningVisible = _Value; }
	void setShowNoImageMessage(bool _Value) { ShowNoImageMessage = _Value; }
	static void setDefaultShowNoImageMessage(bool _Value) { DefaultNoImageMessage = _Value; }
	void setMultiSelection(bool _Value) { MultiSelection = _Value; }
	bool multiSelection() const { return MultiSelection; }
	void setImageEncrypted(bool _Value) { ImageEncrypted = _Value; }
	bool imageEncrypted() const { return ImageEncrypted; }
	static void setTouchInterface(bool _Value) { TouchInterface = _Value; }
	static bool thresholdMoving() { return ThresholdMoving; }
	static void setThresholdMoving(bool _Value) { ThresholdMoving = _Value; }
	static void setMaxLowResImageSize(const QSize& _Size) { MaxLowResImageSize = _Size; }
	static void setSelectedBrush(const QBrush& _Brush) { SelectedBrush = _Brush; } 
	static void setLowResImageDPIs(int _Dpis) { LowResImageDPIs = _Dpis; }
	static int lowResImageDPIs() { return LowResImageDPIs; }
	static bool encryptedByFileName(const QString& _FilePath);
	static void disableMoveConstraints();
	static void restoreMoveConstraints();
	static void setLowResWarning(const QString& _WarningImage, int _MinDpis);

	//-- Information
	QPointF rectPos() const;
	void setRectPos(const QPointF& _Pos);
	void setRectPos(qreal _X, qreal _Y);

protected:
	void paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* );
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );
	void mousePressEvent(QGraphicsSceneMouseEvent* _Event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* _Event);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void dropEvent(QGraphicsSceneDragDropEvent* _Event );

public slots:
	void panImage(const QPointF& _PanningPoint);	
	virtual void setPanningEnabled(bool _Enabled);
	void slotZoomImageIn();
	void slotZoomImageOut();
	void slotRotateImage();
	void slotFitImage();
	
signals:
	void mousePanning(const QPointF& _PanningPoint); 
	void imageDropped(const QString& _FileName, const QString& _MD5Sum); 
	void imageRemoved(const QString& _FileName, const QString& _MD5Sum); 
	void imageListDropped(const QList<QUrl>& _Urls);
};

}

#endif
