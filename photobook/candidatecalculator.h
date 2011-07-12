/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
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

#ifndef CANDIDATECALCULATOR_H
#define CANDIDATECALCULATOR_H
#include "pagelist.h"

namespace STDom
{
	class DDocModel;
}
class QProgressBar;

namespace STPhotoBook
{
class Document;
class TemplateScene;
class CandidateCalculator
{
	PageList UsedTemplates;
	STDom::DDocModel* DocModel;
	int InsertedPhotos, TotalPhotos;
	bool FillBackgrounds;

	QList<bool> LandscapeDocList;

	int numLandscapeDocs(int _FromIndex, int _ToIndex);
	PageList nearestByOrientation(const PageList& _Templates, int _ErrMargin);

public:
	CandidateCalculator(const Document& _PhotoBook, STDom::DDocModel* _Model);
	void reset();
	void configureDate(TemplateScene* _Template, const QDate& _Date);
	TemplateScene* getDateCandidate(const PageList& _Templates, const QDate& _Date);
	PageList::const_iterator lowerBound(PageList::const_iterator begin, PageList::const_iterator end, int _Value);
	PageList::const_iterator upperBound(PageList::const_iterator begin, PageList::const_iterator  end, int _Value);
	TemplateScene* getCandidate(const PageList& _Templates, int _PagesToFill, int _AvgMargin);
	void fillPage(TemplateScene* _Scene, bool _ModifyAllFrames, QProgressBar* _Progress);
	bool photosAvailable() const { return InsertedPhotos < TotalPhotos; }
	int totalPhotos() const { return TotalPhotos; }
	int calcMargin(int _ItemAverage, int _PagesToFill) const { return qMin(_ItemAverage, _PagesToFill -1); }
	void setFillBackgrounds(bool _Value) { FillBackgrounds = _Value; }
	void markAsUsed(TemplateScene* _Template);
};
}
#endif // CandidateCalculator_H
