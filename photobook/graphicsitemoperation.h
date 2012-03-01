/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef GRAPHICSITEMOPERATION_H
#define GRAPHICSITEMOPERATION_H
#include <QColor>
#include <QFont> 
#include <QUndoCommand>
#include <QBrush> 
#include <QPen> 
#include <QTextCharFormat>
#if QT_VERSION >= 0x040600
#include <QGraphicsEffect>
#endif
#include "stphotobookexport.h"
#include "sterror.h" 
#include "collectioninfo.h"
#include "resource.h"

/**
Abstract class for Graphics item operations.

	@author Jordi Fernandez <shadow@softwarelliure.com>
*/
class QGraphicsItem; 
class QAbstractGraphicsShapeItem;
class STImage;

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT GraphicsItemOperation : public QUndoCommand
{
protected:
	QGraphicsItem* Item;
	
public:
	GraphicsItemOperation(QGraphicsItem* _Item, const QString& _Text, QUndoCommand * parent = 0);
	QGraphicsItem* item() const { return Item; } 
	//! Clonation for all supported items.
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem) { return 0; }
};


/**
Zoom the image
	@author jfernandez@ailiimage.com
*/
class GraphicsPhotoItem;
class ST_PHOTOBOOK_EXPORT STScaleImageGIO : public GraphicsItemOperation
{
	qreal Scale;
	
public:
	STScaleImageGIO(GraphicsPhotoItem* _Item, qreal _Scale, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets mask for image frame.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class GraphicsPhotoItem;
class ST_PHOTOBOOK_EXPORT STSetMaskGIO : public GraphicsItemOperation
{
	QString MaskImageFile;
	Resource LastMaskResource;
	
public:
	STSetMaskGIO(GraphicsPhotoItem* _Item, const QString& _MaskImageFile, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets mask for image frame.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class GraphicsPhotoItem;
class ST_PHOTOBOOK_EXPORT STSetFrameGIO : public GraphicsItemOperation
{
	QString FrameImageFile;
	Resource LastFrameResource;

public:
	STSetFrameGIO(GraphicsPhotoItem* _Item, const QString& _FrameImageFile, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

#if QT_VERSION >= 0x040600

/**
Sets QGraphicsEffect for QGraphicsItem.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemGraphicsEffectGIO : public GraphicsItemOperation
{
	QGraphicsEffect* NewEffect;
	QGraphicsEffect* OldEffect;

public:
	STSetItemGraphicsEffectGIO(QGraphicsItem* _Item, QGraphicsEffect* _Effect, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

#endif

/**
Zoom the image
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STRotateImageGIO : public GraphicsItemOperation
{
	qreal Degrees;
	
public:
	STRotateImageGIO(GraphicsPhotoItem* _Item, qreal _Degrees, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Fits in the image to item rect.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STFitInImageGIO : public GraphicsItemOperation
{
	qreal OldScale;
	
public:
	STFitInImageGIO(GraphicsPhotoItem* _Item, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Adjust frame to image size.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STFitInFrameGIO : public GraphicsItemOperation
{
	QRectF OldRect;
	qreal OldScale;

public:
	STFitInFrameGIO(GraphicsPhotoItem* _Item, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Changes the text for TextItem.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class GraphicsTextItem;
class ST_PHOTOBOOK_EXPORT STChangeTextGIO : public GraphicsItemOperation
{
	GraphicsTextItem* CItem;
	QString OldHtml;
	QString NewHtml;
	
public:
	STChangeTextGIO(GraphicsTextItem* _Item, const QString& _NewHtml, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
};

/**
Set Brush .
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemBrushGIO : public GraphicsItemOperation
{
	QBrush OldBrush; 
	QBrush Brush; 
	
public:
	STSetItemBrushGIO(QAbstractGraphicsShapeItem* _Item, const QBrush& _Brush, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Hides an item.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STDeleteItemGIO : public GraphicsItemOperation
{
	
public:
	STDeleteItemGIO(QGraphicsItem* _Item, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Changes the item Z Value.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetZValueGIO : public GraphicsItemOperation
{
	qreal OldZValue; 
	qreal ZValue; 
	
public:
	STSetZValueGIO(QGraphicsItem* _Item, qreal _ZValue, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STRotateItemGIO : public GraphicsItemOperation
{
	qreal Angle; 
	
	void rotateItem(qreal Angle);
public:
	STRotateItemGIO(QGraphicsItem* _Item, qreal _Angle, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STScaleItemGIO : public GraphicsItemOperation
{
	qreal FactorX, FactorY; 
	
	void scaleItem(qreal _FactorX, qreal _FactorY);
public:
	STScaleItemGIO(QGraphicsItem* _Item, qreal _FactorX, qreal _FactorY, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Border size changes...
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemBorderSizeGIO : public GraphicsItemOperation
{
	int BorderSize; 
	QPen OldPen; 
	
public:
	STSetItemBorderSizeGIO(QAbstractGraphicsShapeItem* _Item, int _BorderSize, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Border size changes...
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemBorderColorGIO : public GraphicsItemOperation
{
	QColor Color;
	QPen OldPen; 
	
public:
	STSetItemBorderColorGIO(QAbstractGraphicsShapeItem* _Item, const QColor& _Color, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the frame opacity...
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemOpacityGIO : public GraphicsItemOperation
{
	qreal OldOpacity; 
	qreal Opacity; 
	
public:
	STSetItemOpacityGIO(QGraphicsItem* _Item, qreal _Opacity, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the x pos...
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemXPosGIO : public GraphicsItemOperation
{
	qreal OldXPos; 
	qreal XPos; 
	
public:
	STSetItemXPosGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the y pos...
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemYPosGIO : public GraphicsItemOperation
{
	qreal OldYPos; 
	qreal YPos; 
	
public:
	STSetItemYPosGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the item size ...
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STSetItemSizeGIO : public GraphicsItemOperation
{
	QSizeF OldSize; 
	QSizeF Size; 
	
public:
	STSetItemSizeGIO(QGraphicsItem* _Item, const QSizeF& _Size, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Move the item horizontally leaving the item bouding rect right edge at the given X coordinate.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STMoveRightGIO : public GraphicsItemOperation
{
	qreal OldXPos; 
	qreal XPos; 
	
public:
	STMoveRightGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Move the center of item horizontally at the given X coordinate.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STMoveXCenterGIO : public GraphicsItemOperation
{
	qreal OldXPos; 
	qreal XPos; 
	
public:
	STMoveXCenterGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Move the center of item horizontally at the given Y coordinate.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STMoveYCenterGIO : public GraphicsItemOperation
{
	qreal OldYPos; 
	qreal YPos; 
	
public:
	STMoveYCenterGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Move the item horizontally leaving the item bouding rect right edge at the given X coordinate.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class ST_PHOTOBOOK_EXPORT STMoveBottomGIO : public GraphicsItemOperation
{
	qreal OldYPos; 
	qreal YPos; 
	
public:
	STMoveBottomGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual GraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Sets an image to GraphicsPhotoItem
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/
class GraphicsPhotoItem;
class ST_PHOTOBOOK_EXPORT STSetImageGIO : public GraphicsItemOperation
{
	QString ImageFileName;
	Resource LastImageResource;
	QImage Image; 
	

public:
	STSetImageGIO(GraphicsPhotoItem* _Item, const QImage& _Image, const QString& _ImageFileName, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();

protected:
	STSetImageGIO(GraphicsPhotoItem* _Item, QUndoCommand* _Parent = 0);
	void setImage(const QImage& _Image) { Image = _Image; }
	void setFileName(const QString& _FileName) { ImageFileName = _FileName; }
};



/**
Performs a filter to GraphicsPhotoItem.
	@author Jordi Fernandez <jfernandez@ailiimage.com>
*/

class ST_PHOTOBOOK_EXPORT STImageFilterGIO : public STSetImageGIO
{
public:
	typedef enum EnFilterType
	{
		FTBlackAndWhite, 
		FTSepia,
		FTImproveColorBalance,
		FTFlipX, 
		FTFlipY
	} TFilterType; 

private: 
	ST_DECLARE_ERRORCLASS();
	CollectionInfo PBInfo;
	TFilterType FilterType; 

public:
	STImageFilterGIO(GraphicsPhotoItem* _Item, const CollectionInfo& _PBInfo, TFilterType _FilterType, QUndoCommand* _Parent = 0);
	GraphicsItemOperation* clone(QGraphicsItem* _NewItem);

protected: 
	virtual void processImage(STImage& ); 
};

}

#endif


