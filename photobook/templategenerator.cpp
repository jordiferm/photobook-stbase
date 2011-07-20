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

#include "templategenerator.h"
#include <QRectF>
#include <QDebug>
#include "graphicsphotoitem.h"
#include "templatescene.h"
#include "graphicspageitem.h"


using namespace SPhotoBook;


bool TemplateGenerator::niceTemplate(TemplateScene* _Template)
{
	bool NiceTemplate = true;

	//Check if all frames are within desired aspect ratio.
	QList<GraphicsPhotoItem *> PhotoItems = _Template->photoItems();
	QList<GraphicsPhotoItem *>::const_iterator it = PhotoItems.begin();
	while(NiceTemplate && it != PhotoItems.end())
	{
		double AspectRatio;
		QRectF BRect = (*it)->boundingRect();
		if (BRect.size().width() > BRect.size().height())
			AspectRatio = BRect.size().height() / BRect.size().width();
		else
			AspectRatio = BRect.size().width() / BRect.size().height();
		NiceTemplate = (AspectRatio > MinAspectRatio);

		NiceTemplate = NiceTemplate && BRect.size().width() > MinFrameSize.width() && BRect.size().height() > MinFrameSize.height();
		++it;
	}
	return NiceTemplate;
}

void TemplateGenerator::addCombinations(TemplateScene* _Template, PageList& _TemplateList, QObject* _Parent )
{
	//For each frame
	QList<GraphicsPhotoItem *> PhotoItems = _Template->photoItems();
	for (int Vfor = 0; Vfor < PhotoItems.size(); Vfor++)
	{
		//Split X
		TemplateScene* NewTemplate = new TemplateScene(_Parent);
		NewTemplate->copy(_Template);
		NewTemplate->splitXFrame(Vfor);
		//NewTemplate->shrinkFramesBy(3);
		if (niceTemplate(NewTemplate) && !containsTemplate(_TemplateList, NewTemplate))
			_TemplateList.push_back(NewTemplate);
		if (NewTemplate->photoItems().size() < AllCombinationThreshold)
			addCombinations(NewTemplate, _TemplateList, _Parent);
	}

	for (int Vfor = 0; Vfor < PhotoItems.size(); Vfor++)
	{
		//Split Y
		TemplateScene* NewTemplate = new TemplateScene(_Parent);
		NewTemplate->copy(_Template);
		NewTemplate->splitYFrame(Vfor);
		//NewTemplate->shrinkFramesBy(3);
		if (niceTemplate(NewTemplate) && !containsTemplate(_TemplateList, NewTemplate))
			_TemplateList.push_back(NewTemplate);
		if (NewTemplate->photoItems().size() < AllCombinationThreshold)
			addCombinations(NewTemplate, _TemplateList, _Parent);
	}
}


bool TemplateGenerator::containsTemplate(const PageList& _Templates, TemplateScene* _Template )
{
	bool Res = false;
	//For each frame
	PageList::const_iterator it = _Templates.begin();
	while (!Res && it != _Templates.end())
	{
		Res = (*it)->hasSameFrames(_Template);
		++it;
	}
	return Res;
}

// Adds to _Templates new _NumTemplates templates with numPhotoframes +1 for _Frames with numPhotoframes = _Depth by splitting its biggest photoFrame.
void TemplateGenerator::incTemplatesImageDepth(int _Depth, int _NumTemplates, PageList& _Templates)
{
	int TemplatesToAdd = _NumTemplates;

	_Templates.sort();
	PageList::iterator it = _Templates.end();
	PageList NewTemplates;
	it --;
	while (TemplatesToAdd > 0 && it != _Templates.begin())
	{
		if ((*it)->numPhotoItems() == _Depth -1)
		{
			TemplateScene* NewTemplate = new TemplateScene((*it)->parent());
			NewTemplate->copy(*it);
			int BiggestFrameIndex = NewTemplate->biggestFrameIndex();
			if (BiggestFrameIndex > -1)
			{
				GraphicsPhotoItem* BigFrame = NewTemplate->photoItems()[BiggestFrameIndex];
				QRectF FrameRect = BigFrame->boundingRect();
				if (FrameRect.width() < FrameRect.height())
					NewTemplate->splitYFrame(BiggestFrameIndex);
				else
					NewTemplate->splitXFrame(BiggestFrameIndex);
				if (!containsTemplate(_Templates, NewTemplate))
				{
					NewTemplates.push_back(NewTemplate);
					TemplatesToAdd--;
				}
			}
		}
		--it;
	}
	_Templates += NewTemplates;
}


TemplateGenerator::TemplateGenerator(const QSizeF& _TemplateSize )
	: TemplateSize(_TemplateSize)
{
	MaxImagesPerPage = 10;
	AllCombinationThreshold = 4;//Combination tree depth
	MinFrameSize = QSize(50, 50);
	MinAspectRatio = 0.4;
	MaxNumImagesForTextFrame = 4;
}

void TemplateGenerator::addTextFrames(PageList& _Templates)
{
	_Templates.sort();
	PageList::iterator it = _Templates.begin();
	PageList NewTemplates;
	bool Finished = false;
	while (!Finished && it != _Templates.end())
	{
		Finished = (*it)->numPhotoItems() > MaxNumImagesForTextFrame;
		if (!Finished)
		{
			//Get the biggest Frame
			TemplateScene* NewTemplate = new TemplateScene((*it)->parent());
			NewTemplate->copy(*it);
			int BiggestFrameIndex = NewTemplate->biggestFrameIndex();
			if (BiggestFrameIndex > -1)
			{
				//Add a text Frame
				NewTemplate->addSubTittleTextFrame(BiggestFrameIndex);
				NewTemplates.push_back(NewTemplate);
			}
		}
		++it;
	}
	_Templates += NewTemplates;
}

TemplateScene* TemplateGenerator::createBaseTemplate(QObject* _Parent)
{
	TemplateScene* OneFrameTemplate = new TemplateScene(TemplateSize, _Parent);
	GraphicsPhotoItem* FrameItem = new GraphicsPhotoItem();
	FrameItem->setRect(QRectF(QPointF(0, 0), TemplateSize));
	OneFrameTemplate->addPhotoItem(FrameItem);
	return OneFrameTemplate;
}

PageList TemplateGenerator::generate(QObject* _Parent, double _Margin)
{
	PageList Res;

	//Add template with 1 frame
	TemplateScene* OneFrameTemplate = createBaseTemplate(_Parent);
	Res.push_back(OneFrameTemplate);

	addCombinations(OneFrameTemplate, Res, _Parent);

	//! TODO Revise this !
	int NumManyImagesTemplates = 5;
	//Add Templates with high number of images.
	for (int Vfor = AllCombinationThreshold + 1; Vfor <= MaxImagesPerPage; Vfor++)
	{
		incTemplatesImageDepth(Vfor, NumManyImagesTemplates, Res);
	}

	//Add Margins
	if (_Margin >= 0)
	{
		PageList::iterator it;
		for (it = Res.begin(); it != Res.end(); ++it)
		{
			(*it)->shrinkFramesBy(_Margin);
		}
	}

	Res.sort();
	return Res;
}



