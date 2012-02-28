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
#include "stftpstatuswidget.h"
#include <QLayout>
#include <QLabel>
#include <QFtp> 
#include <QProgressBar>
#include <QDebug>
#include "sprocessstatuswidget.h"

STFtpStatusWidget::STFtpStatusWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), AbsoluteTotalBytes(0), ShowConnectionStatusText(true)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	LabOperation = new QLabel(this); 
	MLayout->addWidget(LabOperation);

	MProcStatusWidget = new SProcessStatusWidget(this);
	MLayout->addWidget(MProcStatusWidget); 
}

void STFtpStatusWidget::connectFtp(QFtp* _Ftp)
{
	connect(_Ftp, SIGNAL(stateChanged(int )), this, SLOT(stateChanged(int )));
	connect(_Ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(dataTransferProgress(qint64, qint64 )));	
}

void STFtpStatusWidget::disconnectFtp(QFtp* _Ftp)
{
	disconnect(_Ftp, SIGNAL(stateChanged(int )), this, SLOT(stateChanged(int )));
	disconnect(_Ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(dataTransferProgress(qint64, qint64 )));
}

void STFtpStatusWidget::setAbsoluteTotalBytes(qint64 _Value)
{
	AbsoluteTotalBytes = _Value;
	DoneAbsoluteTotalBytes = 0;
	DoneAnt = 0;
}


void STFtpStatusWidget::stateChanged(int _State)
{
	QString StatusStr = tr("Unknown"); 
	switch (_State)
	{
		case QFtp::Unconnected :
			StatusStr = tr("Unconnected");
		break; 
		case QFtp::HostLookup :
			StatusStr = tr("HostLookup");
		break; 
		case QFtp::Connecting :
			StatusStr = tr("Connecting");
		break; 
		case QFtp::Connected :
			StatusStr = tr("Connected");
		break; 
		case QFtp::LoggedIn :
			StatusStr = tr("LoggedIn");
		break; 
		case QFtp::Closing :
			StatusStr = tr("Closing");
		break; 
	}
	if (ShowConnectionStatusText)
		LabOperation->setText(QString(tr("<b>Status: %1</b> ")).arg(StatusStr));
}

void STFtpStatusWidget::setStatusText(const QString& _Text)
{
	ShowConnectionStatusText = false;
	LabOperation->setText(_Text);
}

void STFtpStatusWidget::dataTransferProgress(qint64 _Done, qint64 _Total)
{
	qint64 Total;

	if (AbsoluteTotalBytes > 0)
	{
		Total = AbsoluteTotalBytes;
		if (_Done > 0)
		{
			qint64 Diff = _Done - DoneAnt;
			DoneAbsoluteTotalBytes = DoneAbsoluteTotalBytes + Diff;//(_Done - DoneAnt);
		}
		DoneAnt = _Done;
	}
	else
	{
		Total = _Total;
		DoneAbsoluteTotalBytes = _Done;
	}

	if (!MProcStatusWidget->progressBar()->isVisible())
		MProcStatusWidget->showProgressBar(QString(tr("Transfering (%1 of %2)")).arg(DoneAbsoluteTotalBytes).arg(Total), Total);
	else 
		MProcStatusWidget->progressBar()->setRange(0, Total);
	MProcStatusWidget->updateProgress(DoneAbsoluteTotalBytes, QString(tr("Transfering (%1 of %2)")).arg(DoneAbsoluteTotalBytes).arg(Total));
}
