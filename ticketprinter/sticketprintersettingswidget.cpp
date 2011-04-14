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
#include "sticketprintersettingswidget.h"
#include "sticketprintersettings.h"
#include <QLayout>
#include <QRadioButton>
#include <QDataWidgetMapper> 
#include <QButtonGroup>
#include <QLabel> 
#include <QPushButton> 
#include <QPrintDialog> 
#include <QPrinter> 
#include <QProcess> 
#include <QApplication> 
#include <QGroupBox> 
#include <QDir> 
#include "fileselectlineedit.h"
#include "smappablecombobox.h" 


//_____________________________________________________________________________
//
// Class STicketPrinterSettingsSystemDriverWidget
//_____________________________________________________________________________
STicketPrinterSettingsSystemDriverWidget::STicketPrinterSettingsSystemDriverWidget(STicketPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent) : SMappedWidget(_Settings.model(), _Mapper, _Parent)
{
	Model = _Settings.model();
	QGridLayout* MLayout = new QGridLayout(this); 
	MLayout->setSpacing(1); 
	PrnNameIndex = Model->index(_Settings.keyFullName("systemticketprintername")); 
	MLayout->addWidget(createHeaderLabel(PrnNameIndex), 0, 0);
	MLayout->addWidget(createEditor(PrnNameIndex), 1, 0);
	QPushButton* PBPDialog = new QPushButton(tr("System printer"), this); 
	connect(PBPDialog, SIGNAL(clicked( bool )), this, SLOT(getPrinterName())); 
	MLayout->addWidget(PBPDialog, 1, 1); 
	
	ReportFileIndex = Model->index(_Settings.keyFullName("ticketreportfile"));
	MLayout->addWidget(createHeaderLabel(ReportFileIndex), MLayout->rowCount(), 0);
	LERFile = new FileSelectLineEdit(FileSelectLineEdit::GetOpenFileName, this);
	_Mapper->addMapping(LERFile, ReportFileIndex.column());
	MLayout->addWidget(LERFile, MLayout->rowCount(), 0);
	QPushButton* PBRunDesigner = new QPushButton(tr("Design ticket"), this); 
	connect(PBRunDesigner, SIGNAL(clicked( bool )), this, SLOT(openReportEditor())); 
	MLayout->addWidget(PBRunDesigner, MLayout->rowCount() -1, 1);
	
	
}

void STicketPrinterSettingsSystemDriverWidget::getPrinterName()
{
	QPrinter Printer; 
	QPrintDialog PrnDialog(&Printer); 
	if (PrnDialog.exec() == QDialog::Accepted)
	{
		Model->setData(PrnNameIndex, Printer.printerName(), Qt::EditRole);
	}
}

void STicketPrinterSettingsSystemDriverWidget::openReportEditor()
{
#ifndef Q_OS_WIN32
	QString Program = QApplication::applicationDirPath() + "/openrptbin";
#else
	QString Program = QApplication::applicationDirPath() + "/openrptbin.exe";
#endif

	QStringList Arguments;
	//Arguments << Model->data(ReportFileIndex).toString();
	Arguments << LERFile->text(); 
	QProcess::execute(Program, Arguments);
}


//_____________________________________________________________________________
//
// Class SDirectPrintSettingsWidget
//_____________________________________________________________________________

SDirectPrintSettingsWidget::SDirectPrintSettingsWidget(STicketPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent )
: SMappedWidget(_Settings.model(), _Mapper, _Parent)
{
	SAppSettingsModel* Model = _Settings.model();
	QVBoxLayout* MLayout = new QVBoxLayout(this); 

	//--- DevicePrinter ---	
	QGroupBox* FGBDevicePrinter = new QGroupBox(headerLabelText(Model->index(_Settings.keyFullName("isdeviceprinter"))), this);
	FGBDevicePrinter->setCheckable(true);
	_Mapper->addMapping(FGBDevicePrinter, Model->index(_Settings.keyFullName("isdeviceprinter")).column());
	MLayout->addWidget(FGBDevicePrinter);

	QGridLayout* DPrinterLayout = new QGridLayout(FGBDevicePrinter); 

	DPrinterLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("ticketdevice")), 0, 0));
	SMappableComboBox* TicketDevEditor = new SMappableComboBox(this);
	_Mapper->addMapping(TicketDevEditor, Model->index(_Settings.keyFullName("ticketdevice")).column());
	
	TicketDevEditor->setEditable(true);
	QDir DevDir("/dev/");
	QStringList DeviceList = DevDir.entryList(QString("ttyS*,lp*,usb*").split(","), QDir::CaseSensitive | QDir::AllEntries | QDir::AllEntries | QDir::System);
	//QStringList DeviceList = DevDir.entryList(QDir::CaseSensitive | QDir::AllEntries | QDir::System);  
	for (QStringList::iterator it = DeviceList.begin(); it != DeviceList.end(); ++it)
			TicketDevEditor->addItem("/dev/" + *it);
	TicketDevEditor->addItem("/tmp/ticketprinter.txt");
	TicketDevEditor->setUseTextAsData(true);
	DPrinterLayout->addWidget(TicketDevEditor , 0, 1);
	
	DPrinterLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("ticketdevicetype"))), 1, 0);
	SMappableComboBox* TicketDevTypeEditor = new SMappableComboBox(this);
	_Mapper->addMapping(TicketDevTypeEditor, Model->index(_Settings.keyFullName("ticketdevicetype")).column());
	TicketDevTypeEditor->addItem("PrinterTypeEscPOS", STicketPrinterSettings::PrinterTypeEscPOS);
	TicketDevTypeEditor->addItem("PrinterTypeFastEscPOS", STicketPrinterSettings::PrinterTypeFastEscPOS);
	DPrinterLayout->addWidget(TicketDevTypeEditor, 1, 1);

	DPrinterLayout->addWidget(createHeaderLabel(Model->index(_Settings.keyFullName("boxconnector"))), 2, 0);
	SMappableComboBox* BoxConnectEditor = new SMappableComboBox(this);
	_Mapper->addMapping(BoxConnectEditor, Model->index(_Settings.keyFullName("boxconnector")).column());
	BoxConnectEditor->addItem("Conector al pin 2 ", 0);
	BoxConnectEditor->addItem("Conector al pin 5", 1);
	DPrinterLayout->addWidget(BoxConnectEditor, 2, 1);

}


//_____________________________________________________________________________
//
// Class STicketPrinterSettingsWidget
//_____________________________________________________________________________

STicketPrinterSettingsWidget::STicketPrinterSettingsWidget(STicketPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent) : SMappedWidget(_Settings.model(), _Mapper, _Parent)
{
	connect(_Mapper, SIGNAL(currentIndexChanged( int )), this, SLOT(mapperIndexChanged())); 	
	SAppSettingsModel* Model = _Settings.model();
	Model->fetch();
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	QHBoxLayout* HLayout = new QHBoxLayout; 
	MLayout->addLayout(HLayout); 
	
	QButtonGroup* MButtonGroup = new QButtonGroup(this); 
	RBPrinterMode = new QRadioButton(headerLabelText(Model->index(_Settings.keyFullName("directprintmode"))), this);
	HLayout->addWidget(RBPrinterMode);
	MButtonGroup->addButton(RBPrinterMode); 
	_Mapper->addMapping(RBPrinterMode, Model->index(_Settings.keyFullName("directprintmode")).column());
	RBSysDriver = new QRadioButton(tr("Use system printer driver"), this); 
	connect(RBSysDriver, SIGNAL(toggled( bool )), this, SLOT(showSysDriver(bool ))); 
	HLayout->addWidget(RBSysDriver); 
	MButtonGroup->addButton(RBSysDriver);
	
	SysSettingsWidget = new STicketPrinterSettingsSystemDriverWidget(_Settings, _Mapper, _Parent); 
	MLayout->addWidget(SysSettingsWidget);


	DirectPrintSettingsWidget = new SDirectPrintSettingsWidget(_Settings, _Mapper, _Parent);
	MLayout->addWidget(DirectPrintSettingsWidget);
	
	MLayout->addItem(new QSpacerItem(0, 10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding)); 
	showSysDriver(true);
	RBSysDriver->setChecked(true);
}


STicketPrinterSettingsWidget::~STicketPrinterSettingsWidget()
{
}

void STicketPrinterSettingsWidget::mapperIndexChanged()
{
	RBSysDriver->setChecked(!RBPrinterMode->isChecked()); 
}

void STicketPrinterSettingsWidget::showSysDriver(bool _Show)
{
	SysSettingsWidget->setVisible(_Show); 
	DirectPrintSettingsWidget->setVisible(!_Show); 
}
