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
#include "stftpordertransferdialog.h"
#include <QLayout> 
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDir> 
#include <QLabel> 
#include <QProgressBar>
#include "siconfactory.h"
#include "stftpstatuswidget.h"
#include "stftpordertransfer.h"
#include "sprocessstatuswidget.h"
#include "smessagebox.h"
#include "stutils.h"
#include "xmlorder.h"

using namespace STDom;

STFtpOrderTransferDialog::STFtpOrderTransferDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f), Ftp(0)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	CaptionLabel = new QLabel(tr("No action"), this); 
	MLayout->addWidget(CaptionLabel); 
	
	FtpStatusWidget = new STFtpStatusWidget(this); 
	MLayout->addWidget(FtpStatusWidget);	

	MLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));

	QDialogButtonBox* Buttons = new QDialogButtonBox(this);
	
	ButAbort = Buttons->addButton(tr("Abort"), QDialogButtonBox::ActionRole);
	connect(ButAbort, SIGNAL(clicked()), this, SLOT(abortTarnsfer()));
	ButAbort->setIcon(stIcon(SIconFactory::Cancel));
	ButAbort->setIconSize(QSize(32,32));

	OverallStatusWidget = new SProcessStatusWidget(this);
	MLayout->addWidget(OverallStatusWidget); 

 	ButCancel = Buttons->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
 	ButCancel->setIcon(stIcon(SIconFactory::Cancel));
	ButCancel->setVisible(false); 
	
	MLayout->addWidget(Buttons);
	//connect(Buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(Buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


STFtpOrderTransferDialog::~STFtpOrderTransferDialog()
{
	if (Ftp)
	{
		Ftp->abortAll();
		delete Ftp; 
	}
}

void STFtpOrderTransferDialog::transferOrder(const QString& _OrderId)
{
	bool Continue = true;
	HasError = false;
	Aborted = false; 
	while (!Aborted && Continue && !HasError)
	{
		try
		{
			HasError = false;
			Aborted = false;
			Continue = false;
			if (Ftp)
			{
				disconnect(Ftp, SIGNAL(transferError(const QString& )), this, SLOT(setError(const QString& ))); 
				disconnect(Ftp, SIGNAL(overallProcessStep(int , int )), this, SLOT(updateOverallProcess(int, int)));
				delete Ftp; 
			}
			Ftp = new STFtpOrderTransfer(this); 
			connect(Ftp, SIGNAL(transferError(const QString& )), this, SLOT(setError(const QString& ))); 
			connect(Ftp, SIGNAL(overallProcessStep(int , int )), this, SLOT(updateOverallProcess(int, int)));
			CaptionLabel->setText(QString(tr("Transfering order: %1")).arg(_OrderId)); 
			FtpStatusWidget->connectFtp(Ftp);
			Ftp->transferOrder(_OrderId); 
			accept();
		}
		catch(STError& _Error)
		{
			SMessageBox::warning(this, tr("Error transfering order"), _Error.description(), QMessageBox::Retry);
			Continue = true; 
			HasError = false; 
			//reject();
		}
	}
}

void STFtpOrderTransferDialog::execTransferOrder(const QString& _OrderId, QWidget* _Parent)
{
	STFtpOrderTransferDialog* OrderDialog = new  STFtpOrderTransferDialog(_Parent, Qt::Dialog); 
	OrderDialog->show();
	OrderDialog->transferOrder(_OrderId); 
	if (!OrderDialog->aborted() && ! OrderDialog->hasError()) //Defensiva.
	{
		SMessageBox::information(_Parent, tr("Order transfer info."), QString(tr("Order num %1 transfered succesfully.")).arg(_OrderId)); 
		//Lets remove order dir.
		STUtils::rmDir(QDir(XmlOrderInfo::defaultOrderPath(_OrderId)));
	}			
	delete OrderDialog; 	
}

void STFtpOrderTransferDialog::setError(const QString& _Error)
{
	HasError = true; 
	//ButCancel->setVisible(true);
	//ButAbort->setVisible(false);
	CaptionLabel->setText(QString(tr("<b><font color=red weight=bold>Error Transfering order: %1</font></b>")).arg(_Error)); 
}

void STFtpOrderTransferDialog::updateOverallProcess(int _Total, int _Done)
{
	if (!OverallStatusWidget->progressBar()->isVisible())
		OverallStatusWidget->showProgressBar(QString(tr("Overall process... (%1 of %2)")).arg(_Done).arg(_Total), _Total); 
	else 
		OverallStatusWidget->progressBar()->setRange(0, _Total); 
	OverallStatusWidget->updateProgress(_Done, QString(tr("Overall process... (%1 of %2)")).arg(_Done).arg(_Total));
}


void STFtpOrderTransferDialog::abortTarnsfer()
{
	if (SMessageBox::question(this, tr("Aborting transfer process."), tr("Are you sure to abort this process ?")) == QMessageBox::Yes)
	{
		Aborted = true; 
		Ftp->abortAll();
		reject();
	}
}

