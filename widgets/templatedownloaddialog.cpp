/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "templatedownloaddialog.h"
#include <QLabel>
#include <QLayout>
#include "stprogressindicator.h"
#include "stftpstatuswidget.h"
#include "sprocessstatuswidget.h"

TemplateDownloadDialog::TemplateDownloadDialog(QWidget *parent) :
    QDialog(parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MessageLabel = new QLabel(this);
	MLayout->addWidget(MessageLabel);

	SProcessStatusWidget* GlobalProcessStatus = new SProcessStatusWidget(this);
	GlobalProcessStatus->showProgressBar(tr("Dowloading..."), 100, false);
	MLayout->addWidget(GlobalProcessStatus);
	GlobalProgressIndicator = new STProgressIndicator(GlobalProcessStatus->progressBar());
	FtpStatusWidget = new STFtpStatusWidget(this);
	MLayout->addWidget(FtpStatusWidget);
}

STProgressIndicator* TemplateDownloadDialog::progressIndicator() const
{
	return GlobalProgressIndicator;
}

STFtpStatusWidget* TemplateDownloadDialog::ftpStatusWidget() const
{
	return FtpStatusWidget;
}

void TemplateDownloadDialog::setMessage(const QString& _Message)
{
	MessageLabel->setText(_Message);
}
