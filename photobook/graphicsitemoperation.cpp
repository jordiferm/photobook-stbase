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

#include "graphicsitemoperation.h"

#include "graphicsphotoitem.h"
#include "graphicstextitem.h"
#include "graphicsclipartitem.h"
#include "graphicspageitem.h"

//Filter
#include "sttemporaryimagefile.h" 
#include "stimage.h" 
#include "collectioninfo.h"

//Modifications
#include "graphicsitemmodifier.h"

using namespace SPhotoBook;

// ____________________________________________________________________________
//
// Class GraphicsItemOperation
// ____________________________________________________________________________

GraphicsItemOperation::GraphicsItemOperation(QGraphicsItem* _Item, const QString& _Text, QUndoCommand* _Parent)
	: QUndoCommand(_Text, _Parent), Item(_Item)
{
}

	
// ____________________________________________________________________________
//
// Class STScaleImageGIO
// ____________________________________________________________________________

STScaleImageGIO::STScaleImageGIO(GraphicsPhotoItem* _Item, qreal _Scale, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Scale image"), _Parent), Scale(_Scale)
{
} 

void STScaleImageGIO::redo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->scaleImage(Scale); 
}

void STScaleImageGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->scaleImage(1 / Scale); 
}

GraphicsItemOperation* STScaleImageGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STScaleImageGIO(CItem, Scale);

	return Res;
}

// ____________________________________________________________________________
//
// Class STSetMaskGIO
// ____________________________________________________________________________

STSetMaskGIO::STSetMaskGIO(GraphicsPhotoItem* _Item, const QString& _MaskImageFile, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Set Mask"), _Parent), MaskImageFile(_MaskImageFile)
{
} 

void STSetMaskGIO::redo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		LastMaskResource = CItem->maskResource();
		CItem->setAlphaChannel(Resource(MaskImageFile, Resource::TypeMask));
	}
}

void STSetMaskGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setAlphaChannel(LastMaskResource);
}


GraphicsItemOperation* STSetMaskGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetMaskGIO(CItem, MaskImageFile);
	
	return Res;
}

// ____________________________________________________________________________
//
// Class STSetFrameGIO
// ____________________________________________________________________________

STSetFrameGIO::STSetFrameGIO(GraphicsPhotoItem* _Item, const QString& _FrameImageFile, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Set Mask"), _Parent), FrameImageFile(_FrameImageFile)
{
}

void STSetFrameGIO::redo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		LastFrameResource = CItem->frameResource();
		CItem->setFrameResource(Resource(FrameImageFile, Resource::TypeFrame));
	}
}

void STSetFrameGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setFrameResource(LastFrameResource);
}


GraphicsItemOperation* STSetFrameGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STSetFrameGIO(CItem, FrameImageFile);

	return Res;
}



// ____________________________________________________________________________
//
// Class STSetItemGraphicsEffectGIO
// ____________________________________________________________________________

STSetItemGraphicsEffectGIO::STSetItemGraphicsEffectGIO(QGraphicsItem* _Item, QGraphicsEffect* _Effect, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Graphics Effect"), _Parent), NewEffect(_Effect)
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


GraphicsItemOperation* STSetItemGraphicsEffectGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemGraphicsEffectGIO(_NewItem, NewEffect);
}


// ____________________________________________________________________________
//
// Class STRotateImageGIO
// ____________________________________________________________________________

STRotateImageGIO::STRotateImageGIO(GraphicsPhotoItem* _Item, qreal _Degrees, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Rotate image"), _Parent), Degrees(_Degrees)
{
} 

void STRotateImageGIO::redo()
{
	GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item);
	if (!CItem)
		CItem = qgraphicsitem_cast<GraphicsPageItem*>(Item);

	if (CItem)
		CItem->rotateImage(Degrees); 
}

void STRotateImageGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->rotateImage(-Degrees); 
}

GraphicsItemOperation* STRotateImageGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);
	if (!CItem)
		CItem = qgraphicsitem_cast<GraphicsPageItem*>(Item);

	if (CItem)
		Res = new STRotateImageGIO(CItem, Degrees);

	return Res;
}

// ____________________________________________________________________________
//
// Class STRotateImageGIO
// ____________________________________________________________________________

STFitInImageGIO::STFitInImageGIO(GraphicsPhotoItem* _Item, QUndoCommand* _Parent)
			: GraphicsItemOperation(_Item, QObject::tr("Fit In"), _Parent), OldScale(1)
{
}

void STFitInImageGIO::redo()
{
	GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item);
	OldScale = CItem->imageScale();
	CItem->setImageScale(CItem->fitInImageScale());
}
	
void STFitInImageGIO::undo()
{
	GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item);
	CItem->setImageScale(OldScale);
}

GraphicsItemOperation* STFitInImageGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STFitInImageGIO(CItem);

	return Res;
}


// ____________________________________________________________________________
//
// Class STFitInFrameGIO
// ____________________________________________________________________________

STFitInFrameGIO::STFitInFrameGIO(GraphicsPhotoItem* _Item, QUndoCommand* _Parent)
			: GraphicsItemOperation(_Item, QObject::tr("Fit In"), _Parent), OldScale(1)
{
}

void STFitInFrameGIO::redo()
{
	GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item);
	OldScale = CItem->imageScale();
	CItem->setImageScale(1);

	OldRect = CItem->rect();
	CItem->adjustRectToImage();
}

void STFitInFrameGIO::undo()
{
	GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item);
	CItem->setRect(OldRect);
	CItem->setImageScale(OldScale);
}

GraphicsItemOperation* STFitInFrameGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);

	if (CItem)
		Res = new STFitInFrameGIO(CItem);

	return Res;
}


// ____________________________________________________________________________
//
// Class STChangeTextGIO
// ____________________________________________________________________________

STChangeTextGIO::STChangeTextGIO(GraphicsTextItem* _Item, const QString& _NewHtml, QUndoCommand* _Parent)
		: GraphicsItemOperation(_Item, QObject::tr("Change ritch text")), OldHtml(""), NewHtml(_NewHtml),
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
		: GraphicsItemOperation(_Item, QObject::tr("Set brush"), _Parent),  Brush(_Brush)
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

GraphicsItemOperation* STSetItemBrushGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	QAbstractGraphicsShapeItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem)))
		CItem = qgraphicsitem_cast<QGraphicsRectItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemBrushGIO(CItem, Brush);
	
	return Res;
}


// ____________________________________________________________________________
//
// Class STSetItemFontGIO
// ____________________________________________________________________________

STDeleteItemGIO::STDeleteItemGIO(QGraphicsItem* _Item, QUndoCommand* _Parent)
		: GraphicsItemOperation(_Item, QObject::tr("Delete item"), _Parent)
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

GraphicsItemOperation* STDeleteItemGIO::clone(QGraphicsItem* _NewItem)
{
	return new STDeleteItemGIO(_NewItem);
}



// ____________________________________________________________________________
//
// Class STSetItemFontGIO
// ____________________________________________________________________________

STSetZValueGIO::STSetZValueGIO(QGraphicsItem* _Item, qreal _ZValue, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Set Z Value"), _Parent), ZValue(_ZValue)
{	
}

void STSetZValueGIO::redo()
{
	if (Item->type() != GraphicsPageItem::Type)
	{
		OldZValue = Item->zValue(); 
		Item->setZValue(ZValue);
	}
}

void STSetZValueGIO::undo()
{
	if (Item->type() != GraphicsPageItem::Type)
		Item->setZValue(OldZValue);
}

GraphicsItemOperation* STSetZValueGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetZValueGIO(_NewItem, ZValue);
}

// ____________________________________________________________________________
//
// Class STRotateItemGIO
// ____________________________________________________________________________

void STRotateItemGIO::rotateItem(qreal Angle)
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		/*QRectF OrigRect = CItem->rect();
		QRectF NRect = OrigRect; 
		NRect.moveCenter(QPointF(0, 0));
		CItem->setRect(NRect);
		CItem->rotate(Angle);
		QMatrix Mat; 
		Mat.rotate(-Angle);
		QPointF NewCenter = Mat.map(OrigRect.center());
		NRect.moveCenter(NewCenter);
		CItem->setRect(NRect);
		QTransform TransForm;
		TransForm = TransForm.rotate(Angle, Qt::ZAxis);
		CItem->setTransform(TransForm);*/
		CItem->modifier()->rotate(CItem->modifier()->zRotationAngle() + Angle);
	}
	else
	{
		Item->rotate(Angle);
	}
}

STRotateItemGIO::STRotateItemGIO(QGraphicsItem* _Item, qreal _Angle, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Rotate ") + QString::number(_Angle), _Parent), Angle(_Angle)
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
		
GraphicsItemOperation* STRotateItemGIO::clone(QGraphicsItem* _NewItem)
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
	: GraphicsItemOperation(_Item, QObject::tr("Scale") , _Parent), FactorX(_FactorX), FactorY(_FactorY)
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
		
GraphicsItemOperation* STScaleItemGIO::clone(QGraphicsItem* _NewItem)
{
	return new STScaleItemGIO(_NewItem, FactorX, FactorY);
}



// ____________________________________________________________________________
//
// Class STSetItemBorderSizeGIO
// ____________________________________________________________________________

STSetItemBorderSizeGIO::STSetItemBorderSizeGIO(QAbstractGraphicsShapeItem* _Item, int _BorderSize, QUndoCommand* _Parent )
	: GraphicsItemOperation(_Item, QObject::tr("Set border size") + QString::number(_BorderSize), _Parent), BorderSize(_BorderSize)
{
}
	
void STSetItemBorderSizeGIO::redo()
{	
	
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	QPen Pen = CItem->pen();
	
	OldPen = Pen; 
	Pen.setWidth(BorderSize);
	if (BorderSize)
	{
		Pen.setStyle(Qt::SolidLine);
		Pen.setJoinStyle(Qt::MiterJoin);
	}
	else 
		Pen.setStyle(Qt::NoPen);
	
	CItem->setPen(Pen);
}

void STSetItemBorderSizeGIO::undo()
{
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	CItem->setPen(OldPen);
}

GraphicsItemOperation* STSetItemBorderSizeGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	QAbstractGraphicsShapeItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem)))
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
	: GraphicsItemOperation(_Item, QObject::tr("Set border color") + _Color.name(), _Parent), Color(_Color)
{
}
	
void STSetItemBorderColorGIO::redo()
{	
	
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	QPen Pen = CItem->pen();
	OldPen = Pen; 
	Pen.setColor(Color); 
	
	CItem->setPen(Pen);
}

void STSetItemBorderColorGIO::undo()
{
	QAbstractGraphicsShapeItem* CItem = static_cast<QAbstractGraphicsShapeItem*>(Item);
	CItem->setPen(OldPen);
}

GraphicsItemOperation* STSetItemBorderColorGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	QAbstractGraphicsShapeItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem)))
		CItem = qgraphicsitem_cast<QGraphicsRectItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemBorderColorGIO(CItem, Color);
	
	return Res;
}

// ____________________________________________________________________________
//
// Class STSetItemOpacityGIO
// ____________________________________________________________________________

STSetItemOpacityGIO::STSetItemOpacityGIO(QGraphicsItem* _Item, qreal _Opacity, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Set Opacity:") + QString::number(_Opacity), _Parent), Opacity(_Opacity)
{	
}

void STSetItemOpacityGIO::redo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}
	else 		
	if (GraphicsClipartItem* CItem = qgraphicsitem_cast<GraphicsClipartItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}		
	else 		
	if (GraphicsPageItem* CItem = qgraphicsitem_cast<GraphicsPageItem*>(Item))
	{
		OldOpacity = CItem->opacity();
		CItem->setOpacity(Opacity);
	}		
	//TODO Put opacity on a base class ?
}
		
void STSetItemOpacityGIO::undo()
{
	if (GraphicsClipartItem* CItem = qgraphicsitem_cast<GraphicsClipartItem*>(Item))
		CItem->setOpacity(OldOpacity);
	else 
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setOpacity(OldOpacity);
	else 
	if (GraphicsPageItem* CItem = qgraphicsitem_cast<GraphicsPageItem*>(Item))
		CItem->setOpacity(OldOpacity);
}

GraphicsItemOperation* STSetItemOpacityGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	QGraphicsItem* CItem = 0;
	//For all supported types.
	if (!(CItem = qgraphicsitem_cast<GraphicsClipartItem*>(_NewItem)))
		if (!(CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem)))
			CItem = qgraphicsitem_cast<GraphicsPageItem*>(_NewItem);
	
	if (CItem)
		Res = new STSetItemOpacityGIO(CItem, Opacity);
	
	return Res;
}



// ____________________________________________________________________________
//
// Class STSetItemOpacityGIO
// ____________________________________________________________________________

STSetItemXPosGIO::STSetItemXPosGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Set X pos:") + QString::number(_XPos), _Parent), XPos(_XPos)
{	
}

void STSetItemXPosGIO::redo()
{
	OldXPos = Item->x();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		
		OldXPos = CItem->rectPos().x(); 
		CItem->setRectPos(QPointF(XPos, CItem->rectPos().y()));
	}	
	else 
		Item->setPos(XPos, Item->y());
}
		
void STSetItemXPosGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setRectPos(QPointF(OldXPos, CItem->rectPos().y()));
	else 
		Item->setPos(OldXPos, Item->y());
}

GraphicsItemOperation* STSetItemXPosGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemXPosGIO(_NewItem, XPos);
}


// ____________________________________________________________________________
//
// Class STSetItemOpacityGIO
// ____________________________________________________________________________

STSetItemYPosGIO::STSetItemYPosGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent )
	: GraphicsItemOperation(_Item, QObject::tr("Set Y pos:") + QString::number(_YPos), _Parent), YPos(_YPos)
{}

void STSetItemYPosGIO::redo()
{
	OldYPos = Item->y();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		
		OldYPos = CItem->rectPos().y(); 
		CItem->setRectPos(QPointF(CItem->rectPos().x(), YPos));
	}	
	else 
		Item->setPos(Item->x(), YPos);
}

void STSetItemYPosGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setRectPos(QPointF(CItem->rectPos().x(), OldYPos));
	else 
		Item->setPos(Item->x(), OldYPos);
}

GraphicsItemOperation* STSetItemYPosGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemYPosGIO(_NewItem, YPos);
}


// ____________________________________________________________________________
//
// Class STSetItemSizeGIO
// ____________________________________________________________________________

STSetItemSizeGIO::STSetItemSizeGIO(QGraphicsItem* _Item, const QSizeF& _Size, QUndoCommand* _Parent) :
	GraphicsItemOperation(_Item, QObject::tr("Change size"), _Parent), Size(_Size)
{}

void STSetItemSizeGIO::redo()
{
	OldSize = Item->transform().mapRect(Item->boundingRect()).size();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
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
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setRect(QRectF(CItem->rect().topLeft(), OldSize));
	else 
	{
		QSizeF CSize = Item->transform().mapRect(Item->boundingRect()).size();
		qreal Sx =  OldSize.width() / CSize.width();
		qreal Sy =  OldSize.height() / CSize.height();
		Item->scale(Sx, Sy); 
	}
}

GraphicsItemOperation* STSetItemSizeGIO::clone(QGraphicsItem* _NewItem)
{
	return new STSetItemSizeGIO(_NewItem, Size);
}

// ____________________________________________________________________________
//
// Class STMoveRightGIO
// ____________________________________________________________________________

STMoveRightGIO::STMoveRightGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Move right: ") + QString::number(_XPos), _Parent), XPos(_XPos)
{
}

void STMoveRightGIO::redo()
{

	OldXPos = Item->x();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
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
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setRectPos(OldXPos, CItem->rectPos().y()); 
	else 
		Item->setPos(OldXPos, Item->y());
}

GraphicsItemOperation* STMoveRightGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveRightGIO(_NewItem, XPos);
}

// ____________________________________________________________________________
//
// Class STMoveXCenterGIO
// ____________________________________________________________________________

STMoveXCenterGIO::STMoveXCenterGIO(QGraphicsItem* _Item, qreal _XPos, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Move X center: ") + QString::number(_XPos), _Parent), XPos(_XPos)
{
}

void STMoveXCenterGIO::redo()
{

	OldXPos = Item->x();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
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
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
 		CItem->setRectPos(OldXPos, CItem->rectPos().y()); 
 	else 
 		Item->setPos(OldXPos, Item->y());
}

GraphicsItemOperation* STMoveXCenterGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveXCenterGIO(_NewItem, XPos);
}

// ____________________________________________________________________________
//
// Class STMoveYCenterGIO
// ____________________________________________________________________________

STMoveYCenterGIO::STMoveYCenterGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Move X center: ") + QString::number(_YPos), _Parent), YPos(_YPos)
{
}

void STMoveYCenterGIO::redo()
{

	OldYPos = Item->y();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
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
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
 		CItem->setRectPos(CItem->rectPos().x(), OldYPos); 
 	else 
 		Item->setPos(Item->x(), OldYPos);
}

GraphicsItemOperation* STMoveYCenterGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveYCenterGIO(_NewItem, YPos);
}


// ____________________________________________________________________________
//
// Class STMoveRightGIO
// ____________________________________________________________________________

STMoveBottomGIO::STMoveBottomGIO(QGraphicsItem* _Item, qreal _YPos, QUndoCommand* _Parent)
: GraphicsItemOperation(_Item, QObject::tr("Move bottom: ") + QString::number(_YPos), _Parent), YPos(_YPos)
{}

void STMoveBottomGIO::redo()
{
	OldYPos = Item->y();
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
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
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
		CItem->setRectPos(CItem->rectPos().x(), OldYPos); 
	else 
		Item->setPos(Item->x(), OldYPos);
}

GraphicsItemOperation* STMoveBottomGIO::clone(QGraphicsItem* _NewItem)
{
	return new STMoveBottomGIO(_NewItem, YPos);
}



// ____________________________________________________________________________
//
// Class STSetImageGIO
// ____________________________________________________________________________

STSetImageGIO::STSetImageGIO(GraphicsPhotoItem* _Item, const QImage& _Image, const QString& _ImageFileName, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Change image"), _Parent), Image(_Image), ImageFileName(_ImageFileName)
{
} 

void STSetImageGIO::redo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		LastImageResource = CItem->imageResource();
		if (Image.isNull())
			Image.load(ImageFileName);

		CItem->setImage(Image, ImageFileName); 
		Image = QImage(); 
	}
}

void STSetImageGIO::undo()
{
	if (GraphicsPhotoItem* CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(Item))
	{
		CItem->setImage(QImage(LastImageResource.fileInfo().absoluteFilePath()), LastImageResource.fileInfo().absoluteFilePath());
	}
}


STSetImageGIO::STSetImageGIO(GraphicsPhotoItem* _Item, QUndoCommand* _Parent)
	: GraphicsItemOperation(_Item, QObject::tr("Change image"), _Parent)
{
} 



// ____________________________________________________________________________
//
// Class STImageFilterGIO
// ____________________________________________________________________________


STImageFilterGIO::STImageFilterGIO(GraphicsPhotoItem* _Item, const CollectionInfo& _PBInfo, TFilterType _FilterType, QUndoCommand* _Parent)
	: STSetImageGIO(_Item, _Parent ), PBInfo(_PBInfo), FilterType(_FilterType)
{
	QString ImageFileName = _Item->imageResource().fileInfo().absoluteFilePath();
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

GraphicsItemOperation* STImageFilterGIO::clone(QGraphicsItem* _NewItem)
{
	GraphicsItemOperation* Res = 0;
	GraphicsPhotoItem* CItem = 0;
	//For all supported types.
	CItem = qgraphicsitem_cast<GraphicsPhotoItem*>(_NewItem);
	
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


