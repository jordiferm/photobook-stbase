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
#ifndef TPPHOTODELEGATE_H
#define TPPHOTODELEGATE_H

#include <QAbstractItemDelegate>
#include <QToolButton>
#include "sttpopslexport.h"



/**
 * An ItemDelegate which generates thumbnails for images. It also makes sure
 * all items are of the same size.


	@author Shadow
*/

class TPPhotoDelegatePrivate;
class TPPhotoListView;

class ST_TPOPSL_EXPORT TPPhotoDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
	bool SelectItemsWithPhotos;

public:
	TPPhotoDelegate(TPPhotoListView*);
	~TPPhotoDelegate();

	enum ThumbnailDetail {
		FileNameDetail = 1,
		DateDetail     = 2,
		RatingDetail   = 4
	};
	// FIXME: Find out why this cause problems with Qt::Alignment in
	// TPPhotoDelegate!
	//Q_DECLARE_FLAGS(ThumbnailDetails, ThumbnailDetail);
	typedef int ThumbnailDetails;

	/**
	 * Returns which thumbnail details are shown
	 */
	ThumbnailDetails thumbnailDetails() const;

	void setThumbnailDetails(ThumbnailDetails);

	virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	virtual QSize sizeHint( const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/ ) const;
	void setCopyActionVisible(bool _Visible);
	void setEditActionsVisible(bool _Visible);
	void setSelectItemsWithPhotos(bool _Value) { SelectItemsWithPhotos = _Value; }


Q_SIGNALS:
	//void saveDocumentRequested(const KUrl&);
	void editAddRequested(const QModelIndex&); 
	void editDelRequested(const QModelIndex&); 
	void editCloneRequested(const QModelIndex&);

private Q_SLOTS:
	void setThumbnailSize(int);
	//void updateButtonFrameOpacity();

	void slotSaveClicked();

	void slotEditDelClicked();
	void slotEditAddClicked();
	void slotEditCloneClicked();

protected:
	virtual bool eventFilter(QObject*, QEvent*);


private:
	TPPhotoDelegatePrivate* const d;
	friend class TPPhotoDelegatePrivate;

};

#endif
