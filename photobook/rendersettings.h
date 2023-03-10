/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
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
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H
#include "stphotobookexport.h"
#include <QString>


namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT RenderSettings
{
public:

	enum EnPrintPreProcessType
	{
		TypeNone = 0,
		TypeHBooklet = 1,
		TypeVBooklet = 2,
		TypeMultiply = 3
	};

private:
	QString RenderFormat;
	QString CoverRenderFormat;
	bool ForPrint; //This is not to render into pdf.
	bool PrintFirstAtLast; //Print the first page at last.
	EnPrintPreProcessType PrintPreprocessType;

public:
	RenderSettings(const QString& _RenderFormat = "JPG");

	void setRenderFormat(const QString& _Value) { RenderFormat = _Value; }
	QString renderFormat() const { return RenderFormat; }

	void setCoverRenderFormat(const QString& _Value) { CoverRenderFormat = _Value; }
	QString coverRenderFormat() const { return CoverRenderFormat; }

	void setForPrint(bool _Value) { ForPrint = _Value; }
	bool forPrint() const { return ForPrint; }

};

}

#endif // RENDERSETTINGS_H
