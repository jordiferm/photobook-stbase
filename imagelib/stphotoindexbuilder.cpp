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
#include "stphotoindexbuilder.h"
#include "imageTools.h"
#include <QPainter>
#include <QCoreApplication> 
#include <QProgressBar> 
#include <math.h>


QSize STPhotoIndexBuilder::calcLayout(int _DesiredPageImages, const QSizeF& _RenderSize, int _Dpis)
{
	//qDebug(QString("STPhotoIndexBuilder::calcLayout: %1").arg(_DesiredPageImages).toLatin1());
	QSize Res(QSize(mmToPixels(_RenderSize.width(), _Dpis), mmToPixels(_RenderSize.height(), _Dpis)));
	int TotalA, MaxImageA; 
	
	MarginPix = mmToPixels(MarginMm, _Dpis);
	TotalA = (Res.width() - (MarginPix * 2)) * (Res.height() - (MarginPix * 2));
	MaxImageA = TotalA / _DesiredPageImages; 

	// ThumbWidth * ThumbHeight = MaxImageA
	// ThumbWidth / ThumbHeight = ThumbRatio.
	ThumbHeight = sqrt(MaxImageA / ThumbRatio);
	ThumbWidth = ThumbRatio * ThumbHeight; 
	NRowImages = ((Res.width() - (MarginPix * 2)) / ThumbWidth) ;
	
	//Recalc to fit all width with selecten Number of images.
	//ThumbWidth = (Res.width() - (MarginPix * 2)) / NRowImages;
	//ThumbHeight = ThumbWidth / ThumbRatio;

	int MaxPageFitImages = ((Res.height() - (MarginPix * 2)) / ThumbHeight) * NRowImages;
	MaxPageImages = qMin(_DesiredPageImages, MaxPageFitImages);
	return Res;
}

void STPhotoIndexBuilder::calcLayout()
{
	RenderPixelSize = calcLayout(MaxPageImages, RenderSize, Dpis);
}


STPhotoIndexBuilder::STPhotoIndexBuilder(int _DesiredPageImages, const QSizeF& _RenderSize, qreal _MarginMm, qreal _SpacingMm, int _Dpis)
		: MarginMm(_MarginMm), SpacingMm(_SpacingMm), ThumbRatio(1.5), Dpis(_Dpis), RenderSize(_RenderSize), DrawFrames(true), DrawNumbers(true), DrawFileNames(true)
{
	BackgroundColor = Qt::black;
	RenderPixelSize = calcLayout(_DesiredPageImages, _RenderSize, _Dpis); 
}

void STPhotoIndexBuilder::setMaxPageImages(int _MaxPageImages)
{
	MaxPageImages = _MaxPageImages;
	calcLayout();
}

void STPhotoIndexBuilder::setMargin(qreal _MarginMm)
{
	MarginMm = _MarginMm;
	calcLayout();
}

void STPhotoIndexBuilder::setThumbRatio(qreal _Ratio)
{
	ThumbRatio = _Ratio; 
	calcLayout();
}

void STPhotoIndexBuilder::setSpacing(qreal _SpacingMm)
{
	SpacingMm = _SpacingMm;
	calcLayout();
}

void STPhotoIndexBuilder::setImages(const QFileInfoList& _FileInfo)
{
	Images = _FileInfo;
}

void STPhotoIndexBuilder::setBackgroundImageFileName(const QString& _BackgroundImageFileName)
{
	BackgroundImageFileName = _BackgroundImageFileName;
}

void STPhotoIndexBuilder::setBackgroundColor(const QColor& _Color)
{
	//qDebug(QString("BackgroundColor: %1").arg(_Color.name()).toLatin1());
	BackgroundColor = _Color;
}

int STPhotoIndexBuilder::numPages() const
{
	return (Images.size() / MaxPageImages) + 1;
}

/*! 
	\param _RenderSize is the result image size in mm.
	\param _Page Is the page to render and goes from 0 to numPages() - 1.
*/
QImage STPhotoIndexBuilder::render(int _Page, QProgressBar* _PrgBar) const
{
	Assert(_Page < numPages(), Error(QString(QObject::tr("Page out of range: %1")).arg(_Page))); 
	QImage Res(RenderPixelSize, QImage::Format_RGB32);
	QPainter Painter(&Res); 
	Painter.fillRect(Res.rect(), BackgroundColor);
	int CurrentImage = MaxPageImages * _Page; 
	int SpacingPix = mmToPixels(SpacingMm, Dpis); 
	while (CurrentImage < MaxPageImages * (_Page +1) && CurrentImage < Images.size())
	{
		QImage CImage; 
		if (CImage.load(Images[CurrentImage].absoluteFilePath()))
		{
			int CurrPageImage = CurrentImage - (MaxPageImages * _Page);
			if (DrawFrames)
			{
				Painter.fillRect(MarginPix + ThumbWidth * (CurrPageImage % NRowImages), MarginPix + (ThumbHeight * (CurrPageImage / NRowImages)), 
								ThumbWidth, ThumbHeight, QColor(255, 255, 255));
				
				Painter.setPen(QPen(BackgroundColor));
				Painter.drawRect(MarginPix + ThumbWidth * (CurrPageImage % NRowImages), MarginPix + (ThumbHeight * (CurrPageImage / NRowImages)), 
								ThumbWidth, ThumbHeight);
			}
			CImage = CImage.scaled(ThumbWidth - (SpacingPix * 2), ThumbHeight - (SpacingPix * 2), Qt::KeepAspectRatio);
			//Margin values to center image in the thumbnail frame.
			int MarginX = (ThumbWidth - CImage.width()) / 2;
			int MarginY = (ThumbHeight - CImage.height()) / 2;
			int PosX = MarginPix + MarginX + (ThumbWidth * (CurrPageImage % NRowImages));
			int PosY = MarginPix + MarginY + (ThumbHeight * (CurrPageImage / NRowImages));
			Painter.drawImage(PosX, PosY, CImage); 
			QColor TextBackgroundColor(QColor(255, 255, 0, 200)); 
			if (DrawNumbers)
			{
				int CurrNum;
				if (Numbers.size() > 0 && Numbers.size() <= CurrentImage)
					CurrNum = Numbers[CurrentImage]; 
				else 
					CurrNum = CurrentImage; 
				Painter.save(); 
				Painter.setPen(QPen(Qt::black));
				QFont Font = Painter.font(); 
				Font.setBold(true);
				Font.setPointSize(14); 
				Painter.setFont(Font);
				QString NumStr = QString::number(CurrNum); 
				QRect TextBrect = Painter.fontMetrics().boundingRect(NumStr); 
				TextBrect.setWidth(TextBrect.width() + 4);
				TextBrect.setHeight(TextBrect.height() + 4);
				int PosTextX = PosX - MarginX + 2; 
				int PosTextY = PosY - MarginY + ThumbHeight - TextBrect.height();
				TextBrect.moveTo(PosTextX, PosTextY);
				Painter.fillRect(TextBrect, QBrush(TextBackgroundColor));
				Painter.drawText(TextBrect, Qt::AlignCenter, NumStr); 
				Painter.restore(); 
			}
			if (DrawFileNames)
			{
				Painter.save(); 
				Painter.setPen(QPen(Qt::black));
				QFont Font = Painter.font(); 
				Font.setPointSize(11); 
				Painter.setFont(Font);
				QString Text = Images[CurrentImage].baseName(); 
				QRect TextBrect = Painter.fontMetrics().boundingRect(Text); 
				TextBrect.setWidth(TextBrect.width() + 4);
				TextBrect.setHeight(TextBrect.height() + 4);
				int PosTextX = PosX - MarginX + 2; 
				int PosTextY = PosY - MarginY + 2;
				TextBrect.moveTo(PosTextX, PosTextY);
				Painter.fillRect(TextBrect, QBrush(TextBackgroundColor));
				Painter.drawText(TextBrect, Qt::AlignCenter, Text); 
				Painter.restore(); 
			}
		}
		CurrentImage++;

		if (_PrgBar)
		{
			_PrgBar->setValue(_PrgBar->value() + 1);
			QCoreApplication::processEvents();
		}
	}

	return Res;
}
