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

#ifndef STPHOTOLAYOUTTEMPLATEGENERATOR_H
#define STPHOTOLAYOUTTEMPLATEGENERATOR_H

#include <QSizeF>
#include "stphotolayout.h"

class STPhotoLayoutTemplateGenerator
{
	bool UseTextFrames;
	int MaxImagesPerPage;
	int MaxNumImagesForTextFrame; //Max Number of images for a template with Text Frames.
	int AllCombinationThreshold;
	QSizeF TemplateSize;
	QSizeF MinFrameSize;
	QSizeF CoverSize;
	double MinAspectRatio;

	//STPhotoLayoutTemplate::Frame fitFrameToRect(const STPhotoLayoutTemplate::Frame& _Frame, const QRectF& _Rect, const QSizeF& _TemplateSize);
	//STPhotoBookTemplate::TTemplateList getFrames(const QRectF& _Rect, const STPhotoBookTemplate::TTemplateList& _Templates, int _NumFrames );
	//! \return true if template is nice :)
	bool niceTemplate(const STPhotoLayoutTemplate& _Template);
	void addCombinations(const STPhotoLayoutTemplate& _Template, STPhotoBookTemplate::TTemplateList& _TemplateList );
	void incTemplatesImageDepth(int _Depth, int _NumTemplates, STPhotoBookTemplate::TTemplateList& _Templates);
	bool containsTemplate(const STPhotoBookTemplate::TTemplateList& _Templates, STPhotoLayoutTemplate _Template );
	void addTextFrames(STPhotoBookTemplate::TTemplateList& _Templates);


public:
	STPhotoLayoutTemplateGenerator(const QSizeF& _TemplateSize, bool _UseTextFrames = true );

	void setUseTextFrames(bool _Value) { UseTextFrames = _Value; }
	bool useTextFrames() const { return UseTextFrames; }
	void setCoverSize(const QSizeF& _Size) { CoverSize = _Size; }
	QSizeF coverSize() const { return CoverSize; }
	void setMinFrameSize(const QSizeF& _Value) { MinFrameSize = _Value; }
	QSizeF minFrameSize() const { return MinFrameSize; }
	void setMaxImagesPerPage(int _Value) { MaxImagesPerPage = _Value; }
	int maxImagesPerPage() const { return MaxImagesPerPage; }
	void setMinAspectRatio(double _Value) { MinAspectRatio = _Value; }
	double minAspectRatio() const { return MinAspectRatio; }

	STPhotoBookTemplate::TTemplateList generatePhotoBookTemplates();


};

#endif // STPHOTOLAYOUTTEMPLATEGENERATOR_H
