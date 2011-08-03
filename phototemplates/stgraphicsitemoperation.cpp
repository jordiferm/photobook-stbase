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

#include <QFileInfo> 

#include "stgraphicsitemoperation.h"

#include "stgraphicsphotoitem.h"
#include "stgraphicssimpletextitem.h"
#include "stgraphicstextitem.h"
#include "stgraphicsclipartitem.h"
#include "stgraphicspageitem.h"

//Filter
#include "sttemporaryimagefile.h" 
#include "stimage.h" 
#include "stphotobookcollectioninfo.h"

// ____________________________________________________________________________
//
// Class STGraphicsItemOperation
// ____________________________________________________________________________

STGraphicsItemOperation::STGraphicsItemOperation(QGraphicsItem* _Item, const QString& _Text, QUndoCommand* _Parent)
	: QUndoCommand(_Text, _Parent), Item(_Item)
{
}

	
// ____________________________________________________________________________
//
// Class STScaleImageGIO
// ____________________________________________________________________________

STScaleImageGIO::STScaleImageGIO(STGraphicsPhotoItem* _Item, qreal _Scale, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Scale image"), _Parent), Scale(_Scale)
{
} 

void STScaleImageGIO::redo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->scaleImage(Scale); 
}

void STScaleImageGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->scaleImage(1 / Scale); 
}

STGraphicsItemOperation* STScaleImageGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0;
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STScaleImageGIO(CItem, Scale);

	return Res;
}

// ____________________________________________________________________________
//
// Class STSetMaskGIO
// ____________________________________________________________________________

STSetMaskGIO::STSetMaskGIO(STGraphicsPhotoItem* _Item, const QString& _MaskImageFile, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Set Mask"), _Parent), MaskImageFile(_MaskImageFile)
{
} 

void STSetMaskGIO::redo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		LastMaskImage = CItem->alphaChannel(); 
		CItem->setAlphaChannel(QImage(MaskImageFile)); 
	}
}

void STSetMaskGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setAlphaChannel(LastMaskImage); 
}


STGraphicsItemOperation* STSetMaskGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetMaskGIO(CItem, MaskImageFile);
	
	return Res;
}

// ____________________________________________________________________________
//
// Class STSetFrameGIO
// ____________________________________________________________________________

STSetFrameGIO::STSetFrameGIO(STGraphicsPhotoItem* _Item, const QString& _FrameImageFile, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Set Mask"), _Parent), FrameImageFile(_FrameImageFile)
{
}

void STSetFrameGIO::redo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		LastFrameImageFile = CItem->frameImageFile();
		CItem->setFrameImage(FrameImageFile);
	}
}

void STSetFrameGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setFrameImage(LastFrameImageFile);
}


STGraphicsItemOperation* STSetFrameGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0;
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STSetFrameGIO(CItem, FrameImageFile);

	return Res;
}


// ____________________________________________________________________________
//
// Class STSetItemShadowGIO
// ____________________________________________________________________________

STSetItemShadowGIO::STSetItemShadowGIO(STGraphicsPhotoItem* _Item, qreal _ShadowDepth, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Change Shadow"), _Parent), ShadowDepth(_ShadowDepth)
{
} 

void STSetItemShadowGIO::redo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		LastShadowDepth = CItem->shadowDepth(); 
		CItem->setShadowDepth(ShadowDepth); 
	}
}

void STSetItemShadowGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setShadowDepth(LastShadowDepth); 
}


STGraphicsItemOperation* STSetItemShadowGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemShadowGIO(CItem, ShadowDepth);
	
	return Res;
}

// ____________________________________________________________________________
//
// Class STSetItemGraphicsEffectGIO
// ____________________________________________________________________________

STSetItemGraphicsEffectGIO::STSetItemGraphicsEffectGIO(QGraphicsItem* _Item, QGraphicsEffect* _Effect, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Graphics Effect"), _Parent), NewEffect(_Effect)
{
}

void STSetItemGraphicsEffectGIO::redo()
{
	OldEffect = Item->graphicsEffect();
	Item->setGraphicsEffect(NewEffect);
}

void STSetItemGraphicsEffectGIO::undo()
{
	Item->setGraphicsEffect(OldEffect);
}


STGraphicsItemOperation* STSetItemGraphicsEffectGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemGraphicsEffectGIO(_NewItem, NewEffect);
}


// ____________________________________________________________________________
//
// Class STRotateImageGIO
// ____________________________________________________________________________

STRotateImageGIO::STRotateImageGIO(STGraphicsPhotoItem* _Item, qreal _Degrees, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Rotate image"), _Parent), Degrees(_Degrees)
{
} 

void STRotateImageGIO::redo()
{
	STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item);
	if (!CItem)
		CItem = qgraphicsitem_cast<STGraphicsPageItem*>(Item);

	if (CItem)
		CItem->rotateImage(Degrees); 
}

void STRotateImageGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->rotateImage(-Degrees); 
}

STGraphicsItemOperation* STRotateImageGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0;
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);
	if (!CItem)
		CItem = qgraphicsitem_cast<STGraphicsPageItem*>(Item);

	if (CItem)
		Res = new STRotateImageGIO(CItem, Degrees);

	return Res;
}

// ____________________________________________________________________________
//
// Class STRotateImageGIO
// ____________________________________________________________________________

STFitInImageGIO::STFitInImageGIO(STGraphicsPhotoItem* _Item, QUndoCommand* _Parent)
			: STGraphicsItemOperation(_Item, QObject::tr("Fit In"), _Parent), OldScale(1)
{
}

void STFitInImageGIO::redo()
{
	STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item);
	OldScale = CItem->imageScale();
	CItem->setImageScale(CItem->fitInImageScale());
}
	
void STFitInImageGIO::undo()
{
	STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item);
	CItem->setImageScale(OldScale);
}

STGraphicsItemOperation* STFitInImageGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0;
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STFitInImageGIO(CItem);

	return Res;
}


// ____________________________________________________________________________
//
// Class STFitInFrameGIO
// ____________________________________________________________________________

STFitInFrameGIO::STFitInFrameGIO(STGraphicsPhotoItem* _Item, QUndoCommand* _Parent)
			: STGraphicsItemOperation(_Item, QObject::tr("Fit In"), _Parent), OldScale(1)
{
}

void STFitInFrameGIO::redo()
{
	STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item);
	OldScale = CItem->imageScale();
	CItem->setImageScale(1);

	OldRect = CItem->rect();
	CItem->adjustRectToImage();
}

void STFitInFrameGIO::undo()
{
	STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item);
	CItem->setRect(OldRect);
	CItem->setImageScale(OldScale);
}

STGraphicsItemOperation* STFitInFrameGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0;
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STFitInFrameGIO(CItem);

	return Res;
}

// ____________________________________________________________________________
//
// Class STChangeSimpleTextGIO
// ____________________________________________________________________________

STChangeSimpleTextGIO::STChangeSimpleTextGIO(STGraphicsSimpleTextItem* _Item, const QString& _NewText, QUndoCommand* _Parent)
		: STGraphicsItemOperation(_Item, QObject::tr("Change text to:'") + _NewText.left(10) + "'...", _Parent), OldText(""), NewText(_NewText), 
										 	CItem(_Item)
{
	
}

void STChangeSimpleTextGIO::redo()
{
	OldText = CItem->text(); 
	CItem->setText(NewText); 
}

void STChangeSimpleTextGIO::undo()
{
	CItem->setText(OldText);
}

// ____________________________________________________________________________
//
// Class STChangeSimpleTextGIO
// ____________________________________________________________________________

STChangeSimpleTextCharFormatGIO::STChangeSimpleTextCharFormatGIO(STGraphicsSimpleTextItem* _Item, const QTextCharFormat& _NewFormat, QUndoCommand* _Parent) : STGraphicsItemOperation(_Item, QObject::tr("Change character format."), _Parent), NewFormat(_NewFormat), CItem(_Item)
{
	
}

void STChangeSimpleTextCharFormatGIO::redo()
{
	OldFormat = CItem->textCharFormat(); 
	CItem->setTextCharFormat(NewFormat); 
}

void STChangeSimpleTextCharFormatGIO::undo()
{
	CItem->setTextCharFormat(OldFormat);
}


// ____________________________________________________________________________
//
// Class STChangeTextGIO
// ____________________________________________________________________________

STChangeTextGIO::STChangeTextGIO(STGraphicsTextItem* _Item, const QString& _NewHtml, QUndoCommand* _Parent)
		: STGraphicsItemOperation(_Item, QObject::tr("Change ritch text")), OldHtml(""), NewHtml(_NewHtml), 
										 	CItem(_Item)
{}

void STChangeTextGIO::redo()
{
	OldHtml = CItem->toHtml(); 
	CItem->setHtml(NewHtml); 
}
																					
void STChangeTextGIO::undo()
{
	CItem->setHtml(OldHtml);
}

// ____________________________________________________________________________
//
// Class STSetItemsBrushGIO
// ____________________________________________________________________________

STSetItemBrushGIO::STSetItemBrushGIO(QAbstractGraphicsShapeItem* _Item, const QBrush& _Brush, QUndoCommand* _Parent)
		: STGraphicsItemOperation(_Item, QObject::tr("Set brush"), _Parent),  Brush(_Brush)
{}

void STSetItemBrushGIO::redo()
{
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	OldBrush = CItem->brush();
	CItem->setBrush(Brush);
}

void STSetItemBrushGIO::undo()
{
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	CItem->setBrush(OldBrush);
}

STGraphicsItemOperation* STSetItemBrushGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	QAbstractGraphicsShapeItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem)))
		if (!(CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem)))
			CItem = qgraphicsitem_cast<QGraphicsRectItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemBrushGIO(CItem, Brush);
	
	return Res;
}

// ____________________________________________________________________________
//
// Class STSetTextItemBackgroundBrushGIO
// ____________________________________________________________________________

STSetTextItemBackgroundBrushGIO::STSetTextItemBackgroundBrushGIO(STGraphicsSimpleTextItem* _Item, const QBrush& _Brush, QUndoCommand* _Parent )
		: STGraphicsItemOperation(_Item, QObject::tr("Set backgroundBrush"), _Parent),  Brush(_Brush)
{
}

void STSetTextItemBackgroundBrushGIO::redo()
{
	STGraphicsSimpleTextItem* CItem = static_cast<STGraphicsSimpleTextItem*>(Item);
	OldBrush = CItem->backgroundBrush();
	CItem->setBackgroundBrush(Brush);
}

void STSetTextItemBackgroundBrushGIO::undo()
{
	STGraphicsSimpleTextItem* CItem = static_cast<STGraphicsSimpleTextItem*>(Item);
	CItem->setBackgroundBrush(OldBrush );
}

		
STGraphicsItemOperation* STSetTextItemBackgroundBrushGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem))
		Res = new STSetTextItemBackgroundBrushGIO(CItem, Brush);
	
	return Res;
}


// ____________________________________________________________________________
//
// Class STSetItemFontGIO
// ____________________________________________________________________________

STSetItemFontGIO::STSetItemFontGIO(STGraphicsSimpleTextItem* _Item, const QFont& _Font, QUndoCommand* _Parent)
		: STGraphicsItemOperation(_Item, QObject::tr("Set font:") + _Font.family() , _Parent),  Font(_Font)
{
}

void STSetItemFontGIO::redo()
{
	STGraphicsSimpleTextItem* CItem = static_cast<STGraphicsSimpleTextItem*>(Item);
	OldFont = CItem->font();
	CItem->setFont(Font);	
}

void STSetItemFontGIO::undo()
{
	STGraphicsSimpleTextItem* CItem = static_cast<STGraphicsSimpleTextItem*>(Item);
	CItem->setFont(OldFont);	
}

STGraphicsItemOperation* STSetItemFontGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	
	if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem))
		Res = new STSetItemFontGIO(CItem, Font);
	
	return Res;
}


// ____________________________________________________________________________
//
// Class STSetItemFontGIO
// ____________________________________________________________________________

STDeleteItemGIO::STDeleteItemGIO(QGraphicsItem* _Item, QUndoCommand* _Parent)
		: STGraphicsItemOperation(_Item, QObject::tr("Delete item"), _Parent)
{
}

void STDeleteItemGIO::redo()
{
	Item->setVisible(false);
}

void STDeleteItemGIO::undo()
{
	Item->setVisible(true);
}

STGraphicsItemOperation* STDeleteItemGIO::clone(QGraphicsItem* _NewItem)
{
	return new STDeleteItemGIO(_NewItem);
}



// ____________________________________________________________________________
//
// Class STSetItemFontGIO
// ____________________________________________________________________________

STSetZValueGIO::STSetZValueGIO(QGraphicsItem* _Item, qreal _ZValue, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Set Z Value"), _Parent), ZValue(_ZValue)
{	
}

void STSetZValueGIO::redo()
{
	if (Item->type() != STGraphicsPageItem::Type)
	{
		OldZValue = Item->zValue(); 
		Item->setZValue(ZValue);
	}
}

void STSetZValueGIO::undo()
{
	if (Item->type() != STGraphicsPageItem::Type)
		Item->setZValue(OldZValue);
}

STGraphicsItemOperation* STSetZValueGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetZValueGIO(_NewItem, ZValue);
}

// ____________________________________________________________________________
//
// Class STRotateItemGIO
// ____________________________________________________________________________

void STRotateItemGIO::rotateItem(qreal Angle)
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		QRectF OrigRect = CItem->rect();
		QRectF NRect = OrigRect; 
		NRect.moveCenter(QPointF(0, 0));
		CItem->setRect(NRect);
		CItem->rotate(Angle);
		QMatrix Mat; 
		Mat.rotate(-Angle);
		QPointF NewCenter = Mat.map(OrigRect.center());
		NRect.moveCenter(NewCenter);
		CItem->setRect(NRect);
	}
	else 
	{
		Item->rotate(Angle);
	}
}

STRotateItemGIO::STRotateItemGIO(QGraphicsItem* _Item, qreal _Angle, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Rotate ") + QString::number(_Angle), _Parent), Angle(_Angle)
{
}


void STRotateItemGIO::redo()
{
	rotateItem(Angle);
}
		
void STRotateItemGIO::undo()
{
	rotateItem(-Angle);
}
		
STGraphicsItemOperation* STRotateItemGIO::clone(QGraphicsItem* _NewItem)
{
	return new STRotateItemGIO(_NewItem, Angle);
}



// ____________________________________________________________________________
//
// Class STScaleItemGIO
// ____________________________________________________________________________

void STScaleItemGIO::scaleItem(qreal _FactorX, qreal _FactorY)
{
	Item->scale(_FactorX, _FactorY);
}

STScaleItemGIO::STScaleItemGIO(QGraphicsItem* _Item, qreal _FactorX, qreal _FactorY, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Scale") , _Parent), FactorX(_FactorX), FactorY(_FactorY)
{
}

void STScaleItemGIO::redo()
{
	scaleItem(FactorX, FactorY);
}
		
void STScaleItemGIO::undo()
{
	scaleItem(1/FactorX, 1/FactorY);
}
		
STGraphicsItemOperation* STScaleItemGIO::clone(QGraphicsItem* _NewItem)
{
	return new STScaleItemGIO(_NewItem, FactorX, FactorY);
}



// ____________________________________________________________________________
//
// Class STSetItemBorderSizeGIO
// ____________________________________________________________________________

STSetItemBorderSizeGIO::STSetItemBorderSizeGIO(QAbstractGraphicsShapeItem* _Item, int _BorderSize, QUndoCommand* _Parent )
	: STGraphicsItemOperation(_Item, QObject::tr("Set border size") + QString::number(_BorderSize), _Parent), BorderSize(_BorderSize)
{
}
	
void STSetItemBorderSizeGIO::redo()
{	
	
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	QPen Pen = CItem->pen();
	if (STGraphicsSimpleTextItem* TItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem))
		Pen = TItem->borderPen();
	
	OldPen = Pen; 
	Pen.setWidth(BorderSize);
	if (BorderSize)
	{
		Pen.setStyle(Qt::SolidLine);
		Pen.setJoinStyle(Qt::MiterJoin);
	}
	else 
		Pen.setStyle(Qt::NoPen);
	
	if (STGraphicsSimpleTextItem* TItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem))
		TItem->setBorderPen(Pen);
	else 
		CItem->setPen(Pen);
}

void STSetItemBorderSizeGIO::undo()
{
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	if (STGraphicsSimpleTextItem* TItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem))
		TItem->setBorderPen(OldPen);
	else 
		CItem->setPen(OldPen);
}

STGraphicsItemOperation* STSetItemBorderSizeGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	QAbstractGraphicsShapeItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem)))
		if (!(CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem)))
			CItem = qgraphicsitem_cast<QGraphicsRectItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemBorderSizeGIO(CItem, BorderSize);
	
	return Res;
}


// ____________________________________________________________________________
//
// Class STSetItemBorderColorGIO
// ____________________________________________________________________________

STSetItemBorderColorGIO::STSetItemBorderColorGIO(QAbstractGraphicsShapeItem* _Item, const QColor& _Color, QUndoCommand* _Parent )
	: STGraphicsItemOperation(_Item, QObject::tr("Set border color") + _Color.name(), _Parent), Color(_Color)
{
}
	
void STSetItemBorderColorGIO::redo()
{	
	
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	QPen Pen = CItem->pen();
	if (STGraphicsSimpleTextItem* TItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem))
		Pen = TItem->borderPen();
	OldPen = Pen; 
	Pen.setColor(Color); 
	
	if (STGraphicsSimpleTextItem* TItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem))
		TItem->setBorderPen(Pen);
	else 
		CItem->setPen(Pen);
}

void STSetItemBorderColorGIO::undo()
{
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	if (STGraphicsSimpleTextItem* TItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(CItem))
		TItem->setBorderPen(OldPen);
	else 
		CItem->setPen(OldPen);
}

STGraphicsItemOperation* STSetItemBorderColorGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	QAbstractGraphicsShapeItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem)))
		if (!(CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem)))
			CItem = qgraphicsitem_cast<QGraphicsRectItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemBorderColorGIO(CItem, Color);
	
	return Res;
}

// ____________________________________________________________________________
//
// Class STSetSimpleTextPenGIO
// ____________________________________________________________________________

STSetSimpleTextPenGIO::STSetSimpleTextPenGIO(STGraphicsSimpleTextItem* _Item, const QPen& _Pen, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Text Outline"), _Parent), Pen(_Pen), CItem(_Item)
{}

void STSetSimpleTextPenGIO::STSetSimpleTextPenGIO::redo()
{
	OldPen = CItem->pen();
	CItem->setPen(Pen); 
}

void STSetSimpleTextPenGIO::undo()
{
	CItem->setPen(OldPen);
}

STGraphicsItemOperation* STSetSimpleTextPenGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	//For all supported types.
	if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem))
		Res = new STSetSimpleTextPenGIO(CItem, Pen);
		
	return Res;	
}


// ____________________________________________________________________________
//
// Class STSetItemOpacityGIO
// ____________________________________________________________________________

STSetItemOpacityGIO::STSetItemOpacityGIO(QGraphicsItem* _Item, qreal _Opacity, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Set Opacity:") + QString::number(_Opacity), _Parent), Opacity(_Opacity)
{	
}

void STSetItemOpacityGIO::redo()
{
	if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}	
	else 
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}
	else 		
	if (STGraphicsClipartItem* CItem = qgraphicsitem_cast<STGraphicsClipartItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}		
	else 		
	if (STGraphicsPageItem* CItem = qgraphicsitem_cast<STGraphicsPageItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}		
	//TODO Put opacity on a base class ?
}
		
void STSetItemOpacityGIO::undo()
{
	if (STGraphicsSimpleTextItem* CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(Item))
		CItem->setOpacity(OldOpacity);
	else 
	if (STGraphicsClipartItem* CItem = qgraphicsitem_cast<STGraphicsClipartItem*>(Item))
		CItem->setOpacity(OldOpacity);
	else 
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setOpacity(OldOpacity);
	else 
	if (STGraphicsPageItem* CItem = qgraphicsitem_cast<STGraphicsPageItem*>(Item))
		CItem->setOpacity(OldOpacity);
}

STGraphicsItemOperation* STSetItemOpacityGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	QGraphicsItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<STGraphicsSimpleTextItem*>(_NewItem)))
		if (!(CItem = qgraphicsitem_cast<STGraphicsClipartItem*>(_NewItem)))
			if (!(CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem)))
				CItem = qgraphicsitem_cast<STGraphicsPageItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemOpacityGIO(CItem, Opacity);
	
	return Res;
}



// ____________________________________________________________________________
//
// Class STSetItemOpacityGIO
// ____________________________________________________________________________

STSetItemXPosGIO::STSetItemXPosGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Set X pos:") + QString::number(_XPos), _Parent), XPos(_XPos)
{	
}

void STSetItemXPosGIO::redo()
{
	OldXPos = Item->x();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		
		OldXPos = CItem->rectPos().x(); 
		CItem->setRectPos(QPointF(XPos, CItem->rectPos().y()));
	}	
	else 
		Item->setPos(XPos, Item->y());
}
		
void STSetItemXPosGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setRectPos(QPointF(OldXPos, CItem->rectPos().y()));
	else 
		Item->setPos(OldXPos, Item->y());
}

STGraphicsItemOperation* STSetItemXPosGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemXPosGIO(_NewItem, XPos);
}


// ____________________________________________________________________________
//
// Class STSetItemOpacityGIO
// ____________________________________________________________________________

STSetItemYPosGIO::STSetItemYPosGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent )
	: STGraphicsItemOperation(_Item, QObject::tr("Set Y pos:") + QString::number(_YPos), _Parent), YPos(_YPos)	
{}

void STSetItemYPosGIO::redo()
{
	OldYPos = Item->y();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		
		OldYPos = CItem->rectPos().y(); 
		CItem->setRectPos(QPointF(CItem->rectPos().x(), YPos));
	}	
	else 
		Item->setPos(Item->x(), YPos);
}

void STSetItemYPosGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setRectPos(QPointF(CItem->rectPos().x(), OldYPos));
	else 
		Item->setPos(Item->x(), OldYPos);
}

STGraphicsItemOperation* STSetItemYPosGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemYPosGIO(_NewItem, YPos);
}


// ____________________________________________________________________________
//
// Class STSetItemSizeGIO
// ____________________________________________________________________________

STSetItemSizeGIO::STSetItemSizeGIO(QGraphicsItem* _Item, const QSizeF& _Size, QUndoCommand* _Parent) :
	STGraphicsItemOperation(_Item, QObject::tr("Change size"), _Parent), Size(_Size)	
{}

void STSetItemSizeGIO::redo()
{
	OldSize = Item->transform().mapRect(Item->boundingRect()).size();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setRect(QRectF(CItem->rect().topLeft(), Size));
	else 
	{
		qreal Sx = Size.width() / OldSize.width();
		qreal Sy = Size.height() / OldSize.height();
		Item->scale(Sx, Sy); 
	}
}

void STSetItemSizeGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setRect(QRectF(CItem->rect().topLeft(), OldSize));
	else 
	{
		QSizeF CSize = Item->transform().mapRect(Item->boundingRect()).size();
		qreal Sx =  OldSize.width() / CSize.width();
		qreal Sy =  OldSize.height() / CSize.height();
		Item->scale(Sx, Sy); 
	}
}

STGraphicsItemOperation* STSetItemSizeGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemSizeGIO(_NewItem, Size);
}

// ____________________________________________________________________________
//
// Class STMoveRightGIO
// ____________________________________________________________________________

STMoveRightGIO::STMoveRightGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Move right: ") + QString::number(_XPos), _Parent), XPos(_XPos)
{
}

void STMoveRightGIO::redo()
{

	OldXPos = Item->x();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		OldXPos = CItem->rectPos().x();
		CItem->setRectPos(XPos - CItem->transform().mapRect(CItem->rect()).width(), CItem->rectPos().y());
	}
	else
	{
		QRectF ItemRect = Item->transform().mapRect(Item->boundingRect()); 
		Item->setPos(XPos - ItemRect.width(), Item->y());
	}
}

void STMoveRightGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setRectPos(OldXPos, CItem->rectPos().y()); 
	else 
		Item->setPos(OldXPos, Item->y());
}

STGraphicsItemOperation* STMoveRightGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveRightGIO(_NewItem, XPos);
}

// ____________________________________________________________________________
//
// Class STMoveXCenterGIO
// ____________________________________________________________________________

STMoveXCenterGIO::STMoveXCenterGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Move X center: ") + QString::number(_XPos), _Parent), XPos(_XPos)
{
}

void STMoveXCenterGIO::redo()
{

	OldXPos = Item->x();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		OldXPos = CItem->rectPos().x();
		CItem->setRectPos(XPos - CItem->transform().mapRect(CItem->rect()).width() / 2, CItem->rectPos().y());
	}
	else
	{
		//qDebug("STMoveXCenterGIO::redo()"); 
		QRectF ItemRect = Item->transform().mapRect(Item->boundingRect()); 
		Item->setPos(XPos - ItemRect.width() / 2, Item->y());
	}
}

void STMoveXCenterGIO::undo()
{
 	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
 		CItem->setRectPos(OldXPos, CItem->rectPos().y()); 
 	else 
 		Item->setPos(OldXPos, Item->y());
}

STGraphicsItemOperation* STMoveXCenterGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveXCenterGIO(_NewItem, XPos);
}

// ____________________________________________________________________________
//
// Class STMoveYCenterGIO
// ____________________________________________________________________________

STMoveYCenterGIO::STMoveYCenterGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Move X center: ") + QString::number(_YPos), _Parent), YPos(_YPos)
{
}

void STMoveYCenterGIO::redo()
{

	OldYPos = Item->y();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		OldYPos = CItem->rectPos().y();
		CItem->setRectPos(CItem->rectPos().x(), YPos - CItem->transform().mapRect(CItem->rect()).height() / 2);
	}
	else
	{
		//qDebug("STMoveXCenterGIO::redo()"); 
		QRectF ItemRect = Item->transform().mapRect(Item->boundingRect()); 
		Item->setPos(Item->x(), YPos - ItemRect.height() / 2);
	}
}

void STMoveYCenterGIO::undo()
{
 	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
 		CItem->setRectPos(CItem->rectPos().x(), OldYPos); 
 	else 
 		Item->setPos(Item->x(), OldYPos);
}

STGraphicsItemOperation* STMoveYCenterGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveYCenterGIO(_NewItem, YPos);
}


// ____________________________________________________________________________
//
// Class STMoveRightGIO
// ____________________________________________________________________________

STMoveBottomGIO::STMoveBottomGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent)
: STGraphicsItemOperation(_Item, QObject::tr("Move bottom: ") + QString::number(_YPos), _Parent), YPos(_YPos)
{}

void STMoveBottomGIO::redo()
{
	OldYPos = Item->y();
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		OldYPos = CItem->rectPos().y();
		CItem->setRectPos(CItem->rectPos().x(), YPos - CItem->transform().mapRect(CItem->rect()).height());
	}
	else
	{
		QRectF ItemRect = Item->transform().mapRect(Item->boundingRect()); 
		Item->setPos(Item->x(), YPos - ItemRect.height());
	}
}

void STMoveBottomGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
		CItem->setRectPos(CItem->rectPos().x(), OldYPos); 
	else 
		Item->setPos(Item->x(), OldYPos);
}

STGraphicsItemOperation* STMoveBottomGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveBottomGIO(_NewItem, YPos);
}



// ____________________________________________________________________________
//
// Class STSetImageGIO
// ____________________________________________________________________________

STSetImageGIO::STSetImageGIO(STGraphicsPhotoItem* _Item, const QImage& _Image, const QString& _ImageFileName, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Change image"), _Parent), Image(_Image), ImageFileName(_ImageFileName)
{
} 

void STSetImageGIO::redo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		LastImageFileName = CItem->imageFileName();
		if (Image.isNull())
			Image.load(ImageFileName);

		CItem->setImage(Image, ImageFileName); 
		Image = QImage(); 
	}
}

void STSetImageGIO::undo()
{
	if (STGraphicsPhotoItem* CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(Item))
	{
		CItem->setImage(QImage(LastImageFileName), LastImageFileName); 
	}
}


STSetImageGIO::STSetImageGIO(STGraphicsPhotoItem* _Item, QUndoCommand* _Parent)
	: STGraphicsItemOperation(_Item, QObject::tr("Change image"), _Parent)
{
} 



// ____________________________________________________________________________
//
// Class STImageFilterGIO
// ____________________________________________________________________________


STImageFilterGIO::STImageFilterGIO(STGraphicsPhotoItem* _Item, const STPhotobookCollectionInfo& _PBInfo, TFilterType _FilterType, QUndoCommand* _Parent)
	: STSetImageGIO(_Item, _Parent ), PBInfo(_PBInfo), FilterType(_FilterType)
{
	QString ImageFileName = _Item->imageFileName();
	if (!ImageFileName.isEmpty()) //Defensive
	{
		STImage Img;
		Assert(Img.load(ImageFileName), Error(QObject::tr("Unable to load image file %1").arg(ImageFileName))); 
		processImage(Img); 
		QString StoredImageName = _PBInfo.imageFilePath(Img, QFileInfo(ImageFileName).suffix());
		Assert(Img.save(StoredImageName), Error(QString(QObject::tr("Error storing image %1")).arg(StoredImageName)));
		setImage(Img); 
		setFileName(StoredImageName); 
	}	
}

STGraphicsItemOperation* STImageFilterGIO::clone(QGraphicsItem* _NewItem)
{
	STGraphicsItemOperation* Res = 0; 
	STGraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<STGraphicsPhotoItem*>(_NewItem);
	
	if (CItem)
		Res = new STImageFilterGIO(CItem, PBInfo, FilterType);
	
	return Res;
}

void STImageFilterGIO::processImage(STImage& _Image)
{
	switch(FilterType)
	{
		case FTBlackAndWhite :
			_Image.blackAndWhite();
		break; 
		case FTSepia : 
			_Image.sepia(); 
		break; 	
		case FTImproveColorBalance : 
			_Image.improveColorBalance(); 
		break;
		case FTFlipX : 
			_Image = _Image.transform(STImage::FlipX); 
		break; 
		case FTFlipY : 
			_Image = _Image.transform(STImage::FlipY); 			
		break; 
	}	
}


