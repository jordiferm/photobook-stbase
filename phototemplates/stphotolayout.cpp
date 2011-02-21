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

#include "stphotolayout.h"
#include <QFile>
#include <QDir>
#include <QPainter>
#include <QDate>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include "stutils.h" 
#include "sapplication.h"
#include "sttemplatepaths.h"
#include "stftpstatuswidget.h"
#include "stprogressindicator.h"
#include "sterrorstack.h"

#include "stftpordertransfer.h"
#include "stxmlpublishersettings.h"

// _________________________________________________________________________*/
//
//	Class STPhotoLayoutTemplate
// _________________________________________________________________________*/


STPhotoLayoutTemplate::Frame::Frame(const QRectF& _Rect, double _RotationAngle) : QRectF(_Rect), RotationAngle(_RotationAngle), BorderSize(0), 
		ShowPhotoId(false), ShowFileName(false), IgnoreModifyAll(false), TextAlignment(Qt::AlignCenter), FrameType(TypePhoto), ZValue(0)
{
	QDate CDate = QDate::currentDate(); 
	if (CDate.month() > 4)
		setYear(CDate.year() + 1); 
	else 
		setYear(CDate.year());
}

void STPhotoLayoutTemplate::Frame::setText(const QString& _Text)
{
	QString InText = _Text;
	Text = InText.replace("%templateyear%", QString::number(Year));
}

void STPhotoLayoutTemplate::Frame::setMask(const QString& _FilenameToSave, const QImage& _MaskImage)
{
	setMaskFileName(_FilenameToSave); 
	MaskImage = _MaskImage; 
}

QImage STPhotoLayoutTemplate::Frame::maskImage() const 
{ 
	return MaskImage; 
} 

QImage STPhotoLayoutTemplate::Frame::maskImage(const QDir& _TemplateDir) const 
{ 
	QImage Res = MaskImage; 
	if (Res.isNull())
	{
		QString AbsMaskFileName = _TemplateDir.absoluteFilePath(MaskFileName); 
		if (QFile::exists(AbsMaskFileName))
			Res.load(AbsMaskFileName);
	}
	return Res; 
}

bool STPhotoLayoutTemplate::Frame::hasToSaveMask(const QString& _DestFileName) const
{ 
	return !MaskImage.isNull() && !QFile::exists(_DestFileName); 
}


//! Image frame
void STPhotoLayoutTemplate::Frame::setFrame(const QString& _FilenameToSave, const QImage& _FrameImage)
{
	setFrameFileName(_FilenameToSave);
	FrameImage = _FrameImage;
}


QImage STPhotoLayoutTemplate::Frame::frameImage() const
{
	return FrameImage;
}

QImage STPhotoLayoutTemplate::Frame::frameImage(const QDir& _TemplateDir) const
{
	QImage Res = FrameImage;
	if (Res.isNull())
	{
		QString AbsFrameFileName = _TemplateDir.absoluteFilePath(FrameFileName);
		if (QFile::exists(AbsFrameFileName))
			Res.load(AbsFrameFileName);
	}
	return Res;
}

bool STPhotoLayoutTemplate::Frame::hasToSaveFrame(const QString& _DestFileName) const
{
	return !FrameImage.isNull() && !QFile::exists(_DestFileName);
}


int STPhotoLayoutTemplate::CurrKeyValue = 1;

void STPhotoLayoutTemplate::addPicGrid(const Frame& _ParentFrame, const QDomElement& _Frame)
{
	float CellWidth = _Frame.attribute("cellwidth", "0").toDouble();
	float CellHeight = _Frame.attribute("cellheight", "0").toDouble();

	Assert( CellWidth != 0,
		Error(QObject::tr("CellWith is 0!")) );
	Assert( CellHeight != 0,
		Error(QObject::tr("CellHeight is 0!")) );
	// Calculem quants n'hi queven:
	int NFramesX = static_cast<int>(_ParentFrame.width() / CellWidth);
	int NFramesY = static_cast<int>(_ParentFrame.height() / CellHeight);

	float MarginX = (_ParentFrame.width() - (CellWidth * NFramesX)) / (NFramesX + 1);
	float MarginY = (_ParentFrame.height() - (CellHeight * NFramesY)) / (NFramesY + 1);
	
	bool ShowPhotoId = false; 
	bool ShowFileName = false;	
	if (_Frame.attribute("showphotoid", "").toLower() == "true")
		ShowPhotoId = true;
	
	if (_Frame.attribute("showfilename", "").toLower() == "true")
		ShowFileName = true;

	for ( int VforX = 0; VforX < NFramesX; VforX++ )
	{
		for ( int VforY = 0; VforY < NFramesY; VforY++ )
		{
			float PosX = ((VforX + 1)  * MarginX) + (VforX * CellWidth) + _ParentFrame.x();
			if (_Frame.attribute("halign", "center").toLower() == "left")
				PosX -= MarginX;
			else 
			if (_Frame.attribute("halign", "center").toLower() == "right")
				PosX += MarginX;

			float PosY = ((VforY + 1)  * MarginY) + (VforY * CellHeight) + _ParentFrame.y();
			if (_Frame.attribute("valign", "center").toLower() == "top")
				PosY -= MarginY;
			else
			if (_Frame.attribute("valign", "center").toLower() == "bottom")
				PosY += MarginY;
			Frame CurrFrame(QRectF(PosX, PosY, CellWidth, CellHeight));
			CurrFrame.setShowFileName(ShowFileName);
			CurrFrame.setShowPhotoId(ShowPhotoId);
			addFrame(CurrFrame);
		}
	}
}

void STPhotoLayoutTemplate::addFrame(const Frame& _ParentFrame, const QDomElement& _Frame)
{
	float Width = _Frame.attribute("width", "0").toDouble();
	float Height = _Frame.attribute("height", "0").toDouble();
	float Left, Top;
	//qDebug(_Frame.attribute("width", "0").toLatin1());
	//qDebug("_ParentFrame.width(): %f" ,_ParentFrame.width());
	QString VPos = _Frame.attribute("vpos", "center");
	if (VPos.toLower() == "center" )
		Top = (_ParentFrame.height() - Height)/ 2;
	else
	if (VPos.toLower() == "top")
		Top = 0;
	else
	if (VPos.toLower() == "bottom")
		Top = (_ParentFrame.height() - Height);
	else
	{
		bool CanCast;
		Top = VPos.toDouble(&CanCast);
		if (!CanCast)
			qWarning(QString("Problem casting %1 to double value.").arg(VPos).toLatin1());
	}

	QString HPos = _Frame.attribute("hpos", "center");
	if (HPos.toLower() == "center" )
		Left = (_ParentFrame.width() - Width)/ 2;
	else
	if (HPos.toLower() == "left")
		Left = 0;
	else
	if (HPos.toLower() == "right")
		Left = (_ParentFrame.width() - Width);
	else
	{
		bool CanCast;
		Left = HPos.toDouble(&CanCast);
		if (!CanCast)
			qWarning(QString("Problem casting %1 to double value.").arg(HPos).toLatin1());
	}

	Left += _ParentFrame.left();
	Top += _ParentFrame.top();

	Frame CurFrame(QRectF(Left, Top, Width, Height), _Frame.attribute("rotationangle", "0").toDouble());
	if (_Frame.tagName() == "frame")
	{
		CurFrame.setMaskFileName(_Frame.attribute("masksrc", "")); 
		CurFrame.setFrameFileName(_Frame.attribute("framesrc", ""));
	}
	else 
	if (_Frame.tagName() == "calmonthframe")
	{
		CurFrame.setFrameType(Frame::TypeCalMonth);
		CurFrame.setMonth(_Frame.attribute("month", QString::number(QDate::currentDate().month())).toInt());
		QFont Font(_Frame.attribute("fontfamily"), _Frame.attribute("fontsize").toInt());
		CurFrame.setTextColor(QColor(_Frame.attribute("textcolor", "#000000")));
		Font.setBold(_Frame.attribute("fontbold", "false").toLower() == "true");
		CurFrame.setFont(Font);
	}
	else
	if (_Frame.tagName() == "textframe")
	{
		CurFrame.setFrameType(Frame::TypeText);
		QFont Font(_Frame.attribute("fontfamily"), _Frame.attribute("fontsize").toInt());
		Font.setBold(_Frame.attribute("fontbold", "false").toLower() == "true");
		CurFrame.setFont(Font);
		CurFrame.setTextColor(QColor(_Frame.attribute("textcolor", "#000000")));
		CurFrame.setText(_Frame.attribute("text", QObject::tr("Click to change text")));
		CurFrame.setTextAlignment(Qt::AlignCenter); 
		if (_Frame.attribute("textalignment").toLower() == "left")
			CurFrame.setTextAlignment(Qt::AlignLeft);
		if (_Frame.attribute("textalignment").toLower() == "right")
			CurFrame.setTextAlignment(Qt::AlignRight);
	}
	else
	if (_Frame.tagName() == "ritchtextframe")
	{
		CurFrame.setFrameType(Frame::TypeRitchText);
		QFont Font(_Frame.attribute("fontfamily"), _Frame.attribute("fontsize").toInt());
		Font.setBold(_Frame.attribute("fontbold", "false").toLower() == "true");
		CurFrame.setFont(Font);
		CurFrame.setText(_Frame.attribute("text", QObject::tr("Click to change text")));
	}
	else 
	if (_Frame.tagName() == "clipartframe")
	{
		CurFrame.setFrameType(Frame::TypeClipart);
		CurFrame.setClipartFileName(_Frame.attribute("src")); 
	}
	//CurFrame.setRotationAngle(_Frame.attribute("rotationangle", "0").toDouble());
	CurFrame.setBorderSize(_Frame.attribute("bordersize", "0").toInt() );
	CurFrame.setBorderColor(QColor(_Frame.attribute("bordercolor", "#000000")));
	CurFrame.setZValue(_Frame.attribute("zvalue", 0).toInt()); 
	
	if (_Frame.attribute("showphotoid", "").toLower() == "true")
		CurFrame.setShowPhotoId(true);
	
	CurFrame.setIgnoreModifyAll(_Frame.attribute("ignoremodifyall", "").toLower() == "true");
	
	if (_Frame.attribute("showfilename", "").toLower() == "true")
		CurFrame.setShowFileName(true);
	
	// Mirem els seus fills:	

	QDomNodeList ChildNodes = _Frame.childNodes();
	if ( ChildNodes.count() > 0 ) //Una frame amb picgrid no pot contenir cap més frame.
	{
		for (int Vfor = 0; Vfor < ChildNodes.count(); Vfor++ )
		{
			QDomNode CurNode = ChildNodes.at(Vfor);
			QDomElement CurElement = CurNode.toElement();
			Assert(!CurElement.isNull(),
				Error(QObject::tr("Could not convert picgrid node to element!.")));

			if (CurElement.tagName() == "picgrid")
				addPicGrid(CurFrame, CurElement);

			if (CurElement.tagName() == "frame" || CurElement.tagName() == "textframe" || CurElement.tagName() == "calmonthframe" || CurElement.tagName() == "clipartframe" || CurElement.tagName() == "ritchtextframe")
				addFrame(CurFrame, CurElement);
		}
	}
	else
		addFrame(CurFrame); //És una fulla.
}


void STPhotoLayoutTemplate::setLocaleImageFile(const QString& _Locale, const QDomElement& _Element, QString& _ImageFileName)
{
	QString ImageFileName = _Element.attribute("src_" + _Locale, "");
	if (ImageFileName.isEmpty()) //! Potser no hi ha la del locale.
		ImageFileName = _Element.attribute("src_all", "");

	if (!ImageFileName.isEmpty())
	{
//		QFileInfo ImageFileInfo(ImageFileName);
//		if (ImageFileInfo.isRelative())
//		{
//			QFileInfo AbsImageFileInfo(TemplateFilePath + "/" + ImageFileName);
//			AbsImageFileInfo.makeAbsolute();
//			ImageFileName = AbsImageFileInfo.absoluteFilePath();
//		}
		_ImageFileName = ImageFileName;
	}
	else
		qWarning("No image name specified.");
}

void STPhotoLayoutTemplate::loadImageIfNull(const QString& _FileName, QImage& _Image) const
{
	QString FileName = _FileName;
	//Mirem si és relatiu
	if (QDir::isRelativePath(FileName))
		FileName = TemplateFilePath + "/" + FileName;
	
	//Carreguem la imatge.
	if (_Image.isNull())
		if (!_Image.load(FileName))
			qWarning(QString("Error loading image %1").arg(FileName).toLatin1());
}

QImage STPhotoLayoutTemplate::superimposeImage()
{
	if (!SuperImposeImageFile.isEmpty())
		loadImageIfNull(SuperImposeImageFile, SuperImposeImage);
	return SuperImposeImage;
}

QImage STPhotoLayoutTemplate::dsiImage()
{
	if (!DSIImageFile.isEmpty())
		loadImageIfNull(DSIImageFile, DSIImage);
	return DSIImage;
}

//!!! DEPRECATED !!!! 
QImage STPhotoLayoutTemplate::thumbnailImage()
{
	if (!ThumbnailImageFile.isEmpty())
		loadImageIfNull(ThumbnailImageFile, ThumbnailImage);
	return ThumbnailImage;
}

bool STPhotoLayoutTemplate::resourcesOnDisk() const
{
	bool Res = true;
	qDebug() << "SuperImpose:" << superImposeImageFilePath() << "SuperImposeFile:" << SuperImposeImageFile
			<< "Background:" << backgroundImageFilePath() << "BackgroundFile" << BackgroundImageFile;
	if (!SuperImposeImageFile.isEmpty())
		Res = Res && QFile::exists(superImposeImageFilePath());

	if (!BackgroundImageFile.isEmpty())
		Res = Res && QFile::exists(backgroundImageFilePath());
	return Res;
}


//!!!TODO: Move it to another class !!!!
void STPhotoLayoutTemplate::downloadResources(const STDom::STXmlPublisherSettings& _PXmlS, STFtpStatusWidget* _StatusWidget)
{
	STDom::STFtpOrderTransfer* FtpTrans = new STDom::STFtpOrderTransfer;
	if (_StatusWidget)
		_StatusWidget->connectFtp(FtpTrans);
	if (!SuperImposeImageFile.isEmpty())
		if (!QFile::exists(superImposeImageFilePath()))
			FtpTrans->getFile(SuperImposeImageFile, STTemplatePaths::remoteTemplateRelativePath(TemplateFilePath), superImposeImageFilePath(),
							  _PXmlS.dbHost(), _PXmlS.dbPort(), _PXmlS.dbUser(), _PXmlS.dbPassword(),
							  static_cast<QFtp::TransferMode>(_PXmlS.dbTransferMode()));

	if (!BackgroundImageFile.isEmpty())
		if (!QFile::exists(backgroundImageFilePath()))
			FtpTrans->getFile(BackgroundImageFile, STTemplatePaths::remoteTemplateRelativePath(TemplateFilePath), backgroundImageFilePath(),
							  _PXmlS.dbHost(), _PXmlS.dbPort(), _PXmlS.dbUser(), _PXmlS.dbPassword(),
							  static_cast<QFtp::TransferMode>(_PXmlS.dbTransferMode()));

	if (_StatusWidget)
		_StatusWidget->disconnectFtp(FtpTrans);
	delete FtpTrans;
}


QImage STPhotoLayoutTemplate::thumbnailImage() const
{
	QImage Res; 
	if (!ThumbnailImageFile.isEmpty())
		loadImageIfNull(ThumbnailImageFile, Res);
	return Res;
}

QImage STPhotoLayoutTemplate::backgroundImage()
{
	if (!BackgroundImageFile.isEmpty())
		loadImageIfNull(BackgroundImageFile, BackgroundImage);
	return BackgroundImage;
}	


/*!
	The keys are generated only when template is loaded.
*/
void STPhotoLayoutTemplate::getNewKey()
{
	Key = CurrKeyValue;
	CurrKeyValue++;
}

STPhotoLayoutTemplate::STPhotoLayoutTemplate(const QString& _Locale ) : NumPhotoFrames(0), IsFirstPage(false), ModifyAllFrames(false),
	RenderFramesOnly(false), CurrLocale(_Locale), Dpi(0), Key(-1), Encrypted(true)
{
#ifndef ENCRYPTED_TEMPLATES
	Encrypted = false;
#endif
	if (CurrLocale.isEmpty())
		CurrLocale = SApplication::currentLocale();
}

/*!
	Make all the paths relative to _RootDir.
*/
void STPhotoLayoutTemplate::makePathsRelative(const QDir& _RootDir)
{
	TFrameList::iterator it; 
	
	for (it = PicFrames.begin(); it != PicFrames.end(); ++it)
	{
		if (!it->clipartFileName().isEmpty())
			it->setClipartFileName(_RootDir.relativeFilePath(it->clipartFileName()));
		if (!it->maskFileName().isEmpty())
		{
			QFileInfo MaskFInfo(it->maskFileName()); 
			if (MaskFInfo.isFile())
				it->setMaskFileName(_RootDir.relativeFilePath(it->maskFileName()));
		}
		if (!it->frameFileName().isEmpty())
		{
			QFileInfo FrameFInfo(it->frameFileName());
			if (FrameFInfo.isFile())
				it->setFrameFileName(_RootDir.relativeFilePath(it->frameFileName()));
		}
	}
	if (!backgroundImageFilePath().isEmpty())
	{
		//qDebug(QString("setBackgroundImageFile-Relative %1 -> %2").arg(backgroundImageFilePath()).arg(_RootDir.relativeFilePath(backgroundImageFilePath())).toLatin1() );  
		setBackgroundImageFile(_RootDir.relativeFilePath(backgroundImageFilePath()));
	}
	if (! superImposeImageFilePath().isEmpty())
	{
		//qDebug(QString("setBackgroundImageFile-Relative %1 -> %2").arg(backgroundImageFilePath()).arg(_RootDir.relativeFilePath(backgroundImageFilePath())).toLatin1() );
		SuperImposeImageFile = _RootDir.relativeFilePath(superImposeImageFilePath());
	}
}


/*!
	If the clipart source file is not in _ClipatSharedDir copies it in _StorageDir.
*/
void STPhotoLayoutTemplate::storeCliparts(const QDir& _ClipartSharedDir, const QDir& _StorageDir)
{
	TFrameList::iterator it; 
	for (it = PicFrames.begin(); it != PicFrames.end(); ++it)
	{
		if (!it->clipartFileName().isEmpty())
		{
			QString ClipFileName = it->clipartFileName();
			if (QDir::isRelativePath(ClipFileName))
				ClipFileName = _ClipartSharedDir.filePath(ClipFileName); 
			QFileInfo ClipFInfo(ClipFileName);
			if (!ClipFInfo.dir().absolutePath().contains(_ClipartSharedDir.absolutePath()))
			{
				QString DestFileName = _StorageDir.absoluteFilePath(ClipFInfo.fileName()); 
				if (!QFile::exists(DestFileName))
					Assert(QFile::copy(ClipFInfo.absoluteFilePath(), DestFileName), 
						Error(QObject::tr("Error copying clipart file from %1 to %2").arg(ClipFInfo.absoluteFilePath()).arg(DestFileName)));
				it->setClipartFileName(_StorageDir.relativeFilePath(DestFileName));
			}
		}
	}
}

void STPhotoLayoutTemplate::storeMasks(const QDir& _StorageDir)
{
	TFrameList::iterator it; 
	for (it = PicFrames.begin(); it != PicFrames.end(); ++it)
	{
		QString DestMaskFile = _StorageDir.absoluteFilePath(QFileInfo(it->maskFileName()).fileName());
		QFileInfo MaskFInfo(DestMaskFile);
		if (!MaskFInfo.isDir())
		{
			if (it->hasToSaveMask(DestMaskFile))
			{
				//qDebug(QString("Storing.. %1").arg(DestMaskFile).toLatin1()); 
				Assert(it->maskImage().save(DestMaskFile, "PNG"), Error(QObject::tr("Error saving mask image: %1").arg(DestMaskFile)));
			}
			if (QFile::exists(DestMaskFile))
				it->setMaskFileName(DestMaskFile); 
		}
	}
}

void STPhotoLayoutTemplate::storeFrames(const QDir& _StorageDir)
{
	TFrameList::iterator it;
	for (it = PicFrames.begin(); it != PicFrames.end(); ++it)
	{
		QString DestFrameFile = _StorageDir.absoluteFilePath(QFileInfo(it->frameFileName()).fileName());
		QFileInfo FrameFInfo(DestFrameFile);
		if (!FrameFInfo.isDir())
		{
			if (it->hasToSaveFrame(DestFrameFile))
			{
				Assert(it->frameImage().save(DestFrameFile, "PNG"), Error(QObject::tr("Error saving frame image: %1").arg(DestFrameFile)));
			}
			if (QFile::exists(DestFrameFile))
				it->setFrameFileName(DestFrameFile);
		}
	}
}

QDomElement STPhotoLayoutTemplate::createElement(QDomDocument& _Doc)
{
	QDomElement MElement = _Doc.createElement("layouttemplate");
	MElement.setAttribute("width", size().width());
	MElement.setAttribute("height", size().height());
	if (dpi() > 0)
		MElement.setAttribute("dpi", dpi());
	if (isFirstPage())
		MElement.setAttribute("firstpage", "true"); 

	MElement.setAttribute("backgroundcolor", backgroundcolor().name()); 
	if ( !backgroundImageFilePath().isEmpty())
	{
		QDomElement BGImageElement = _Doc.createElement("backgroundimage"); 
		
		BGImageElement.setAttribute("src_all", backgroundImageFilePath()); 
		if (!CurrLocale.isEmpty())
			BGImageElement.setAttribute("src_" + CurrLocale, backgroundImageFilePath()); 
		MElement.appendChild(BGImageElement); 
	}
	if ( ! superImposeImageFilePath().isEmpty())
	{
		QDomElement SIImageElement = _Doc.createElement("superimposeimage");

		SIImageElement.setAttribute("src_all", superImposeImageFilePath());
		if (!CurrLocale.isEmpty())
			SIImageElement.setAttribute("src_" + CurrLocale, superImposeImageFilePath());
		MElement.appendChild(SIImageElement);
	}

	TFrameList::iterator it; 
	for (it = PicFrames.begin(); it != PicFrames.end(); ++it)
	{
		QDomElement NFrame;
		switch( it->frameType())
		{
			case Frame::TypePhoto :
				NFrame = _Doc.createElement("frame"); 
				if (!it->maskFileName().isEmpty())
					NFrame.setAttribute("masksrc", it->maskFileName());
				if (!it->frameFileName().isEmpty())
					NFrame.setAttribute("framesrc", it->frameFileName());
			break; 
			case Frame::TypeText :
				NFrame = _Doc.createElement("textframe");
				NFrame.setAttribute("fontsize", it->font().pointSize()); 
				NFrame.setAttribute("fontfamily", it->font().family()); 
				NFrame.setAttribute("fontbold", it->font().bold()); 
				NFrame.setAttribute("textcolor", it->textColor().name()); 
				NFrame.setAttribute("text", it->text()); 
				switch (it->textAlignment())
				{
					case Qt::AlignCenter : 
						NFrame.setAttribute("textalignment", "center"); 
					break; 
					case Qt::AlignLeft : 
						NFrame.setAttribute("textalignment", "left"); 
					break; 
					case Qt::AlignRight : 
						NFrame.setAttribute("textalignment", "right"); 
					break; 
				}
			break; 
			case Frame::TypeRitchText :
				NFrame = _Doc.createElement("ritchtextframe");
				NFrame.setAttribute("fontsize", it->font().pointSize()); 
				NFrame.setAttribute("fontfamily", it->font().family()); 
				NFrame.setAttribute("fontbold", it->font().bold()); 
				NFrame.setAttribute("text", it->text()); 
			break; 
			case Frame::TypeCalMonth : 
				NFrame = _Doc.createElement("calmonthframe"); 
				NFrame.setAttribute("month", it->month()); 
				NFrame.setAttribute("textcolor", it->textColor().name());
			break;
			case Frame::TypeClipart : 
			
				NFrame = _Doc.createElement("clipartframe"); 
				NFrame.setAttribute("src", it->clipartFileName()); 
			break; 

		}
		NFrame.setAttribute("width", it->width()); 
		NFrame.setAttribute("height", it->height()); 
		NFrame.setAttribute("vpos", it->top()); 
		NFrame.setAttribute("hpos", it->left()); 
		NFrame.setAttribute("rotationangle", it->rotationAngle()); 
		NFrame.setAttribute("bordersize", it->borderSize()); 
		NFrame.setAttribute("bordercolor", it->borderColor().name()); 
		NFrame.setAttribute("zvalue", it->zValue()); 
		MElement.appendChild(NFrame); 
	}
	return MElement; 
}

//! La mida està en el template.
void STPhotoLayoutTemplate::load(QDomNode& _Node)
{
	getNewKey();
	QDomElement TempElement = _Node.toElement();
	Assert(!TempElement.isNull(),
		Error(QObject::tr("Could not convert template node to element.")));
	//!!!!!!!!!!!!!!!!!!! Que passa amb aquesta conversió de double a int ? !!!!!!!!!!
	QSizeF Size(TempElement.attribute("width","0").toDouble(),
 					TempElement.attribute("height","0").toDouble());
	
	//!!! OBSOLETE !!!
	setRenderFramesOnly(TempElement.attribute("renderframesonly", "false").toLower() == "true");
	

	load(_Node, Size, TempElement.attribute("dpi", "300").toInt());
}

//! If template has not width and height attributes it uses _Size for template size.
void STPhotoLayoutTemplate::load(QDomNode& _Node, QSizeF& _Size, int _Dpi)
{
	getNewKey(); 
	NumPhotoFrames = 0;
	PicFrames.clear();
	//! El primer fill és la descripció.
	QDomElement TempElement = _Node.toElement();

	Assert(!TempElement.isNull(),
		Error(QObject::tr("Could not convert template node to element.")));

	IsFirstPage	= TempElement.attribute("firstpage", "false").toLower() == "true";
	BGColor = QColor(TempElement.attribute("backgroundcolor", "#FFFFFF"));

	setModifyAllFrames(TempElement.attribute("modifyallframes", "false").toLower() == "true");

#ifdef ENCRYPTED_TEMPLATES
	setEncrypted(TempElement.attribute("encrypted", "true").toLower() == "true");
#else
	setEncrypted(TempElement.attribute("encrypted", "false").toLower() == "true");
#endif

	setAspectRatioMode(static_cast<Qt::AspectRatioMode>(TempElement.attribute("aspectratiomode", QString::number(static_cast<int>(Qt::KeepAspectRatioByExpanding))).toInt()));
	
	QSizeF MSize(TempElement.attribute("width",QString::number(_Size.width())).toDouble(),
 					TempElement.attribute("height",QString::number(_Size.height())).toDouble());
	
	Size = MSize;
	BaseSize.setWidth(TempElement.attribute("basewidth","0").toDouble());
	BaseSize.setHeight(TempElement.attribute("baseheight","0").toDouble());


	Dpi = _Dpi;

/*	QDomNode DescNode =  TempElement.firstChild();
	Assert(!DescNode.isNull(),
		Error(QObject::tr("TemplateNode has not child nodes!.")));

	QDomElement DescElement = DescNode.toElement();
	Assert(!DescElement.isNull(),
		Error(QObject::tr("Could not convert description node to element!.")));*/
	
	Frame RootFrame(QRectF(0, 0, Size.width(), Size.height()));

	//QDomNodeList FrameNodes = TempElement.elementsByTagName("frame");
	QDomNodeList FrameNodes = TempElement.childNodes();
	Assert(FrameNodes.count() > 0,
		Error(QObject::tr("TemplateNode has not child nodes!.")));

	for (int Vfor = 0; Vfor < FrameNodes.count(); Vfor++ )
	{
		QDomNode CurrFrameNode = FrameNodes.at(Vfor);
		QDomElement CurrFrameElement = CurrFrameNode.toElement();
		
		Assert(!CurrFrameElement.isNull(),
			Error(QObject::tr("Could not convert frame node to element!.")));

		if (CurrFrameElement.tagName() == "description")
		{
			QString CDescription = CurrFrameElement.attribute(CurrLocale, "");
			if (CDescription.isEmpty())
				CDescription = CurrFrameElement.attribute("all", QObject::tr("No description"));
			setDescription(CDescription);
		}

		//if (CurrFrameElement.parentNode().toElement().tagName() == "layouttemplate")
		if (CurrFrameElement.tagName() == "frame" || CurrFrameElement.tagName() == "textframe" || CurrFrameElement.tagName() == "calmonthframe" || CurrFrameElement.tagName() == "clipartframe" || CurrFrameElement.tagName() == "ritchtextframe" )
			addFrame(RootFrame, CurrFrameElement);

		if (CurrFrameElement.tagName() == "backgroundimage")
			setLocaleImageFile(CurrLocale, CurrFrameElement, BackgroundImageFile);

		if (CurrFrameElement.tagName() == "superimposeimage")
			setLocaleImageFile(CurrLocale, CurrFrameElement, SuperImposeImageFile);
		
		if (CurrFrameElement.tagName() == "dsiimage")
			setLocaleImageFile(CurrLocale, CurrFrameElement, DSIImageFile);

		if (CurrFrameElement.tagName() == "thumbnailimage")
			setLocaleImageFile(CurrLocale, CurrFrameElement, ThumbnailImageFile);
	}
}

void STPhotoLayoutTemplate::addFrame(const Frame& _Frame)
{
	if (_Frame.frameType() == Frame::TypePhoto)
		NumPhotoFrames++;

	PicFrames.push_back(_Frame);
}

int STPhotoLayoutTemplate::dpi() const
{
	return Dpi;
}

QSizeF STPhotoLayoutTemplate::baseSize() const
{
	return BaseSize;
}


void STPhotoLayoutTemplate::setDpi(int _Dpi)
{
	Dpi = _Dpi;
}

QString STPhotoLayoutTemplate::typeName() const
{
	return TypeName;
}

void STPhotoLayoutTemplate::setTypeName(const QString& _TypeName)
{
	TypeName = _TypeName;
}

QString STPhotoLayoutTemplate::subTypeName() const
{
	return SubTypeName;
}

void STPhotoLayoutTemplate::setSubTypeName(const QString& _SubTypeName)
{
	SubTypeName = _SubTypeName;
}

QString STPhotoLayoutTemplate::description() const
{
	return Description;
}

void STPhotoLayoutTemplate::setDescription(const QString& _Description)
{
	Description = _Description;
}

/*!
- TO CONVERT TO MILLIMETERS; MULTIPLY INCHES X 25.4
- TO CONVERT TO INCHES; MULTIPLY MILLIMETERS X 0.03937 (1/25.4)
- FOR SLIGHTLY GREATER ACCURACY WHEN CONVERTING TO INCHES; DIVIDE MILLIMETERS BY 25.4


*/
QSize STPhotoLayoutTemplate::pixelSize() const
{
	return pixelSize(Size, Dpi);
}

QSize STPhotoLayoutTemplate::pixelSize(const QSizeF& _Size, int _Dpi)
{
	// Convertim els mm a Inches.
	float InchWidth = _Size.width() * 0.03937;
	float InchHeight = _Size.height() * 0.03937;
	QSize Res(static_cast<int>(InchWidth * _Dpi), static_cast<int>(InchHeight * _Dpi));
	return Res;
}

int STPhotoLayoutTemplate::heightForWidth(int _Width) const
{
	return static_cast<int>(Size.height() * _Width / Size.width());
}

//! Retorna la mida el pixels del costat més gran.
int STPhotoLayoutTemplate::bigSide() const
{
	QSize MSize = pixelSize();
	return qMax(MSize.width(),MSize.height());
}

//! Retorna la mida el pixels del costat més petit.
int STPhotoLayoutTemplate::smallSide() const
{
	QSize MSize = pixelSize();
	return qMin(MSize.width(),MSize.height());
}

QSize STPhotoLayoutTemplate::fitInSize(const QSize& _RefSize) const
{
	QSize Res;
	int RefBigSide = qMax(_RefSize.width(), _RefSize.height());
	int RefSmallSide = qMin(_RefSize.width(), _RefSize.height());

	//qDebug("RefSize Ratio %d >= TamplateSize Ratio %d", )
	if ( ((double)RefSmallSide / RefBigSide) >= ((double)smallSide() / bigSide()))
		Res = QSize(RefBigSide,
							(smallSide() * RefBigSide) / bigSide() );
	else
		Res = QSize((bigSide() * RefSmallSide) / smallSide(),
								RefSmallSide);

	return Res;
}


QSizeF STPhotoLayoutTemplate::size() const
{
	return Size;
}

void STPhotoLayoutTemplate::setSize(const QSizeF& _Size)
{
	Size = _Size;
}

void STPhotoLayoutTemplate::convertToSize(const QSizeF& _Size)
{
	TFrameList::iterator it; 
	for ( it = PicFrames.begin(); it != PicFrames.end(); ++it )
	{
		QRectF NewRect = translateRectf(*it, _Size); 
		(*it).setRect(NewRect.x(), NewRect.y(), NewRect.width(), NewRect.height());
	}
	setSize(_Size); 
}


int STPhotoLayoutTemplate::frameAtIndex(const QPointF& /*_Posicion*/, const QSize& /*_ImageSize*/)
{
	return 0;
}



QRect STPhotoLayoutTemplate::translateRect(const QRectF& _Rect, const QSize& _DestSize) const
{
	int XPos = static_cast<int>((_Rect.x() * _DestSize.width()) / Size.width());
	int Width = static_cast<int>((_Rect.width() * _DestSize.width()) / Size.width());
	int YPos = static_cast<int>((_Rect.y() * _DestSize.height()) / Size.height());
	int Height = static_cast<int>((_Rect.height() * _DestSize.height()) / Size.height());
	return QRect(XPos, YPos, Width, Height);
}

QRectF STPhotoLayoutTemplate::translateRectf(const QRectF& _Rect, const QSizeF& _DestSize) const
{
	qreal XPos = static_cast<int>((_Rect.x() * _DestSize.width()) / Size.width());
	qreal Width = static_cast<int>((_Rect.width() * _DestSize.width()) / Size.width());
	qreal YPos = static_cast<int>((_Rect.y() * _DestSize.height()) / Size.height());
	qreal Height = static_cast<int>((_Rect.height() * _DestSize.height()) / Size.height());
	return QRectF(XPos, YPos, Width, Height);
}

/*!
	Adapta l'aspect ratio de la imatge a l'aspect ratio del _ThumbnailSize de forma que si el template
és més ample que llarg i la mida del thumbnail és més llarg que ampla o viceversa donarà un template rotat.
	if _LayoutOrientation then the thumbnail is returned with the same orientation than the LayoutTemplate, otherwise the _ThumbnailSize orientation is used.
*/

QImage STPhotoLayoutTemplate::sampleThumbnail(const QSize& _ThumbnailSize, bool _LayoutOrientation) 
{
	QImage ResImage;

	bool RotateResult = false;
	if ((_ThumbnailSize.width() > _ThumbnailSize.height() && Size.width() < Size.height()) ||
		(_ThumbnailSize.height() > _ThumbnailSize.width() && Size.height() < Size.width()))
	{
		ResImage = QImage(QSize(_ThumbnailSize.height(), _ThumbnailSize.width()) , QImage::Format_RGB32);
		RotateResult = !_LayoutOrientation;
	}
	else
		ResImage = QImage(_ThumbnailSize, QImage::Format_RGB32);

	QPainter Painter(&ResImage);
	ResImage.fill(QColor("#FFD95C").rgb());
	QBrush BGBrush(BGColor);
	BGBrush.setStyle(Qt::Dense5Pattern); 
	Painter.setBrush(BGBrush);
	Painter.drawRect(ResImage.rect()); 
	Painter.setBrush(Qt::NoBrush); 
	Painter.setPen(Qt::black);
	TFrameList::const_iterator it; 
	for ( it = PicFrames.begin(); it != PicFrames.end(); ++it )
	{
		Painter.save(); 
		QRectF CRect = translateRect(*it, ResImage.size());
		Painter.translate(CRect.topLeft()); 
		Painter.rotate(it->rotationAngle()); 
		CRect.moveTo(0, 0); 
		if (it->isTextFrame())
		{
			Painter.fillRect(CRect, QBrush(QColor("#FFFBE2")));
			Painter.setPen(QPen(Qt::DotLine)); 
			Painter.drawRect(CRect);
			Painter.drawText(CRect, Qt::AlignCenter, QString(46, 'A'), &CRect);
		}
		else
		{ 
			Painter.fillRect(CRect, QBrush(QColor("#FFFBAD")));
			Painter.drawRect(CRect);
		}

		Painter.restore(); 
	}

	if (!thumbnailImage().isNull())
		Painter.drawImage(0,0, thumbnailImage().scaled(ResImage.size()));

	Painter.end();

	if (RotateResult)
	{
		QMatrix Matrix;
		Matrix.rotate(90);
		ResImage = ResImage.transformed(Matrix);
	}
	return ResImage;
}

void STPhotoLayoutTemplate::setTemplateFilePath(const QString& _TemplateFilePath)
{
	TemplateFilePath = _TemplateFilePath;
}

QString STPhotoLayoutTemplate::templateFilePath() const 
{
	return TemplateFilePath; 
}

STPhotoLayoutTemplate::TFrameList STPhotoLayoutTemplate::picFrames() const
{
	return PicFrames;
}


QString STPhotoLayoutTemplate::typeToStr(EnType _Type)
{
	QString Res;
	switch(_Type)
	{
		case TCard :
			Res = "card";
		break;
		case TAlbum :
			Res = "album";
		break;
		case TIndex :
			Res = "index";
		break;
		case TCDCover :
			Res = "cdcover";
		break;
		case TBusinesCard :
			Res = "businescard";
		break;
		case TBookMark :
			Res = "bookmark";
		break;
		case TDeco:
			Res = "deco";
		break;
	}
	return Res;
}

//! Retorna el path absolut de la imatge de background o un string buit si no està assignada.
QString STPhotoLayoutTemplate::backgroundImageAbsoluteFilePath() const
{
	QString Res;
	if (!BackgroundImageFile.isEmpty())
		Res = QDir(TemplateFilePath).absoluteFilePath(BackgroundImageFile);//TemplateFilePath + "/" + BackgroundImageFile;
	return Res;
}

QString STPhotoLayoutTemplate::backgroundImageFilePath() const
{
	QString Res;
	if (!BackgroundImageFile.isEmpty())
		Res = TemplateFilePath + "/" + BackgroundImageFile;
	return Res;
}

void STPhotoLayoutTemplate::setBackgroundImageFile(const QString& _FileName) 
{
	BackgroundImageFile = _FileName;
}

QString STPhotoLayoutTemplate::superImposeImageFilePath() const
{
	QString Res;
	if (!SuperImposeImageFile.isEmpty())
		Res = TemplateFilePath + "/" + SuperImposeImageFile;
	return Res;
}

QString STPhotoLayoutTemplate::dsiImageFilePath() const
{
	QString Res;
	if (!DSIImageFile.isEmpty())
		Res = TemplateFilePath + "/" + DSIImageFile;
	return Res;
}


bool STPhotoLayoutTemplate::operator!=(const STPhotoLayoutTemplate& _Other) const
{
	return !operator==(_Other); 
}

bool STPhotoLayoutTemplate::operator==(const STPhotoLayoutTemplate& _Other) const
{
	bool Res = false; 
	if (Key != -1 && _Other.Key!= -1)
		Res = Key == _Other.Key;
	return Res; 
}


bool STPhotoLayoutTemplate::operator<(const STPhotoLayoutTemplate& _Other) const
{
	bool Res = NumPhotoFrames < _Other.NumPhotoFrames;
	if (Key != -1 && _Other.Key != -1 && NumPhotoFrames == _Other.NumPhotoFrames)
		Res = Key < _Other.Key; 
	return Res;
}

bool STPhotoLayoutTemplate::operator>(const STPhotoLayoutTemplate& _Other) const
{
	return !operator<(_Other);
}

bool STPhotoLayoutTemplate::operator<(int _Value) const
{
	if (Key != -1 && NumPhotoFrames == _Value)
		return Key < _Value; 
	else 
		return NumPhotoFrames < _Value;
}

bool STPhotoLayoutTemplate::operator>(int _Value) const
{
	return !operator<(_Value);
}

QString STPhotoLayoutTemplate::thumbnailImage(const QString& _ImgFile)
{
	return _ImgFile + ".thumb";
}

int STPhotoLayoutTemplate::numPhotoFrames() const
{
	return NumPhotoFrames;
}

bool STPhotoLayoutTemplate::isFirstPage() const
{
	return IsFirstPage;
}

QColor STPhotoLayoutTemplate::backgroundcolor() const
{
	return BGColor;
}


STPhotoLayoutTemplate::TDateList STPhotoLayoutTemplate::holidays(int _Year) const
{
	TDateList Res = Holidays; 
	//Lets calc holidayFuncs
	QStringList::const_iterator it; 
	for (it = HolidayFuncs.begin(); it != HolidayFuncs.end(); ++it)
	{
		if (*it == "pascua")
		{
			QDate PascuaDate = STUtils::pascuaDate(_Year);
			Res.push_back(PascuaDate);
			//Res.push_back(PascuaDate.addDays(-1));
			Res.push_back(PascuaDate.addDays(-2));
		}
	}
	return Res;
}


// _________________________________________________________________________*/
//
//	Class STPhotoBookTemplate
// _________________________________________________________________________*/

QString STPhotoBookTemplate::DefaultRef = "000000";

STPhotoBookTemplate::STPhotoBookTemplate(const QString& _Locale) : CurrLocale(_Locale), PrintPageWidth(0), CutPagesOnPrint(true), PreprocessType(TypeNone),
	IsAtomic(true), PreferMinPages(false), PrintFirstPageAtLast(false)
{
	if (CurrLocale.isEmpty())
		CurrLocale = SApplication::currentLocale();
}

STPhotoBookTemplate::TTemplateList& STPhotoBookTemplate::templates()
{
	return Templates;
}

STPhotoBookTemplate::TTemplateList STPhotoBookTemplate::templates() const
{
	return Templates;
}

STPhotoBookTemplate::TColorList STPhotoBookTemplate::colors() const
{
	return Colors;
}

void STPhotoBookTemplate::setColors(const STPhotoBookTemplate::TColorList& _Colors)
{
	Colors = _Colors;
}


void STPhotoBookTemplate::setDescription(const QString& _Value)
{
	Description = _Value;
}

QString STPhotoBookTemplate::description() const
{
	return Description;
}

void STPhotoBookTemplate::setBGImageDirPath(const QString& _Value)
{
	TemplatesDirPath = _Value;
}

QString STPhotoBookTemplate::bgImageDirPath() const
{
	return TemplatesDirPath;
}

void STPhotoBookTemplate::setClipartDirPath(const QString& _Value)
{
	ClipartDirPath = _Value;
}

QString STPhotoBookTemplate::clipartDirPath() const
{
	return ClipartDirPath;
}

void STPhotoBookTemplate::setMaskDirPath(const QString& _Value)
{
	MaskDirPath = _Value;
}

QString STPhotoBookTemplate::maskDirPath() const
{
	return MaskDirPath;
}

void STPhotoBookTemplate::setFrameDirPath(const QString& _Value)
{
	FrameDirPath = _Value;
}

QString STPhotoBookTemplate::frameDirPath() const
{
	return FrameDirPath;
}

void STPhotoBookTemplate::setSize(const QSizeF& _Size)
{
	Size = _Size;
}

QSizeF STPhotoBookTemplate::size() const
{
	return Size;
}

void STPhotoBookTemplate::setDpis(int _Dpis)
{
	Dpis = _Dpis;
}

int STPhotoBookTemplate::dpis() const
{
	return Dpis;
}

//!Dots per meter;
int STPhotoBookTemplate::dpM() const
{
	return Dpis / 0.0254;
}

void STPhotoBookTemplate::clear()
{
	Templates.clear(); 
	Colors.clear();	
	Description= ""; 
	Dpis = 0; 
	Size = QSize(); 
	TemplatesDirPath = "./backgrounds/"; 
	ClipartDirPath = "./clipart/"; 
	MaskDirPath = "./masks/"; 
	FrameDirPath = "./frames/";
	PageMarginRects.clear(); 
	CoverMarginRects.clear(); 
	setPreprocessType(TypeNone);
	setDescription(QObject::tr("-- No Description --"));
}


void STPhotoBookTemplate::loadAlbum(const QDomNode& _AlbumNode, const QString& _TemplateFilePath, const STPhotoLayoutTemplate::TDateList& _Holidays, const QStringList& _HolidayFuncs)
{
	clear();
	QDomElement AlbumEl = _AlbumNode.toElement();

	Assert(!AlbumEl.isNull(),
		Error(QObject::tr("Error converting album node to element.")));


	QSizeF Size(AlbumEl.attribute("width","0").toDouble(),
 					AlbumEl.attribute("height","0").toDouble());
	int Dpi = AlbumEl.attribute("dpi", "300").toInt();
	

	setSize(Size);
	setDpis(Dpi);
	setMaxPages(AlbumEl.attribute("maxpages", "10000").toInt()); 
	setMinPages(AlbumEl.attribute("minpages", "0").toInt());
	setModPages(AlbumEl.attribute("modpages", "0").toInt());
	setPreferMinPages(AlbumEl.attribute("preferminpages", "false").toLower() == "true");
	double PrintPageWidth = AlbumEl.attribute("printpagewidth", QString::number(Size.width())).toDouble();
	if (PrintPageWidth > Size.width())
		PrintPageWidth = Size.width();
	setPrintPageWidth(PrintPageWidth);
	setCutPagesOnPrint(AlbumEl.attribute("cutpagesonprint", "true").toLower() == "true");
	setPrintFirstPageAtLast(AlbumEl.attribute("printfirstpageatlast", "false").toLower() == "true");
	setPreprocessType(static_cast<EnPreprocessType>(AlbumEl.attribute("preprocesstype", QString::number(static_cast<int>(TypeNone))).toInt()));
	setRef(AlbumEl.attribute("ref", DefaultRef));
	setIsAtomic(AlbumEl.attribute("atomic", "true").toLower() == "true");

	QDomNodeList ChildNodes = _AlbumNode.childNodes();
	if ( ChildNodes.count() > 0 )
	{
		for (int Vfor = 0; Vfor < ChildNodes.count(); Vfor++ )
		{
			QDomElement CurrEl = ChildNodes.at(Vfor).toElement();
			if (!CurrEl.isNull())
			{
				if (CurrEl.tagName() == "description" )
				{
					setDescription(CurrEl.attribute(CurrLocale, ""));
					if (Description.isEmpty())
						Description = CurrEl.attribute("all", QObject::tr("-- No description --"));
				}
				else
				if (CurrEl.tagName() == "infourl")
					setInfoUrl(QString(TEMPLATEINFO_BASE_URL) + "/"  + CurrEl.attribute("src", ""));
				else
				if (CurrEl.tagName() == "backgroundimagesdir" )
					setBGImageDirPath(_TemplateFilePath + "/" + CurrEl.attribute("src", "./backgrounds/"));
				else 
				if (CurrEl.tagName() == "clipartimagesdir" )
					setClipartDirPath(_TemplateFilePath + "/" + CurrEl.attribute("src", "./clipart/"));
				else 
				if (CurrEl.tagName() == "maskimagesdir" )
					setMaskDirPath(_TemplateFilePath + "/" + CurrEl.attribute("src", "./masks/"));
				else 
				if (CurrEl.tagName() == "frameimagesdir" )
					setFrameDirPath(_TemplateFilePath + "/" + CurrEl.attribute("src", "./frames/"));
				else
				if (CurrEl.tagName() == "pagemarginrect" )
					PageMarginRects.push_back(QRectF(CurrEl.attribute("hpos").toDouble(), CurrEl.attribute("vpos").toDouble(), 
													CurrEl.attribute("width").toDouble(), CurrEl.attribute("height").toDouble()));
				else
				if (CurrEl.tagName() == "covermarginrect" )
					CoverMarginRects.push_back(QRectF(CurrEl.attribute("hpos").toDouble(), CurrEl.attribute("vpos").toDouble(), 
													CurrEl.attribute("width").toDouble(), CurrEl.attribute("height").toDouble()));
				else 
				if (CurrEl.tagName() == "backgroundcolors" )
				{
					STPhotoBookTemplate::TColorList Colors;
					QDomNodeList ColorNodes = CurrEl.elementsByTagName("color");
					for ( int VforT = 0; VforT < ColorNodes.count(); VforT++ )
					{
						QDomElement CurrColorElement = ColorNodes.at(VforT).toElement();
						Colors.push_back(QColor(CurrColorElement.attribute("value", "#000000")));
					}
					setColors(Colors);
				}
			}
		}
	}

	QDomNodeList TemplateNodes = AlbumEl.elementsByTagName("layouttemplate");
	for ( int VforT = 0; VforT < TemplateNodes.count(); VforT++ )
	{
		QDomNode CurrTemplateNode = TemplateNodes.at(VforT);
		STPhotoLayoutTemplate CurrTemplate;
		CurrTemplate.setTemplateFilePath(_TemplateFilePath);
		CurrTemplate.setHolidays(_Holidays);
		CurrTemplate.setHolidayFuncs(_HolidayFuncs);
		//CurrTemplate.setTypeName(CurrType);
		//CurrTemplate.setSubTypeName(CurrSubType);
		CurrTemplate.load(CurrTemplateNode, Size, Dpi);
		templates().push_back(CurrTemplate);
	}
}

void STPhotoBookTemplate::load(const QString& _TemplateFileName)
{
 	QDomDocument Doc;
	QFile DocFile(_TemplateFileName);
	QFileInfo DocFileInfo(_TemplateFileName);
	QString ParseError; 
	int Line, Column; 
	Assert(DocFile.open(QIODevice::ReadOnly),
		Error(QString(QObject::tr("Could not open file: %1")).arg(_TemplateFileName)));
	Assert(Doc.setContent(&DocFile, &ParseError, &Line, &Column),
		Error(QString(QObject::tr("Could set document content from file: \n %1 \n %2 at line %3, column %4")).arg(_TemplateFileName).arg(ParseError).arg(Line).arg(Column)));

	QDomElement DocEl = Doc.documentElement();
	QDomNode CNode = DocEl.firstChild();

	STPhotoLayoutTemplate::TDateList LHoliDays;
	QStringList LHoliDayFuncs;
	STPhotoLayout::loadHolidays(DocEl, LHoliDays, LHoliDayFuncs); 

	while(!CNode.isNull())
	{
		QDomElement CEl = CNode.toElement(); // try to convert the node to an element.
		if(!CEl.isNull())
		{
			if (CEl.tagName().toLower() == "album" )
				loadAlbum(CNode, DocFileInfo.absolutePath(), LHoliDays, LHoliDayFuncs);
			else 
			if (CEl.tagName().toLower() == "author")
			{
				AuthorName = CEl.attribute("name"); 
				AuthorOrganization = CEl.attribute("organization"); 
				AuthorEmail = CEl.attribute("email"); 
			}
		}
		CNode = CNode.nextSibling();
	}
}

void STPhotoBookTemplate::downloadRemoteContents(const STDom::STXmlPublisherSettings& _PubSettings,
													STProgressIndicator* _ProgressIndicator, STFtpStatusWidget* _StatusWidget,
													STErrorStack* _ErrorStack)
{
	TTemplateList::iterator it;
	int NumToDownload = 0;
	for (it=Templates.begin(); it != Templates.end(); ++it)
	{
		if (!it->resourcesOnDisk())
				NumToDownload++;
	}

	_ProgressIndicator->setRange(0, NumToDownload);
	_ProgressIndicator->setValue(0);

	for (it=Templates.begin(); it != Templates.end(); ++it)
	{
		if (!it->resourcesOnDisk())
		{
			try
			{
				it->downloadResources(_PubSettings, _StatusWidget);
			}
			catch (STError& _Error)
			{
					if (_ErrorStack)
							_ErrorStack->push(_Error);
			}
			_ProgressIndicator->incValue();
		}
	}
}

void STPhotoBookTemplate::save(const QString& _TemplateFileName)
{
	QFile TLFile(_TemplateFileName);
	QDomDocument Doc("starphob");
	QDomElement Root = Doc.createElement("KPSPLTML");
	Root.setAttribute("version", "1.0.0");
	Doc.appendChild(Root);
	QDomElement AuthorEl = Doc.createElement("author"); 
	AuthorEl.setAttribute("name", AuthorName);
	AuthorEl.setAttribute("organization", AuthorOrganization); 
	AuthorEl.setAttribute("email", AuthorEmail); 
	Root.appendChild(AuthorEl); 

	QDomElement AlbumEl = Doc.createElement("album"); 
	AlbumEl.setAttribute("width", Size.width()); 
	AlbumEl.setAttribute("height", Size.height()); 
	AlbumEl.setAttribute("printpagewidth", PrintPageWidth); 
	AlbumEl.setAttribute("maxpages", MaxPages); 
	AlbumEl.setAttribute("minpages", MinPages); 
	AlbumEl.setAttribute("dpi", Dpis); 
	AlbumEl.setAttribute("ref", Ref);
	AlbumEl.setAttribute("atomic", IsAtomic ? "true" : "false");
	AlbumEl.setAttribute("preprocesstype", static_cast<int>(PreprocessType));

	QFileInfo TLFileInfo(TLFile); 

	//TODO: Save all locales
	QDomElement DescriptionEl = Doc.createElement("description");
	DescriptionEl.setAttribute("all", Description);
	AlbumEl.appendChild(DescriptionEl);

	QDomElement InfoUrlEl = Doc.createElement("infourl");
	InfoUrlEl.setAttribute("src", InfoUrl);
	AlbumEl.appendChild(InfoUrlEl);

	QDomElement BGImagesDirEl = Doc.createElement("backgroundimagesdir"); 
	BGImagesDirEl.setAttribute("src", TLFileInfo.dir().relativeFilePath(bgImageDirPath())); 
	AlbumEl.appendChild(BGImagesDirEl); 

	QDomElement ClipartImagesDirEl = Doc.createElement("clipartimagesdir"); 
	ClipartImagesDirEl.setAttribute("src", TLFileInfo.dir().relativeFilePath(clipartDirPath())); 
	AlbumEl.appendChild(ClipartImagesDirEl); 

	QDomElement MaskImagesDirEl = Doc.createElement("maskimagesdir"); 
	MaskImagesDirEl.setAttribute("src", TLFileInfo.dir().relativeFilePath(maskDirPath())); 
	AlbumEl.appendChild(MaskImagesDirEl); 

	QDomElement FrameImagesDirEl = Doc.createElement("frameimagesdir");
	FrameImagesDirEl.setAttribute("src", TLFileInfo.dir().relativeFilePath(frameDirPath()));
	AlbumEl.appendChild(FrameImagesDirEl);

	TMarginRectList::const_iterator pmit; 
	for (pmit = PageMarginRects.begin(); pmit != PageMarginRects.end(); pmit++)
	{
		QDomElement NewPageMRect = Doc.createElement("pagemarginrect"); 
		NewPageMRect.setAttribute("width", pmit->width()); 
		NewPageMRect.setAttribute("height", pmit->height()); 
		NewPageMRect.setAttribute("vpos", pmit->y()); 
		NewPageMRect.setAttribute("hpos", pmit->x()); 
		AlbumEl.appendChild(NewPageMRect); 
	}

	TMarginRectList::const_iterator cmit; 
	for (cmit = CoverMarginRects.begin(); cmit != CoverMarginRects.end(); cmit++)
	{
		QDomElement NewPageMRect = Doc.createElement("covermarginrect"); 
		NewPageMRect.setAttribute("width", cmit->width()); 
		NewPageMRect.setAttribute("height", cmit->height()); 
		NewPageMRect.setAttribute("vpos", cmit->y()); 
		NewPageMRect.setAttribute("hpos", cmit->x()); 
		AlbumEl.appendChild(NewPageMRect); 
	}

	//Save templatelist 
	TTemplateList::iterator it; 
	for (it = Templates.begin(); it != Templates.end(); ++it)
	{
		it->storeMasks(maskDirPath());
		it->storeFrames(frameDirPath());
		it->storeCliparts(clipartDirPath(), TLFileInfo.dir());
		it->makePathsRelative(TLFileInfo.dir()); 
		AlbumEl.appendChild(it->createElement(Doc)); 
	}	
	Root.appendChild(AlbumEl); 
	
	Assert(TLFile.open(QFile::WriteOnly | QFile::Truncate), Error(QString(QObject::tr("Could not open file %1")).arg(TLFile.fileName())));
	QTextStream Out(&TLFile);
	Out.setCodec(QTextCodec::codecForName("UTF-8"));
	Out << Doc.toString();
	TLFile.close(); 
}

bool STPhotoBookTemplate::hasFirstPages() const
{
	bool Res = false;
	TTemplateList::const_iterator it = Templates.begin();

	while (!Res && it != Templates.end())
	{
		Res = it->isFirstPage();
		++it;
	}

	return Res;
}


// _________________________________________________________________________*/
//
//	Class STPhotoLayout
// _________________________________________________________________________*/

void STPhotoLayout::loadTemplateList(const QDomNode& _TemplateListNode, const QString& _TemplateFilePath, const QString& _Type, const STPhotoLayoutTemplate::TDateList& _Holidays, const QStringList& _HolidayFuncs)
{
	QDomElement TemplateListEl = _TemplateListNode.toElement();

	Assert(!TemplateListEl.isNull(),
		Error(QObject::tr("Error converting templatelist node to element.")));

	QString CurrType = TemplateListEl.attribute("type", "");
	QString CurrSubType = TemplateListEl.attribute("subtype", "");

	if (CurrType == _Type || _Type == "")
	{
		QDomNodeList TemplateNodes = TemplateListEl.elementsByTagName("layouttemplate");
		for ( int VforT = 0; VforT < TemplateNodes.count(); VforT++ )
		{
			QDomNode CurrTemplateNode = TemplateNodes.at(VforT);
			STPhotoLayoutTemplate CurrTemplate(CurrLocale);
			CurrTemplate.setTemplateFilePath(_TemplateFilePath);
			CurrTemplate.setTypeName(CurrType);
			CurrTemplate.setSubTypeName(CurrSubType);
			CurrTemplate.setHolidays(_Holidays);
			CurrTemplate.setHolidayFuncs(_HolidayFuncs);
			CurrTemplate.load(CurrTemplateNode);
			Templates.push_back(CurrTemplate);
		}
	}
}

//! _TemplateFilePath the path of the file templage (Absolute Path only without filename )
void STPhotoLayout::loadAlbum(const QDomNode& _AlbumNode, const QString& _TemplateFilePath, const STPhotoLayoutTemplate::TDateList& _Holidays, const QStringList& _HolidayFuncs)
{
	STPhotoBookTemplate CurrAlbum(CurrLocale);
	CurrAlbum.loadAlbum(_AlbumNode, _TemplateFilePath, _Holidays, _HolidayFuncs);
	Albums.push_back(CurrAlbum);
}

void STPhotoLayout::loadFile(const QString& _XmlTemplateFile, const QString& _Type)
{
 	QDomDocument Doc;
	QFile DocFile(_XmlTemplateFile);
	QFileInfo DocFileInfo(_XmlTemplateFile);
	Assert(DocFile.open(QIODevice::ReadOnly),
		Error(QString(QObject::tr("Could not open file: %1")).arg(_XmlTemplateFile)));
	QString ParseError;
	int _Line = 0;
	int _Column = 0;
   Assert(Doc.setContent(&DocFile, &ParseError, &_Line, &_Column),
		Error(QString(QObject::tr("Could set document content from file: \n %1 \n %2 at line %3, column %4")).arg(_XmlTemplateFile).arg(ParseError).arg(_Line).arg(_Column)));

	QDomElement DocEl = Doc.documentElement();

// 	QDomNode RootNode = DocEl.firstChild();
// 	Assert(!RootNode.isNull(),
// 		Error(QObject::tr("Could not find document root node.")));
// 	QDomElement RootEl = RootNode.toElement();
// 	Assert(!RootEl.isNull(),
// 		Error(QObject::tr("Coult not convert root node to element")));
// 
// 	qDebug(RootEl.tagName().toLatin1());

	QDomNodeList SubtypeList = DocEl.elementsByTagName("subtypes");
	for ( int Vfor = 0; Vfor < SubtypeList.count(); Vfor++ )
	{
		QDomNode CurrList = SubtypeList.at(Vfor);
		QDomElement SubTypeListEl = CurrList.toElement();
		
		Assert(!SubTypeListEl.isNull(),
			Error(QObject::tr("Error converting subtypelist node to element.")));
		QString CurrType = SubTypeListEl.attribute("type", "");
		if (CurrType == _Type || _Type == "")
		{
			addSubTypes(SubTypeListEl, CurrType, DocFileInfo.absolutePath());
		}
	}

	STPhotoLayoutTemplate::TDateList LHoliDays;
	QStringList LHoliDayFuncs;
	STPhotoLayout::loadHolidays(DocEl, LHoliDays, LHoliDayFuncs); 

	QDomNodeList TemplateLists = DocEl.elementsByTagName("templatelist");
	for ( int Vfor = 0; Vfor < TemplateLists.count(); Vfor++ )
		loadTemplateList(TemplateLists.at(Vfor), DocFileInfo.absolutePath(), _Type, LHoliDays, LHoliDayFuncs);
	
	//FIXME Remove it, album Templates can load itself.
	QDomNodeList AlbumLists = DocEl.elementsByTagName("album");
	for ( int Vfor = 0; Vfor < AlbumLists.count(); Vfor++ )
		loadAlbum(AlbumLists.at(Vfor), DocFileInfo.absolutePath(), LHoliDays, LHoliDayFuncs);


	DocFile.close();
	
}



/*!
	\param _PathRoot indica el directori on estem llegint els templates. Per si les imatges de les icones 
	s'especifiquen de forma relativa.
*/
void STPhotoLayout::addSubTypes(const QDomElement& _SubTypeListElement, const QString& _MainType, 
	const QString& _PathRoot)
{
	QDomNodeList SubTypeLists = _SubTypeListElement.elementsByTagName("subtype");
	for ( int Vfor = 0; Vfor < SubTypeLists.count(); Vfor++ )
	{
		QDomNode CurrSubType = SubTypeLists.at(Vfor);
		QDomElement CurrSubTypeEl = CurrSubType.toElement();
		
		Assert(!CurrSubTypeEl.isNull(),
			Error(QObject::tr("Error converting subtype node to element.")));
		QString STName = CurrSubTypeEl.attribute("name", "");

		QDomNodeList ChildNodes = CurrSubType.childNodes();
		if ( ChildNodes.count() > 0 ) 
		{
			QString Description, IconFile, ToolTipText, ResourcesDir = "";
			for (int Vfor = 0; Vfor < ChildNodes.count(); Vfor++ )
			{
				QDomElement CurrEl = ChildNodes.at(Vfor).toElement();
				if (!CurrEl.isNull())
				{
					if (CurrEl.tagName() == "description" )
					{
						Description = CurrEl.attribute(CurrLocale, "");
						if (Description.isEmpty())
							Description = CurrEl.attribute("all", QObject::tr("No description"));
					}
					if (CurrEl.tagName() == "tooltiptext" )
					{
						ToolTipText = CurrEl.attribute(CurrLocale, "");
						if (ToolTipText.isEmpty())
							ToolTipText = CurrEl.attribute("all", "");
					}
					if (CurrEl.tagName() == "icon" )
					{
						IconFile = CurrEl.attribute("src", "");
						if (QDir::isRelativePath(IconFile))
							IconFile = _PathRoot + "/" + IconFile;
					}
					if (CurrEl.tagName() == "resourcesdir" )
					{
						ResourcesDir = CurrEl.attribute("src", "");
						if (QDir::isRelativePath(ResourcesDir))
							ResourcesDir = _PathRoot + "/" + ResourcesDir;
					}
				}
			}
			if (ResourcesDir.isEmpty())
				ResourcesDir = _PathRoot; 
			SubTypes.push_back(STPhotoLayoutSubtype(STName, Description, IconFile, _MainType, ToolTipText, ResourcesDir));
		}
	}
}


STPhotoLayout::STPhotoLayout(const QString& _Locale) : CurrLocale(_Locale)
{
	if (CurrLocale.isEmpty())
		CurrLocale = SApplication::currentLocale();
}

void STPhotoLayout::clear()
{
	Albums.clear();
	Templates.clear();
	SubTypes.clear();
}

void STPhotoLayout::saveTemplateList(const QString& _XmlTemplateFile, const QString& _Author, const QString& _Organization, const QString& _Email)
{
	QFile TLFile(_XmlTemplateFile);
	QDomDocument Doc("starphob");
	QDomElement Root = Doc.createElement("KPSPLTML");
	Root.setAttribute("version", "1.0.0");
	Doc.appendChild(Root);
	QDomElement AuthorEl = Doc.createElement("author"); 
	AuthorEl.setAttribute("name", _Author);
	AuthorEl.setAttribute("organization", _Organization); 
	AuthorEl.setAttribute("email", _Email); 
	Root.appendChild(AuthorEl); 
	//Save templatelist 
	QFileInfo  TLFileInfo(TLFile);
	QDomElement TListElement = Doc.createElement("templatelist");
	TTemplateList::iterator it; 
	for (it = Templates.begin(); it != Templates.end(); ++it)
	{
		it->storeMasks(TLFileInfo.dir().absolutePath());
		it->storeFrames(TLFileInfo.dir().absolutePath());
		it->storeCliparts(TLFileInfo.dir(), TLFileInfo.dir());
		it->makePathsRelative(TLFileInfo.dir()); 
		TListElement.appendChild(it->createElement(Doc)); 
	}	
	Root.appendChild(TListElement); 
	
	Assert(TLFile.open(QFile::WriteOnly | QFile::Truncate), Error(QString(QObject::tr("Could not open file %1")).arg(TLFile.fileName())));
	QTextStream Out(&TLFile);
	Out.setCodec(QTextCodec::codecForName("UTF-8"));
	Out << Doc.toString();
	TLFile.close(); 
}

void STPhotoLayout::load(const QString& _XmlTemplateFile)
{
	loadFile(_XmlTemplateFile);
}

void STPhotoLayout::load(const QString& _XmlTemplateFile, STPhotoLayoutTemplate::EnType _Type)
{
	loadFile(_XmlTemplateFile, STPhotoLayoutTemplate::typeToStr(_Type));
}

void STPhotoLayout::loadDir(const QString& _Dir)
{
	QDir Dir(_Dir);
	QStringList FileList = Dir.entryList(QStringList("*.kpspltml"), QDir::Files);
	QStringList::const_iterator it;
	for ( it = FileList.begin(); it != FileList.end(); ++it )
	{
		load(Dir.absolutePath() + "/" + *it);
	}
}

void STPhotoLayout::loadDir(const QString& _Dir, STPhotoLayoutTemplate::EnType _Type)
{
	QDir Dir(_Dir);
	QStringList FileList = Dir.entryList(QStringList("*.kpspltml"), QDir::Files);
	QStringList::const_iterator it;
	for ( it = FileList.begin(); it != FileList.end(); ++it )
	{
		load(Dir.absolutePath() + "/" + *it, _Type);
	}
}

void STPhotoLayout::addTemplate(const STPhotoLayoutTemplate& _Template)
{
	Templates.push_back(_Template); 
}

STPhotoLayout::TTemplateList STPhotoLayout::templates() const
{
	return Templates;
}

STPhotoLayout::TTemplateList STPhotoLayout::templates(const QString& _SubTypeName) const
{
	STPhotoLayout::TTemplateList Res; 
	STPhotoLayout::TTemplateList::const_iterator it; 
	for (it = Templates.begin(); it != Templates.end(); ++it)
	{
		if (it->subTypeName() == _SubTypeName)
		Res.push_back(*it);
	}
	return Res; 
}


STPhotoLayout::TAlbumsList STPhotoLayout::albums() const
{
	return Albums;
}



STPhotoLayout::TSubtypeList STPhotoLayout::subTypes() const
{
	return SubTypes;
}

//Related non member functions.
bool operator<(int _Value, const STPhotoLayoutTemplate& _Other)
{
	return _Other > _Value; 
}

void STPhotoLayout::loadHolidays(const QDomElement& _DocEl, STPhotoLayoutTemplate::TDateList& _LHoliDays, QStringList& _LHoliDayFuncs)
{
	QDomNodeList HolidayList = _DocEl.elementsByTagName("holidays");
	if (HolidayList.count() > 0)
	{
		for ( int Vfor = 0; Vfor < HolidayList.count(); Vfor++ )
		{
			QDomElement CurrEl = HolidayList.at(Vfor).toElement();
			QDomNodeList DayNodes = CurrEl.elementsByTagName("day");
			for ( int VforT = 0; VforT < DayNodes.count(); VforT++ )
			{
				QDomElement CurrDayElement = DayNodes.at(VforT).toElement();
				QDate NewDate; 
				QString CurrDateStr = CurrDayElement.attribute("value").toLower();
				QRegExp RexpFunc("%(\\S+)%");
				int Pos = RexpFunc.indexIn(CurrDateStr);
				if (Pos > -1) 
					_LHoliDayFuncs.push_back(RexpFunc.cap(1));
				else
					NewDate = QDate::fromString(CurrDayElement.attribute("value"), "dd/MM"); 
				_LHoliDays.push_back(NewDate);
			}
		}
	}
}

QString STPhotoLayout::resourceSubDirName(STPhotoLayout::EnResourceSubDirType _Type)
{
	QString Res; 
	switch (_Type)
	{
		case MasksDir : 
			Res = "masks"; 
		break;
		case FramesDir :
			Res = "frames";
		break;
		case ClipartDir : 
			Res = "clipart"; 
		break; 
		case BackgroundsDir :
			Res = "backgrounds";
		break;
	}
	return Res; 
}
