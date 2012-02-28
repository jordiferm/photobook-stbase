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
#ifndef STICKETPRINTERSETTINGSWIDGET_H
#define STICKETPRINTERSETTINGSWIDGET_H

#include <QModelIndex> 
#include "smappedwidget.h"
#include "stticketprinterexport.h"
#include "sticketprintersettings.h"


class FileSelectLineEdit; 
class ST_TICKETPRINTER_EXPORT STicketPrinterSettingsSystemDriverWidget : public SMappedWidget
{
	Q_OBJECT

	SAppSettingsModel* Model;
	QModelIndex PrnNameIndex;
	QModelIndex ReportFileIndex;
	FileSelectLineEdit* LERFile;
			
public:	
	STicketPrinterSettingsSystemDriverWidget(STicketPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent = 0);
public slots: 
	void getPrinterName(); 
	void openReportEditor();
};


class ST_TICKETPRINTER_EXPORT SDirectPrintSettingsWidget : public SMappedWidget
{
	
			
public:	
	SDirectPrintSettingsWidget(STicketPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent = 0);
};

/**
	@author
*/
class QDataWidgetMapper;
class QRadioButton; 
class ST_TICKETPRINTER_EXPORT STicketPrinterSettingsWidget : public SMappedWidget
{
	Q_OBJECT
	QRadioButton* RBSysDriver;
	QRadioButton* RBPrinterMode;
	STicketPrinterSettingsSystemDriverWidget* SysSettingsWidget;
	SDirectPrintSettingsWidget* DirectPrintSettingsWidget; 
		
public:
	STicketPrinterSettingsWidget(STicketPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent = 0);

	~STicketPrinterSettingsWidget();
private slots:
	void mapperIndexChanged();
	void showSysDriver(bool _Show);
};

#endif
