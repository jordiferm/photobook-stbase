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
#ifndef STFTPORDERTRANSFERDIALOG_H
#define STFTPORDERTRANSFERDIALOG_H

#include <QDialog>
#include "stdomexport.h"

class STFtpStatusWidget;
class SProcessStatusWidget;
class QLabel;
class QPushButton;

namespace STDom
{

/**
	@author Shadow
	Dialog to transfer Aili print orders.
*/
class STFtpOrderTransfer;
class Publisher;
class ST_DOM_EXPORT STFtpOrderTransferDialog : public QDialog
{
	Q_OBJECT;

	STFtpStatusWidget* FtpStatusWidget;
	SProcessStatusWidget* OverallStatusWidget;
	QLabel* CaptionLabel;
	QPushButton* ButCancel;
	QPushButton* ButAbort;
	STFtpOrderTransfer* Ftp;
	bool HasError, Aborted;

public:
	STFtpOrderTransferDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~STFtpOrderTransferDialog();
	void transferOrder(const QString& _OrderId, const Publisher& _Publisher);
	static void execTransferOrder(const QString& _OrderId, QWidget* _Parent, const Publisher& _Publisher);
	bool hasError() const { return HasError; }
	bool aborted() const { return Aborted; }
	
private slots:	
	void setError(const QString& _Error);
	void updateOverallProcess(int _Total, int _Done);
	void abortTarnsfer();
};
} //namespace
#endif
