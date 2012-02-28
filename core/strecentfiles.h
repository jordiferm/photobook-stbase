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

#ifndef STRECENTFILES_H
#define STRECENTFILES_H
#include <QStringList>
#include "stcoreexport.h"

class ST_CORE_EXPORT STRecentFiles
{
	static int MaxRecentFiles;

public:
	static void setMaxRecentFiles(int _Value) {  MaxRecentFiles = _Value; }
	static int maxRecentFiles() { return MaxRecentFiles; }
	static QStringList files();
	static QStringList files(int _NumFiles);
	static void addFile(const QString& _FilePath);
	static void removeFile(const QString& _FilePath);
};

#endif // STRECENTFILES_H
