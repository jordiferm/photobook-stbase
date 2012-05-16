/****************************************************************************
**
** Copyright (C) 2006-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** SoftTopia reserves all rights not expressly granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TEMPLATESCENE_H
#define TEMPLATESCENE_H

#include <QGraphicsScene>
#include <QFont>
#include <QList> 
#include <QImage> 
#include <QUrl>
#include <QFileInfoList>
#include <QDomElement>
#include "sterror.h"
#include "stphotobookexport.h"
#include "ichangescontrol.h"
#include "collectioninfo.h"
#include "templatedatacontext.h"
#include "metainfo.h"

/**
	QGraphicsScene to represent templates.
*/

class QColor;
class QPixmap;
class QProgressBar;
class QDomDocument;

namespace SPhotoBook
{

class GraphicsPhotoItem;
class GraphicsPageItem;
class CollectionInfo;
class GraphicsItemOperation;
class GraphicsClipartItem;
class GraphicsSuperImposeItem;
class AbstractGraphicsItem;
class GraphicsMonthItem;
class GraphicsTextItem;
class Resource;

class ST_PHOTOBOOK_EXPORT TemplateScene : public QGraphicsScene, public IChangesControl
{
Q_OBJECT
		
public:
	ST_DECLARE_ERRORCLASS();
	
private:	
	GraphicsPageItem* PageItem;
	QFont TextFrameFont;
	bool ItemsMovable;
	bool ItemsResizable; 
	static qreal PageZValue;
	bool ModifyAllFrames;
	bool AutoAdjustFrames;
	bool ExpandImagesToFillFrames;
	bool IgnoreExifRotation;
	bool SnapToBounds;
	bool HasChanges;
	double FixedOutMargin;
	MetaInfo MetInfo;
	TemplateDataContext DataContext;
	QList<GraphicsPhotoItem *> PhotoItems;
	
	QStringList storePhotoItemImage(GraphicsPhotoItem* _CItem, const SPhotoBook::CollectionInfo& _CInfo, bool _OnlyDesignImages = false);
	QString storeClipartItemFile(GraphicsClipartItem* _CItem, const CollectionInfo& _CInfo);
	void init();

public:
	TemplateScene(const MetaInfo& _PageSize, QObject* _Parent = 0);
	void copy(TemplateScene* _Other);
	static QRectF translatedRectF(const QRectF& _Rect, const QSizeF& _DestSize, const QSizeF& _SourceSize);
	void resize(const QSizeF& _NewSize);
	void replaceTemplate(TemplateScene* _Other);
	void setImageToSelectedItems(const QPixmap& _ThumbNail, const QString& _ImageFileName);
	void setDummyImages(const QList<QImage>& _ImageList);
	void setBackgroundImage(const QImage& _ThumbNail, const QString& _ImageFileName, bool _Encrypted);
	bool hasBackgroundImage() const;
	QString thumbnailImage(const QString& _ImgFile);
	QImage getThumbnail(const QString& _ImageFileName, bool _Encrypted, bool _CreateIfNotExist);
	void setBgBrush(const QBrush& _Brush);
	QBrush bgBrush() const;
	void clear();
	MetaInfo metaInfo() const { return MetInfo; }

	QList<GraphicsPhotoItem *> photoItems() const;
	QRectF photoItemsBoundingRect(GraphicsPhotoItem* _ExcludeMe = 0) const;

	QList<GraphicsMonthItem *> monthItems() const;

	void loadHiResImages(GraphicsPhotoItem* _Item);
	void loadHiResImages(QProgressBar* _Progress = 0);
	void unloadImage(GraphicsPhotoItem* _Item);
	void unloadImages();
	void clearImages();
	void updateImage(GraphicsPhotoItem* _Item);
	void updateImages();
	void setEmptyFramesVisible(bool _Value);
	void setWarningsVisible(bool _Visible);
	void prepareForPrint();
	GraphicsPageItem* pageItem() const { return PageItem; }
	void addRandomPhotoFrame();
	void addPhotoItem(GraphicsPhotoItem* _PhotoItem);
	void setPageItem(GraphicsPageItem* _PageItem);
	void deletePageItem();
	QGraphicsItem* addClipartItem(const QString& _FileName);
	GraphicsTextItem* addTextFrame(const QString& _Html);
	void addSubTittleTextFrame(int _FrameIndex);
	GraphicsMonthItem* createMonthTextFrameItem();
	//void performSelectedItemsOp(STGraphicsItemOperation* _Operation);
	void setItemsResizable(bool _Resizable);
	void setItemsMovable(bool _Movable);
	//! Adds the element to scene.
	QGraphicsItem* addElement(QDomElement& _Element, const QString& _LoadDir = "");
	void loadElement(const QDomElement& _SceneElement, const QString& _LoadDir = "");
	QDomElement createElement(QDomDocument& _Doc, const QString& _StoreDir = "");
	QStringList storePhotoItemImages(const CollectionInfo& _CInfo, bool _OnlyDesignImages = false);
	qreal topZValue() const;
	qreal bottomZValue() const;
	void configureItem(AbstractGraphicsItem* _Item);
	//! Adds an items and sets topZValue + 1 to it.
	void addItemOnTop(QGraphicsItem* _Item);
	QGraphicsItem* currentItem() const;
	void applyResource(const Resource& _Resource);


	//--- Selection
	void selectNone();
	void selectAll();
	void setAllControlsVisible(bool _Value);
	void selectAllByType(int _Type);

	//--- Autocreation
	void shrinkFramesBy(double _Amount);
	void setSpacing(double _Spacing);
	void splitXFrame(int _FrameIndex);
	void splitYFrame(int _FrameIndex);

	//--- Statistics
	//! \return Items of type _Type that are selected.
	QList<QGraphicsItem *> selectedItemsOfType(int _Type);
	//! \return true is photobook currently contains image with MD5Sum _ImageMD5Sum.
	bool containsImage(const QString& _ImageMD5Sum); 
	bool hasEmptyPhotoItems() const; 
	int numPhotoItems() const ;
	int numMonthItems() const;
	int numEmptyPhotoItems() const;
	int numLandscapeFrames() const;
	bool hasSameFrames(const TemplateScene* _Other) const;
	int biggestFrameIndex();

	//-- Operators
	bool operator==(const TemplateScene& _Other) const;
	bool operator<(const TemplateScene& _Other) const;
	bool operator>(const TemplateScene& _Other) const;
	bool operator<(int _Value) const;
	bool operator>(int _Value) const;

	void selectFirstEmptyPhotoItem();
	bool modifyAllFrames() const { return ModifyAllFrames; }
	void setModifyAllFrames(bool _Value) { ModifyAllFrames = _Value; }
	void setAutoAdjustFrames(bool _Value) { AutoAdjustFrames = _Value; }
	bool autoAdjustFrames() const { return AutoAdjustFrames; }
	void setExpandImagesToFillFrames(bool _Value) { ExpandImagesToFillFrames = _Value; }
	bool expandImagesToFillFrames() const { return ExpandImagesToFillFrames; }
	void setSnapToBounds(bool _Value) { SnapToBounds = _Value; }
	bool snapToBounds() const { return SnapToBounds; }
	void setIgnoreExifRotation(bool _Value) { IgnoreExifRotation = _Value; }
	bool ignoreExifRotation() const { return IgnoreExifRotation; }
	void setFixedOutMargin(double _Value) { FixedOutMargin = _Value; }
	double fixedOutMargin() const { return FixedOutMargin; }
	//IChangesControl
	void modified();
	void clearChanges();
	bool hasChanges() const { return HasChanges; }
	void autoFillImages(const QFileInfoList& _Images);

	//Variables
	void getContextVariables();
	void setDataContext(const TemplateDataContext& _DataContext);
	TemplateDataContext& dataContext();

protected:
	bool event(QEvent* _Event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _Event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* _Event);
	void contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent );
	void dropEvent(QGraphicsSceneDragDropEvent* _Event );

private slots:
	void panSelectedPhotoItems(const QPointF&);
	void slotCheckModifyAll(const QString& _ImagePath);

	void slotImageListDropped(const QList<QUrl>& _Urls);

signals:
	void doubleClicked();
	void clicked();
	void itemContextMenu(QGraphicsItem* _Item, const QPoint&);
	void imageDropped(const QString& _FileName, const QString& _MD5Sum); 
	void imageListDropped(const QList<QUrl>& _Urls);
	void imageRemoved(const QString& _FileName, const QString& _MD5Sum);
	void templateDropped(SPhotoBook::TemplateScene* _Scene, SPhotoBook::TemplateScene* _Template);
};

bool operator<(int _Value, const TemplateScene& _Other);

}

#endif
