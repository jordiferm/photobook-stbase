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

#include "stphotolayouttemplategenerator.h"

bool STPhotoLayoutTemplateGenerator::niceTemplate(const STPhotoLayoutTemplate& _Template)
{
	bool NiceTemplate = true;

	//Check if all frames are within desired aspect ratio.
	STPhotoLayoutTemplate::TFrameList Frames = _Template.frames();
	STPhotoLayoutTemplate::TFrameList::const_iterator it = Frames.begin();
	while(NiceTemplate && it != Frames.end())
	{
		double AspectRatio;
		if (it->size().width() > it->size().height())
			AspectRatio = it->size().height() / it->size().width();
		else
			AspectRatio = it->size().width() / it->size().height();
		NiceTemplate = (AspectRatio > MinAspectRatio);

		NiceTemplate = NiceTemplate && it->size().width() > MinFrameSize.width() && it->size().height() > MinFrameSize.height();
		++it;
	}
	return NiceTemplate;
}

void STPhotoLayoutTemplateGenerator::addCombinations(const STPhotoLayoutTemplate& _Template, STPhotoBookTemplate::TTemplateList& _TemplateList )
{
	//For each frame
	STPhotoLayoutTemplate::TFrameList Frames = _Template.frames();

	for (int Vfor = 0; Vfor < Frames.size(); Vfor++)
	{
		//Split X
		STPhotoLayoutTemplate NewTemplate = _Template;
		NewTemplate.splitXFrame(Vfor);
		if (niceTemplate(NewTemplate))
			_TemplateList.push_back(NewTemplate);
		if (NewTemplate.numPhotoFrames() < AllCombinationThreshold)
			addCombinations(NewTemplate, _TemplateList);
	}

	for (int Vfor = 0; Vfor < Frames.size(); Vfor++)
	{
		//Split Y
		STPhotoLayoutTemplate NewTemplate = _Template;
		NewTemplate.splitYFrame(Vfor);
		if (niceTemplate(NewTemplate))
			_TemplateList.push_back(NewTemplate);
		if (NewTemplate.numPhotoFrames() < AllCombinationThreshold)
			addCombinations(NewTemplate, _TemplateList);
	}
}


bool STPhotoLayoutTemplateGenerator::containsTemplate(const STPhotoBookTemplate::TTemplateList& _Templates, STPhotoLayoutTemplate _Template )
{
	bool Res = false;
	STPhotoBookTemplate::TTemplateList::const_iterator it = _Templates.begin();
	while (!Res && it != _Templates.end())
	{
		Res = it->hasSameFrames(_Template);
		++it;
	}
	return Res;
}

// Adds to _Templates new templates with numPhotoFrames +1 for _Frames with numPhotoframes = _Depth by splitting its biggest photoFrame.
void STPhotoLayoutTemplateGenerator::incTemplatesImageDepth(int _Depth, int _NumTemplates, STPhotoBookTemplate::TTemplateList& _Templates)
{
	int TemplatesToAdd = _NumTemplates;

	qSort(_Templates.begin(), _Templates.end());
	STPhotoBookTemplate::TTemplateList::iterator it = _Templates.end();
	it --;
	while (TemplatesToAdd > 0 && it != _Templates.begin())
	{
		if (it->numPhotoFrames() == _Depth -1)
		{
			STPhotoLayoutTemplate NewTemplate = *it;
			int BiggestFrameIndex = NewTemplate.biggestFrameIndex();
			if (BiggestFrameIndex > -1)
			{
				STPhotoLayoutTemplate::Frame BigFrame = NewTemplate.frames()[BiggestFrameIndex];
				if (BigFrame.width() < BigFrame.height())
					NewTemplate.splitYFrame(BiggestFrameIndex);
				else
					NewTemplate.splitXFrame(BiggestFrameIndex);
				if (!containsTemplate(_Templates, NewTemplate))
				{
					_Templates.push_back(NewTemplate);
					TemplatesToAdd--;
				}
			}
		}
		--it;
	}
}

void STPhotoLayoutTemplateGenerator::addTextFrames(STPhotoBookTemplate::TTemplateList& _Templates)
{
	qSort(_Templates.begin(), _Templates.end());
	STPhotoBookTemplate::TTemplateList::iterator it = _Templates.begin();
	bool Finished = false;
	while (!Finished && it != _Templates.end())
	{
		Finished = it->numPhotoFrames() > MaxNumImagesForTextFrame;
		if (!Finished)
		{
			//Get the biggest Frame
			STPhotoLayoutTemplate NewTemplate = *it;
			int BiggestFrameIndex = NewTemplate.biggestFrameIndex();
			if (BiggestFrameIndex > -1)
			{
				//Add a text Frame
				NewTemplate.addSubTittleTextFrame(BiggestFrameIndex);
				_Templates.push_back(NewTemplate);
			}
		}
		++it;
	}
}

STPhotoLayoutTemplateGenerator::STPhotoLayoutTemplateGenerator(const QSizeF& _TemplateSize, bool _UseTextFrames )
	: TemplateSize(_TemplateSize), UseTextFrames(_UseTextFrames)
{
	MaxImagesPerPage = 10;
	AllCombinationThreshold = 6;//Combination tree depth
	MinFrameSize = QSize(50, 50);
	MinAspectRatio = 0.4;
	MaxNumImagesForTextFrame = 4;
}


STPhotoBookTemplate::TTemplateList STPhotoLayoutTemplateGenerator::generatePhotoBookTemplates()
{
	STPhotoBookTemplate::TTemplateList Res;

	//Add template with 1 frame
	STPhotoLayoutTemplate OneFrameTemplate;
	OneFrameTemplate.setSize(TemplateSize);
	OneFrameTemplate.addFrame(STPhotoLayoutTemplate::Frame(QRectF(QPointF(0, 0), TemplateSize)));
	Res.push_back(OneFrameTemplate);

	addCombinations(OneFrameTemplate, Res);

	int NumManyImagesTemplates = 5;
	//Add Templates with high number of images.
	for (int Vfor = AllCombinationThreshold + 1; Vfor <= MaxImagesPerPage; Vfor++)
	{
		incTemplatesImageDepth(Vfor, NumManyImagesTemplates, Res);
	}

	//Add Texts
	if (UseTextFrames)
	{
		addTextFrames(Res);
	}

	//Generate keys for each template
	STPhotoBookTemplate::TTemplateList::iterator it;
	for (it = Res.begin(); it != Res.end(); ++it)
	{
		it->generateKey();
	}

	//qSort(Res.begin(), Res.end());
	return Res;
}



