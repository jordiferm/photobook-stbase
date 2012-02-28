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

#ifndef TEMPLATEDOWNLOADDIALOG_H
#define TEMPLATEDOWNLOADDIALOG_H

#include <QDialog>
#include "stwidgetsexport.h"

class STProgressIndicator;
class STFtpStatusWidget;
class QLabel;
class ST_WIDGETS_EXPORT TemplateDownloadDialog : public QDialog
{
Q_OBJECT
	STProgressIndicator* GlobalProgressIndicator;
	STFtpStatusWidget* FtpStatusWidget;
	QLabel* MessageLabel;

public:
    explicit TemplateDownloadDialog(QWidget *parent = 0);
	STProgressIndicator* progressIndicator() const;
	STFtpStatusWidget* ftpStatusWidget() const;
	void setMessage(const QString& _Message);

signals:

public slots:

};

#endif // TEMPLATEDOWNLOADDIALOG_H
