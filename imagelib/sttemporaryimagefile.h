/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef STTEMPORARYIMAGEFILE_H
#define STTEMPORARYIMAGEFILE_H

#include <QFile>
#include "stimagelibexport.h"

/**
	@author
*/
class ST_IMAGELIB_EXPORT STTemporaryImageFile : public QFile
{
	QString SourceImageFile; 
	QString TemplateName; 
	bool AutoRemove; 
	
	QString getTempFileName() const;
	
public:
	STTemporaryImageFile(const QString& _SourceImageFile, const QString& _TemplateName = "st_temp");
	~STTemporaryImageFile();
	QString fileName() const; 
	//! Copies _SourceImageFile to a temp filename. \returns false if we could not create it.
	bool create(); 
	void setAutoRemove(bool _Value) { AutoRemove = _Value; }

};

#endif
