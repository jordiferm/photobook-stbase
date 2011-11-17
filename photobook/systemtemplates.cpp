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
#include "systemtemplates.h"
#include <QDir>
#include <QDebug>

#include "templatepaths.h"
#include "stutils.h"
#include "collectioninfo.h"
#include "stftpordertransfer.h"

using namespace SPhotoBook;

SPhotoBook::TemplateInfoList SystemTemplates::load()
{
	SPhotoBook::TemplateInfoList Res;
	//For each path
	QStringList PathList = TemplatePaths::pathList();
	QStringList::iterator pit = PathList.begin();
	while (pit != PathList.end())
	{
		//For each template name
		QDir BaseDir(*pit);
		QStringList TNameList = BaseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		QStringList::iterator tnit = TNameList.begin();
		while (tnit != TNameList.end())
		{
			//For each size
			QDir NameDir(BaseDir.absoluteFilePath(*tnit));
			QStringList TSizeList = NameDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			QStringList::iterator snit = TSizeList.begin();
			while (snit != TSizeList.end())
			{
				Res.addDesignNames(BaseDir, NameDir.dirName(), QDir(*snit).dirName());
				++snit;
			}
			++tnit;
		}
		++pit;
	}
	return Res;
}

void SystemTemplates::import(const QDir& _SourceDir, const QString& _NewName)
{
	Assert(existMetaInfo(_SourceDir), Error(QObject::tr("Error importing templates, template metainfo does not exist: %1").arg(_SourceDir.absolutePath())));
	MetaInfo MInfo = templateMetaInfo(_SourceDir);

	TemplateInfo TInfo(MInfo);
	DesignInfo DInfo(MInfo.designName());

	if (!_NewName.isEmpty())
		TInfo.setName(_NewName);

	QString DestDir = TInfo.absolutePath(DInfo);
	Assert(!QFile::exists(DestDir), Error(QObject::tr("Error importing template, template already exist: %1").arg(DestDir)));

	QDir BasePath = QDir(TInfo.basePath());
	Assert(BasePath.mkpath(DestDir), Error(QObject::tr("Error, could not create dir path %1").arg(DestDir)));
	Assert(STUtils::copyDirRecursive(_SourceDir, QDir(DestDir)), Error(QObject::tr("Error copying dir from %1 to %2").arg(
																		   _SourceDir.absolutePath()).arg(DestDir)));

	if (!_NewName.isEmpty())
	{
		MInfo.setName(_NewName);
		MInfo.save(CollectionInfo::xmlMetaInfoFileNameFromPath(DestDir));
	}
}

bool SystemTemplates::existMetaInfo(const QDir& _SourceDir)
{
	QString MetaInfoFileName = CollectionInfo::xmlMetaInfoFileNameFromPath(_SourceDir);
	return QFile::exists(MetaInfoFileName);
}

MetaInfo SystemTemplates::templateMetaInfo(const QDir& _SourceDir)
{
	MetaInfo Res;
	QString MetaInfoFileName = CollectionInfo::xmlMetaInfoFileNameFromPath(_SourceDir);
	if(QFile::exists(MetaInfoFileName))
	{
		Res.load(MetaInfoFileName);
	}
	return Res;
}

bool SystemTemplates::existTemplate(const MetaInfo& _MetaInfo)
{
	TemplateInfo TInfo(_MetaInfo);
	DesignInfo DInfo(_MetaInfo.designName());
	return QFile::exists(TInfo.absolutePath(DInfo));
}

void SystemTemplates::exportTemplateDesign(const QDir& _DestDir, const TemplateInfo& _Template, const DesignInfo& _Design)
{
	QString DesignPath = _Template.absolutePath(_Design);
	QDir DesignDir(DesignPath);
	if (DesignDir.exists())
		Assert(STUtils::copyDirRecursive(DesignDir, _DestDir), Error(QObject::tr("Error copying dir from %1 to %2").arg(
																		 DesignDir.absolutePath()).arg(_DestDir.absolutePath())));
}

void SystemTemplates::deleteTemplateDesign(const TemplateInfo& _Template, const DesignInfo& _Design)
{
	QString DesignPath = _Template.absolutePath(_Design);
	if (!DesignPath.isEmpty()) //Defensive
		STUtils::rmDir(QDir(DesignPath));
}

void SystemTemplates::deleteTemplateDesign(const MetaInfo& _MetaInfo)
{
	deleteTemplateDesign(TemplateInfo(_MetaInfo), DesignInfo(_MetaInfo.designName()));
}


void SystemTemplates::uploadTemplateDesign(const STDom::Publisher& _Publisher, const TemplateInfo& _TemplateInfo,
										   const DesignInfo& _DesignInfo, SProcessStatusWidget* _ProcessWidget)
{
	QString SourceDir = _TemplateInfo.absolutePath(_DesignInfo);

	QString DestDir = SourceDir;
	DestDir.remove(_TemplateInfo.basePath());
	STDom::STFtpOrderTransfer* FtpTrans = new STDom::STFtpOrderTransfer;
	try
	{

		FtpTrans-> putDir(SourceDir, _Publisher.ftpUrl(),
						  _Publisher.ftpPort(), _Publisher.userName(), _Publisher.password(),
						  _Publisher.initDir() + "/" + remoteTemplatesDir() + "/" + DestDir, static_cast<QFtp::TransferMode>(_Publisher.transferMode()),
						  _ProcessWidget);
		delete FtpTrans;
	}
	catch (...)
	{
		delete FtpTrans;
		throw;
	}
}

void SystemTemplates::downloadTemplateDesignMetaInfo(const STDom::Publisher& _Publisher, const TemplateInfo& _TemplateInfo,
													 const DesignInfo& _DesignInfo)
{
	QFileInfo TFileInfo(_TemplateInfo.absolutePath(_DesignInfo));

	QString TemplateDir = TFileInfo.absoluteFilePath();
	TemplateDir.remove(_TemplateInfo.basePath());
	QDir BaseDir(TemplateDir);
	Assert(BaseDir.mkpath(BaseDir.relativeFilePath(TFileInfo.absoluteFilePath())), Error(QObject::tr("Error creating dir %1").arg(TemplateDir)));
	TemplateDir = _Publisher.initDir() + "/" + remoteTemplatesDir() + "/" + TemplateDir;
	QFileInfo MetaDataFile(_TemplateInfo.metaInfoFileName(_DesignInfo));

	STDom::STFtpOrderTransfer* FtpTrans = new STDom::STFtpOrderTransfer;
	try
	{
		FtpTrans->getFile(MetaDataFile.fileName(), TemplateDir, MetaDataFile.absoluteFilePath(), _Publisher.ftpUrl(),
						  _Publisher.ftpPort(), _Publisher.userName(), _Publisher.password(),
						  static_cast<QFtp::TransferMode>(_Publisher.transferMode()));
		delete FtpTrans;
	}
	catch (...)
	{
		delete FtpTrans;
		throw;
	}
}

void SystemTemplates::downloadTemplateDesign(const STDom::Publisher& _Publisher, const TemplateInfo& _TemplateInfo,
												const DesignInfo& _DesignInfo, SProcessStatusWidget* _ProcessWidget)
{

}
