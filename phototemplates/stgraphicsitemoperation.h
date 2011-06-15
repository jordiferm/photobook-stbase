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

#ifndef STGRAPHICSITEMOPERATION_H
#define STGRAPHICSITEMOPERATION_H
#include <QColor>
#include <QFont> 
#include <QUndoCommand>
#include <QBrush> 
#include <QPen> 
#include <QTextCharFormat>
#include <QGraphicsEffect>
#include "stphototemplatesexport.h"
#include "sterror.h" 
#include "stphotobookcollectioninfo.h"

/**
Abstract class for Graphics item operations.

	@author Jordi Fernandez <shadow@softwarelliure.com>
*/
class QGraphicsItem; 
class ST_PHOTOTEMPLATES_EXPORT STGraphicsItemOperation : public QUndoCommand
{
protected:
	QGraphicsItem* Item;
	
public:
	STGraphicsItemOperation(QGraphicsItem* _Item, const QString& _Text, QUndoCommand * parent = 0);
	QGraphicsItem* item() const { return Item; } 
	//! Clonation for all supported items.
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem) { return 0; }
};


/**
Zoom the image
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsPhotoItem;
class ST_PHOTOTEMPLATES_EXPORT STScaleImageGIO : public STGraphicsItemOperation
{
	qreal Scale;
	
public:
	STScaleImageGIO(STGraphicsPhotoItem* _Item, qreal _Scale, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets mask for image frame.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsPhotoItem;
class ST_PHOTOTEMPLATES_EXPORT STSetMaskGIO : public STGraphicsItemOperation
{
	QString MaskImageFile;
	QImage LastMaskImage; 
	
public:
	STSetMaskGIO(STGraphicsPhotoItem* _Item, const QString& _MaskImageFile, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets mask for image frame.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsPhotoItem;
class ST_PHOTOTEMPLATES_EXPORT STSetFrameGIO : public STGraphicsItemOperation
{
	QString FrameImageFile;
	QImage LastFrameImage;

public:
	STSetFrameGIO(STGraphicsPhotoItem* _Item, const QString& _FrameImageFile, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets shadow for image frame.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsPhotoItem;
class ST_PHOTOTEMPLATES_EXPORT STSetItemShadowGIO : public STGraphicsItemOperation
{
	qreal ShadowDepth;
	qreal LastShadowDepth; 
	
public:
	STSetItemShadowGIO(STGraphicsPhotoItem* _Item, qreal _ShadowDepth, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets QGraphicsEffect for QGraphicsItem.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemGraphicsEffectGIO : public STGraphicsItemOperation
{
	QGraphicsEffect* NewEffect;
	QGraphicsEffect* OldEffect;

public:
	STSetItemGraphicsEffectGIO(QGraphicsItem* _Item, QGraphicsEffect* _Effect, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Zoom the image
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STRotateImageGIO : public STGraphicsItemOperation
{
	qreal Degrees;
	
public:
	STRotateImageGIO(STGraphicsPhotoItem* _Item, qreal _Degrees, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Fits in the image to item rect.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STFitInImageGIO : public STGraphicsItemOperation
{
	qreal OldScale;
	
public:
	STFitInImageGIO(STGraphicsPhotoItem* _Item, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Adjust frame to image size.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STFitInFrameGIO : public STGraphicsItemOperation
{
	QRectF OldRect;
	qreal OldScale;

public:
	STFitInFrameGIO(STGraphicsPhotoItem* _Item, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Changes the text for simpleTextItem.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsSimpleTextItem;
class ST_PHOTOTEMPLATES_EXPORT STChangeSimpleTextGIO : public STGraphicsItemOperation
{
	STGraphicsSimpleTextItem* CItem;
	QString OldText;
	QString NewText;
	
public:
	STChangeSimpleTextGIO(STGraphicsSimpleTextItem* _Item, const QString& _NewText, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
};


/**
Changes the CharFormat for simpleTextItem.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STChangeSimpleTextCharFormatGIO : public STGraphicsItemOperation
{
	STGraphicsSimpleTextItem* CItem;
	QTextCharFormat OldFormat;
	QTextCharFormat NewFormat;
	
public:
	STChangeSimpleTextCharFormatGIO(STGraphicsSimpleTextItem* _Item, const QTextCharFormat& _NewFormat, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
};

/**
Changes the text for TextItem.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsTextItem;
class ST_PHOTOTEMPLATES_EXPORT STChangeTextGIO : public STGraphicsItemOperation
{
	STGraphicsTextItem* CItem;
	QString OldHtml;
	QString NewHtml;
	
public:
	STChangeTextGIO(STGraphicsTextItem* _Item, const QString& _NewHtml, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
};

/**
Set Brush .
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class QAbstractGraphicsShapeItem;
class ST_PHOTOTEMPLATES_EXPORT STSetItemBrushGIO : public STGraphicsItemOperation
{
	QBrush OldBrush; 
	QBrush Brush; 
	
public:
	STSetItemBrushGIO(QAbstractGraphicsShapeItem* _Item, const QBrush& _Brush, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Set Brush .
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class QAbstractGraphicsShapeItem;
class ST_PHOTOTEMPLATES_EXPORT STSetTextItemBackgroundBrushGIO : public STGraphicsItemOperation
{
	QBrush OldBrush; 
	QBrush Brush; 
	
public:
	STSetTextItemBackgroundBrushGIO(STGraphicsSimpleTextItem* _Item, const QBrush& _Brush, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Changes the font for STGraphicsSimpleTextItem*.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemFontGIO : public STGraphicsItemOperation
{
	QFont OldFont; 
	QFont Font; 
	
public:
	STSetItemFontGIO(STGraphicsSimpleTextItem* _Item, const QFont& _Font, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Hides an item.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STDeleteItemGIO : public STGraphicsItemOperation
{
	
public:
	STDeleteItemGIO(QGraphicsItem* _Item, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Changes the item Z Value.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetZValueGIO : public STGraphicsItemOperation
{
	qreal OldZValue; 
	qreal ZValue; 
	
public:
	STSetZValueGIO(QGraphicsItem* _Item, qreal _ZValue, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STRotateItemGIO : public STGraphicsItemOperation
{
	qreal Angle; 
	
	void rotateItem(qreal Angle);
public:
	STRotateItemGIO(QGraphicsItem* _Item, qreal _Angle, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STScaleItemGIO : public STGraphicsItemOperation
{
	qreal FactorX, FactorY; 
	
	void scaleItem(qreal _FactorX, qreal _FactorY);
public:
	STScaleItemGIO(QGraphicsItem* _Item, qreal _FactorX, qreal _FactorY, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Border size changes...
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemBorderSizeGIO : public STGraphicsItemOperation
{
	int BorderSize; 
	QPen OldPen; 
	
public:
	STSetItemBorderSizeGIO(QAbstractGraphicsShapeItem* _Item, int _BorderSize, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Border size changes...
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemBorderColorGIO : public STGraphicsItemOperation
{
	QColor Color;
	QPen OldPen; 
	
public:
	STSetItemBorderColorGIO(QAbstractGraphicsShapeItem* _Item, const QColor& _Color, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Changes the text pen.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetSimpleTextPenGIO : public STGraphicsItemOperation
{
	STGraphicsSimpleTextItem* CItem;
	QPen Pen;
	QPen OldPen;
	
public:
	STSetSimpleTextPenGIO(STGraphicsSimpleTextItem* _Item, const QPen& _Pen, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the frame opacity...
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemOpacityGIO : public STGraphicsItemOperation
{
	qreal OldOpacity; 
	qreal Opacity; 
	
public:
	STSetItemOpacityGIO(QGraphicsItem* _Item, qreal _Opacity, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the x pos...
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemXPosGIO : public STGraphicsItemOperation
{
	qreal OldXPos; 
	qreal XPos; 
	
public:
	STSetItemXPosGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the y pos...
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemYPosGIO : public STGraphicsItemOperation
{
	qreal OldYPos; 
	qreal YPos; 
	
public:
	STSetItemYPosGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Sets the item size ...
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STSetItemSizeGIO : public STGraphicsItemOperation
{
	QSizeF OldSize; 
	QSizeF Size; 
	
public:
	STSetItemSizeGIO(QGraphicsItem* _Item, const QSizeF& _Size, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Move the item horizontally leaving the item bouding rect right edge at the given X coordinate.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STMoveRightGIO : public STGraphicsItemOperation
{
	qreal OldXPos; 
	qreal XPos; 
	
public:
	STMoveRightGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};

/**
Move the center of item horizontally at the given X coordinate.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STMoveXCenterGIO : public STGraphicsItemOperation
{
	qreal OldXPos; 
	qreal XPos; 
	
public:
	STMoveXCenterGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Move the center of item horizontally at the given Y coordinate.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STMoveYCenterGIO : public STGraphicsItemOperation
{
	qreal OldYPos; 
	qreal YPos; 
	
public:
	STMoveYCenterGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Move the item horizontally leaving the item bouding rect right edge at the given X coordinate.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STMoveBottomGIO : public STGraphicsItemOperation
{
	qreal OldYPos; 
	qreal YPos; 
	
public:
	STMoveBottomGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();
	virtual STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);
};


/**
Sets an image to GraphicsPhotoItem
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/
class STGraphicsPhotoItem;
class ST_PHOTOTEMPLATES_EXPORT STSetImageGIO : public STGraphicsItemOperation
{
	QString ImageFileName;
	QString LastImageFileName; 
	QImage Image; 
	

public:
	STSetImageGIO(STGraphicsPhotoItem* _Item, const QImage& _Image, const QString& _ImageFileName, QUndoCommand* _Parent = 0);
	virtual void redo();
	virtual void undo();

protected:
	STSetImageGIO(STGraphicsPhotoItem* _Item, QUndoCommand* _Parent = 0);
	void setImage(const QImage& _Image) { Image = _Image; }
	void setFileName(const QString& _FileName) { ImageFileName = _FileName; }
};



/**
Performs a filter to GraphicsPhotoItem.
	@author Jordi Fernandez <shadow@starblitz-k.com>
*/

class STImage; 
class ST_PHOTOTEMPLATES_EXPORT STImageFilterGIO : public STSetImageGIO
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
	STPhotobookCollectionInfo PBInfo;
	TFilterType FilterType; 

public:
	STImageFilterGIO(STGraphicsPhotoItem* _Item, const STPhotobookCollectionInfo& _PBInfo, TFilterType _FilterType, QUndoCommand* _Parent = 0);
	STGraphicsItemOperation* clone(QGraphicsItem* _NewItem);

protected: 
	virtual void processImage(STImage& ); 
};



#endif


