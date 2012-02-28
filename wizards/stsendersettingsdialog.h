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
#ifndef STSENDERSETTINGSDIALOG_H
#define STSENDERSETTINGSDIALOG_H

#include <QWizard>
#include "storderprintswizard.h" 
#include "stwizardsexport.h"

class ST_WIZARDS_EXPORT MyOPWUserDataPage : public OPWUserDataPage
{
public:
	MyOPWUserDataPage(QWidget* _Parent = 0) : OPWUserDataPage(_Parent) {}
	int nextId() const { return -1; }
};

/**
Dialog for sender settings.

	@author
*/
class ST_WIZARDS_EXPORT STSenderSettingsDialog : public QWizard
{
	Q_OBJECT

public:
	STSenderSettingsDialog(QWidget *parent = 0);
	~STSenderSettingsDialog();
};

#endif
