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

#ifndef STGRAPHICSSIMPLETEXTITEM_H
#define STGRAPHICSSIMPLETEXTITEM_H

#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen> 
#include <QTextCharFormat>
#include "stphotolayout.h"
#include "stabstractgraphicsitem.h"
#include "stphototemplatesexport.h"

/**
Class to create graphics text items.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class ST_PHOTOTEMPLATES_EXPORT STGraphicsSimpleTextItem : public QObject, public QGraphicsSimpleTextItem, public STAbstractGraphicsItem	
{

Q_OBJECT 

public:	
	enum { Type = UserType + 2 };


	
private:
	STPhotoLayoutTemplate::Frame Frame;
	QBrush BGBrush; 
	QPen BorderPen; 
	qreal Opacity; 
	void init();

public:
	STGraphicsSimpleTextItem(const STPhotoLayoutTemplate::Frame& _Frame, QGraphicsItem * parent = 0);
	STGraphicsSimpleTextItem(QGraphicsItem * parent = 0);
	~STGraphicsSimpleTextItem();
	void setText(const QString& _Text);
	int type() const { return Type; }	
	void loadElement(QDomElement& _Element);
	QDomElement createElement(QDomDocument& _Doc);
	QBrush backgroundBrush() const { return BGBrush; }
	void setBackgroundBrush(const QBrush& _Brush); 
	void setBorderPen(const QPen& _Pen);
	QPen borderPen() { return BorderPen; }
	void setOpacity(qreal _Value); 
	qreal opacity() const { return Opacity; }
	QTextCharFormat textCharFormat() const;
	void setTextCharFormat(const QTextCharFormat& _Format);
	static QString tagName() { return "simpletextitem"; }

protected:
	void paint(QPainter* _P, const QStyleOptionGraphicsItem* _Option, QWidget* _Widget = 0);	
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
	void editText(); 
};

#endif
