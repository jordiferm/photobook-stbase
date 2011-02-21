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

#ifndef STTEMPLATESCENE_H
#define STTEMPLATESCENE_H

#include <QGraphicsScene>
#include <QFont>
#include <QList> 
#include <QImage> 
#include "stphotolayout.h"
#include "sterror.h"
#include "stphototemplatesexport.h"

/**
	QGraphicsScene to represent templates.
	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/
class QColor; 
class QPixmap;
class STGraphicsPhotoItem;
class QProgressBar;
class STGraphicsPageItem;
class QDomDocument;
class STPhotobookCollectionInfo; 
class STGraphicsItemOperation; 
class STGraphicsClipartItem;
class STGraphicsSuperImposeItem; 
class STAbstractGraphicsItem;
class STGraphicsMonthItem;
class ST_PHOTOTEMPLATES_EXPORT STTemplateScene : public QGraphicsScene
{
Q_OBJECT
		
public:
	ST_DECLARE_ERRORCLASS();
	
private:	
	STGraphicsPageItem* PageItem;
	STGraphicsSuperImposeItem* SImposeItem;
	STPhotoLayoutTemplate Template;
	QFont TextFrameFont;
	bool ItemsMovable;
	bool ItemsResizable; 
	static qreal PageZValue;
	static qreal SImposeZValue;
	bool ModifyAllFrames;
	bool AutoAdjustFrames;
	QSizeF RenderBaseSize;
	
	QStringList storePhotoItemImage(STGraphicsPhotoItem* _Item, const STPhotobookCollectionInfo& _CInfo);
	QString storeClipartItemFile(STGraphicsClipartItem* _CItem, const STPhotobookCollectionInfo& _CInfo);
	
public:
	STTemplateScene(QObject* _Parent = 0);
	void loadPageTemplate(const STPhotoLayoutTemplate& _Template);
	STPhotoLayoutTemplate getPageTemplate() const; 
	void replaceTemplate(const STPhotoLayoutTemplate& _Template);
	void setImageToSelectedItems(const QPixmap& _ThumbNail, const QString& _ImageFileName);
	void setDummyImages(const QList<QImage>& _ImageList);
	void setBackgroundImage(const QPixmap& _ThumbNail, const QString& _ImageFileName, bool _Encrypted);
	QPixmap getThumbnail(const QString& _ImageFileName, bool _Encrypted, bool _CreateIfNotExist);
	void setSuperImposeImage(const QString& _ImageFileName, bool _Encrypted);
	void setBgBrush(const QBrush& _Brush);
	QBrush bgBrush() const;
	void clear();
	QList<STGraphicsPhotoItem *> photoItems() const;
	void loadHiResImages(STGraphicsPhotoItem* _Item);
	void loadHiResImages(QProgressBar* _Progress = 0);
	void unloadImage(STGraphicsPhotoItem* _Item);
	void unloadImages();
	void updateImage(STGraphicsPhotoItem* _Item);
	void updateImages();
	void setEmptyFramesVisible(bool _Value);
	void setWarningsVisible(bool _Visible);
	void prepareForPrint();
	STGraphicsPageItem* pageItem() const { return PageItem; }
	void addRandomPhotoFrame();
	void addPhotoItem(STGraphicsPhotoItem* _PhotoItem);
	void setPageItem(STGraphicsPageItem* _PageItem); 
	void deletePageItem();
	QGraphicsItem* addSimpleTextFrame(const QString& _Text, const QFont& _Font);
	QGraphicsItem* addClipartItem(const QString& _FileName);
	QGraphicsItem* addTextFrame(const QString& _Html);
	STGraphicsMonthItem* createMonthTextFrameItem();
	//void performSelectedItemsOp(STGraphicsItemOperation* _Operation);
	void setItemsResizable(bool _Resizable);
	void setItemsMovable(bool _Movable);
	//! Adds the element to scene.
	QGraphicsItem* addElement(const QString& _ImageSourcePath, QDomElement& _Element);
	void loadElement(const QString& _ImageSourcePath, QDomElement& _SceneElement);
	QDomElement createElement(QDomDocument& _Doc);
	QStringList storePhotoItemImages(const STPhotobookCollectionInfo& _CInfo); 
	qreal topZValue() const;
	qreal bottomZValue() const;
	void configureItem(STAbstractGraphicsItem* _Item);
	//! Adds an items and sets topZValue + 1 to it.
	void addItemOnTop(QGraphicsItem* _Item);
	QGraphicsItem* currentItem() const;
	void selectNone();
	void selectAll();
	void setAllControlsVisible(bool _Value);
	void selectAllByType(int _Type);
	//! \return Items of type _Type that are selected.
	QList<QGraphicsItem *> selectedItemsOfType(int _Type);
	//! \return true is photobook currently contains image with MD5Sum _ImageMD5Sum.
	bool containsImage(const QString& _ImageMD5Sum); 
	bool hasEmptyPhotoItems() const; 
	void selectFirstEmptyPhotoItem();
	STPhotoLayoutTemplate currentTemplate() const { return Template; }
	QSizeF renderBaseSize() const { return RenderBaseSize; }
	bool modifyAllFrames() const { return ModifyAllFrames; }
	void setAutoAdjustFrames(bool _Value) { AutoAdjustFrames = _Value; }
	bool autoAdjustFrames() const { return AutoAdjustFrames; }

protected:
	bool event(QEvent* _Event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _Event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* _Event);
	void contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent );
	void dropEvent(QGraphicsSceneDragDropEvent* _Event );

private slots:
	void panSelectedPhotoItems(const QPointF&);
	void slotCheckModifyAll(const QString& _ImagePath);

signals:
	void doubleClicked();
	void clicked();
	void itemContextMenu(QGraphicsItem* _Item, const QPoint&);
	void imageDropped(const QString& _FileName, const QString& _MD5Sum); 
	void imageRemoved(const QString& _FileName, const QString& _MD5Sum); 
	void templateDropped(STTemplateScene* _Scene, const STPhotoLayoutTemplate& _Template);
	void clipartDropped(const QString& _FileName, const QPointF _Pos);
};

#endif
