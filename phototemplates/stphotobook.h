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
#ifndef STPHOTOBOOK_H
#define STPHOTOBOOK_H

#include <QObject>
#include <QList> 
#include <QImage>

#include "stphotolayout.h"
#include "sterror.h"
#include "stphotobookcollectioninfo.h"
#include "stphototemplatesexport.h"
#include "ddocmodel.h"


struct ST_PHOTOTEMPLATES_EXPORT STPhotoBookRenderSettings
{

	typedef enum EnPrintPreProcessType
	{
		TypeNone = 0,
		TypeBooklet = 1
	};

	QString RenderFormat; 
	QString CoverRenderFormat; 
	bool ForPrint; //This is not to render into pdf.
	bool PrintFirstAtLast; //Print the first page at last.
	EnPrintPreProcessType PrintPreprocessType;

	STPhotoBookRenderSettings(const QString& _RenderFormat = "JPG") 
	: RenderFormat(_RenderFormat), CoverRenderFormat(_RenderFormat), ForPrint(true), PrintFirstAtLast(true)	{}
};

/**
Class that stores data of a PhotoBook.

	@author
*/
class STTemplateScene;
class OPhotoCollectionImageModel;
class STGraphicsPhotoItem;
class QProgressBar;
class STProgressIndicator;
class QGraphicsScene;
class QGraphicsItem;
class STErrorStack; 
class SProcessStatusWidget;
class ST_PHOTOTEMPLATES_EXPORT STPhotoBook : public QObject
{
	Q_OBJECT 
					
public:			
	enum EnItemType
	{
		TextItemType, 
  		RitchTextItemType,
		PhotoItemType, 
		PageItemType, 
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
	
	typedef QList<STTemplateScene*> TPagesList; 
	
private:
	STPhotoBookTemplate Template; 
	TPagesList Pages; 	
	STPhotobookCollectionInfo PBInfo; 
	QString Description; 
	QString TemplateFilePath; 
	QString SourceImagesPath; 
	QString EncryptionKey;
	bool HasChanges; 

	STTemplateScene* createPage();
	STTemplateScene* createPage(STPhotoLayoutTemplate _Template, QList<STGraphicsPhotoItem*>& _PhotoItems);
	STTemplateScene* createPage(STPhotoLayoutTemplate _Template);
	void setHasChanges(bool _Value);

public:
	ST_DECLARE_ERRORCLASS();

public:
	STPhotoBook(QObject* _Parent = 0);
	~STPhotoBook();
	STPhotoLayoutTemplate randomTemplate(const STPhotoBookTemplate::TTemplateList& _Templates);
	STPhotoLayoutTemplate candidateTemplate(int _AvaliablePhotos, const STPhotoBookTemplate::TTemplateList& _AllTemplates, bool _FirstPage
														, int _PagesToFill = 0, int _AvgMargin = 0, const STPhotoBookTemplate::TTemplateList& _UsedTemplates =  STPhotoBookTemplate::TTemplateList());
	void setTemplate(STPhotoBookTemplate& _Template) { Template = _Template; }
	STPhotoBookTemplate::TMarginRectList pageMarginRects() const { return Template.pageMarginRects(); }
	STPhotoBookTemplate::TMarginRectList coverMarginRects() const { return Template.coverMarginRects(); }

	STPhotoBookTemplate photoBookTemplate() const { return Template; }
	//! Sets only the template file path, does not load the template. To do it use setTemplate()
	void setTemplateFilePath(const QString& _FilePath) { TemplateFilePath = _FilePath; }
	QString templateFilePath() const { return TemplateFilePath; }
	void setSourceImagesPath(const QString& _Value) { SourceImagesPath = _Value; }
	QString sourceImagesPath() const { return SourceImagesPath; } 
	void clear(); 
	bool isEmpty() const;
	void insertPage(STTemplateScene* _Page, int _Index);
	void insertRandomPage(int _Index);
	//!Updates the page(_Index) images.
	void updatePage(int _Index);
	void removePage(int _Index); 
	void addMinimumPages();
	void autoBuild(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress);
	QSize renderSize(STTemplateScene* _Scene) const;
	QImage renderPage(int _Page, QProgressBar* _LoadImagesPrgBar = 0);
	void renderPage(STTemplateScene* _PageScene , QProgressBar* _LoadImagesPrgBar, QPainter* _Painter);
	void renderPageToPdf(int _Page, QProgressBar* _LoadImagesPrgBar, const QString& _PdfFileName);
	//! return images prepared for print according to current template printPageWidth.
	QList<QImage> prepareForPrint(const QImage& _AlbumPageImage); 
	void setName(const QString& _Name) { PBInfo.setPhotoBookName(_Name); }
	QString name() const { return PBInfo.photoBookName(); }
	void setDescription(const QString& _Description) { Description = _Description; } 
	QString description() const { return Description; }
	void createRootPath(); 
	STPhotobookCollectionInfo info() const { return PBInfo; }
	QDomDocument createDoc();
	//!Saves into collection. Using STPhotobookCollectionInfo data.
	void save(STProgressIndicator* _ProgressBar = 0, bool _AutoSave = false);
	void saveAs(const QDir& _Dir, STProgressIndicator* _ProgressBar = 0, bool _AutoSave = false);
	void saveAs(const QDir& _RootPath, const QString& _Name, STProgressIndicator* _ProgressBar = 0, bool _AutoSave = false);
	void closePhotoBook();
	bool autoSaved(const QString& _Name) const;
	bool autoSaved(const QDir& _Dir) const;
	void load(const QString& _Name, QProgressBar* _ProgressBar = 0, bool _AutoSaved = false);
	void load(const QDir& _Dir, QProgressBar* _ProgressBar = 0, bool _AutoSaved = false);
	void load(const QDir& _RootPath, const QString& _Name, QProgressBar* _ProgressBar = 0, bool _AutoSaved = false);
	static QString getTemplateFilePath(const QDir& _Dir);
	TPagesList pages() const { return Pages; }
	//! \return true is photobook currently contains image with MD5Sum _ImageMD5Sum.
	bool containsImage(const QString& _ImageMD5Sum) const;
	int numImageMatches(const QString& _ImageMD5Sum) const;
	static EnItemType itemType(QGraphicsItem* _Item);
	static EnSelectionType selectionType(QList<QGraphicsItem *> _SelectedItems);
	//! \returns true if all the items in _SelectedItems has the same type.
	static bool isSingleTypeSelection(QList<QGraphicsItem*> _SelectedItems);
	bool hasChanges() const { return HasChanges; }
	void cleanHasChanges() { setHasChanges(false); }
	int maxPages() const; 
	int minPages() const; 
	//! \returns true if this photobook contains any photoitem with no image assigned.
	bool hasEmptyPhotoItems() const; 
	void movePage(int _Source, int _Destination); 
	bool suitableTemplate(int _PageIndex, const STPhotoLayoutTemplate& _Template, QString& _Reason);
	bool isExportedAsBooklet(const STPhotoBookRenderSettings& _RSettings) const;
	int numRenderedPages(bool _Booklet);
	bool exportImages(const QString& _ExportDir, const STPhotoBookRenderSettings& _RSettings, STErrorStack& _Errors, SProcessStatusWidget* _StatusWidget);
	QImage getPageThumbnail(int _Index, const QSize& _MaxSize);
	QImage getLastPageThumbnail(const QSize& _MaxSize);
	bool isPhotoBookCorrect(QString& _ErrorMessage, bool _CheckToOrder = true);
	//Encryption
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
	void templateDropped(STTemplateScene* _Scene, const STPhotoLayoutTemplate& _Template);
	void clipartDropped(const QString& _FileName, const QPointF _Point);
};

#endif
