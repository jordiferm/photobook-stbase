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
#ifndef PLUSMINUSITEMDELEGATE_H
#define PLUSMINUSITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include "stwidgetsexport.h"

/**
 * An ItemDelegate which generates thumbnails for images. It also makes sure
 * all items are of the same size.


	@author Shadow
*/

class PlusMinusItemDelegatePrivate;
class STThumbnailView;

class ST_WIDGETS_EXPORT PlusMinusItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT

public:
	PlusMinusItemDelegate(STThumbnailView*);
	~PlusMinusItemDelegate();

	virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	virtual QSize sizeHint( const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/ ) const;

Q_SIGNALS:
	//void rotateDocumentLeftRequested(const QModelIndex&);
	void plusButtonClicked(const QModelIndex&); 
	void minusButtonClicked(const QModelIndex&); 

private Q_SLOTS:
	void setThumbnailSize(int);
	void slotPlusButtonClicked();
	void slotMinusButtonClicked();
	//void updateButtonFrameOpacity();

protected:
	virtual bool eventFilter(QObject*, QEvent*);


private:
	PlusMinusItemDelegatePrivate* const d;
	friend class PlusMinusItemDelegatePrivate;

};

#endif
