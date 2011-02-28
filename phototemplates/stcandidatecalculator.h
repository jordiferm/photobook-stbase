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

#ifndef STCANDIDATECALCULATOR_H
#define STCANDIDATECALCULATOR_H
#include "stphotolayout.h"

namespace STDom
{
	class DDocModel;
}
class STTemplateScene;
class QProgressBar;
class STCandidateCalculator
{
	STPhotoBookTemplate::TTemplateList Templates;
	STPhotoBookTemplate::TTemplateList UsedTemplates;
	STDom::DDocModel* DocModel;
	int InsertedPhotos, TotalPhotos;

	QList<bool> LandscapeDocList;

	int numLandscapeDocs(int _FromIndex, int _ToIndex);
	int numLandscapeFrames(const STPhotoLayoutTemplate& _Template);
	STPhotoBookTemplate::TTemplateList nearestByOrientation(const STPhotoBookTemplate::TTemplateList& _Templates, int _ErrMargin);

public:
	STCandidateCalculator(const STPhotoBookTemplate::TTemplateList& _Templates, STDom::DDocModel* _Model);
	void reset();
	STPhotoLayoutTemplate getCandidate(bool _IsFirstPage, int _PagesToFill, int _AvgMargin, bool _HasFirstPages);
	void fillPage(STTemplateScene* _Scene, const STPhotoLayoutTemplate& _Template, QProgressBar* _Progress);
	bool photosAvailable() const { return InsertedPhotos < TotalPhotos; }
	bool templatesAvailable() const { return Templates.count() > 0 ; }
	int totalPhotos() const { return TotalPhotos; }
	int calcMargin(int _ItemAverage) const { return qMin(_ItemAverage, (TotalPhotos - InsertedPhotos) / _ItemAverage); }
};

#endif // STCANDIDATECALCULATOR_H
