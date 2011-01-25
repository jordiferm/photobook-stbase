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

#include "stfontthumbnailmodel.h"
#include <QFontDatabase>
#include <QPixmap> 
#include <QPainter> 

void STFontThumbnailModel::loadFonts()
{
	QFontDatabase Database;
	Fonts.clear();
	foreach (QString family, Database.families()) 
	{
		foreach (QString style, Database.styles(family)) 
		{
			Fonts.push_back(Database.font(family, style, 22));
		}
	}
}

QPixmap STFontThumbnailModel::createThumbNail(const QFont& _Font) const
{
	QPixmap Pixmap(ThumbnailMaxSize);
	//Font.setPointSize(18);
	QPainter Painter(&Pixmap);
	Painter.fillRect(Pixmap.rect(), Qt::white);
	Painter.setFont(_Font);
	QString Text = "A";
	QRect FontBrect = Painter.fontMetrics().boundingRect(Text); 
	Painter.drawText((Pixmap.width() / 2) - (FontBrect.width() / 2), (Pixmap.height() / 2) + (FontBrect.height() / 2) , Text);
	Painter.end();
	return Pixmap; 
}

STFontThumbnailModel::STFontThumbnailModel(QObject* _Parent) : QAbstractListModel(_Parent)
{
	ThumbnailMaxSize = QSize(150, 100);
	loadFonts();
}

int STFontThumbnailModel::rowCount(const QModelIndex & parent) const
{
	return Fonts.size();
}

QVariant STFontThumbnailModel::data(const QModelIndex& _Index, int _Role) const
{
	QVariant Res;
	//Hem de tenir en compte: Exif i QPixmapCache
	if (_Index.isValid())
	{
		if (_Index.row() < Fonts.size())
		{
			QFont Font = Fonts[_Index.row()];
			if (_Role == Qt::DecorationRole)
			{
// 				QPixmap ColPixmap(ThumbnailMaxSize);
// 				ColPixmap.fill(QColor("#000000"));
// 				Res = ColPixmap;

				Res = createThumbNail(Font);
			}
			else 
			if (_Role == Qt::DisplayRole)	
			{
				Res = Font.family() + " - " + fontStyleToString(Font.style());
			}
			else 
			if (_Role == FontRole)
			{
				Res = Font;
			}
		}
	}
	return Res;

}

QVariant STFontThumbnailModel::headerData ( int section, Qt::Orientation _Orientation, int _Role) const
{
	return "Font";
}

void STFontThumbnailModel::setThumbnailMaxSize(QSize _Value)
{
	ThumbnailMaxSize = _Value;
}

QSize STFontThumbnailModel::thumbnailMaxSize() const
{
	return ThumbnailMaxSize;
}		


QString STFontThumbnailModel::fontStyleToString(const QFont::Style _Style)
{
	QString Res;
	switch(_Style)
	{
		case QFont::StyleNormal : 
			Res = tr("normal");
		break;
		case QFont::StyleItalic : 
			Res = tr("italic");
		break;
		case QFont::StyleOblique : 
			Res = tr("italic");
		break;
	}
	return Res;
}