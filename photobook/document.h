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
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QList> 
#include <QImage>

#include "sterror.h"
#include "collectioninfo.h"
#include "stphotobookexport.h"
#include "ddocmodel.h"
#include "buildoptions.h"
#include "rendersettings.h"
#include "pagelist.h"
#include "metainfo.h"
#include "resourcelist.h"

/**
Class that stores data of a PhotoBook.

	@author
*/
class QProgressBar;
class STProgressIndicator;
class QGraphicsScene;
class QGraphicsItem;
class STErrorStack; 
class SProcessStatusWidget;

namespace SPhotoBook
{

class TemplateScene;
class GraphicsPhotoItem;
class BuildOptions;
class MetaInfo;

class ST_PHOTOBOOK_EXPORT Document : public QObject
{
	Q_OBJECT 
					
public:			
	enum EnItemType
	{
		TextItemType, 
  		RitchTextItemType,
		PhotoItemType, 
		PageItemType, 
		ClipartItemType,
		MonthItemType,
		UnknownItemType
	};

	enum EnSelectionType
	{
		NoneSelected, 
		MultiSelectionType, 
		TextSelectionType, 
		PhotoSelectionType, 
		PageSelectionType, 
		UnknownSelectionType
	};
	
private:
    PageList Pages, Layouts, Covers, LastPageLayouts, FirstPageLayouts;
	ResourceList Resources;
	CollectionInfo PBInfo;
	QString Description; 
	QString SourceImagesPath; 
	QString EncryptionKey;
	bool HasChanges; 
	BuildOptions BOptions;
	MetaInfo MetInfo;

	TemplateScene* createPage();
	void configurePage(TemplateScene* _Page);
	TemplateScene* createPage(TemplateScene* _Template, QList<GraphicsPhotoItem*>& _PhotoItems);
	TemplateScene* createPage(TemplateScene* _Template);
	void clearAllSceneChanges();
	bool anySceneHasChanges() const;
	void setHasChanges(bool _Value);
	void setTemplateDataContext(const BuildOptions& _BuildOptions, TemplateScene* _Template);

public:
	ST_DECLARE_ERRORCLASS();

public:
	Document(QObject* _Parent = 0);
	Document(const Document& _OTher);
	~Document();
	TemplateScene* randomTemplate(const PageList& _Templates);

	//-- Info and pages
	void setName(const QString& _Name) { PBInfo.setPhotoBookName(_Name); }
	QString name() const { return PBInfo.photoBookName(); }
	void setDescription(const QString& _Description) { MetInfo.setDescription(_Description); }
	void createRootPath();
	CollectionInfo info() const { return PBInfo; }
	void setMetaInfo(const MetaInfo& _MetaInfo) { MetInfo = _MetaInfo; }
	MetaInfo metaInfo() const { return MetInfo; }
	void setBuildOptions(const BuildOptions& _Options);
	BuildOptions buildOptions() const { return BOptions; }
	PageList pages() const { return Pages; }
	void setPages(const PageList& _Pages) { Pages = _Pages; }
	PageList layouts() const { return Layouts; }
	void setLayouts(const PageList& _Layouts) { Layouts = _Layouts; }
	PageList covers() const { return Covers; }
	void setCovers(const PageList& _Covers) { Covers = _Covers; }
    PageList lastPageLayouts() const { return LastPageLayouts; }
    void setLastPageLayouts(const PageList& _Layouts) { LastPageLayouts = _Layouts; }
    PageList firstPageLayouts() const { return FirstPageLayouts; }
    void setFirstPageLayouts(const PageList& _Layouts) { FirstPageLayouts = _Layouts; }
    ResourceList resources() const { return Resources; }
	void addResource(const Resource& _Resource);
	void removeResource(const Resource& _Resource);

	//-- Generation and manipulation
	void clear(); 
	bool isEmpty() const;
	void insertPage(TemplateScene* _Page, int _Index);
	void insertRandomPage(int _Index);
	void updatePage(int _Index);//!Updates the page(_Index) images.
	void removePage(int _Index); 
	void buildCalendar(STDom::DDocModel* _PhotoModel, const QDate& _FromDate, const QDate& _ToDate, QProgressBar* _Progress);
	void autoBuild(QProgressBar* _Progress);
    PageList suitablePageList(int _NPage, int _PagesToFill) const;
    int calcPagesToFill(STDom::DDocModel* _PhotoModel) const;
    void autoBuild(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress);
	void autoFill(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress);
	void clearImages();
	void movePage(int _Source, int _Destination);
	void resizePage(int _Page, const QSizeF& _NewSize);
	void resizeAll(const QSizeF& _NewSize);

	//-- Rendering
	QSize renderPageSize() const;
	QImage renderPage(int _Page, QProgressBar* _LoadImagesPrgBar = 0);
	void renderPage(TemplateScene* _PageScene , QProgressBar* _LoadImagesPrgBar, QPainter* _Painter);
	void renderPageToPdf(int _Page, QProgressBar* _LoadImagesPrgBar, const QString& _PdfFileName);
	//! return images prepared for print according to current template printPageWidth.
	QList<QImage> prepareForPrint(const QImage& _AlbumPageImage);

	//-- Input and output
	//!Saves into collection. Using DocumentCollectionInfo data.
	void save(STProgressIndicator* _ProgressBar = 0, bool _AutoSave = false);
	void saveAs(const QDir& _Dir, STProgressIndicator* _ProgressBar = 0, bool _AutoSave = false, bool _OnlyDesignImages = false);
	void saveAs(const QDir& _RootPath, const QString& _Name, STProgressIndicator* _ProgressBar = 0, bool _AutoSave = false, bool _OnlyDesignImages = false);
	void closePhotoBook();
	bool autoSaved(const QString& _Name) const;
	bool autoSaved(const QDir& _Dir) const;
	void load(const QString& _Name, QProgressBar* _ProgressBar = 0, bool _AutoSaved = false);
	void load(const QDir& _Dir, QProgressBar* _ProgressBar = 0, bool _AutoSaved = false);
	void loadDesign(const QDir& _DesignDir, QProgressBar* _ProgressBar = 0);
	void load(const QDir& _RootPath, const QString& _Name, QProgressBar* _ProgressBar = 0, bool _AutoSaved = false);
	QFileInfoList exportImages(const QString& _ExportDir, const RenderSettings& _RSettings, STErrorStack& _Errors, SProcessStatusWidget* _StatusWidget);

	//-- Statistics
	//! \return true is photobook currently contains image with MD5Sum _ImageMD5Sum.
	bool containsImage(const QString& _ImageMD5Sum) const;
	int numImageMatches(const QString& _ImageMD5Sum) const;
	int numPhotoFrames() const;
	static EnItemType itemType(QGraphicsItem* _Item);
	static EnSelectionType selectionType(QList<QGraphicsItem *> _SelectedItems);
	//! \returns true if all the items in _SelectedItems has the same type.
	static bool isSingleTypeSelection(QList<QGraphicsItem*> _SelectedItems);
	bool hasChanges() const { return HasChanges; }
	void cleanHasChanges() { setHasChanges(false); }
	void modified() { setHasChanges(true); }
	//! \returns true if this photobook contains any photoitem with no image assigned.
	bool hasEmptyPhotoItems() const; 
	bool suitableTemplate(int _PageIndex, TemplateScene* _Template, QString& _Reason);
	bool isExportedAsBooklet() const;
	//int numRenderedPages(bool _Booklet);
	QImage getPageThumbnail(int _Index, const QSize& _MaxSize);
	QImage getLastPageThumbnail(const QSize& _MaxSize);
	bool isPhotoBookCorrect(QString& _ErrorMessage, bool _CheckToOrder = true);
    int pagesPerSheet() const;

	//-- Encryption
	void setEncryptionKey(const QString& );
	//! Means no encryption
	void clearEncryptionKey();
	
private slots: 
	void slotSceneSelectionChange(); 
	void slotSceneDoubleClicked();
	void slotSceneItemContextMenu(QGraphicsItem* _Item, const QPoint&);
	void someSceneChanged();

signals:
	void sceneSelectionChanged(QGraphicsScene* );
	void sceneDoubleClicked(QGraphicsScene* );
	void sceneItemContextMenu(QGraphicsItem*, QGraphicsScene*, const QPoint&);
	void changed(); 
	void imageDropped(const QString& _FileName, const QString& _MD5Sum);
	void imageRemoved(const QString& _FileName, const QString& _MD5Sum);
	//! Emited when a new page is inserted.
	void newPageCreated();
	void templateDropped(SPhotoBook::TemplateScene* _Scene, SPhotoBook::TemplateScene* _Template);
	void sceneClicked();
};
}
#endif
