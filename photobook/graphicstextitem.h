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

#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H

#include <QGraphicsTextItem>
#include "abstractgraphicsitem.h"
#include "stphotobookexport.h"

/**
Class to create graphics text items.

*/
class QTextCharFormat;
namespace SPhotoBook
{
class TemplateDataContext;
class ST_PHOTOBOOK_EXPORT GraphicsTextItem : public QGraphicsTextItem, public AbstractGraphicsItem
{
	Q_OBJECT

public:	
	enum { Type = UserType + 3 };
	
private:
	void init();
	
public:
	GraphicsTextItem(QGraphicsItem * parent = 0);
//	void setText(const QString& _Text);
	int type() const { return Type; }
	void setMovable(bool );
	void setCharFormat(const QTextCharFormat& _Format);
	void mergeCharFormat(const QTextCharFormat& _Format);
	QTextCharFormat charFormat() const;
	void loadElement(const QDomElement& _Element, const QString& _LoadDir = "");
	QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "") const;
	static QString tagName() { return "textitem"; }	
	Qt::Alignment alignment() const;
	void setAlignment(Qt::Alignment alignment);
	virtual AbstractGraphicsItem* clone() const;
	virtual void replaceTextVariables(const TemplateDataContext& );



protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent* _Event);
private slots:
	void slotDocumentLayoutChanged();
};
}
#endif
