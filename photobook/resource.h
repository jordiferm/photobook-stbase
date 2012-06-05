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

#ifndef RESOURCE_H
#define RESOURCE_H
#include "stphotobookexport.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include "sterror.h"

namespace SPhotoBook
{
class ST_PHOTOBOOK_EXPORT Resource
{
public:
	ST_DECLARE_ERRORCLASS();
	enum EnResourceType
	{
		TypeBackground = 0,
		TypeClipart,
		TypeFrame,
		TypeFrameMask,
		TypeFont,
		TypeMask,
		TypeImage
	};

private:
	QString Name;
	QString HashCode;
	EnResourceType Type;
	QDir Dir;
	QFileInfo FInfo;

	void init(const QFileInfo& _FileInfo, EnResourceType _Type );

public:
	Resource(const QFileInfo& _FileInfo);
	Resource(const QFileInfo& _FileInfo, EnResourceType _Type );
	Resource(const QDir& _Dir = QDir(), const QString& _Name = "", EnResourceType _Type = TypeBackground);
	void setName(const QString& _Name) { Name = _Name; }
	QString name() const { return Name; }
	void setType(EnResourceType _Type)  { Type = _Type; }
	EnResourceType type() const { return Type; }
	QDir dir() const { return Dir; }
	bool isEncrypted() const { return false; }

	//! Returns the files saved
	QStringList save(const QFileInfo& _DestFileInfo);
	QStringList save(const QDir& _StoreDir);

	QFileInfo fileInfo() const;

	bool isNull() const { return Name.isEmpty(); }

	//Hash codes for comparsion operators
	void calcHashCode();
	QString getHashCode() const;
	QString hashCode() const;

	bool operator==(const Resource& _Other ) const;
	bool operator!=(const Resource& _Other ) const;

	static QStringList fileFilter(EnResourceType _Type);
	static QString suffixFilter(EnResourceType _Type);
	static QString filePrefix(EnResourceType _Type);
	static EnResourceType fileResourceType(const QString& _FileName);
	static bool isResource(const QFileInfo& _FileInfo);
	static QFileInfo frameMaskFile(const Resource& _FrameResource);
	static Resource resourceFromXmlSrc(const QString& _XmlSrc, const QString& _LoadDir);
	static QFileInfo fileInfoFromXmlSrc(const QString& _XmlSrc, const QString& _LoadDir);
	static QString fileInfoToXmlSrc(const QFileInfo& _FileInfo, const QString& _SaveDir);
};
}
#endif // RESOURCE_H
