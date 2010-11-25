/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Starblitz reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "sttemporaryimagefile.h"
#include <QTemporaryFile>
#include <QDir> 


QString STTemporaryImageFile::getTempFileName() const
{
	QString Res = QDir::tempPath() + "/" + TemplateName;
	QTemporaryFile TmpFile(QDir::tempPath() + "/" + TemplateName); 
	if (TmpFile.open())
		Res = TmpFile.fileName();
	return Res; 
//  		Assert(TmpFile.open(), Error("Unable to create temporary files.")); 
//  		QString TmpFileName = TmpFile.fileName(); //Close file clears the filename of TmpFile.
//  		TmpFile.close(); 
//  		Assert(QFile::copy(CurrImageFile, TmpFileName), Error(QString(tr("Error copying from file %1 to file 
}

STTemporaryImageFile::STTemporaryImageFile(const QString& _SourceImageFile, const QString& _TemplateName)
 : QFile(), SourceImageFile(_SourceImageFile), TemplateName(_TemplateName), AutoRemove(true)
{
}

STTemporaryImageFile::~STTemporaryImageFile()
{
	if (AutoRemove)
		QFile::remove(); 
}

QString STTemporaryImageFile::fileName() const
{
	QString TmpFileName = getTempFileName();
	TmpFileName =	TmpFileName.replace(".", "_"); 
	QFileInfo SrcImageFInfo(SourceImageFile); 
	TmpFileName += "." + SrcImageFInfo.completeSuffix();
	return TmpFileName; 
}

bool STTemporaryImageFile::create()
{
	QString MyName = fileName(); 
	setName(MyName);
	return QFile::copy(SourceImageFile, MyName);
}

