/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "oofficetemplate.h"
#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QObject>
#include <QDebug>
#include <QTextStream>
#include <QTemporaryFile>
#include "stutils.h"

QString OOfficeTemplate::ZipBinFilePath = "";
QString OOfficeTemplate::UnzipBinFilePath = "";

QString OOfficeTemplate::parseLine(const QString& _Line, OOfficeDataSupplier* _DataSupplier)
{
	QString Res = _Line;
	//QRegExp RxVar("\\[%([a-zA-Z0-9\\.]*)%\\]");
	QRegExp RxVar("(sum)?\\[%([a-zA-Z0-9_]+)(\\([a-zA-Z0-9_&;@=<>!]+\\))?\\.([a-zA-Z0-9_]+)%\\]([0-9]*)");
	int CIndex = 0;
	while( (CIndex = RxVar.indexIn(Res, CIndex)) != -1 )
	{
		bool ReplaceVar = true;
		QString Op = RxVar.cap(1);
		QString VarName = RxVar.cap(2);
		QString Filter = RxVar.cap(3).remove("(").remove(")").replace("&apos;", "'");
		QString Field = RxVar.cap(4);
		int Index = RxVar.cap(5).toInt();

		//qDebug() << "Line:" << RxVar.cap(0);
		//qDebug() << "---> VarName:" << VarName << " Filter: " << Filter << " Field: " << Field << " Index: " << Index << " Op: " << Op ;

		QString RepValue;

		if (!VarName.isEmpty())
		{
			if (Vars.contains(VarName))
			{
				RepValue = Vars[VarName].toString();
			}
			else
			{
				if (_DataSupplier)
				{
					if (_DataSupplier->accepts(VarName))
						RepValue = _DataSupplier->getVar(VarName, Field, Filter, Op, Index).toString();
					else
						ReplaceVar = false;
				}
			}
		}

		if (ReplaceVar)
		{
			Res = Res.replace(RxVar.cap(0), RepValue);
			CIndex += RepValue.length();
		}
		else
			CIndex += RxVar.cap(0).length();
	}
	return transformOOCalcNumericValues(Res);


}

QString OOfficeTemplate::transformOOCalcNumericValues(const QString& _Input)
{
	QString Res = _Input;
	QRegExp RxVar("value-type=\"string\"><text:p>([0-9]+)</text:p>");
	int CIndex = 0;
	while( (CIndex = RxVar.indexIn(Res, CIndex)) != -1 )
	{
		//qDebug() << RxVar.cap(0) << RxVar.cap(1);
		//CIndex += RxVar.cap(0).length();

		QString RepValue = QString("value-type=\"float\" office:value=\"%1\"><text:p>%1</text:p>").arg(RxVar.cap(1));
		Res = Res.replace(RxVar.cap(0), RepValue);
		CIndex += RepValue.length();

	}
	return Res;
	//value-type="float" office:value="1"><text:p>1</text:p>
	//value-type="string"><text:p>4</text:p>

}

void OOfficeTemplate::parseFile(const QString& _FileName, OOfficeDataSupplier* _DataSupplier)
{
	if (_DataSupplier)
		_DataSupplier->clearCache();

	QFile ContentFile(_FileName);
	Assert(ContentFile.open(QFile::ReadOnly), Error(QObject::tr("Error openning file %1").arg(_FileName)));
	QTemporaryFile OutFile;
	Assert(OutFile.open(), Error(QObject::tr("Error creating temporary file.")));

	QTextStream StrmIn(&ContentFile);
	StrmIn.setCodec("UTF-8");
	QTextStream StrmOut(&OutFile);
	StrmOut.setCodec("UTF-8");
	QString CurrentLine;
	do {
		CurrentLine = StrmIn.readLine();
		StrmOut << parseLine(CurrentLine, _DataSupplier);
	} while (!CurrentLine.isNull());
	ContentFile.close();
	OutFile.close();
	Assert(ContentFile.remove(), Error(QObject::tr("Could not remove file %1").arg(_FileName)));
	Assert(QFile::copy(OutFile.fileName(), _FileName), Error(QObject::tr("Could not copy file %1 to File %2").arg(OutFile.fileName()).arg(_FileName)));
}


OOfficeTemplate::OOfficeTemplate(const QString& _FilePath)
	: FilePath(_FilePath)
{
}

void OOfficeTemplate::addDataSupplier(OOfficeDataSupplier* _DataSupplier)
{
	OOfficeDataSupplier::TVarList SupVars = _DataSupplier->globalVars();
	OOfficeDataSupplier::TVarList::iterator it = SupVars.begin();
	while (it != SupVars.end())
	{
		addVar(it.key(), it.value());
		++it;
	}
	DataSuppliers.push_back(_DataSupplier);
}

void OOfficeTemplate::addVar(const QString& _Key, const QVariant& _Value)
{
	Vars.insert(_Key, _Value);
}

void OOfficeTemplate::clearVars()
{
	Vars.clear();
}

void OOfficeTemplate::setZipBinFilePath(const QString& _FilePath)
{
	ZipBinFilePath = _FilePath;
}

void OOfficeTemplate::setUnzipBinFilePath(const QString& _FilePath)
{
	UnzipBinFilePath = _FilePath;
}

bool OOfficeTemplate::isTemplate(const QString& _DocFilePath)
{
	QFileInfo FInfo(_DocFilePath);
	return FInfo.suffix() == templateExtension();
}

QString OOfficeTemplate::templateExtension()
{
	return "soot";
}

QString OOfficeTemplate::destFileName(const QString& _FileName, const QString& _ProjectName)
{
	QString RemoveExt = QString(".") + templateExtension();
	QString FileName = _FileName;
	FileName = FileName.remove(RemoveExt);
	QFileInfo FInfo(FileName);
	QString Res = FInfo.fileName();
	if (!_ProjectName.isEmpty())
	{
		int UndPos = Res.indexOf("_");
		if ( UndPos != -1)
		{
			Res = Res.left(UndPos) + "_" + _ProjectName + "." + FInfo.completeSuffix();
		}
	}
	return Res;
}

void OOfficeTemplate::parse(const QString& _OutputDocFilePath)
{
	QDir TempPath = QDir::tempPath();
	QString TempDirName = "stootemp";

	if (TempPath.exists(TempPath.absoluteFilePath(TempDirName)));
		STUtils::rmDir(QDir(TempPath.absoluteFilePath(TempDirName)));

	Assert(TempPath.mkdir(TempDirName), Error(QObject::tr("Error creating Temp path: %1").arg(TempPath.absoluteFilePath(TempDirName))));
	TempPath.cd(TempDirName);

	//Unzip the doc
	QProcess Unzip;
	Unzip.setWorkingDirectory(TempPath.absolutePath());
	Unzip.start(UnzipBinFilePath, QStringList() << FilePath);

	Assert(Unzip.waitForStarted(), Error(QObject::tr("Could not start unzip with file %1").arg(FilePath)));
	Assert(Unzip.waitForFinished(60000), Error(QObject::tr("Could unzip file %1").arg(FilePath)));
	//qDebug() << "File " << FilePath << " unzipped at: " << TempPath.absolutePath();

	//Parsing file
	if (!DataSuppliers.isEmpty())
	{
		TDataSupplierList::iterator it;
		for (it = DataSuppliers.begin(); it != DataSuppliers.end(); ++it)
		{
			parseFile(TempPath.absoluteFilePath("content.xml"), *it);
		}
	}
	else
		parseFile(TempPath.absoluteFilePath("content.xml"));

	//Zipping aggain
	QProcess Zip;
	Zip.setWorkingDirectory(TempPath.absolutePath() + "/");
	Zip.start(ZipBinFilePath, QStringList() << "-R" << _OutputDocFilePath << "*" );

	Assert(Zip.waitForStarted(), Error(QObject::tr("Could not start zip with file %1").arg(_OutputDocFilePath)));
	Assert(Zip.waitForFinished(60000), Error(QObject::tr("Could zip file %1").arg(_OutputDocFilePath)));

	//qDebug() << "File " << FilePath << " zipped at: " << _OutputDocFilePath << "Res is" << Zip.error() << "Error is" << Zip.errorString();

}
