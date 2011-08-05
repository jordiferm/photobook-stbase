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
#include "graphicsclipartitem.h"
#include <QDomElement> 
#include <QFileInfo> 
#include <QSvgRenderer> 
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>
#include <QTransform>

#include "graphicsitemmodifier.h"

using namespace SPhotoBook;
void GraphicsClipartItem::init()
{
	QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable);
  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable);
	Opacity = 1; 
	AbstractGraphicsItem::updateToolTip();
	createStandardCorners();
	setControlsVisible(false);
	OldRenderer = 0;
}



GraphicsClipartItem::GraphicsClipartItem(const Resource& _Resource, QGraphicsItem* _Parent)
 : QGraphicsSvgItem(_Resource.fileInfo().absoluteFilePath(), _Parent), ClipartResource(_Resource), AbstractGraphicsItem(this)
{
	init();
}

GraphicsClipartItem::GraphicsClipartItem(QGraphicsItem* _Parent)
 : QGraphicsSvgItem(_Parent), AbstractGraphicsItem(this)
{
	init(); 
}

void GraphicsClipartItem::loadElement(const QDomElement& _Element, const QString& _LoadDir)
{
	setPos(_Element.attribute("x", "0").toDouble(), 
			_Element.attribute("y", "0").toDouble());
	Opacity = _Element.attribute("opacity", "1").toDouble();

	setClipartResource(Resource::resourceFromXmlSrc(_Element.attribute("src", ""), _LoadDir));
	//renderer()->load(ImageSourcePath + "/" + _Element.attribute("src", ""));
	setTransform(AbstractGraphicsItem::loadTransformElement(_Element));
	AbstractGraphicsItem::loadEffectElements(this,  _Element);
	AbstractGraphicsItem::updateToolTip();
}

QDomElement GraphicsClipartItem::createElement(QDomDocument& _Doc, const QString& _StoreDir) const
{
	QDomElement MElement = _Doc.createElement(tagName());
	MElement.setAttribute("x", pos().x()); 
	MElement.setAttribute("y", pos().y()); 
	MElement.setAttribute("opacity", Opacity); 
	MElement.setAttribute("src",  Resource::fileInfoToXmlSrc(ClipartResource.fileInfo(), _StoreDir));
	MElement.appendChild(AbstractGraphicsItem::createTransformElement(this, _Doc));
	//Effects
	AbstractGraphicsItem::appendEffectElements(MElement, this, _Doc);

	return MElement; 	
}

void GraphicsClipartItem::setClipartResource(const Resource& _Resource)
{
	ClipartResource = _Resource;
	//Tricky to update geometry !
	QSvgRenderer *renderer = new QSvgRenderer(_Resource.fileInfo().absoluteFilePath());
	setSharedRenderer(renderer);
	if (OldRenderer)
		delete OldRenderer;
	OldRenderer = renderer;

	//renderer()->load(_Resource.fileInfo().absoluteFilePath());
	//MBoundingRect.setSize(QRectF(QPointF(0, 0), renderer()->defaultSize()).size());
}

QStringList GraphicsClipartItem::saveResources(const QDir& _StoreDir)
{
	QStringList Res;
	if (!ClipartResource.isNull())
		Res << ClipartResource.save(_StoreDir);
	return Res;
}


void GraphicsClipartItem::setOpacity(qreal _Value)
{
	Opacity = _Value;
	qDebug("Set Opacity %f", _Value);
	update(); 
	modified();
}


void GraphicsClipartItem::scaleToHeight(qreal _Height)
{
	QRectF BRect = boundingRect();
	qreal SY = _Height / BRect.height(); 
	Modifier->scale(SY, SY); 
	modified();
}

void GraphicsClipartItem::scaleToWidth(qreal _Width)
{
	QRectF BRect =  transform().mapRect(boundingRect());
	qreal SX = _Width / BRect.width(); 
	Modifier->scale(SX, SX); 
	modified();
}

AbstractGraphicsItem* GraphicsClipartItem::clone() const
{
	GraphicsClipartItem* Res = new GraphicsClipartItem;
	QDomDocument Doc;
	Res->loadElement(createElement(Doc));
	return Res;
}


void GraphicsClipartItem::paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* _Widget)
{
	_P->setOpacity(Opacity);
	QGraphicsSvgItem::paint(_P, _Option, _Widget);
}


QVariant GraphicsClipartItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		setControlsVisible(isSelected());
	else
	if (change == ItemPositionHasChanged)
	{
		AbstractGraphicsItem::updateToolTip();
		modified();
	}
	return QGraphicsItem::itemChange(change, value);
}

