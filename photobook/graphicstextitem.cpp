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

#include "graphicstextitem.h"
#include <QFontMetrics> 
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlock>
#include <QDomText>
#include <QDebug>
#include <QAbstractTextDocumentLayout>

#include "graphicsitemmodifier.h"
#include "templatedatacontext.h"
#include "corneritem.h"

using namespace SPhotoBook;

void GraphicsTextItem::init()
{
	QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable);

	//setTextInteractionFlags(Qt::NoTextInteraction);
	setTextInteractionFlags(Qt::TextEditorInteraction);
  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, true);
	//QGraphicsItem::setFlag(QGraphicsItem::ItemIsFocusable, false);
}

void GraphicsTextItem::createCustomCorners()
{
    CornerItem* Corner = new CornerItem(Qt::TopRightCorner, CornerItem::SetWidth, Modifier);
    Corner->setPixmapName(":/photobook/transform-move.png");

    Modifier->layoutChildren();
}


GraphicsTextItem::GraphicsTextItem(QGraphicsItem* _Parent)
 : QGraphicsTextItem(_Parent), AbstractGraphicsItem(this)
{
	init();
	AbstractGraphicsItem::updateToolTip();
	createStandardCorners();
    createCustomCorners();
	setControlsVisible(false);
	if (document())
		connect(document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(slotDocumentLayoutChanged()));

}


void GraphicsTextItem::setMovable(bool _Movable)
{
	//Does nothing.
//  	if (_Movable)
//  		setTextInteractionFlags(Qt::NoTextInteraction);
//  	else 
//  		setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);
// 	
//  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, _Movable);
//  	QGraphicsItem::setFlag(QGraphicsItem::ItemIsFocusable, !_Movable);
	
}

void GraphicsTextItem::setCharFormat(const QTextCharFormat& _Format)
{
	QTextCursor Cursor = textCursor();
	Cursor.select(QTextCursor::Document);
	Cursor.setCharFormat(_Format);
}

void GraphicsTextItem::mergeCharFormat(const QTextCharFormat& _Format)
{
	QTextCursor Cursor = textCursor();
	Cursor.select(QTextCursor::Document);
	Cursor.mergeCharFormat(_Format);
}

QTextCharFormat GraphicsTextItem::charFormat() const
{
	QTextCursor TCursor(document());
	TCursor.select(QTextCursor::Document);
	return TCursor.charFormat();
}


void GraphicsTextItem::prepareForPrint()
{
    if (textCursor().hasSelection())
    {
        QTextCursor TextCursor = textCursor();
        TextCursor.clearSelection();
        setTextCursor(TextCursor);
    }
}

void GraphicsTextItem::loadElement(const QDomElement& _Element, const QString& _LoadDir)
{
	setPos(_Element.attribute("x", "0").toDouble(), 
			_Element.attribute("y", "0").toDouble());
	setHtml(_Element.text());
	setTextWidth(_Element.attribute("textwidth", "-1").toDouble());

	//--- Char Format
	QString OutlineCol = _Element.attribute("outlinecolor", "");
	if (!OutlineCol.isEmpty())
	{
		QTextCharFormat Format;
		Format.setTextOutline(QColor(OutlineCol));
		QTextCursor Cursor = textCursor();
		Cursor.select(QTextCursor::Document);
		Cursor.mergeCharFormat(Format);
	}

	setTransform(AbstractGraphicsItem::loadTransformElement(_Element));
    modifier()->updateInitTransform();
	AbstractGraphicsItem::loadEffectElements(this,  _Element);
	AbstractGraphicsItem::updateToolTip();
}

QDomElement GraphicsTextItem::createElement(QDomDocument& _Doc, const QString& _StoreDir) const
{
	QDomElement MElement = _Doc.createElement(tagName());
	MElement.setAttribute("x", pos().x()); 
	MElement.setAttribute("y", pos().y()); 
	MElement.setAttribute("textwidth", textWidth());

	//--- Char Format
	if (textCursor().charFormat().textOutline() != Qt::NoPen)
		MElement.setAttribute("outlinecolor", textCursor().charFormat().textOutline().color().name());

	QDomText CText = _Doc.createTextNode(toHtml());
	MElement.appendChild(CText);
	MElement.appendChild(AbstractGraphicsItem::createTransformElement(this, _Doc));
	//Effects
	AbstractGraphicsItem::appendEffectElements(MElement, this, _Doc);

	return MElement; 
}

Qt::Alignment GraphicsTextItem::alignment() const
{
	Qt::Alignment Res;
	if (document())
		Res = document()->begin().blockFormat().alignment();

	return Res;
}

void GraphicsTextItem::setAlignment(Qt::Alignment alignment)
{
	QTextBlockFormat format;
	format.setAlignment(alignment);
	QTextCursor cursor = textCursor();
	cursor.select(QTextCursor::Document);
	cursor.mergeBlockFormat(format);
	cursor.clearSelection();
	setTextCursor(cursor);
}

AbstractGraphicsItem* GraphicsTextItem::clone() const
{
	GraphicsTextItem* Res = new GraphicsTextItem;

	QDomDocument Doc;
	Res->loadElement(createElement(Doc));
	return Res;
}

void GraphicsTextItem::replaceTextVariables(const TemplateDataContext& _DataContext)
{
	QString Text = toHtml();


	QStringList Vars = _DataContext.vars();

	QStringList::iterator it;
	for (it = Vars.begin(); it != Vars.end(); ++it)
	{
		Text.replace(TemplateDataContext::varPattern(*it), _DataContext.value(*it));
	}
	setHtml(Text);


	/*QRegExp ReVars("%(.*)%");

	int Pos = 0;
	while ((Pos = ReVars.indexIn(Text, Pos)) != -1)
	{
		 VarName << ReVars.cap(1);
		 Text

		 Pos += ReVars.matchedLength();


	}*/
}


QVariant GraphicsTextItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionHasChanged)
		AbstractGraphicsItem::updateToolTip();
	if (change == QGraphicsItem::ItemSelectedHasChanged)
		setControlsVisible(isSelected());
	else
	if (change == ItemPositionChange)
	{
		QPointF newPos = value.toPointF();
		if (SnapToGrid)
		{
			newPos.setX(snapToGridValue(newPos.x()));
			newPos.setY(snapToGridValue(newPos.y()));
			return newPos; 
		}
		modified();
	}
	return QGraphicsItem::itemChange(change, value);
}
 
void GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* _Event)
{
	_Event->accept();
	if (!_Event->modifiers() && Qt::ControlModifier)
		scene()->clearSelection();

	setSelected(true);
	setControlsVisible(isSelected());
	//if (TouchInterface && MultiSelection)
	//	_Event->setModifiers(_Event->modifiers() | Qt::ControlModifier);
	QGraphicsTextItem::mousePressEvent(_Event);
}

void GraphicsTextItem::slotDocumentLayoutChanged()
{
	if (textWidth() == -1)
	{
		if (document() && scene()) //Defensive
		{
			if (pos().x() + document()->size().width() > scene()->width())
				setTextWidth(scene()->width() - pos().x());
		}
	}
	Modifier->layoutChildren();
}
