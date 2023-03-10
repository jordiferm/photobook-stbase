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
#include "document.h"
#include <QProgressBar>
#include <QApplication>
#include <QPainter>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <QPrinter> //Pdf Rendering.
#include <QDebug>

#include "stutils.h"
#include "stimage.h"
#include "templatescene.h"

#include "graphicsphotoitem.h"
#include "graphicspageitem.h"
#include "graphicstextitem.h"
#include "graphicsclipartitem.h"
#include "graphicsmonthitem.h"
#include "sterrorstack.h"
#include "sprocessstatuswidget.h" 
#include "stprogressindicator.h"
#include "candidatecalculator.h"
#include "ddoc.h"

#define ITEM_AVERAGE_MARGIN 2

using namespace SPhotoBook;

TemplateScene* Document::createPage()
{
	TemplateScene* Scene = new TemplateScene(MetInfo, this);
	configurePage(Scene);
	return Scene; 
}

void Document::configurePage(TemplateScene* _Page)
{
	//_Page->setAutoAdjustFrames(BOptions.autoadjustFrames());
	//_Page->setIgnoreExifRotation(BOptions.ignoreExifRotation());
	//_Page->setExpandImagesToFillFrames(BOptions.expandImagesToFillFrames());
	/*_Page->setAutoAdjustFrames(MetInfo.autoAdjustFrames());
	_Page->setIgnoreExifRotation(!MetInfo.autoAdjustFrames());
	_Page->setExpandImagesToFillFrames(MetInfo.expandImagesToFillFrames());
	_Page->setFixedOutMargin(MetInfo.fixedDotMargin());
	_Page->setModifyAllFrames(MetInfo.multiPhoto());*/
	connect(_Page, SIGNAL(selectionChanged()), this, SLOT(slotSceneSelectionChange()));
	connect(_Page, SIGNAL(doubleClicked()), this, SLOT(slotSceneDoubleClicked()));
	connect(_Page, SIGNAL(itemContextMenu(QGraphicsItem*, const QPoint&)), this, SLOT(slotSceneItemContextMenu(QGraphicsItem*, const QPoint&)));
	connect(_Page, SIGNAL(imageDropped(const QString&, const QString&)), this, SIGNAL(imageDropped(const QString&, const QString&)));
	connect(_Page, SIGNAL(imageRemoved(const QString&, const QString&)), this, SIGNAL(imageRemoved(const QString&, const QString&)));
	connect(_Page, SIGNAL(templateDropped(SPhotoBook::TemplateScene*,SPhotoBook::TemplateScene*)), this, SIGNAL(templateDropped(SPhotoBook::TemplateScene*,SPhotoBook::TemplateScene*)));
	connect(_Page, SIGNAL(clicked()), this, SIGNAL(sceneClicked()));

	connect(_Page, SIGNAL(changed( const QList< QRectF >& )), this, SLOT(someSceneChanged()));
}

TemplateScene* Document::createPage(TemplateScene* _Template, QList<GraphicsPhotoItem*>& _PhotoItems)
{
	TemplateScene* Scene = createPage();
	
	Scene->copy(_Template);
	Scene->getContextVariables();
	
	_PhotoItems = Scene->photoItems();
	return Scene;
}

/*!
	overload function created for convenience.
*/
TemplateScene* Document::createPage(TemplateScene* _Template)
{
	QList<GraphicsPhotoItem*> DummyList;
	return createPage(_Template, DummyList);
}

void Document::clearAllSceneChanges()
{
	PageList::iterator it;
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		(*it)->clearChanges();
	}
}

bool Document::anySceneHasChanges() const
{
	bool Res = false;
	PageList::const_iterator it = Pages.begin();

	while (!Res && it != Pages.end())
	{
		Res = (*it)->hasChanges();
		++it;
	}

	return Res;
}

void Document::setHasChanges(bool _Value)
{
	if (!_Value)
		clearAllSceneChanges();
	if (_Value != HasChanges)
	{
		HasChanges = _Value; 
		emit changed(); 
	}
}

void Document::setTemplateDataContext(const SPhotoBook::BuildOptions& _BuildOptions, TemplateScene* _Template)
{
	_Template->dataContext().setTitle(_BuildOptions.title());
	_Template->dataContext().setSubTitle(_BuildOptions.subTitle());
	_Template->dataContext().setAuthor(_BuildOptions.author());
}

Document::Document(QObject* _Parent) : QObject(_Parent),  HasChanges(false)
{
}

Document::Document(const Document& _Other)
{
	Pages = _Other.Pages;
	PBInfo = _Other.PBInfo;
	Description = _Other.Description;
	EncryptionKey = _Other.EncryptionKey;
	HasChanges = _Other.HasChanges;
	BOptions = _Other.BOptions;
}


Document::~Document()
{
	closePhotoBook();
}

TemplateScene* Document::randomTemplate(const PageList& _Templates)
{
	TemplateScene* Res = 0;
	
	if (_Templates.size() > 0)
	{
		Res = _Templates[0];
		int NTemplate = qrand() % _Templates.size();
		Res = _Templates[NTemplate];
	}		
	return Res;
}

TemplateScene* Document::randomTemplate(int _Index)
{
	return randomTemplate(suitablePageList(_Index, Pages.count() + 1));
}


void Document::createRootPath()
{
	QDir AlbumRootDir(PBInfo.photoBookPath());
	Assert(AlbumRootDir.mkpath(PBInfo.photoBookPath()), Error(QString(tr("Error creating path %1")).arg(PBInfo.photoBookPath())));
}

void Document::setBuildOptions(const BuildOptions& _Options)
{
	BOptions = _Options;
}

void Document::addResource(const Resource& _Resource)
{
	Resources.push_back(_Resource);
}

void Document::removeResource(const Resource& _Resource)
{
	int Index = Resources.indexOf(_Resource);
	if (Index >= 0)
		Resources.takeAt(Index);
}


/*! After clear() call isEmpty() returns true
*/
void Document::clear()
{
	PageList::iterator it;
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		(*it)->unloadImages();
		delete (*it);
	}
	Pages.clear();
	setHasChanges(false); 
}

bool Document::isEmpty() const
{
	return Pages.size() == 0; 
}

void Document::insertPage(TemplateScene* _Page, int _Index)
{

	Pages.insert(_Index, _Page);
	modified();
}

bool Document::insertRandomPage(int _Index, QString& _CantInsertReason)
{
	bool Res = false;
	TemplateScene* RandomScene = randomTemplate(suitablePageList(_Index, Pages.count() + 1));
	if (canInsertTemplate(_Index, RandomScene, _CantInsertReason))
	{
		insertPage(createPage(RandomScene), _Index);
		Res = true;
	}
	return Res;
}

void Document::updatePage(int _Index)
{
	if (_Index >= 0 && _Index < Pages.count()) //Defensive 
		Pages[_Index]->updateImages(); 
}

void Document::removePage(int _Index)
{
	Assert(_Index >= 0 && _Index < Pages.count(), Error(tr("Document::removePage Range Checking Error.")));
	Pages[_Index]->clear(); 
	Pages.removeAt(_Index);
	modified();
}

void Document::buildCalendar(STDom::DDocModel* _PhotoModel, const QDate& _FromDate, const QDate& _ToDate, QProgressBar* _Progress)
{
	clear();
	CandidateCalculator CCalculator(*this, _PhotoModel);
	int NumMonths = STUtils::monthsTo(_FromDate, _ToDate) + 1;
	if (MetInfo.preferMinPages()) //Ex: Anual calendars.
		NumMonths = qMin(MetInfo.minPages(), NumMonths);

	if (_Progress)
		_Progress->setRange(0, NumMonths);

	//If there is firstpage, put the first page.
	if (!Covers.isEmpty())
	{
		TemplateScene* FirstPageTemplate = randomTemplate(Covers);
		FirstPageTemplate->dataContext().setDate(_FromDate);
		setTemplateDataContext(BOptions, FirstPageTemplate);
		TemplateScene* NewPage = createPage(FirstPageTemplate);
		Pages.push_back(NewPage);
		CCalculator.fillPage(NewPage, FirstPageTemplate, _Progress);
		emit newPageCreated();
	}
	//For each month
	QDate CDate = _FromDate;
	for (int Vfor = 0; Vfor < NumMonths; Vfor++)
	{
		TemplateScene* CurrTemplate = CCalculator.newDateCandidate(Layouts, CDate);
		Assert(CurrTemplate, Error(tr("Error creating calendar: No candidates found")));
		CurrTemplate->dataContext().setDate(CDate);
		setTemplateDataContext(BOptions, CurrTemplate);
		CDate = CDate.addMonths(1);
		TemplateScene* NewPage = createPage(CurrTemplate);
		Pages.push_back(NewPage);
		CCalculator.fillPage(NewPage, CurrTemplate->modifyAllFrames(), _Progress);
		delete CurrTemplate;
		emit newPageCreated();
	}
}

PageList Document::suitablePageList(int _NPage, int _PagesToFill) const
{
	PageList PList = Layouts;
    if (_NPage == 0 && Covers.size() > 0) //Cover
        PList = Covers;
    else
    {
        if (_NPage == _PagesToFill - 1 && LastPageLayouts.count() > 0)
            PList = LastPageLayouts;
        else
			if ((_NPage == 1 && LastPageLayouts.size() > 0 && Covers.size() > 0) ||
				(_NPage == 0 && LastPageLayouts.size() > 0 && Covers.size() == 0)) //Page 1
                PList = FirstPageLayouts;
            else
                PList = Layouts;
    }
    return PList;
}

int Document::calcPagesToFill(STDom::DDocModel* _PhotoModel) const
{
    int PagToFill = qMin(BOptions.pagesToFill(), MetInfo.maxPages());

    if (BOptions.pagesFromImages())
    {
        PagToFill = _PhotoModel->rowCount() / qMax(1, MetInfo.numOptimalImagesPerPage());
    }

    if (MetInfo.preferMinPages())
        PagToFill = MetInfo.minPages();

    if (PagToFill == 0)
        PagToFill = MetInfo.minPages();

	return qMin(PagToFill, MetInfo.maxPages());
}

//! Overload procedure provided for convenience
void Document::autoBuild(QProgressBar* _Progress)
{
    STDom::DDocModel Model;
    autoBuild(&Model, _Progress);
}

void Document::autoBuild(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress)
{
	clear();
    bool ThereIsTemplates = Covers.size() > 0 || Layouts.size() > 0 || LastPageLayouts.size() > 0 || FirstPageLayouts.size() > 0;
	CandidateCalculator CCalculator(*this, _PhotoModel);

	if (_Progress)
		_Progress->setRange(0, CCalculator.totalPhotos());

    int NPages = 0;
    int PagToFill = calcPagesToFill(_PhotoModel);

	while (CCalculator.photosAvailable() && ThereIsTemplates && NPages < PagToFill )
	{
		//Agafem un template qualsevol
		TemplateScene* CurrTemplate;
        PageList PList = suitablePageList(NPages, PagToFill);
		CurrTemplate = CCalculator.getCandidate(PList, PagToFill - NPages,
                                                CCalculator.calcMargin(ITEM_AVERAGE_MARGIN, PagToFill - NPages));
		if (!CurrTemplate && !PList.isEmpty())
		{
			CurrTemplate = CCalculator.randomUnused(PList);
			if (!CurrTemplate)
				CurrTemplate = CCalculator.randomUsed(PList);
		}
		if (CurrTemplate) //Defensive
		{
			setTemplateDataContext(BOptions, CurrTemplate);
			TemplateScene* NewPage = createPage(CurrTemplate);
			Pages.push_back(NewPage);
			CCalculator.fillPage(NewPage, MetInfo.multiPhoto(), _Progress);
			CCalculator.markAsUsed(CurrTemplate);
			emit newPageCreated();
		}
		NPages++;
	}
    if (_Progress)
		_Progress->setValue(CCalculator.totalPhotos());

	//There's no images and still pages to fill.
	while (NPages < PagToFill && ThereIsTemplates)
	{
        //Agafem un template qualsevol
        TemplateScene* CurrTemplate = 0;
        PageList PList = suitablePageList(NPages, PagToFill);

		CurrTemplate = CCalculator.getEmptyCandidate(PList, MetInfo.numOptimalImagesPerPage(), ITEM_AVERAGE_MARGIN);
		setTemplateDataContext(BOptions, CurrTemplate);
		if (CurrTemplate) //Defensive
		{
			insertPage(createPage(CurrTemplate), NPages);
			CCalculator.markAsUsed(CurrTemplate);
		}
		NPages++;
	}
}

void Document::autoFill(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress)
{
	CandidateCalculator CCalculator(*this, _PhotoModel);

	if (_Progress)
		_Progress->setRange(0, CCalculator.totalPhotos());

	PageList::iterator it = Pages.begin();
	while (it != Pages.end())
	{
		CCalculator.fillPage(*it, MetInfo.multiPhoto(), _Progress);
		++it;
	}
	if (_Progress)
		_Progress->setValue(CCalculator.totalPhotos());
}

void Document::clearImages()
{
	PageList::iterator it;
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		(*it)->clearImages();
	}
}

void Document::movePage(int _Source, int _Destination)
{
	if (_Source >= 0 && _Source < Pages.size() && _Destination >= 0 && _Destination < Pages.size())
	{
		Pages.move(_Source, _Destination);
		modified();
	}
}

void Document::resizePage(int _Page, const QSizeF& _NewSize)
{
	Assert(_Page >= 0 && _Page < Pages.size(), Error("Document::renderPage Range Checking Error"));
	TemplateScene* PageScene = Pages[_Page];
	PageScene->resize(_NewSize);
}

void Document::resizeAll(const QSizeF& _NewSize)
{
	Pages.resize(_NewSize);
	Layouts.resize(_NewSize);
	Covers.resize(_NewSize);
    LastPageLayouts.resize(_NewSize);
    FirstPageLayouts.resize(_NewSize);
}

QSize Document::renderPageSize() const
{
	QSize Res = STDom::DDocFormat::pixelSize(MetInfo.printPageSize(), MetInfo.dpis());
	if (MetInfo.printPreprocessType() == RenderSettings::TypeHBooklet)
		Res.setWidth(Res.width() / 2);
	else
	if (MetInfo.printPreprocessType() == RenderSettings::TypeVBooklet)
		Res.setHeight(Res.height() / 2);
	return Res;
	/*QSize Res(0,0);
	QSize PageSize = STDom::DDocFormat::pixelSize(_Scene->sceneRect().size(), MetInfo.dpis());
	if (!_Scene->renderBaseSize().isEmpty())
	{
		QSize DestSize = STDom::DDocFormat::pixelSize(_Scene->renderBaseSize(), MetInfo.dpis());
		if (DestSize.isValid() && DestSize.width() >=  PageSize.width() && DestSize.height() >= PageSize.height() )
			Res = DestSize;
		else
			Res = PageSize;
	}
	else
		Res = PageSize;
	return Res;*/
}

//! _Page goes from 0 to numPages - 1.
QImage Document::renderPage(int _Page, QProgressBar* _LoadImagesPrgBar)
{
	Assert(_Page >= 0 && _Page < Pages.size(), Error("Document::renderPage Range Checking Error"));
	TemplateScene* PageScene = Pages[_Page];

	QSize PageSize = renderPageSize();
	QSize TemplateSize = STDom::DDocFormat::pixelSize(PageScene->sceneRect().size(), MetInfo.dpis());
	QSize RenderSize;
	if (PageSize.width() >= TemplateSize.width() && PageSize.height() >= TemplateSize.height())
		RenderSize = PageSize;
	else
		RenderSize = TemplateSize;

	QImage ResImg(RenderSize, QImage::Format_RGB32);
	ResImg.setDotsPerMeterX(MetInfo.dpM());
	ResImg.setDotsPerMeterY(MetInfo.dpM());
	QPainter Painter(&ResImg);
	Painter.fillRect(ResImg.rect(), Qt::white);
	renderPage(PageScene, _LoadImagesPrgBar, &Painter); 
	return ResImg;
}

void Document::renderPage(TemplateScene* _PageScene , QProgressBar* _LoadImagesPrgBar, QPainter* _Painter)
{
	Assert(_PageScene, Error("Document::renderPage NULL TemplateScene assigned."));
	_PageScene->setEmptyFramesVisible(false);
	_PageScene->prepareForPrint();
	_PageScene->loadHiResImages(_LoadImagesPrgBar); 
	QSize TemplateSize = STDom::DDocFormat::pixelSize(_PageScene->sceneRect().size(), MetInfo.dpis());
	QSize PageSize = renderPageSize();
	if (PageSize.width() >= TemplateSize.width() && PageSize.height() >= TemplateSize.height())
	{
		if (MetInfo.printPreprocessType() == RenderSettings::TypeMultiply)
		{
			STImage TmpImg(TemplateSize, QImage::Format_RGB32);
			QPainter ImgPainter(&TmpImg);
			_PageScene->render(&ImgPainter, QRect(QPoint(0,0), TemplateSize));
			TmpImg.multiplyImage(PageSize, _Painter);
		}
		else //Center to page
			_PageScene->render(_Painter, QRect(QPoint((PageSize.width() - TemplateSize.width()) / 2,
													  (PageSize.height() - TemplateSize.height()) / 2), TemplateSize));
	}
	else
		_PageScene->render(_Painter, QRect(QPoint(0,0), TemplateSize));

	_PageScene->unloadImages();
	_PageScene->setEmptyFramesVisible(true);
}

void Document::renderPageToPdf(int _Page, QProgressBar* _LoadImagesPrgBar, const QString& _PdfFileName)
{
	Assert(_Page >= 0 && _Page < Pages.size(), Error("Document::renderPage Range Checking Error"));
	TemplateScene* PageScene = Pages[_Page];
	QPrinter Printer;
	Printer.setOutputFormat(QPrinter::PdfFormat);
	#if QT_VERSION >= 0x040400
	Printer.setPaperSize(PageScene->sceneRect().size(), QPrinter::Millimeter);
	Printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter); 
	#endif
	Printer.setOutputFileName(_PdfFileName);
	Printer.setResolution(MetInfo.dpis());
	QPainter Painter;
	Assert(Painter.begin(&Printer), Error(tr("failed to open PDF file: %1, is it writable?").arg(_PdfFileName)));
	renderPage(PageScene, _LoadImagesPrgBar, &Painter); 
	Painter.end(); 
}

QList<QImage> Document::prepareForPrint(const QImage& _AlbumPageImage)
{
	QList<QImage> Res;
	QSizeF PageSize = renderPageSize();
	if (_AlbumPageImage.size().width() > PageSize.width() || _AlbumPageImage.size().height() > PageSize.height())
	{
		int NPages = (_AlbumPageImage.size().width() * _AlbumPageImage.size().height()) /
					(PageSize.width() * PageSize.height());
		int NumRows = _AlbumPageImage.size().height() / PageSize.height();
		for (int Vfor = 0 ;Vfor < NPages; Vfor++)
		{
			int CurrRow = Vfor % NumRows;
			int CurrCol = Vfor / NumRows;
                        int NumCols = NPages / NumRows;
                        int OverlayMarginPixels = STDom::DDocFormat::mmToPixels(MetInfo.overlayMargin(), MetInfo.dpis());
                        int CopyRectX = qMax((int)(CurrCol * PageSize.width()) - OverlayMarginPixels, 0);
                        int CopyRectY = qMax((int)(CurrRow * PageSize.height()) - OverlayMarginPixels, 0);
                        int CopyRectWidth = PageSize.width();
                        if (NumCols > 1)
                            CopyRectWidth += OverlayMarginPixels;
                        int CopyRectHeight = PageSize.height();
                        if (NumRows > 1)
                            CopyRectHeight += OverlayMarginPixels;

                        Res.push_back(_AlbumPageImage.copy(CopyRectX, CopyRectY,CopyRectWidth, CopyRectHeight));
		}
	}
	else
		Res.push_back(_AlbumPageImage);

	return Res;
/*	QList<QImage> Res;
	if (MetInfo.cutPagesOnPrint())
	{
		int PPageWidthWidth = MetInfo.printPageSize().width();
		if (PPageWidthWidth == 0)
			PPageWidthWidth = _SceneSize.width();
		QSize PrintPageSize = STDom::DDocFormat::pixelSize(QSize(PPageWidthWidth, _AlbumPageImage.height()), qMax(MetInfo.dpis(), 10));
		int NPages = _AlbumPageImage.size().width() / PrintPageSize.width(); 
		QSize CImgSize; 
		CImgSize.setHeight(_AlbumPageImage.height()); 
		for (int Vfor = 0 ;Vfor < NPages; Vfor++)
		{
			if (Vfor == NPages -1)
				CImgSize.setWidth(_AlbumPageImage.size().width() - (PrintPageSize.width() * (NPages - 1)));
			else 
				CImgSize.setWidth(PrintPageSize.width()); 
	
			Res.push_back(_AlbumPageImage.copy(Vfor * PrintPageSize.width(), 0, CImgSize.width(), CImgSize.height())); 
		}
	}
	else 
		Res.push_back(_AlbumPageImage); 
	return Res;*/
}

void Document::save(STProgressIndicator* _Progress , bool _AutoSave)
{
	saveAs(QDir(PBInfo.defaultRootPathName()), PBInfo.photoBookName(), _Progress, _AutoSave);
}

void Document::saveAs(const QDir& _Dir, STProgressIndicator* _Progress, bool _AutoSave, bool _OnlyDesignImages )
{
	CollectionInfo MInfo(_Dir);
	saveAs(QDir(MInfo.rootPathName()), MInfo.photoBookName(), _Progress, _AutoSave, _OnlyDesignImages);
}

void Document::saveAs(const QDir& _RootPath, const QString& _Name, STProgressIndicator* _Progress, bool _AutoSave, bool _OnlyDesignImages)
{
	//CollectionInfo MInfo(_Name, _RootPath.absolutePath());
	PBInfo.setPhotoBookName(_Name); 
	PBInfo.setRootPathName(_RootPath.absolutePath()); 
	QDir PBDir(PBInfo.photoBookPath()); 
	Assert(PBDir.mkpath(PBDir.absolutePath()), Error(QString(tr("Error creating Photo Book Path %1")).arg(PBDir.absolutePath()))); 

	if (_AutoSave)
		Pages.saveXml(PBInfo.xmlAutoSaveFileName());
	else
	{
		QStringList StoredFiles;
		if (!_OnlyDesignImages)
		{
			StoredFiles = Pages.saveResources(PBInfo, false, _Progress);
		}
		StoredFiles += Layouts.saveResources(PBInfo, true, _Progress);
		StoredFiles += Covers.saveResources(PBInfo, true, _Progress);
        StoredFiles += LastPageLayouts.saveResources(PBInfo, true, _Progress);
        StoredFiles += FirstPageLayouts.saveResources(PBInfo, true, _Progress);

		// Delete unused images and unused mask images.
		QDir PBooKDir(PBInfo.photoBookPath());
		QStringList PBookFiles = PBooKDir.entryList(QDir::Files);
		QStringList::iterator sit;
		for (sit = PBookFiles.begin(); sit != PBookFiles.end(); ++sit)
		{
			QString CurrAbsPathFile = PBInfo.photoBookPath() + "/" + (*sit);
			if (!StoredFiles.contains(CurrAbsPathFile) &&
				(CurrAbsPathFile != PBInfo.trayImagesFileName()) )
			{
				QFile DelFile(CurrAbsPathFile);
				//TODO Put Log here !!!
				if (!DelFile.remove())
					qWarning(QString("Error removing file %1").arg(CurrAbsPathFile).toLatin1());
			}
		}

		//Save Thumbnail
		if (Pages.size() > 0)
		{
			QImage ResImg = getPageThumbnail(0, QSize(150, 100));
			Assert(ResImg.save(PBInfo.thumbnailFileName()), Error(QString(tr("Error saving photo book thumbnail at: %1")).arg(PBInfo.thumbnailFileName())));
		}

		Pages.saveXml(PBInfo.xmlFileName());
		MetInfo.save(PBInfo.xmlMetaInfoFileName());//It saves Thumbnail image.

		Layouts.saveXml(PBInfo.xmlLayoutsFileName());
		Covers.saveXml(PBInfo.xmlCoversFileName());
        LastPageLayouts.saveXml(PBInfo.xmlLastPageLayoutFileName());
        FirstPageLayouts.saveXml(PBInfo.xmlFirstPageLayoutFileName());

		//Resources manually added and loaded from design
		//Resources.save(PBDir); //We only save used resources.

	}
}

void Document::closePhotoBook()
{
	//Correctly closed => remove autosave
	if (!name().isEmpty())
	{
		CollectionInfo MInfo(name());
		QFile AutoSaveFile(MInfo.xmlAutoSaveFileName());
		if (AutoSaveFile.exists())
			AutoSaveFile.remove();
	}
}

bool Document::autoSaved(const QString& _Name) const
{
	CollectionInfo MInfo(_Name);
	return QFile::exists(MInfo.xmlAutoSaveFileName());
}

bool Document::autoSaved(const QDir& _Dir) const
{
	CollectionInfo MInfo(_Dir);
	return QFile::exists(MInfo.xmlAutoSaveFileName());
}

void Document::load(const QString& _Name, QProgressBar* _ProgressBar, bool _AutoSaved)
{
	CollectionInfo MInfo(_Name);
	load(QDir(MInfo.rootPathName()), MInfo.photoBookName(), _ProgressBar, _AutoSaved);
}

void Document::load(const QDir& _Dir, QProgressBar* _ProgressBar, bool _AutoSaved)
{
	CollectionInfo MInfo(_Dir);
	load(QDir(MInfo.rootPathName()), MInfo.photoBookName(), _ProgressBar, _AutoSaved);
}

void Document::loadDesign(const QDir& _DesignDir, QProgressBar* _ProgressBar)
{
	CollectionInfo MInfo(_DesignDir);
	Layouts.loadXml(MInfo.xmlLayoutsFileName(), this, MetInfo, EncryptionKey, _ProgressBar);
	Covers.loadXml(MInfo.xmlCoversFileName(), this, MetInfo, EncryptionKey, _ProgressBar);
	if (QFile::exists(MInfo.xmlLastPageLayoutFileName()))
		LastPageLayouts.loadXml(MInfo.xmlLastPageLayoutFileName(), this, MetInfo, EncryptionKey, _ProgressBar);
	if (QFile::exists(MInfo.xmlFirstPageLayoutFileName()))
		FirstPageLayouts.loadXml(MInfo.xmlFirstPageLayoutFileName(), this, MetInfo, EncryptionKey, _ProgressBar);
	Resources.load(QDir(MInfo.photoBookPath()));
}

/*! Loads the photoBook and also the template */
void Document::load(const QDir& _RootPath, const QString& _Name, QProgressBar* _ProgressBar, bool _AutoSaved)
{
	closePhotoBook();
	setName(_Name);

	CollectionInfo MInfo(_Name, _RootPath.absolutePath());
	MetInfo.load(MInfo.xmlMetaInfoFileName());

	if (_AutoSaved)
		Pages.loadXml(MInfo.xmlAutoSaveFileName(), this, MetInfo, EncryptionKey, _ProgressBar);
	else
		Pages.loadXml(MInfo.xmlFileName(), this, MetInfo, EncryptionKey, _ProgressBar);

	PageList::iterator it  = Pages.begin();
	while (it != Pages.end())
	{
		configurePage(*it);
		++it;
	}
	loadDesign(MInfo.photoBookPath());
}

QFileInfoList Document::exportImages(const QString& _ExportDir, const RenderSettings& _RSettings,
	STErrorStack& _ErrorStack, SProcessStatusWidget* _StatusWidget)
{
	QFileInfoList Res;
	//bool Booklet = (_RSettings.PrintPreprocessType == STPhotoBookRenderSettings::TypeBooklet || Template.preprocessType() == STPhotoBookTemplate::TypeBooklet);
	bool Booklet = isExportedAsBooklet();
	bool PrintFirstAtLast = (Booklet);
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if (_StatusWidget)
	{
		_StatusWidget->setWindowTitle(tr("Rendering, please wait..."));
		_StatusWidget->showProgressBar(tr("Rendering in progress..."), pages().size());
		_StatusWidget->incrementProgress();
		_StatusWidget->show();
		QApplication::processEvents();
	}
	QStringList PageFiles;
	QString Format;
    int CurrentPageNumber = 0;
	for (int Vfor = 0; Vfor < pages().size(); Vfor++)
	{
		try
		{
			if (Vfor == 0)
				Format = _RSettings.coverRenderFormat();
			else
				 Format = _RSettings.renderFormat();

			if (Format == "PDF" && _RSettings.forPrint()) //No Pdf for printing.
				Format = "JPG";

			int PageOrder = Vfor;
			if (( PrintFirstAtLast || Booklet ) && PageOrder == 0)
				PageOrder = pages().size();

			QString PageName;
			if (MetInfo.printPreprocessType() == RenderSettings::TypeNone || MetInfo.printPreprocessType() == RenderSettings::TypeMultiply)
				PageName = "page";
			else
				PageName = "tmppage";

			if (Format != "PDF")
			{
				QList<QImage> PageImages = prepareForPrint(renderPage(Vfor));
				QList<QImage>::iterator it;
				int CntPage = 0;
				for (it = PageImages.begin(); it != PageImages.end(); ++it)
				{
                    int CurrentPageOrder = CurrentPageNumber;
                    if (PrintFirstAtLast || Booklet) //TODO: Revisar-ho
                    {
                        int	HalfPageOrder = PageOrder;
                        if (CntPage > 0)
                            HalfPageOrder = Vfor;
                            CurrentPageOrder = ((HalfPageOrder) * PageImages.size()) + CntPage++;
                    }

                    QString CurrImageFileName = _ExportDir + "/" + QString(PageName + "_%1").arg(QString::number(CurrentPageOrder), 4, '0') + "." + Format;
                    StackAssert(it->save(CurrImageFileName), Error(tr("There was problems storing the following files: %1").arg(CurrImageFileName)), _ErrorStack);
					PageFiles.push_back(CurrImageFileName);
					Res.push_back(QFileInfo(CurrImageFileName));
                    CurrentPageNumber++;
				}
			}
			else
			{
				QString CurrImageFileName = _ExportDir + "/" + QString("page_%1.pdf").arg(QString::number(PageOrder), 4, '0');
				renderPageToPdf(Vfor, 0, CurrImageFileName);
				Res.push_back(QFileInfo(CurrImageFileName));
                CurrentPageNumber++;
			}

		}
		catch (STError& _Error)
		{
			QApplication::restoreOverrideCursor();
			_ErrorStack.push_back(_Error);
		}
		if (_StatusWidget)
		{
			_StatusWidget->incrementProgress();
			QApplication::processEvents();
		}
	}

	//Post Processing
	if (Booklet)
	{
		Res.clear();
		if (_StatusWidget)
		{
			_StatusWidget->setWindowTitle(tr("Post processing, please wait..."));
			_StatusWidget->showProgressBar(tr("Post processing in progress..."), PageFiles.size());
			_StatusWidget->incrementProgress();
			_StatusWidget->show();
			QApplication::processEvents();
		}

		QString Format = _RSettings.renderFormat();

		if (Format == "PDF" && _RSettings.forPrint()) //No Pdf for printing.
			Format = "JPG";

		for (int Vfor = 0; Vfor < PageFiles.size() / 2; Vfor++)
		{
			QString LPageFile, RPageFile;
			if (Vfor == 0)
			{
				LPageFile = PageFiles[Vfor];
				RPageFile = PageFiles[Vfor + 1];
			}
			else
			{
				if (Vfor % 2 == 0)
				{
					LPageFile = PageFiles[PageFiles.size() - Vfor];
					RPageFile = PageFiles[Vfor + 1];
				}
				else
				{
					LPageFile = PageFiles[Vfor + 1];
					RPageFile = PageFiles[PageFiles.size() - Vfor];
				}
			}
			QImage LPage, RPage;
			if (!LPage.load(LPageFile))
				_ErrorStack.push_back(Error(tr("Error in print preprocess. Error loading file: %1").arg(LPageFile)));
			if (!QFile::remove(LPageFile))
				_ErrorStack.push_back(Error(tr("Error in print preprocess. Error removing file: %1").arg(LPageFile)));

			if (_StatusWidget)
			{
				_StatusWidget->incrementProgress();
				QApplication::processEvents();
			}

			if (!RPage.load(RPageFile))
				_ErrorStack.push_back(QString(tr("Error in print preprocess. Error loading file: %1").arg(RPageFile)));
			if (!QFile::remove(RPageFile))
				_ErrorStack.push_back(QString(tr("Error in print preprocess. Error removing file: %1").arg(RPageFile)));
			QImage BookletPage;
			if (MetInfo.printPreprocessType() == RenderSettings::TypeHBooklet)
			{
				BookletPage = QImage(LPage.width() + RPage.width(), qMax(LPage.height(), RPage.height()), QImage::Format_RGB32);
				QPainter Painter(&BookletPage);
				Painter.drawImage(QPoint(0, 0), LPage);
				Painter.drawImage(QPoint(LPage.width(), 0), RPage);
				Painter.end();
			}
			else
			{
				BookletPage = QImage(qMax(LPage.width() , RPage.width()), LPage.height() + RPage.height(), QImage::Format_RGB32);
				QPainter Painter(&BookletPage);
				Painter.drawImage(QPoint(0, 0), LPage);
				Painter.drawImage(QPoint(0, LPage.height()), RPage);
				Painter.end();
			}

			QString CurrImageFileName = _ExportDir + "/" + QString("page_%1").arg(
				QString::number(Vfor), 4, '0') + "." + Format;

			if (!BookletPage.save(CurrImageFileName))
				_ErrorStack.push_back(QString(tr("There was problems storing the following files: %1").arg(CurrImageFileName)));
			else
				Res.push_back(CurrImageFileName);

			if (_StatusWidget)
			{
				_StatusWidget->incrementProgress();
				QApplication::processEvents();
			}
		}
	}

	if (_StatusWidget)
		_StatusWidget->hide();

	QApplication::restoreOverrideCursor();
	return Res;
}

bool Document::containsImage(const QString& _ImageMD5Sum) const
{
	PageList::const_iterator it = Pages.begin();
	bool Found = false; 
	while (it != Pages.end() && ! Found)
	{
		Found = (*it)->containsImage(_ImageMD5Sum); 
		++it; 
	}
	return Found; 
}

int Document::numImageMatches(const QString& _ImageMD5Sum) const
{
	PageList::const_iterator it = Pages.begin();
	int Res = 0;
	while (it != Pages.end())
	{
		if ((*it)->containsImage(_ImageMD5Sum))
			Res++;
		++it;
	}
	return Res;
}

int Document::numPhotoFrames() const
{
	PageList::const_iterator it = Pages.begin();
	int Res = 0;
	while (it != Pages.end())
	{
		Res +=(*it)->numPhotoItems();
		++it;
	}
	return Res;

}

Document::EnItemType Document::itemType(QGraphicsItem* _Item)
{
	EnItemType CurrType = UnknownItemType; 
	switch (_Item->type())
	{
		case GraphicsPhotoItem::Type:
			CurrType = PhotoItemType;
		break;
		case GraphicsTextItem::Type:
			CurrType = RitchTextItemType;
		break; 
		case GraphicsPageItem::Type:
			CurrType = PageItemType;
		break;
		case GraphicsClipartItem::Type:
			CurrType = ClipartItemType;
		break;
		case GraphicsMonthItem::Type:
			CurrType = MonthItemType;
		break;
		default:
			CurrType = PageItemType;
	}
	return CurrType;
}

Document::EnSelectionType Document::selectionType(QList<QGraphicsItem *> _SelectedItems)
{
	Document::EnSelectionType Res = UnknownSelectionType;
	
	if (_SelectedItems.size() > 0 )
	{
		int CType = _SelectedItems.first()->type(); 
		bool SameType = true; 
		int Cnt = 0; 
		while (SameType && Cnt < _SelectedItems.size())
		{
			SameType = (CType == _SelectedItems[Cnt]->type());
			Cnt++;
		}
		if (SameType)
		{
			switch(CType)
			{
				case GraphicsPhotoItem::Type:
					Res = PhotoSelectionType;
				break;
				case GraphicsTextItem::Type:
					Res = TextSelectionType;
				break; 
				case GraphicsPageItem::Type:
					Res = PageSelectionType;
				break;
				default:
					Res = UnknownSelectionType;
			}
		}
		else 
			Res = MultiSelectionType;
	}	
	else 
		Res = NoneSelected; 
	return Res; 
}

bool Document::isSingleTypeSelection(QList<QGraphicsItem*> _SelectedItems)
{
	bool Res = _SelectedItems.size() > 0; 
	if (Res)
	{
		EnItemType BaseType = itemType(_SelectedItems[0]);
		EnItemType CType = BaseType; 
		int Cnt = 1; 
		while (Res && Cnt < _SelectedItems.size())
		{
			CType = itemType(_SelectedItems[Cnt]);
			Res = CType == BaseType; 
			Cnt++;
		}
	}
	return Res; 
}


bool Document::hasEmptyPhotoItems() const
{
	PageList::const_iterator it = Pages.begin();
	bool Found = false; 
	while (it != Pages.end() && ! Found)
	{
		Found = (*it)->hasEmptyPhotoItems(); 
		++it; 
	}
	return Found; 
}


bool Document::canRemovePage(int _PageIndex)
{
	bool Res = true;
	if (Covers.count() > 0)
	{
		if (_PageIndex == 0)
			Res = false;
		if (FirstPageLayouts.count() > 0 && _PageIndex == 1)
			Res = false;
	}
	else
	{
		if (FirstPageLayouts.count() > 0 && _PageIndex == 0)
			Res = false;
	}

	if (LastPageLayouts.count() > 0)
	{
		if (_PageIndex == Pages.count() -1)
			Res = false;
	}

	return Res;
}


bool Document::canInsertTemplate(int _PageIndex, TemplateScene* _Template, QString& _Reason)
{
	bool Res = suitableTemplate(_PageIndex, _Template, _Reason, Pages.count() + 1);
    if (Pages.count() > 0 && Res)
    {
        if (Covers.contains(_Template)) //We already have a cover.
		{
			_Reason = tr("We already have a cover");
            Res = false;
		}
		if (FirstPageLayouts.contains(_Template)) //Its a first page template
        {
            if (Pages.count() > 1 && Covers.count() > 0)
            {
				_Reason = tr("First page already exist");
				Res = false;
            }
			else
            if (Covers.count() == 0)
            {
				if (Pages.count() > 0)
				{
					_Reason = tr("First page already exist");
                    Res = false;
				}
			}
        }
        if (LastPageLayouts.contains(_Template)) //Its a last page template
        {
			_Reason = tr("Last page template can never be inserted.");
			Res = false;
        }
    }
    return Res;
}

/*!
	\return True if template _Template is suitable for page with index _PageIndex.
*/

bool Document::suitableTemplate(int _PageIndex, TemplateScene* _Template, QString& _Reason, int _PageCount)
{
	int PageCount = _PageCount;
	if (_PageCount == -1)
		PageCount = Pages.count();
	bool Res = true;
	int FirstPageIndex = 0;
	if (Covers.size() > 0)
	{
		//if (_PageIndex != 0 && _Template.isFirstPage())
		if (_PageIndex != 0 && Covers.contains(_Template))
		{
            _Reason = tr("Template is for cover only.");
            Res = false;
		}
		else
		if (_PageIndex == 0 && !Covers.contains(_Template))
		{
            _Reason = tr("Template is not for cover.");
            Res = false;
		}
		FirstPageIndex = 1;
	}


    if (FirstPageLayouts.size() > 0)
    {
		if (_PageIndex != FirstPageIndex && FirstPageLayouts.contains(_Template) )
        {
            _Reason = tr("Template is for first page only");
            Res = false;
        }
        else
		if (_PageIndex == FirstPageIndex && ! FirstPageLayouts.contains(_Template))
        {
            _Reason = tr("Template is not for first page");
            Res = false;
        }
    }

    if (LastPageLayouts.size() > 0)
    {
		if (_PageIndex != PageCount -1 && LastPageLayouts.contains(_Template))
        {
            _Reason = tr("Template is for last page only.");
            Res = false;
        }
		else if (_PageIndex == PageCount -1 && !LastPageLayouts.contains(_Template))
        {
            _Reason = tr("Template is not for last page");
            Res = false;
        }
    }

    return Res;
}


bool Document::isExportedAsBooklet() const
{
	return  MetInfo.printPreprocessType() == RenderSettings::TypeVBooklet || MetInfo.printPreprocessType() == RenderSettings::TypeHBooklet ;
}

/*int Document::numRenderedPages(bool _Booklet)
{
	int Res = pages().count();
	if (MetInfo.cutPagesOnPrint() && !_Booklet && MetInfo.printPreprocessType() != RenderSettings::TypeBooklet)
	{
		double PSizeWidth = 0;
		if (!pages().isEmpty())
			PSizeWidth = Pages[0]->sceneRect().size().width();

		int PPageWidthWidth = MetInfo.printPageSize().width();
		if (PPageWidthWidth == 0)
		{
			if (!pages().isEmpty())
				PPageWidthWidth = PSizeWidth;
		}
		int SheetsPerPage = PSizeWidth / PPageWidthWidth;
		Res = pages().count() * qMax(SheetsPerPage, 1);
	}
	return Res;
}*/


QImage Document::getPageThumbnail(int _Index, const QSize& _MaxSize)
{
	QImage Res;
	if (_Index >= 0 && _Index < Pages.size())
	{
		TemplateScene* Scene = Pages[_Index];

		QSize ThumbSize = _MaxSize;
		if (Scene->width() > Scene->height() && ThumbSize.width() < ThumbSize.height() ||
			Scene->width() < Scene->height() && ThumbSize.width() > ThumbSize.height() )
			ThumbSize.transpose();
		ThumbSize.setHeight(Scene->height() * ThumbSize.width() / Scene->width());
		QImage ResImg(ThumbSize, QImage::Format_RGB32);

		ResImg.fill(Qt::white);
		QPainter Painter(&ResImg);
		Scene->setEmptyFramesVisible(false);
		Scene->prepareForPrint();
		Scene->render(&Painter, ResImg.rect());
		Scene->setEmptyFramesVisible(true);
		Res = ResImg;
	}
	return Res;
}

QImage Document::getLastPageThumbnail(const QSize& _MaxSize)
{
	return getPageThumbnail(Pages.size() - 1, _MaxSize);
}

bool Document::isPhotoBookCorrect(QString& _ErrorMessage, bool _CheckToOrder)
{
	bool Res = true;

	if (hasEmptyPhotoItems())
	{
		_ErrorMessage = tr("The model contains empty frames. Please delete or fill it.");
		Res = false;
	}
	if (MetInfo.modPages() > 0)
	{
		if (pages().size() % MetInfo.modPages() != 0)
		{
			_ErrorMessage = QString(tr("The model should have the number of pages in multiple of %1")).arg(MetInfo.modPages());
			Res = false;
		}
	}
	if (_CheckToOrder)
	{
		if (pages().size() < MetInfo.minPages())
		{
			_ErrorMessage = QString(tr("The Model must have at least %1 pages")).arg(MetInfo.minPages());
			Res = false;
		}
	}
	return Res;
}

int Document::pagesPerSheet() const
{
    int Res = (MetInfo.size().width() * MetInfo.size().height()) / (MetInfo.printPageSize().width() * MetInfo.printPageSize().height());
    if (MetInfo.printPreprocessType() == RenderSettings::TypeHBooklet || MetInfo.printPreprocessType() == RenderSettings::TypeHBooklet )
        Res *= 2;
    return Res;
}

void Document::setEncryptionKey(const QString& _Key)
{
	EncryptionKey = _Key;
}

void Document::clearEncryptionKey()
{
	EncryptionKey.clear();
}


void Document::slotSceneSelectionChange()
{
	if (QGraphicsScene* CScene = qobject_cast<QGraphicsScene*>(sender()))
		emit sceneSelectionChanged(CScene);
}

void Document::slotSceneDoubleClicked()
{
	if (QGraphicsScene* CScene = qobject_cast<QGraphicsScene*>(sender()))
		emit sceneDoubleClicked(CScene);
}

void Document::slotSceneItemContextMenu(QGraphicsItem* _Item, const QPoint& _Pos)
{
	if (QGraphicsScene* CScene = qobject_cast<QGraphicsScene*>(sender()))
		emit sceneItemContextMenu(_Item, CScene, _Pos); 
}

void Document::someSceneChanged()
{
	//Check if the chages made on scene are important:
	if (anySceneHasChanges())
		setHasChanges(true);
}

