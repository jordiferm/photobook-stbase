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

#include "sprocessstatuswidget.h"
#include <QLayout> 
#include <QProgressBar> 
#include <QLabel>
#include <QDesktopWidget>

#include "qxtprogresslabel.h"

SProcessStatusWidget::SProcessStatusWidget(QWidget* _Parent, Qt::WindowFlags _Flags, Qt::Orientation _Orientation )
 : QWidget(_Parent, _Flags)
{	
	QLayout* MLayout;
	if (_Orientation == Qt::Vertical)
		MLayout = new QVBoxLayout(this); 
	else 
	{
		MLayout = new QHBoxLayout(this); 
		MLayout->setMargin(0); 
	}
	
	LabStatus = new QxtProgressLabel(this); 
	MLayout->addWidget(LabStatus);

	LabPixmap = new QLabel(this);
	LabPixmap->setAlignment(Qt::AlignCenter);
	MLayout->addWidget(LabPixmap);
	LabPixmap->hide();
	
	MProgressBar = new QProgressBar(this); 
	MProgressBar->setVisible(false);
   connect(MProgressBar, SIGNAL(valueChanged(int)), LabStatus, SLOT(setValue(int)));
	MLayout->addWidget(MProgressBar);
}

QProgressBar* SProcessStatusWidget::progressBar() const
{
	return MProgressBar;
}

void SProcessStatusWidget::moveToScreenCenter()
{
	QDesktopWidget Desktop;
	QRect ScreenRect = Desktop.screenGeometry(this);
	move(QPoint(ScreenRect.center().x() - width() / 2, ScreenRect.center().y() - height() / 2));
}

void SProcessStatusWidget::showProgressBar(QString _Message, int _NumSteps, bool _ShowTimes)
{
	MProgressBar->setRange(0, _NumSteps); 
	MProgressBar->show();
	statusChanged(_Message, 0, _ShowTimes);
	if (_ShowTimes)
		restartTimes();
}

void SProcessStatusWidget::hideProgressBar()
{
	MProgressBar->hide();
}

/*!
	Restart times showed in Status Messages.
*/
void SProcessStatusWidget::restartTimes()
{
	LabStatus->restart();
}


/*!
	Returns current progress in steps
*/
int SProcessStatusWidget::currentProgress()
{
	return MProgressBar->value();
}

/*! 
	Updates the progress bar by one step
*/

void SProcessStatusWidget::incrementProgress()
{
	MProgressBar->setValue(MProgressBar->value() + 1);
}

/*!
	Overloaded function provided for convenience.
*/
void SProcessStatusWidget::updateProgress(int _Progress, QString _NewMessage)
{
	statusChanged(_NewMessage, _Progress);
}

void SProcessStatusWidget::showPixmap(const QPixmap& _Pixmap)
{
	LabPixmap->show();
	LabPixmap->setPixmap(_Pixmap);
}

		
/*!
	Updates _StatusMsg and ProgressBar with _Progress value. If _ShowTimes is true (The Default) and _Progress > -1, the label displays enlapsed and estimated times.
*/
void SProcessStatusWidget::statusChanged(const QString& _StatusMsg, int _Progress, bool _ShowTimes )
{
	QString StatusMessage;
	if (!_StatusMsg.isEmpty())
	{
		CurrStatusMessage = _StatusMsg; 
		StatusMessage = _StatusMsg; 
	}
	else 
		StatusMessage = CurrStatusMessage; 
	if (_ShowTimes && _Progress != -1)
		StatusMessage += tr(" - Enlapsed: %e - Estimated: %r");
	LabStatus->setContentFormat(StatusMessage); 
	if (_Progress != -1)
		MProgressBar->setValue(_Progress);
}
