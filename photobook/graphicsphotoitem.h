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

/**
	Thread for load GraphicsPhotoItem images in background.
*/
class QGraphicsSceneMouseEvent;
class QAction;

namespace SPhotoBook
{

class GraphicsPhotoItem;

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
class ST_PHOTOBOOK_EXPORT GraphicsPhotoItem : public QObject, public QGraphicsRectItem, public AbstractGraphicsItem
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
	STImage CurrImage;
	QImage PaintedImage;
	QImage MaskImage;
	QImage FrameImage;
	QString FrameImageFile;
	QRect LastMaxResRect;
	QString CurrImageFileName;
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
	qreal ShadowDepth;
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
	void setCurrImageFileName(const QString& _ImageFileName);
	virtual void updateCursor();
	QSize imageWindowDpis(const QSizeF& _ItemRectInmm, const QSize& _ImageClipSize, const QSize& _PaintedImageSize, const QSize& _OriginalImageSize);
	void createPaintedImage(const Qt::TransformationMode&  _CTransformMode, const QRect& _MaxResRect);
	void updateToolTip();
	
private:	
	void createControls();
	void layoutControlWidget();

	void init();
	QPointF insideSceneRect(const QPointF& _Point);
	void checkForImageOrientation(); 

public:
	GraphicsPhotoItem(QGraphicsItem* _Parent = 0);
	~GraphicsPhotoItem();
	void setAspectRatioMode(Qt::AspectRatioMode _Value) { AspectRatioMode = _Value; }
	Qt::AspectRatioMode aspectRatioMode() const { return AspectRatioMode; }

	// --- Alpha channel ---
	bool hasAlphaChannel() const { return !MaskImage.isNull(); }
	QImage alphaChannel() const { return MaskImage; }
	QString alphaChannelFileName() const;
	void setAlphaChannel(const QImage& _AlphaChannel);
	void setNoAlplaChannel();

	// --- Frame image ---
	bool hasFrameImage() const { return !FrameImage.isNull(); }
	QImage frameImage() const { return FrameImage; }
	QString frameImageFile() const { return FrameImageFile; }
	QString frameImageFileName() const;
	static QFileInfo frameMaskFile(const QString& _FrameImage);
	void setFrameImage(const QString& _FrameImage);

	//! OnScreen Image
	QImage paintedImage() const { return PaintedImage; }
	//! On Screen Image generated from CurrentImage.
	QImage originalPaintedImage();
	void updatePaintedImage(const QImage& _Image);

	//Auto adjusting
	void adjustRectToImage();
	void adjustRectToImage(const QSize& _ImageSize);
	void setAutoAdjustFramesToImages(bool _Value);
	void setIgnoreExifRotation(bool _Value) { IgnoreExifRotation = _Value; }

	//! Sets preloaded image.
	void setImage(const QImage& _Image, const QString& _ImageFileName);
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
	int type() const { return Type; }
	//! Rotates image in counterclockwise degrees.
	void rotateImage(qreal _Degrees = -90);
	void scaleImage(qreal _Scale);
	void setImageScale(qreal _Scale);
	qreal imageScale();
	qreal fitInImageScale();
	//! Sets only the filename without memory and thumbnail images changes.
	void setImageFileName(const QString& _Value);
	QString imageFileName() const { return CurrImageFileName; }
	void setImageMode(EnImageResMode _ImageMode) { ImageMode = _ImageMode; }
	//! Path to find images when calls to loadElement()
	void setImageSourcePath(const QString& _ImagesSourcePath);
	QString imageSourcePath() const { return ImagesSourcePath;}
	void loadElement(const QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc) const;
	void setOpacity(qreal _Value); 
	qreal opacity() const { return Opacity; }
	QString imageMD5Sum() const { return ImageMD5Sum; }
	bool hasImage() const { return !CurrImageFileName.isEmpty(); }
	void setShadowDepth( qreal _Value);
	qreal shadowDepth() const { return ShadowDepth; }
	void setWarningVisible(bool _Value) { WarningVisible = _Value; }
	void setShowNoImageMessage(bool _Value) { ShowNoImageMessage = _Value; }
	void setMultiSelection(bool _Value) { MultiSelection = _Value; }
	bool multiSelection() const { return MultiSelection; }
	void setImageEncrypted(bool _Value) { ImageEncrypted = _Value; }
	bool imageEncrypted() const { return ImageEncrypted; }

	static void setTouchInterface(bool _Value) { TouchInterface = _Value; }
	static QString tagName() { return "photoitem"; }
	static void setMaxLowResImageSize(const QSize& _Size) { MaxLowResImageSize = _Size; }
	static void setSelectedBrush(const QBrush& _Brush) { SelectedBrush = _Brush; } 
	static void setLowResImageDPIs(int _Dpis) { LowResImageDPIs = _Dpis; }
	static int lowResImageDPIs() { return LowResImageDPIs; }
	static bool encryptedByFileName(const QString& _FilePath);

	QRectF boundingRect() const;
	QPointF rectPos() const;
	void setRectPos(const QPointF& _Pos);
	void setRectPos(qreal _X, qreal _Y);
	static void setLowResWarning(const QString& _WarningImage, int _MinDpis);

	virtual AbstractGraphicsItem* clone() const;


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
