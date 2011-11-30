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

#include "wprintermanager.h"

#include <QLayout>
#include <QGroupBox> 
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton> 

#include <QPrinter> 
#include <QPrintDialog> 
#include <QComboBox>
#include <QAbstractItemView>
#include <QFtp>
#include <QDebug>

#include "frecdialog.h"
#include "ftablemanager.h"
#include "fgenmanager.h"
#include "fsqlmodelviewutils.h" 
#include "globals.h"
#include "fsqlactiontableview.h"
#include "fsearchwidget.h" 
#include "kpprinterinfo.h"
#include "fileselectlineedit.h" 

#include "fsqlquery.h"
#include "fintvalueselector.h"


// _________________________________________________________________________*/
//
//	Class WPrinterRecord
// _________________________________________________________________________*/

void WPrinterRecord::createWidget()
{
	setMinimumWidth(300); 
	QGridLayout* MLayout = new QGridLayout(this);
	MLayout->addWidget(createEditorsGroup("name", new QGroupBox(tr("General"), this)), 0, 0);
	
	CBType = new FIntValueSelector(this);
	CBType->addItem(tr("Remote FTP Printer"), KPPrinterInfo::TypeRemoteFTP);
	CBType->addItem(tr("Hot Folder"), KPPrinterInfo::TypeHotFolder);
	CBType->addItem(tr("System printer"), KPPrinterInfo::TypeSystemPrinter);
	connect(CBType, SIGNAL(selectedValueChanged()), this, SLOT(typeChanged()));
	MLayout->addWidget(CBType, 1, 0);
	Mapper->addMapping(CBType, FSqlModelViewUtils::indexOf(Model,"type"));
	
	FTPPrinterEditors = createEditorsGroup("hosturl,ftplogin,ftppassword,ftpinitdir", new QGroupBox(tr("Remote FTP Printer"), this));

	// Hot Folder
	HotFolderEditors = new QGroupBox(tr("Hot Folder")); 
	QVBoxLayout* HFLayout = new QVBoxLayout(HotFolderEditors); 
	HFLayout->addWidget(createHeaderLabel("hotfolderurl"));
	FileSelectLineEdit* LEHotFolder = new FileSelectLineEdit(FileSelectLineEdit::GetExistingDirectory, this);
	Mapper->addMapping(LEHotFolder->lineEdit(), FSqlModelViewUtils::indexOf(Model,"hotfolderurl"));
	qDebug() << FSqlModelViewUtils::indexOf(Model,"hotfolderurl");
	HFLayout->addWidget(LEHotFolder);

	// System Printer
	SystemPrinterEditors = new QFrame(this); 
	QVBoxLayout* SPrnLayout = new QVBoxLayout(SystemPrinterEditors); 
	SPrnLayout->setMargin(0); 
	QPushButton* PBGetSystemPrinter = new QPushButton(tr("Get system printer settings"), SystemPrinterEditors);
	connect(PBGetSystemPrinter, SIGNAL(clicked()), this, SLOT(getSystemPrinterSettings())); 
	SPrnLayout->addWidget(PBGetSystemPrinter); 
	SPrnLayout->addWidget(createEditorsGroup("systemname", new QGroupBox(tr("System printer settings"), this)));
	
	if (QLineEdit* PasswEditor = qobject_cast<QLineEdit*>(fieldEditor("ftppassword")))
		PasswEditor->setEchoMode(QLineEdit::PasswordEchoOnEdit);
	
	if (QGridLayout* FtpLayout = qobject_cast<QGridLayout*>(FTPPrinterEditors->layout())) // Defensiva
	{
		FtpLayout->addWidget(createHeaderLabel("transfermode"), FtpLayout->rowCount(), 0);
		FIntValueSelector* CBTransferMode = new FIntValueSelector(this);
		CBTransferMode->addItem(tr("Active"), QFtp::Active);
		CBTransferMode->addItem(tr("Passive"), QFtp::Passive);
		Mapper->addMapping(CBTransferMode, FSqlModelViewUtils::indexOf(Model,"transfermode"));
		FtpLayout->addWidget(CBTransferMode, FtpLayout->rowCount() -1, 1);
	}
	
 	FTPPrinterEditors->setEnabled(false);
 	HotFolderEditors->setEnabled(false);
 	SystemPrinterEditors->setEnabled(false); 
	
	MLayout->addWidget(FTPPrinterEditors,2, 0);
	MLayout->addWidget(HotFolderEditors,3, 0);
	MLayout->addWidget(SystemPrinterEditors,4, 0);
}

WPrinterRecord::WPrinterRecord(FRecDialog* _Parent)
: FRecordWidget(_Parent->model(), _Parent)
{
}


void WPrinterRecord::typeChanged()
{
 	int SelType = CBType->value().toInt();
	//If we hide it Mapper has problems.
 	SystemPrinterEditors->setEnabled(SelType == KPPrinterInfo::TypeSystemPrinter); 
 	FTPPrinterEditors->setEnabled(SelType == KPPrinterInfo::TypeRemoteFTP);
 	HotFolderEditors->setEnabled(SelType == KPPrinterInfo::TypeHotFolder );
}

void WPrinterRecord::getSystemPrinterSettings()
{	
	QPrinter MPrinter; 
	QPrintDialog Dialog(&MPrinter); 
	if (Dialog.exec() == QDialog::Accepted)
	{
		Model->setData(Model->index(Mapper->currentIndex(), FSqlModelViewUtils::indexOf(Model,"systemname")), MPrinter.printerName());
// 	#ifdef Q_OS_WIN32
// 		Model->setData(Model->index(Mapper->currentIndex(), FSqlModelViewUtils::indexOf(Model,"papersize")), MPrinter.winPageSize());
// 	#else 
// 		Model->setData(Model->index(Mapper->currentIndex(), FSqlModelViewUtils::indexOf(Model,"papersize")), MPrinter.pageSize());
// 	#endif 
		  
	}
}

WPrinterManager::WPrinterManager(QWidget* _Parent)
 : FGenManager("printers", _Parent, "name,type,hotfolderurl,systemname", true)
{
	FRecDialog* MRecDialog = new FRecDialog(Model, TManager);
	WPrinterRecord* MRecWidget = new WPrinterRecord(MRecDialog);

	MRecDialog->setMainWidget(MRecWidget);
	TManager->setRecordWidget(MRecDialog);	
	TManager->enableSaveState("printersmanager");
	TManager->setEditable(false);
	connect(TManager->actionTableView(), SIGNAL(beforeRemoveRow(int , bool& )),
	        this, SLOT(beforeRemoveRow(int , bool& )));
}

void WPrinterManager::beforeRemoveRow(int _Index, bool& _PerformOp)
{
 	if (_PerformOp)
 	{
 		FSqlQuery Query;
 		QSqlRecord CurRec = Model->record(_Index);
 		//qDebug(QString("WDigiPrintManager::::beforeRemoveRow:" + CurRec.value("idproduct").toString()).toLatin1() );
 		_PerformOp = _PerformOp && Query.exec("DELETE FROM printersettings WHERE printers_idprinters='" + CurRec.value("idprinters").toString() + "'");
 		_PerformOp = _PerformOp && Query.exec("DELETE FROM jobs WHERE printers_idprinters='" + CurRec.value("idprinters").toString() + "'");
 	}	
}


