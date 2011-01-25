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
#include "stphotobookinfowidget.h"
#include <QLayout> 
#include <QLabel> 
#include <QTextStream> 

#include "stphotobook.h" 

STPhotoBookInfoWidget::STPhotoBookInfoWidget(QWidget *parent)
 : QWidget(parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setMargin(0); 
	MLayout->setSizeConstraint(QLayout::SetMinAndMaxSize); 
	InfoLabel = new QLabel(this); 
	MLayout->addWidget(InfoLabel); 
	InfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft); 
	InfoLabel->setWordWrap(true);
}

void STPhotoBookInfoWidget::setPhotoBook(const STPhotoBook* _PhotoBook)
{
	MPhotoBook = _PhotoBook; 
	updateInfo(); 
}

void STPhotoBookInfoWidget::updateInfo()
{
	if (MPhotoBook)
	{
		QString LabText; 
		QTextStream Strm(&LabText); 
		Strm << "<h1 align=center ><font color=blue>Information</font></h1>"; 
		Strm << "<ul>";
		Strm << "<li><b>Name:</b>" << MPhotoBook->name() << "</li>";
		Strm << "<li><b>Description:</b><br/>" << MPhotoBook->description() << "</li>";
		QSizeF PBSize = MPhotoBook->photoBookTemplate().size();
		Strm << "<li><b>Size:</b>" << QString("%1 mm x %2 mm").arg(PBSize.width()).arg(PBSize.height()) << "</li>";
		
		
		Strm << "<li><b>Num of Pages:</b>" << MPhotoBook->pages().size() << "</li>";
		Strm << "<li><b>Min of Pages:</b>" << MPhotoBook->minPages() << "</li>";
		Strm << "<li><b>Max of Pages:</b>" << MPhotoBook->maxPages() << "</li>";

		QFileInfo PBXmlFileInfo(MPhotoBook->info().xmlFileName());
		Strm << "<li><b>Created:</b>" << PBXmlFileInfo.created().toString("dd/MM/yyyy hh:mm") << "</li>";
		Strm << "<li><b>Last Read:</b>" << PBXmlFileInfo.lastRead().toString("dd/MM/yyyy hh:mm") << "</li>";
		//Strm << "<li><b>Images at:<br/></b>" << MPhotoBook->sourceImagesPath() << "</li>";
		Strm << "</ul>";
		InfoLabel->setText(LabText); 
	}
	else 
		InfoLabel->setText("<h3>There's no available info.</h3>");
}
