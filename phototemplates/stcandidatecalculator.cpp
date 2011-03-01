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

#include "stcandidatecalculator.h"
#include <QProgressBar>
#include <QApplication>
#include <QDebug>

#include "ddocmodel.h"
#include "dimagedoc.h"
#include "stutils.h"
#include "sttemplatescene.h"
#include "stgraphicsphotoitem.h"
#include "stgraphicspageitem.h"
#include "stphotobook.h"

int STCandidateCalculator::numLandscapeDocs(int _FromIndex, int _ToIndex)
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

int STCandidateCalculator::numLandscapeFrames(const STPhotoLayoutTemplate& _Template)
{
	int Res = 0;
	STPhotoLayoutTemplate::TFrameList TemplateFrames = _Template.frames();
	STPhotoLayoutTemplate::TFrameList::const_iterator it;
	for (it = TemplateFrames.begin(); it != TemplateFrames.end(); ++it)
	{
		if (it->width() > it->height())
			Res++;
	}
	return Res;
}

STPhotoBookTemplate::TTemplateList STCandidateCalculator::nearestByOrientation(const STPhotoBookTemplate::TTemplateList& _Templates, int _ErrMargin)
{
	STPhotoBookTemplate::TTemplateList Res;

	STPhotoBookTemplate::TTemplateList::const_iterator it = _Templates.begin();

	while (it != _Templates.end())
	{
		int NumLandscapeFrames = numLandscapeFrames(*it);
		int NumLandscapeDocs = numLandscapeDocs(InsertedPhotos, InsertedPhotos + it->numPhotoFrames() );
		if (abs(numLandscapeFrames(*it) - numLandscapeDocs(InsertedPhotos, InsertedPhotos + it->numPhotoFrames()))  <= _ErrMargin)
			Res.push_back(*it);
		++it;
	}
	return Res;
}

STCandidateCalculator::STCandidateCalculator(const STPhotoBook& _PhotoBook, STDom::DDocModel* _Model) :
		DocModel(_Model)
{
	Templates = _PhotoBook.photoBookTemplate().templates();
	setFillBackgrounds(_PhotoBook.autoFillBackgrounds());
	reset();
}

void STCandidateCalculator::reset()
{
	qSort(Templates.begin(), Templates.end()); //Sorted by number of gaps.
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
					qDebug() << "Orientation:" << Orientation;
					LandscapeDocList[Cnt] = (Orientation == ExifMetadata::Orientation_Top_Left ||
											 Orientation == ExifMetadata::Orientation_Top_Right ||
											 Orientation == ExifMetadata::Orientation_Bottom_Right ||
											 Orientation == ExifMetadata::Orientation_Bottom_Left);
				}
			}
			qDebug() << CDoc->fileInfo().fileName() << "->Landscape:" << LandscapeDocList[Cnt];
		}
		Cnt++;

	}
}


void STCandidateCalculator::configureDate(STPhotoLayoutTemplate& _Template, const QDate& _Date)
{
	STPhotoLayoutTemplate::TFrameList MonthFrames = _Template.monthFrames();
	if (MonthFrames.size() > 1)
	{
		bool Swapped;
		do
		{
			Swapped = false;
			//Sort Frames by month
			for (int Vfor = 1; Vfor < MonthFrames.size(); Vfor++)
			{
				if (MonthFrames[Vfor].month() < MonthFrames[Vfor -1].month())
				{
					STPhotoLayoutTemplate::Frame TmpFrame = MonthFrames[Vfor];
					MonthFrames[Vfor] = MonthFrames[Vfor -1];
					MonthFrames[Vfor -1] = TmpFrame;
					Swapped = true;
				}
			}
		} while (Swapped);
	}
	_Template.clearMonthFrames();
	STPhotoLayoutTemplate::TFrameList::iterator it = MonthFrames.begin();
	int Middle = MonthFrames.size() / 2;
	QDate CDate = _Date;
	CDate = CDate.addMonths(-Middle);
	while (it != MonthFrames.end())
	{
		it->setYear(CDate.year());
		it->setMonth(CDate.month());
		CDate = CDate.addMonths(1);
		_Template.addFrame(*it);
		++it;
	}
	_Template.setYear(_Date.year());
}

STPhotoLayoutTemplate STCandidateCalculator::getDateCandidate(const QDate& _Date)
{
	STPhotoLayoutTemplate Res;
	STPhotoBookTemplate::TTemplateList::const_iterator it = Templates.begin();

	//Look for a month template with 3 months.
	bool Found = false;
	while (it != Templates.end() && !Found)
	{
		Found = it->numMonthFrames() == 3;
		if (!Found)
			++it;
	}
	if (!Found)
	{
		it = Templates.begin();
		while (it != Templates.end() && !Found)
		{
			Found = it->numMonthFrames() > 0;
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

STPhotoLayoutTemplate STCandidateCalculator::getCandidate(bool _IsFirstPage, int _PagesToFill, int _AvgMargin, bool _HasFirstPages)
{
	int AvaliablePhotos = TotalPhotos - InsertedPhotos;
	STPhotoBookTemplate::TTemplateList Candidates;
	STPhotoBookTemplate::TTemplateList::const_iterator lBoundIt;
	STPhotoBookTemplate::TTemplateList::const_iterator it;
	//Search a template between per page average +- _AvgMargin
	if (_PagesToFill > 0)
	{
		int Avg = AvaliablePhotos / _PagesToFill;
		lBoundIt = qLowerBound(Templates.begin(), Templates.end(), Avg - _AvgMargin);
		STPhotoBookTemplate::TTemplateList::const_iterator UBoundIt = qUpperBound(Templates.begin(), Templates.end(), Avg + _AvgMargin);

		for (it = lBoundIt; it != UBoundIt; ++it)
		{
			if ((*it).isFirstPage() == _IsFirstPage || !_HasFirstPages)
				Candidates.push_back(*it);
		}
	}

	//If not find a template with less than _AvaliablePhotos.
	if (Candidates.isEmpty())
	{
		lBoundIt = qLowerBound(Templates.begin(), Templates.end(), AvaliablePhotos);
		//qDebug("Candidates of: %d", _AvaliablePhotos);

		for (it = Templates.begin(); it!=lBoundIt; ++it)
		{
			if ((*it).isFirstPage() == _IsFirstPage || !_HasFirstPages)
				Candidates.push_back(*it);
			//qDebug("Pushing_back %d", (*it).numPhotoFrames());
		}
	}

	STPhotoBookTemplate::TTemplateList BestCandidates;
	//Get the nearest template in portrait and landscape number
	BestCandidates = nearestByOrientation(Candidates, 0);

	if (BestCandidates.isEmpty())
		BestCandidates = nearestByOrientation(Candidates, 2);

	if (!BestCandidates.isEmpty())
		Candidates = BestCandidates;

	//Remove used templates from Candidates.
	if (!UsedTemplates.isEmpty())
	{
		STPhotoBookTemplate::TTemplateList UnusedCandidates;
		for (it = Candidates.begin(); it != Candidates.end(); ++it)
		{
			if (!UsedTemplates.contains(*it))
				UnusedCandidates.push_back(*it);
		}
		if (!UnusedCandidates.isEmpty())
			Candidates = UnusedCandidates;
	}

	STPhotoLayoutTemplate Res;
	if (Candidates.count() > 0)
	{
		if (!UsedTemplates.isEmpty() && UsedTemplates.size() < Templates.size()) //We get the template with smallest key.
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
		//Bafem el mes petit que no sigui primera pagina.
		if (!Templates.isEmpty())
		{
			it = Templates.begin();
			bool FirstPageOk = ((*it).isFirstPage() != _IsFirstPage || !_HasFirstPages);
			while(FirstPageOk && it != Templates.end())
			{
				FirstPageOk = ((*it).isFirstPage() != _IsFirstPage || !_HasFirstPages);
				++it;
			}
			if (it == Templates.end()) //No hi ha mï¿½s. Agafem el primer i milles.
				Res = *(Templates.begin());
			else
				Res = *it;
		}
	}
	return Res;

}

void STCandidateCalculator::fillPage(STTemplateScene* _Scene, const STPhotoLayoutTemplate& _Template, QProgressBar* _Progress)
{
	QList<STGraphicsPhotoItem*> AvailableItems = _Scene->photoItems();
	while (InsertedPhotos < TotalPhotos && !AvailableItems.isEmpty() )
	{
		//Look for an item with the same Orientation
		bool Found = false;
		int CntItem = 0;
		STGraphicsPhotoItem* TestedItem;
		while (!Found && CntItem < AvailableItems.count())
		{
			TestedItem = AvailableItems.at(CntItem);
			Found = ((TestedItem->rect().width() > TestedItem->rect().height() && LandscapeDocList[InsertedPhotos]) ||
					 (TestedItem->rect().width() < TestedItem->rect().height() && !LandscapeDocList[InsertedPhotos]));
			CntItem++;
		}
		STGraphicsPhotoItem* CurrPhotoW;
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
				_Scene->setBackgroundImage(CDoc->thumbnail(), CDoc->fileInfo().absoluteFilePath(), false);
				_Scene->pageItem()->setOpacity(0.4);
			}
		}


		if ((_Template.modifyAllFrames() && AvailableItems.isEmpty()) || !_Template.modifyAllFrames())
			InsertedPhotos++ ;

		if (_Progress)
		{
			_Progress->setValue(InsertedPhotos);
			QApplication::processEvents();
		}
	}
}
