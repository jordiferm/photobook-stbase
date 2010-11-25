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
#ifndef CHECKITEMDELEGATE_H
#define CHECKITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include "stwidgetsexport.h"

/**
 * An ItemDelegate which generates thumbnails for images. It also makes sure
 * all items are of the same size.


	@author Shadow
*/

class CheckItemDelegatePrivate;
class STThumbnailView;

class ST_WIDGETS_EXPORT CheckItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
	bool ShowToolTips;

public:
	CheckItemDelegate(STThumbnailView*);
	~CheckItemDelegate();

	virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	virtual QSize sizeHint( const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/ ) const;
	void setShowToolTips(bool _Value) { ShowToolTips = _Value; }

Q_SIGNALS:
	//void rotateDocumentLeftRequested(const QModelIndex&);
	void acceptButtonClicked(const QModelIndex&); 
	void cancelButtonClicked(const QModelIndex&); 

private Q_SLOTS:
	void setThumbnailSize(int);
	void slotAcceptButtonClicked();
	void slotCancelButtonClicked();
	//void updateButtonFrameOpacity();

protected:
	virtual bool eventFilter(QObject*, QEvent*);


private:
	CheckItemDelegatePrivate* const d;
	friend class CheckItemDelegatePrivate;

};

#endif
