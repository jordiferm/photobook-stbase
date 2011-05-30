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
#include "stphotobook.h"
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
#include "sttemplatescene.h"

#include "stgraphicsphotoitem.h"
#include "stgraphicssimpletextitem.h"
#include "stgraphicspageitem.h"
#include "stgraphicstextitem.h"
#include "sterrorstack.h"
#include "sprocessstatuswidget.h" 
#include "stprogressindicator.h"
#include "stcandidatecalculator.h"

#define ITEM_AVERAGE_MARGIN 2

STTemplateScene* STPhotoBook::createPage()
{
	STTemplateScene* Scene = new STTemplateScene(this);
	Scene->setAutoAdjustFrames(AutoAdjustFrames);
	Scene->setIgnoreExifRotation(IgnoreExifRotation);
	connect(Scene, SIGNAL(selectionChanged()), this, SLOT(slotSceneSelectionChange()));
	connect(Scene, SIGNAL(doubleClicked()), this, SLOT(slotSceneDoubleClicked()));
	connect(Scene, SIGNAL(itemContextMenu(QGraphicsItem*, const QPoint&)), this, SLOT(slotSceneItemContextMenu(QGraphicsItem*, const QPoint&)));
	connect(Scene, SIGNAL(imageDropped(const QString&, const QString&)), this, SIGNAL(imageDropped(const QString&, const QString&))); 
	connect(Scene, SIGNAL(imageRemoved(const QString&, const QString&)), this, SIGNAL(imageRemoved(const QString&, const QString&))); 
	connect(Scene, SIGNAL(templateDropped(STTemplateScene*,STPhotoLayoutTemplate)), this, SIGNAL(templateDropped(STTemplateScene*,STPhotoLayoutTemplate)));
	connect(Scene, SIGNAL(clipartDropped(QString, QPointF)), this, SIGNAL(clipartDropped(QString, QPointF)));
	connect(Scene, SIGNAL(clicked()), this, SIGNAL(sceneClicked()));

	//FIXME: changed no és prou bon indicador.
	connect(Scene, SIGNAL(changed( const QList< QRectF >& )), this, SLOT(someSceneChanged()));
	
	return Scene; 
}

STTemplateScene* STPhotoBook::createPage(STPhotoLayoutTemplate _Template, QList<STGraphicsPhotoItem*>& _PhotoItems)
{
	STTemplateScene* Scene = createPage();
	
	Scene->loadPageTemplate(_Template); 
	
	_PhotoItems = Scene->photoItems();
	return Scene;
}

/*!
	overload function created for convenience.
*/
STTemplateScene* STPhotoBook::createPage(STPhotoLayoutTemplate _Template)
{
	QList<STGraphicsPhotoItem*> DummyList;
	return createPage(_Template, DummyList);
}

void STPhotoBook::setHasChanges(bool _Value)
{
	if (_Value != HasChanges)
	{
		HasChanges = _Value; 
		emit changed(); 
	}
}

void STPhotoBook::setBuildOptions(const STPhotoBookBuildOptions& _Options)
{
	setIgnoreExifRotation(_Options.ignoreExifRotation());
	setAutoFillBackgrounds(_Options.autoFillBackgrounds());
	setPagesToFill(_Options.pagesToFill());
	setAutoAdjustFrames(_Options.autoadjustFrames());
	qDebug() << "_Options.autoadjustFrames()" << _Options.autoadjustFrames();
	if (!_Options.useTexts())
		Template.removeTextTemplates();
	if (!_Options.title().isEmpty())
		Template.setTitleText(_Options.title());
}



STPhotoBook::STPhotoBook(QObject* _Parent) : QObject(_Parent),  HasChanges(false), SourceImagesPath(""), AutoAdjustFrames(true), IgnoreExifRotation(false), AutoFillBackgrounds(false), PagesToFill(0)
{
}

STPhotoBook::~STPhotoBook()
{
	closePhotoBook();
}

STPhotoLayoutTemplate STPhotoBook::randomTemplate(const STPhotoBookTemplate::TTemplateList& _Templates)
{
	STPhotoLayoutTemplate Res; 
	
	if (_Templates.size() > 0)
	{
		Res = _Templates[0];
		int NTemplate = qrand() % _Templates.size();
		STPhotoLayoutTemplate CTemplate = _Templates[NTemplate];
		bool Found = false;
		int NInspectedItems = 1;
		while (!Found && NInspectedItems <= _Templates.size())
		{
			Found = !CTemplate.isFirstPage();
			if (!Found)
			{
				NTemplate = (NTemplate + 1) % _Templates.size();
				CTemplate = _Templates[NTemplate];
				NInspectedItems++;
			}
			else 
				Res = CTemplate;
		}
	}		
	return Res;
}

/*!
	Retorna un template candidat setons el numero de fotos restants per a posar.
	_AllTemplates ha d'estar ordenat de menys a més segons el nombre de fotos per template.
*/
STPhotoLayoutTemplate STPhotoBook::candidateTemplate(int _AvaliablePhotos,
	const STPhotoBookTemplate::TTemplateList& _AllTemplates, bool _FirstPage, int _PagesToFill, int _AvgMargin, const STPhotoBookTemplate::TTemplateList& _UsedTemplates)
{
	STPhotoBookTemplate::TTemplateList Candidates;
	STPhotoBookTemplate::TTemplateList::const_iterator lBoundIt;
	STPhotoBookTemplate::TTemplateList::const_iterator it;
	bool HasFirstPages = Template.hasFirstPages();
	//Search a template between per page average +- _AvgMargin 
	if (_PagesToFill > 0)
	{
		int Avg = _AvaliablePhotos / _PagesToFill; 
		lBoundIt = qLowerBound(_AllTemplates.begin(), _AllTemplates.end(), Avg - _AvgMargin);
		STPhotoBookTemplate::TTemplateList::const_iterator UBoundIt = qUpperBound(_AllTemplates.begin(), _AllTemplates.end(), Avg + _AvgMargin);
		
		for (it = lBoundIt; it != UBoundIt; ++it)
		{
			if ((*it).isFirstPage() == _FirstPage || !HasFirstPages)
				Candidates.push_back(*it);
		}	
	}

	//If not find a template with less than _AvaliablePhotos.
	if (Candidates.isEmpty())
	{
		lBoundIt = qLowerBound(_AllTemplates.begin(), _AllTemplates.end(), _AvaliablePhotos);
		//qDebug("Candidates of: %d", _AvaliablePhotos);
		
		for (it = _AllTemplates.begin(); it!=lBoundIt; ++it)
		{
			if ((*it).isFirstPage() == _FirstPage || !HasFirstPages)
				Candidates.push_back(*it);
			//qDebug("Pushing_back %d", (*it).numPhotoFrames());
		}
	}

	if (!_UsedTemplates.isEmpty())
	{
		//Remove used templates from Candidates.
		STPhotoBookTemplate::TTemplateList UnusedCandidates;
		for (it = Candidates.begin(); it != Candidates.end(); ++it) 
		{
			if (!_UsedTemplates.contains(*it))
				UnusedCandidates.push_back(*it);
		}
		if (!UnusedCandidates.isEmpty())
			Candidates = UnusedCandidates;
	}

	STPhotoLayoutTemplate Res;
	if (Candidates.count() > 0)
	{
		if (!_UsedTemplates.isEmpty() && _UsedTemplates.size() < _AllTemplates.size()) //We get the template with smallest key.
		{
			int Cnt = 0;
			//int CurrKey = Candidates[Cnt].key();
			Res = Candidates[Cnt++];
			while (Cnt < Candidates.count())
			{
				int CurrKey = Candidates[Cnt].key();
				if ((Res.key() > CurrKey && Candidates[Cnt].numPhotoFrames() == Res.numPhotoFrames()) ||
					Candidates[Cnt].numPhotoFrames() > Res.numPhotoFrames())
					Res = Candidates[Cnt];
				Cnt++; 
			}
		}
		else 
			Res = Candidates[STUtils::random(0, Candidates.count())];
	}
	else
	{ // Noi, no hi ha candidats => Poden sobrar frames.
		//Bafem el mï¿½s petit que no sigui primera pï¿½gina.
		if (!_AllTemplates.isEmpty())
		{
			it = _AllTemplates.begin();
			bool FirstPageOk = ((*it).isFirstPage() != _FirstPage || !HasFirstPages);
			while(FirstPageOk && it != _AllTemplates.end())
			{
				FirstPageOk = ((*it).isFirstPage() != _FirstPage || !HasFirstPages);
				++it;
			}
			if (it == _AllTemplates.end()) //No hi ha mï¿½s. Agafem el primer i milles.
				Res = *(_AllTemplates.begin());
			else
				Res = *it;
		}
	}
	return Res;
}


void STPhotoBook::setTemplate(STPhotoBookTemplate& _Template, const STPhotoBookBuildOptions& _Options)
{
	Template = _Template;
	setBuildOptions(_Options);
}


/*! After clear() call isEmpty() returns true
*/
void STPhotoBook::clear()
{
	TPagesList::iterator it;
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		(*it)->unloadImages();
		delete (*it);
	}
	Pages.clear();
	setHasChanges(false); 
}

bool STPhotoBook::isEmpty() const 
{
	return Pages.size() == 0; 
}

void STPhotoBook::insertPage(STTemplateScene* _Page, int _Index)
{
	Pages.insert(_Index, _Page);
}

void STPhotoBook::insertRandomPage(int _Index)
{
	insertPage(createPage(randomTemplate(Template.templates())), _Index);
}

void STPhotoBook::updatePage(int _Index)
{
	if (_Index >= 0 && _Index < Pages.count()) //Defensive 
		Pages[_Index]->updateImages(); 
}

void STPhotoBook::removePage(int _Index)
{
	Assert(_Index >= 0 && _Index < Pages.count(), Error(tr("STPhotoBook::removePage Range Checking Error.")));
	Pages[_Index]->clear(); 
	Pages.removeAt(_Index); 
}

void STPhotoBook::addMinimumPages()
{
	//Insert Main page.
	STPhotoLayoutTemplate MainPageTemplate = candidateTemplate(1000, Template.templates(), true);
	insertPage(createPage(MainPageTemplate), 0);

	int NPages = 1;
	STPhotoBookTemplate::TTemplateList UsedTemplates;
	UsedTemplates.push_back(MainPageTemplate);
	while (NPages < Template.minPages())
	{
		STPhotoLayoutTemplate CurrTemplate = candidateTemplate(1000, Template.templates(),
			false, Template.maxPages() - NPages, ITEM_AVERAGE_MARGIN, UsedTemplates);
		if (!UsedTemplates.contains(CurrTemplate))
			UsedTemplates.push_back(CurrTemplate);
		insertPage(createPage(CurrTemplate), NPages);
		NPages++;
	}
}


/*
void STPhotoBook::autoBuild(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress)
{
	clear();
	STPhotoBookTemplate::TTemplateList Templates = Template.templates();
	STPhotoBookTemplate::TTemplateList UsedTemplates;
	qSort(Templates.begin(), Templates.end()); //Sorted by number of gaps.
	int InsertedPhotos = 0;
	int TotalPhotos = _PhotoModel->rowCount();
	STUtils::randomize();

	if (_Progress)
		_Progress->setRange(0, TotalPhotos);
	int NPages = 0;
	while (InsertedPhotos < TotalPhotos && Templates.count() > 0 && NPages < Template.maxPages() &&
		   ((Template.preferMinPages() && NPages < Template.minPages()) || !Template.preferMinPages()))
	{
		//Agafem un template qualsevol
		STPhotoLayoutTemplate CurrTemplate = candidateTemplate(TotalPhotos - InsertedPhotos, Templates,
			NPages == 0, Template.maxPages() - NPages, qMin(ITEM_AVERAGE_MARGIN, (TotalPhotos - InsertedPhotos) / ITEM_AVERAGE_MARGIN), UsedTemplates);
		if (!UsedTemplates.contains(CurrTemplate))
			UsedTemplates.push_back(CurrTemplate);

		QList<STGraphicsPhotoItem*> PhotoItems;
		STTemplateScene* NewPage = createPage(CurrTemplate, PhotoItems);
		Pages.push_back(NewPage);
		int CntCurrPhoto = 0;
		while (InsertedPhotos < TotalPhotos && CntCurrPhoto < PhotoItems.count())
		{
			STGraphicsPhotoItem* CurrPhotoW = PhotoItems.at(CntCurrPhoto);

			QModelIndex CIndex = _PhotoModel->index(InsertedPhotos, 0);
			STDom::DDoc* CDoc = _PhotoModel->doc(CIndex);
			CurrPhotoW->setDoc(CDoc);

			//OPhotoCollectionImage COImage = _PhotoModel->image(CIndex);
			//CurrPhotoW->setImage(COImage.thumbnail(), COImage.fileInfo().absoluteFilePath());

			CntCurrPhoto++;
			if ((CurrTemplate.modifyAllFrames() && CntCurrPhoto == PhotoItems.count()) || !CurrTemplate.modifyAllFrames())
				InsertedPhotos++ ;

			if (_Progress)
			{
				_Progress->setValue(InsertedPhotos);
				QApplication::processEvents();
			}
		}
		NPages++;
		emit newPageCreated();
	}
	if (_Progress)
		_Progress->setValue(TotalPhotos);

	while (NPages < Template.minPages())
	{
		//Agafem un template qualsevol
		STPhotoLayoutTemplate CurrTemplate = candidateTemplate(qMax(TotalPhotos - InsertedPhotos, 1), Templates,
			Pages.size() == 0, Template.maxPages() - NPages, 1, UsedTemplates);
		InsertedPhotos += CurrTemplate.picFrames().size();
		if (!UsedTemplates.contains(CurrTemplate))
			UsedTemplates.push_back(CurrTemplate);
		//insertPage(createPage(randomTemplate(Template.templates())), NPages);
		insertPage(createPage(CurrTemplate), NPages);
		NPages++;
	}
}
*/

void STPhotoBook::buildCalendar(STDom::DDocModel* _PhotoModel, const QDate& _FromDate, const QDate& _ToDate, QProgressBar* _Progress)
{
	if (!Template.isVariableCalendar())
	{
		autoBuild(_PhotoModel, _Progress);
		return;
	}
	clear();
	STCandidateCalculator CCalculator(*this, _PhotoModel);
	int NumMonths = STUtils::monthsTo(_FromDate, _ToDate) + 1;
	if (_Progress)
		_Progress->setRange(0, NumMonths);

	STPhotoBookTemplate::TTemplateList TemplateList = Template.templates();
	//If there is firstpage, put the first page.
	STPhotoBookTemplate::TTemplateList::iterator it = TemplateList.begin();
	int Found = false;
	while (it != TemplateList.end() && !Found)
	{
		Found = it->isFirstPage();
		if (!Found)
			++it;
	}
	if (Found)
	{
		STPhotoLayoutTemplate FirstPageTemplate = *it;
		FirstPageTemplate.setYear(_FromDate.year());
		STTemplateScene* NewPage = createPage(FirstPageTemplate);
		Pages.push_back(NewPage);
		CCalculator.fillPage(NewPage, FirstPageTemplate, _Progress);
		emit newPageCreated();
	}

	//For each month
	QDate CDate = _FromDate;
	for (int Vfor = 0; Vfor < NumMonths; Vfor++)
	{
		STPhotoLayoutTemplate CurrTemplate = CCalculator.getDateCandidate(CDate);
		CDate = CDate.addMonths(1);
		STTemplateScene* NewPage = createPage(CurrTemplate);
		Pages.push_back(NewPage);
		CCalculator.fillPage(NewPage, CurrTemplate, _Progress);
		emit newPageCreated();
	}
}

void STPhotoBook::autoBuild(STDom::DDocModel* _PhotoModel, QProgressBar* _Progress)
{
	clear();
	STCandidateCalculator CCalculator(*this, _PhotoModel);

	if (_Progress)
		_Progress->setRange(0, CCalculator.totalPhotos());
	int NPages = 0;
	int PagToFill = qMin(PagesToFill, Template.maxPages());
	if (PagToFill == 0)
		PagToFill = Template.minPages();

	while (CCalculator.photosAvailable() && CCalculator.templatesAvailable() > 0 && NPages < PagToFill &&
		   ((Template.preferMinPages() && NPages < Template.minPages()) || !Template.preferMinPages()))
	{
		//Agafem un template qualsevol
		STPhotoLayoutTemplate CurrTemplate = CCalculator.getCandidate(NPages == 0, PagToFill - NPages,
																	  CCalculator.calcMargin(ITEM_AVERAGE_MARGIN, PagToFill - NPages),
																	  Template.hasFirstPages());
		STTemplateScene* NewPage = createPage(CurrTemplate);
		Pages.push_back(NewPage);
		CCalculator.fillPage(NewPage, CurrTemplate, _Progress);
		CCalculator.markAsUsed(CurrTemplate);
		NPages++;
		emit newPageCreated();
	}
	if (_Progress)
		_Progress->setValue(CCalculator.totalPhotos());

	//There's no images and still pages to fill.
	while (NPages < Template.minPages())
	{
		//Agafem un template qualsevol
		STPhotoLayoutTemplate CurrTemplate = CCalculator.getCandidate(NPages == 0, Template.maxPages() - NPages, Template.numOptimalImagesPerPage(),
																	  Template.hasFirstPages());
		insertPage(createPage(CurrTemplate), NPages);
		CCalculator.markAsUsed(CurrTemplate);
		NPages++;
	}
}

QSize STPhotoBook::renderSize(STTemplateScene* _Scene) const
{
	QSize Res(0,0);
	QSize PageSize = STPhotoLayoutTemplate::pixelSize(_Scene->sceneRect().size(), Template.dpis());
	if (!_Scene->renderBaseSize().isEmpty())
	{
		QSize DestSize = STPhotoLayoutTemplate::pixelSize(_Scene->renderBaseSize(), Template.dpis());
		if (DestSize.isValid() && DestSize.width() >=  PageSize.width() && DestSize.height() >= PageSize.height() )
			Res = DestSize;
		else
			Res = PageSize;
	}
	else
		Res = PageSize;
	return Res;
}

//! _Page goes from 0 to numPages - 1.
QImage STPhotoBook::renderPage(int _Page, QProgressBar* _LoadImagesPrgBar)
{
	Assert(_Page >= 0 && _Page < Pages.size(), Error("STPhotoBook::renderPage Range Checking Error"));
	STTemplateScene* PageScene = Pages[_Page];
	QSize PageSize = renderSize(PageScene);
	QImage ResImg(PageSize, QImage::Format_RGB32);
	ResImg.setDotsPerMeterX(Template.dpM());
	ResImg.setDotsPerMeterY(Template.dpM());
	QPainter Painter(&ResImg);
	renderPage(PageScene, _LoadImagesPrgBar, &Painter); 
	return ResImg;
}

void STPhotoBook::renderPage(STTemplateScene* _PageScene , QProgressBar* _LoadImagesPrgBar, QPainter* _Painter)
{
	Assert(_PageScene, Error("STPhotoBook::renderPage NULL TemplateScene assigned."));
	_PageScene->setEmptyFramesVisible(false);
	_PageScene->prepareForPrint();
	_PageScene->loadHiResImages(_LoadImagesPrgBar); 
	QSize PageSize = STPhotoLayoutTemplate::pixelSize(_PageScene->sceneRect().size(), Template.dpis());
	QSize RenderSize = renderSize(_PageScene);
	if (PageSize != RenderSize)
	{
		if (RenderSize.isValid() && RenderSize.width() >=  PageSize.width() && RenderSize.height() >= PageSize.height() )
		{
			STImage TmpImg(PageSize, QImage::Format_RGB32);
			QPainter ImgPainter(&TmpImg);
			_PageScene->render(&ImgPainter, QRect(QPoint(0,0), PageSize));
			TmpImg.multiplyImage(RenderSize, _Painter);
		}
	}
	else
		_PageScene->render(_Painter, QRect(QPoint(0,0), RenderSize));

	_PageScene->unloadImages();
	_PageScene->setEmptyFramesVisible(true);
}

void STPhotoBook::renderPageToPdf(int _Page, QProgressBar* _LoadImagesPrgBar, const QString& _PdfFileName)
{
	Assert(_Page >= 0 && _Page < Pages.size(), Error("STPhotoBook::renderPage Range Checking Error"));
	STTemplateScene* PageScene = Pages[_Page];
	QPrinter Printer;
	Printer.setOutputFormat(QPrinter::PdfFormat);
	#if QT_VERSION >= 0x040400
	Printer.setPaperSize(PageScene->sceneRect().size(), QPrinter::Millimeter);
	Printer.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter); 
	#endif
	Printer.setOutputFileName(_PdfFileName);
	Printer.setResolution(Template.dpis()); 
	QPainter Painter;
	Assert(Painter.begin(&Printer), Error(tr("failed to open PDF file: %1, is it writable?").arg(_PdfFileName)));
	renderPage(PageScene, _LoadImagesPrgBar, &Painter); 
	Painter.end(); 
}

QList<QImage> STPhotoBook::prepareForPrint(const QImage& _AlbumPageImage)
{
	QList<QImage> Res; 
	if (Template.cutPagesOnPrint())
	{
		int PPageWidthWidth = Template.printPageWidth();
		if (PPageWidthWidth == 0)
			PPageWidthWidth = Template.size().width();
		QSize PrintPageSize = STPhotoLayoutTemplate::pixelSize(QSize(PPageWidthWidth, _AlbumPageImage.height()), qMax(Template.dpis(), 10));
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
	return Res;
}

void STPhotoBook::createRootPath()
{
	QDir AlbumRootDir(PBInfo.photoBookPath()); 
	Assert(AlbumRootDir.mkpath(PBInfo.photoBookPath()), Error(QString(tr("Error creating path %1")).arg(PBInfo.photoBookPath())));
}

QDomDocument STPhotoBook::createDoc()
{
	QDomDocument Doc("starphob");
	QDomElement Root = Doc.createElement("photobook");
	Doc.appendChild(Root);
	Root.setAttribute("version", "1.0.0");
	Root.setAttribute("description", Description);
	Root.setAttribute("templatefilepath", TemplateFilePath);
	Root.setAttribute("sourceimagespath", SourceImagesPath);

	TPagesList::iterator it;
	for (it = Pages.begin(); it != Pages.end(); ++it)
	{
		Root.appendChild((*it)->createElement(Doc));
	}
	return Doc;
}

void STPhotoBook::save(STProgressIndicator* _Progress , bool _AutoSave)
{
	saveAs(QDir(PBInfo.defaultRootPathName()), PBInfo.photoBookName(), _Progress, _AutoSave);
}

void STPhotoBook::saveAs(const QDir& _Dir, STProgressIndicator* _Progress, bool _AutoSave )
{
	STPhotobookCollectionInfo MInfo(_Dir); 
	saveAs(QDir(MInfo.rootPathName()), MInfo.photoBookName(), _Progress, _AutoSave);
}

void STPhotoBook::saveAs(const QDir& _RootPath, const QString& _Name, STProgressIndicator* _Progress, bool _AutoSave)
{
	//STPhotobookCollectionInfo MInfo(_Name, _RootPath.absolutePath()); 
	PBInfo.setPhotoBookName(_Name); 
	PBInfo.setRootPathName(_RootPath.absolutePath()); 
	QDir PBDir(PBInfo.photoBookPath()); 
	Assert(PBDir.mkpath(PBDir.absolutePath()), Error(QString(tr("Error creating Photo Book Path %1")).arg(PBDir.absolutePath()))); 
	
	QFile PBFile;
	if (_AutoSave)
		PBFile.setFileName(PBInfo.xmlAutoSaveFileName());
	else
		PBFile.setFileName(PBInfo.xmlFileName());
	Assert(PBFile.open(QFile::WriteOnly | QFile::Truncate), Error(QString(tr("Could not open file %1")).arg(PBFile.fileName())));
		
	if (_Progress)
	{
		if (_AutoSave)
			_Progress->setRange(0, 1);
		else
			_Progress->setRange(0, Pages.size() - 1);
		_Progress->start(tr("Saving Model..."));
		_Progress->setValue(0);
	}
	
	QString StoreImagesError;
	if (!_AutoSave)
	{
		TPagesList::iterator it;
		//Store first Page as a thumbnail.
		if (Pages.size() > 0)
		{
			QImage ResImg = getPageThumbnail(0, QSize(150, 100));
			Assert(ResImg.save(PBInfo.thumbnailFileName()), Error(QString(tr("Error saving photo book thumbnail at: %1")).arg(PBInfo.thumbnailFileName())));
		}
		QStringList StoredFiles;
		for (it = Pages.begin(); it != Pages.end(); ++it)
		{
			try
			{
				StoredFiles += (*it)->storePhotoItemImages(PBInfo);
			}
			catch(STError& _Error)
			{
				StoreImagesError += _Error.description() + "\n";
			}
			if (_Progress)
				_Progress->incValue();
		}
		// Delete unused images and unused mask images. (Guardar a una llista les que s'utilitzen i borrar les demés ?)
		QDir PBooKDir(PBInfo.photoBookPath());
		QStringList PBookFiles = PBooKDir.entryList(QDir::Files);
		QStringList::iterator sit;
		for (sit = PBookFiles.begin(); sit != PBookFiles.end(); ++sit)
		{
			QString CurrAbsPathFile = PBInfo.photoBookPath() + "/" + (*sit);
			if (!StoredFiles.contains(CurrAbsPathFile) &&
				(CurrAbsPathFile != PBInfo.thumbnailFileName()) && (CurrAbsPathFile != PBInfo.xmlFileName()) &&
				(CurrAbsPathFile != PBInfo.trayImagesFileName()) && CurrAbsPathFile != TemplateFilePath)
			{
				QFile DelFile(CurrAbsPathFile);
				//TODO Put Log here !!!
				if (!DelFile.remove())
					qWarning(QString("Error removing file %1").arg(CurrAbsPathFile).toLatin1());
			}
		}
	}
	QString OutputStr;
	QDomDocument Doc = createDoc();
	if (EncryptionKey.isEmpty())
	{
		QTextStream Out(&PBFile);
		//Out.setCodec(QTextCodec::codecForName("ISO-8859-1"));
		Out.setCodec(QTextCodec::codecForName("UTF-8"));
		Out << Doc.toString();
	}
	else
	{
		QDataStream Out(&PBFile);
		QByteArray InputData(Doc.toString().toUtf8());
		QByteArray OutputData = STUtils::encode(InputData, EncryptionKey);
		Out << OutputData;
		//Out.writeBytes(OutputData, OutputData.length());
	}

	if (_Progress)
		_Progress->stop();

	Assert(StoreImagesError.isNull(), Error(QString(tr("There was errors storing images: %1")).arg(StoreImagesError)));
}

void STPhotoBook::closePhotoBook()
{
	//Correctly closed => remove autosave
	if (!name().isEmpty())
	{
		STPhotobookCollectionInfo MInfo(name());
		QFile AutoSaveFile(MInfo.xmlAutoSaveFileName());
		if (AutoSaveFile.exists())
			AutoSaveFile.remove();
	}
}

bool STPhotoBook::autoSaved(const QString& _Name) const
{
	STPhotobookCollectionInfo MInfo(_Name);
	return QFile::exists(MInfo.xmlAutoSaveFileName());
}

bool STPhotoBook::autoSaved(const QDir& _Dir) const
{
	STPhotobookCollectionInfo MInfo(_Dir);
	return QFile::exists(MInfo.xmlAutoSaveFileName());
}

void STPhotoBook::load(const QString& _Name, QProgressBar* _ProgressBar, bool _AutoSaved)
{
	STPhotobookCollectionInfo MInfo(_Name); 
	load(QDir(MInfo.rootPathName()), MInfo.photoBookName(), _ProgressBar, _AutoSaved);
}

void STPhotoBook::load(const QDir& _Dir, QProgressBar* _ProgressBar, bool _AutoSaved)
{
	STPhotobookCollectionInfo MInfo(_Dir); 
	load(QDir(MInfo.rootPathName()), MInfo.photoBookName(), _ProgressBar, _AutoSaved);
}

/*! Loads the photoBook and also the template */
void STPhotoBook::load(const QDir& _RootPath, const QString& _Name, QProgressBar* _ProgressBar, bool _AutoSaved)
{
	closePhotoBook();
	STPhotobookCollectionInfo MInfo(_Name, _RootPath.absolutePath()); 
	bool Res = false;
	QDomDocument Doc("starphob");
	QFile File;
	if (_AutoSaved)
		File.setFileName(MInfo.xmlAutoSaveFileName());
	else
		File.setFileName(MInfo.xmlFileName());

	QFileInfo XmlFInfo(File);
	Assert(File.open(QIODevice::ReadOnly), Error(QString(tr("Loading Photo Book from file %1")).arg(File.fileName())));
	{
		if (EncryptionKey.isEmpty())
		{
			QTextStream StrIn(&File);
			StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
			Res = Doc.setContent(StrIn.readAll());
			File.close();
		}
		else
		{
			QDataStream DataIn(&File);
			QByteArray BArray;
			DataIn >> BArray;
			QByteArray DecodedData = STUtils::decode(BArray, EncryptionKey);
			Res = Doc.setContent(QString::fromUtf8(DecodedData));
//			qDebug() << File.bytesAvailable();
//			QByteArray BArray(File.bytesAvailable(), 0);
//			DataIn.readRawData(BArray.data(), BArray.size());
//			QByteArray DecodedData = STUtils::decode(BArray, EncryptionKey);
//			Res = Doc.setContent(QString(DecodedData));
		}
	}
	QDomElement RootEl = Doc.documentElement();
	if (_ProgressBar)
	{
		_ProgressBar->setRange(0, RootEl.childNodes().count());
		_ProgressBar->setValue(0); 
		QApplication::processEvents();
	}
	setName(_Name);
	setDescription(RootEl.attribute("description", ""));
	setTemplateFilePath(RootEl.attribute("templatefilepath", ""));
	setSourceImagesPath(RootEl.attribute("sourceimagespath", ""));
	QDomNode CNode = RootEl.firstChild();
	while(!CNode.isNull())
	{
		if (_ProgressBar)
		{
			_ProgressBar->setValue(_ProgressBar->value() + 1); 
			QApplication::processEvents();
		}
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "scene" )
			{
				STTemplateScene* NewPage = createPage();
				NewPage->loadElement(XmlFInfo.absolutePath(), CEl);
				Pages.push_back(NewPage);
			}
		}
		CNode = CNode.nextSibling();
	}
}

QString STPhotoBook::getTemplateFilePath(const QDir& _Dir)
{
	STPhotobookCollectionInfo MInfo(_Dir);
	bool Res = false;
	QDomDocument Doc("starphob");
	QFile File(MInfo.xmlFileName());
	QFileInfo XmlFInfo(File);
	Assert(File.open(QIODevice::ReadOnly), Error(QString(tr("Loading Photo Book from file %1")).arg(File.fileName())));
	QTextStream StrIn(&File);
	StrIn.setCodec(QTextCodec::codecForName("UTF-8"));
	Res = Doc.setContent(StrIn.readAll());
	File.close();
	QDomElement RootEl = Doc.documentElement();
	return RootEl.attribute("templatefilepath", "");
}

bool STPhotoBook::containsImage(const QString& _ImageMD5Sum) const
{
	TPagesList::const_iterator it = Pages.begin();
	bool Found = false; 
	while (it != Pages.end() && ! Found)
	{
		Found = (*it)->containsImage(_ImageMD5Sum); 
		++it; 
	}
	return Found; 
}

int STPhotoBook::numImageMatches(const QString& _ImageMD5Sum) const
{
	TPagesList::const_iterator it = Pages.begin();
	int Res = 0;
	while (it != Pages.end())
	{
		if ((*it)->containsImage(_ImageMD5Sum))
			Res++;
		++it;
	}
	return Res;
}


STPhotoBook::EnItemType STPhotoBook::itemType(QGraphicsItem* _Item)
{
	EnItemType CurrType = UnknownItemType; 
	switch (_Item->type())
	{
		case STGraphicsPhotoItem::Type:
			CurrType = PhotoItemType;
		break;
		case STGraphicsSimpleTextItem::Type:
			CurrType = TextItemType;
		break;
		case STGraphicsTextItem::Type: 
			CurrType = RitchTextItemType;
		break; 
		case STGraphicsPageItem::Type:
			CurrType = PageItemType;
		break;
		default:
			CurrType = PageItemType;
	}
	return CurrType;
}

STPhotoBook::EnSelectionType STPhotoBook::selectionType(QList<QGraphicsItem *> _SelectedItems)
{
	STPhotoBook::EnSelectionType Res = UnknownSelectionType; 
	
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
				case STGraphicsPhotoItem::Type:
					Res = PhotoSelectionType;
				break;
				case STGraphicsSimpleTextItem::Type:
					Res = TextSelectionType;
				break;
				case STGraphicsTextItem::Type: 
					Res = TextSelectionType;
				break; 
				case STGraphicsPageItem::Type:
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

bool STPhotoBook::isSingleTypeSelection(QList<QGraphicsItem*> _SelectedItems)
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

int STPhotoBook::maxPages() const
{
	return Template.maxPages();
}

int STPhotoBook::minPages() const
{
	return Template.minPages(); 
}

bool STPhotoBook::hasEmptyPhotoItems() const
{
	TPagesList::const_iterator it = Pages.begin();
	bool Found = false; 
	while (it != Pages.end() && ! Found)
	{
		Found = (*it)->hasEmptyPhotoItems(); 
		++it; 
	}
	return Found; 
}

void STPhotoBook::movePage(int _Source, int _Destination)
{
	if (_Source >= 0 && _Source < Pages.size() && _Destination >= 0 && _Destination < Pages.size())
	{
		Pages.move(_Source, _Destination); 
	}
}

/*!
	\return True if template _Template is suitable for page with index _PageIndex.
*/

bool STPhotoBook::suitableTemplate(int _PageIndex, const STPhotoLayoutTemplate& _Template, QString& _Reason)
{
	bool Res = false;
	if (Template.hasFirstPages())
	{
		if (_PageIndex != 0 && _Template.isFirstPage())
		{
			_Reason = tr("Template is for first page only.");
		}
		else
		if (_PageIndex == 0 && !_Template.isFirstPage())
		{
			_Reason = tr("Template is not for first page.");
		}
		else
			Res = true;
	}
	else
		Res = true;
	return Res; 
}


bool STPhotoBook::isExportedAsBooklet(const STPhotoBookRenderSettings& _RSettings) const
{
	return  _RSettings.PrintPreprocessType == STPhotoBookRenderSettings::TypeBooklet || Template.preprocessType() == STPhotoBookTemplate::TypeBooklet;
}

int STPhotoBook::numRenderedPages(bool _Booklet)
{
	int Res = pages().count();
	if (Template.cutPagesOnPrint() && !_Booklet && Template.preprocessType() != STPhotoBookTemplate::TypeBooklet)
	{
		int PPageWidthWidth = Template.printPageWidth();
		if (PPageWidthWidth == 0)
			PPageWidthWidth = Template.size().width();
		int SheetsPerPage = Template.size().width() / PPageWidthWidth;
		Res = pages().count() * qMax(SheetsPerPage, 1);
	}
	return Res;
}

QFileInfoList STPhotoBook::exportImages(const QString& _ExportDir, const STPhotoBookRenderSettings& _RSettings,
	STErrorStack& _ErrorStack, SProcessStatusWidget* _StatusWidget)
{
	QFileInfoList Res;
	//bool Booklet = (_RSettings.PrintPreprocessType == STPhotoBookRenderSettings::TypeBooklet || Template.preprocessType() == STPhotoBookTemplate::TypeBooklet);
	bool Booklet = isExportedAsBooklet(_RSettings);
	bool PrintFirstAtLast = (_RSettings.PrintFirstAtLast || Template.preprocessType() == STPhotoBookTemplate::TypeBooklet || Template.printFirstPageAtLast());
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
	for (int Vfor = 0; Vfor < pages().size(); Vfor++)
	{
		try
		{
			if (Vfor == 0)
				Format = _RSettings.CoverRenderFormat; 
			else 
				 Format = _RSettings.RenderFormat; 

			if (Format == "PDF" && _RSettings.ForPrint) //No Pdf for printing.
				Format = "JPG"; 

			int PageOrder = Vfor; 
			if (( PrintFirstAtLast || Booklet ) && PageOrder == 0)
				PageOrder = pages().size(); 

			QString PageName;
			if (_RSettings.PrintPreprocessType == STPhotoBookRenderSettings::TypeNone)
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
					int	HalfPageOrder = PageOrder;
					if (CntPage > 0)
						HalfPageOrder = Vfor; 

					QString CurrImageFileName = _ExportDir + "/" + QString(PageName + "_%1").arg(
						QString::number(((HalfPageOrder) * PageImages.size()) + CntPage++), 4, '0') + "." + Format; 
					StackAssert(it->save(CurrImageFileName), Error(tr("There was problems storing the following files: %1").arg(CurrImageFileName)), _ErrorStack);
					PageFiles.push_back(CurrImageFileName); 
					Res.push_back(QFileInfo(CurrImageFileName));
				}
			}
			else 
			{
				QString CurrImageFileName = _ExportDir + "/" + QString("page_%1.pdf").arg(QString::number(PageOrder), 4, '0'); 
				renderPageToPdf(Vfor, 0, CurrImageFileName); 
				Res.push_back(QFileInfo(CurrImageFileName));
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

		QString Format = _RSettings.RenderFormat; 

		if (Format == "PDF" && _RSettings.ForPrint) //No Pdf for printing.
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
			QImage BookletPage(LPage.width() + RPage.width(), qMax(LPage.height(), RPage.height()), QImage::Format_RGB32);
			QPainter Painter(&BookletPage); 
			Painter.drawImage(QPoint(0, 0), LPage); 
			Painter.drawImage(QPoint(LPage.width(), 0), RPage); 
			Painter.end(); 

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

QImage STPhotoBook::getPageThumbnail(int _Index, const QSize& _MaxSize)
{
	QImage Res;
	if (_Index >= 0 && _Index < Pages.size())
	{
		STTemplateScene* Scene = Pages[_Index];

		QSize ThumbSize = _MaxSize;
		if (Scene->width() > Scene->height() && ThumbSize.width() < ThumbSize.height() ||
			Scene->width() < Scene->height() && ThumbSize.width() > ThumbSize.height() )
			ThumbSize.transpose();
		ThumbSize.setHeight(Scene->height() * ThumbSize.width() / Scene->width());
		QImage ResImg(ThumbSize, QImage::Format_RGB32);

		ResImg.fill(Qt::white);
		QPainter Painter(&ResImg);
		Scene->render(&Painter, ResImg.rect());
		Res = ResImg;
	}
	return Res;
}

QImage STPhotoBook::getLastPageThumbnail(const QSize& _MaxSize)
{
	return getPageThumbnail(Pages.size() - 1, _MaxSize);
}

bool STPhotoBook::isPhotoBookCorrect(QString& _ErrorMessage, bool _CheckToOrder)
{
	bool Res = true;

	if (hasEmptyPhotoItems())
	{
		_ErrorMessage = tr("The model contains empty frames. Please delete or fill it.");
		Res = false;
	}
	if (photoBookTemplate().modPages() > 0)
	{
		if (pages().size() % photoBookTemplate().modPages() != 0)
		{
			_ErrorMessage = QString(tr("The model should have the number of pages in multiple of %1")).arg(photoBookTemplate().modPages());
			Res = false;
		}
	}
	if (_CheckToOrder)
	{
		if (pages().size() < minPages())
		{
			_ErrorMessage = QString(tr("The Model must have at least %1 pages")).arg(minPages());
			Res = false;
		}
	}
	return Res;
}

void STPhotoBook::setEncryptionKey(const QString& _Key)
{
	EncryptionKey = _Key;
}

void STPhotoBook::clearEncryptionKey()
{
	EncryptionKey.clear();
}


void STPhotoBook::slotSceneSelectionChange()
{
	if (QGraphicsScene* CScene = qobject_cast<QGraphicsScene*>(sender()))
		emit sceneSelectionChanged(CScene);
}

void STPhotoBook::slotSceneDoubleClicked()
{
	if (QGraphicsScene* CScene = qobject_cast<QGraphicsScene*>(sender()))
		emit sceneDoubleClicked(CScene);
}

void STPhotoBook::slotSceneItemContextMenu(QGraphicsItem* _Item, const QPoint& _Pos)
{
	if (QGraphicsScene* CScene = qobject_cast<QGraphicsScene*>(sender()))
		emit sceneItemContextMenu(_Item, CScene, _Pos); 
}

void STPhotoBook::someSceneChanged()
{
	setHasChanges(true); 
}

