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

#ifndef TEMPLATEGENERATOR_H
#define TEMPLATEGENERATOR_H

#include <QSizeF>
#include "pagelist.h"

class QObject;
namespace SPhotoBook
{
class TemplateScene;
class TemplateGenerator
{
	int MaxImagesPerPage;
	int MaxNumImagesForTextFrame; //Max Number of images for a template with Text Frames.
	int AllCombinationThreshold;
	QSizeF TemplateSize;
	QSizeF MinFrameSize;
	double MinAspectRatio;

	//STPhotoLayoutTemplate::Frame fitFrameToRect(const STPhotoLayoutTemplate::Frame& _Frame, const QRectF& _Rect, const QSizeF& _TemplateSize);
	//STPhotoBookTemplate::TTemplateList getFrames(const QRectF& _Rect, const STPhotoBookTemplate::TTemplateList& _Templates, int _NumFrames );
	//! \return true if template is nice :)
	bool niceTemplate(TemplateScene* _Template);
	void addCombinations(TemplateScene* _Template, PageList& _TemplateList, QObject* _Parent);
	void incTemplatesImageDepth(int _Depth, int _NumTemplates, PageList& _Templates);
	bool containsTemplate(const PageList& _Templates, TemplateScene* _Template );


public:
	TemplateGenerator(const QSizeF& _TemplateSize);

	void addTextFrames(PageList& _Templates);
	void setMinFrameSize(const QSizeF& _Value) { MinFrameSize = _Value; }
	QSizeF minFrameSize() const { return MinFrameSize; }
	void setMaxImagesPerPage(int _Value) { MaxImagesPerPage = _Value; }
	int maxImagesPerPage() const { return MaxImagesPerPage; }
	void setMinAspectRatio(double _Value) { MinAspectRatio = _Value; }
	double minAspectRatio() const { return MinAspectRatio; }

	TemplateScene* createBaseTemplate(QObject* _Parent);
	PageList generate(QObject* _Parent, double _Margin = 3);
};
}

#endif // TemplateGenerator_H
