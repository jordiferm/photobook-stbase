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
#ifndef STFTPSTATUSWIDGET_H
#define STFTPSTATUSWIDGET_H

#include <QWidget>
#include "stwidgetsexport.h"

/**
Widget to inform of a QFtp status.

	@author
*/
class QFtp;
class QLabel; 
class SProcessStatusWidget;
class ST_WIDGETS_EXPORT  STFtpStatusWidget : public QWidget
{
	Q_OBJECT
	QLabel* LabOperation;
	SProcessStatusWidget* MProcStatusWidget;
	qint64 AbsoluteTotalBytes;
	qint64 DoneAbsoluteTotalBytes;
	qint64 DoneAnt;
	bool ShowConnectionStatusText;

public:
	STFtpStatusWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	void connectFtp(QFtp* _Ftp);
	void disconnectFtp(QFtp* _Ftp);
	//! If != 0 progress values uses it for total instead of _Done parameter in dataTransferProgress method.
	void setAbsoluteTotalBytes(qint64 _Value);
	void setStatusText(const QString& _Text);
	void showConnectionStatusText(bool _Value) { ShowConnectionStatusText = _Value; }

private slots:
	void stateChanged(int _State);
	void dataTransferProgress(qint64 _Done, qint64 _Total);

};

#endif
