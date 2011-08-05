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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "candidatecalculator.h"
#include <QProgressBar>
#include <QApplication>
#include <QDebug>

#include "ddocmodel.h"
#include "dimagedoc.h"
#include "stutils.h"
#include "templatescene.h"
#include "graphicsphotoitem.h"
#include "graphicsmonthitem.h"
#include "graphicspageitem.h"
#include "document.h"

using namespace SPhotoBook;

int CandidateCalculator::numLandscapeDocs(int _FromIndex, int _ToIndex)
{
	int Res = 0;
	int Cnt = _FromIndex;
	while (Cnt < LandscapeDocList.count() && Cnt < _ToIndex)
	{
		if (LandscapeDocList[Cnt++])
			Res++;
	}
	return Res;
}

PageList CandidateCalculator::nearestByOrientation(const PageList& _Templates, int _ErrMargin)
{
	PageList Res;

	PageList::const_iterator it = _Templates.begin();

	while (it != _Templates.end())
	{
		int NumLandscapeFrames = (*it)->numLandscapeFrames();
		int NumLandscapeDocs = numLandscapeDocs(InsertedPhotos, InsertedPhotos + (*it)->numPhotoItems() );
		if (abs(NumLandscapeFrames - NumLandscapeDocs)  <= _ErrMargin)
			Res.push_back(*it);
		++it;
	}
	return Res;
}

CandidateCalculator::CandidateCalculator(const Document& _PhotoBook, STDom::DDocModel* _Model) :
		DocModel(_Model)
{
	setFillBackgrounds(_PhotoBook.buildOptions().autoFillBackgrounds());
	reset();
}

void CandidateCalculator::reset()
{
	STUtils::randomize();
	InsertedPhotos = 0;
	UsedTemplates.clear();
	TotalPhotos = DocModel->rowCount();
	//Lets calc portrait and landscape docs
	LandscapeDocList.clear();
	int Cnt = 0;
	while (Cnt < DocModel->rowCount())
	{
		LandscapeDocList.push_back(true);
		STDom::DDoc* CDoc = DocModel->doc(DocModel->index(Cnt, 0));
		if (CDoc) // Defensive
		{
			if (CDoc->type() == STDom::DImageDoc::Type)
			{
				STDom::DImageDoc* CImage = static_cast<STDom::DImageDoc*>(CDoc);
				STDom::DImageDoc::CInfo ImageInfo = CImage->getInfo();
				if (!ImageInfo.isNull())
				{
					ExifMetadata::EnOrientation Orientation = ImageInfo.orientation();
					if (ImageInfo.size().isNull())
						LandscapeDocList[Cnt] = (Orientation == ExifMetadata::Orientation_Top_Left ||
												 Orientation == ExifMetadata::Orientation_Top_Right ||
												 Orientation == ExifMetadata::Orientation_Bottom_Right ||
												 Orientation == ExifMetadata::Orientation_Bottom_Left);
					else
						LandscapeDocList[Cnt] = ImageInfo.size().width() >  ImageInfo.size().height();

					//qDebug() << CImage->fileInfo().fileName() << "->Orientation:" << Orientation << "Size " << ImageInfo.size().width() << "x"<< ImageInfo.size().height();
				}
			}
			//qDebug() << CDoc->fileInfo().fileName() << "->Landscape:" << LandscapeDocList[Cnt];
		}
		Cnt++;

	}
}


void CandidateCalculator::configureDate(TemplateScene* _Template, const QDate& _Date)
{
	QList<GraphicsMonthItem*> MonthFrames = _Template->monthItems();
	if (MonthFrames.size() > 1)
	{
		bool Swapped;
		do
		{
			Swapped = false;
			//Sort Frames by month
			for (int Vfor = 1; Vfor < MonthFrames.size(); Vfor++)
			{
				if (MonthFrames[Vfor]->month() < MonthFrames[Vfor -1]->month())
				{
					GraphicsMonthItem* TmpFrame = MonthFrames[Vfor];
					MonthFrames[Vfor] = MonthFrames[Vfor -1];
					MonthFrames[Vfor -1] = TmpFrame;
					Swapped = true;
				}
			}
		} while (Swapped);
	}

	QList<GraphicsMonthItem*>::iterator it = MonthFrames.begin();
	int Middle = MonthFrames.size() / 2;
	QDate CDate = _Date;
	CDate = CDate.addMonths(-Middle);
	while (it != MonthFrames.end())
	{
		(*it)->setMonth(CDate.month(), CDate.year());
		CDate = CDate.addMonths(1);
		++it;
	}
}

TemplateScene* CandidateCalculator::getDateCandidate(const PageList& _Templates, const QDate& _Date)
{

	PageList Templates = _Templates;
	qSort(Templates.begin(), Templates.end()); //Sorted by number of gaps.

	TemplateScene* Res;
	PageList::const_iterator it = Templates.begin();

	//Look for a month template with 3 months.
	bool Found = false;
	while (it != Templates.end() && !Found)
	{
		Found = (*it)->numMonthItems() == 3;
		if (!Found)
			++it;
	}
	if (!Found)
	{
		it = Templates.begin();
		while (it != Templates.end() && !Found)
		{
			Found = (*it)->numMonthItems() > 0;
			if (!Found)
				++it;
		}
	}

	if (Found)
	{
		Res = *it;
		configureDate(Res, _Date);
	}

	return Res;

}


PageList::const_iterator CandidateCalculator::lowerBound(PageList::const_iterator begin, PageList::const_iterator end, int _Value)
{
	// Implementation is duplicated from QAlgorithmsPrivate to keep existing code
	// compiling. We have to allow using *begin and value with different types,
	// and then implementing operator< for those types.
	PageList::const_iterator middle;
	int n = end - begin;
	int half;

	while (n > 0) {
		half = n >> 1;
		middle = begin + half;
		if (*(*middle) < _Value) {
			begin = middle + 1;
			n -= half + 1;
		} else {
			n = half;
		}
	}
	return begin;
}

PageList::const_iterator CandidateCalculator::upperBound(PageList::const_iterator begin, PageList::const_iterator  end, int _Value)
{
	// Implementation is duplicated from QAlgorithmsPrivate.
	PageList::const_iterator middle;
	int n = end - begin;
	int half;

	while (n > 0) {
		half = n >> 1;
		middle = begin + half;
		if (_Value < *(*middle)) {
			n = half;
		} else {
			begin = middle + 1;
			n -= half + 1;
		}
	}
	return begin;
}


PageList CandidateCalculator::unusedTemplates(const PageList& _Templates)
{
	PageList Res;
	//Remove used templates from Candidates.
	if (!UsedTemplates.isEmpty())
	{
		PageList UnusedCandidates;
		PageList::const_iterator it;
		for (it = _Templates.begin(); it != _Templates.end(); ++it)
		{
			if (!UsedTemplates.containsTemplate(*it))
			{
				UnusedCandidates.push_back(*it);
			}
		}
		if (!UnusedCandidates.isEmpty())
			Res = UnusedCandidates;
	}
	else
		Res = _Templates;
	return Res;
}

TemplateScene* CandidateCalculator::randomUnused(const PageList& _Templates)
{
	PageList Candidates = unusedTemplates(_Templates);
	TemplateScene* Res = 0;
	if (Candidates.count() > 0)
	{
		Res = Candidates[STUtils::random(0, Candidates.count())];
	}
	return Res;
}

PageList CandidateCalculator::candidatesByNumImages(const PageList& _Templates, int _ImagesPerPage, int _AvgMargin)
{
	PageList Candidates;
	PageList::const_iterator lBoundIt;
	PageList::const_iterator it;
	//Search a template between per page average +- _AvgMargin
	//qDebug() << "AvaliablePhotos" << AvaliablePhotos << " _PagesToFill:" << _PagesToFill << " Avg:" << Avg << "_AvgMargin" << _AvgMargin;
	lBoundIt = lowerBound(_Templates.begin(), _Templates.end(), _ImagesPerPage - _AvgMargin);
	PageList::const_iterator UBoundIt = upperBound(_Templates.begin(), _Templates.end(), _ImagesPerPage + _AvgMargin + 1);

	for (it = lBoundIt; it != UBoundIt; ++it)
	{
		Candidates.push_back(*it);
	}

	return Candidates;
}

/*!
  Candidates without seeing images.
*/
TemplateScene* CandidateCalculator::getEmptyCandidate(const PageList& _Templates, int _ImagesPerPage, int _AvgMargin)
{
	TemplateScene* Res = 0;
	PageList Candidates = candidatesByNumImages(_Templates, _ImagesPerPage, _AvgMargin);

	if (Candidates.isEmpty())
		Candidates = _Templates;

	Res = randomUnused(Candidates);
	if (!Res)
	{ // Noi, no hi ha candidats => Poden sobrar frames.
		//Bafem el mes petit que no sigui primera pagina.
		if (!_Templates.isEmpty())
			Res = *(_Templates.begin());
	}
	return Res;
}

TemplateScene* CandidateCalculator::getCandidate(const PageList& _Templates, int _PagesToFill, int _AvgMargin)
{
	int AvaliablePhotos = TotalPhotos - InsertedPhotos;
	PageList Candidates;
	if (_PagesToFill > 0)
		Candidates = candidatesByNumImages(_Templates, AvaliablePhotos / _PagesToFill, _AvgMargin);

	//If not, look for a template with less than _AvaliablePhotos.
	if (Candidates.isEmpty())
	{
		PageList::const_iterator lBoundIt;
		PageList::const_iterator it;
		lBoundIt = lowerBound(_Templates.begin(), _Templates.end(), AvaliablePhotos);
		//qDebug("Candidates of: %d", _AvaliablePhotos);

		for (it = _Templates.begin(); it!=lBoundIt; ++it)
		{
			Candidates.push_back(*it);
			//qDebug("Pushing_back %d", (*it).numPhotoFrames());
		}
	}

	PageList BestCandidates;
	//Get the nearest template in portrait and landscape number
	BestCandidates = nearestByOrientation(Candidates, 0);

	if (BestCandidates.isEmpty())
		BestCandidates = nearestByOrientation(Candidates, 2);

	if (!BestCandidates.isEmpty())
		Candidates = BestCandidates;

	TemplateScene* Res = randomUnused(Candidates);
	if (!Res)
	{ // Noi, no hi ha candidats => Poden sobrar frames.
		//Bafem el mes petit que no sigui primera pagina.
		if (!_Templates.isEmpty())
			Res = *(_Templates.begin());
	}
	return Res;
}

void CandidateCalculator::fillPage(TemplateScene* _Scene, bool _ModifyAllFrames, QProgressBar* _Progress)
{
	QList<GraphicsPhotoItem*> AvailableItems = _Scene->photoItems();
	while (InsertedPhotos < TotalPhotos && !AvailableItems.isEmpty() )
	{
		//Look for an item with the same Orientation
		bool Found = false;
		int CntItem = 0;
		GraphicsPhotoItem* TestedItem;
		while (!Found && CntItem < AvailableItems.count())
		{
			TestedItem = AvailableItems.at(CntItem);
			Found = ((TestedItem->rect().width() > TestedItem->rect().height() && LandscapeDocList[InsertedPhotos]) ||
					 (TestedItem->rect().width() < TestedItem->rect().height() && !LandscapeDocList[InsertedPhotos]));
			CntItem++;
		}
		GraphicsPhotoItem* CurrPhotoW;
		if (Found)
			CurrPhotoW = TestedItem;
		else
			CurrPhotoW = AvailableItems.first();

		AvailableItems.removeAt(AvailableItems.indexOf(CurrPhotoW));

		QModelIndex CIndex = DocModel->index(InsertedPhotos, 0);
		STDom::DDoc* CDoc = DocModel->doc(CIndex);
		CurrPhotoW->setDoc(CDoc);


		if (FillBackgrounds)
		{
			if (!_Scene->hasBackgroundImage())
			{
				_Scene->setBackgroundImage(CDoc->thumbnail().toImage(), CDoc->fileInfo().absoluteFilePath(), false);
				_Scene->setBgBrush(QBrush(Qt::white, Qt::SolidPattern));
				_Scene->pageItem()->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
				_Scene->pageItem()->setOpacity(0.3);
			}
		}


		if ((_ModifyAllFrames && AvailableItems.isEmpty()) || !_ModifyAllFrames)
			InsertedPhotos++ ;

		if (_Progress)
		{
			_Progress->setValue(InsertedPhotos);
			QApplication::processEvents();
		}
	}
}

void CandidateCalculator::markAsUsed(TemplateScene* _Template)
{
	UsedTemplates.push_back(_Template);
}
