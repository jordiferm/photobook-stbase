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

#ifndef OWEBINFOWIDGET_H
#define OWEBINFOWIDGET_H

#include <QWidget>
#include "storomelexport.h"

class QWebView;
class QProgressBar;
class QUrl;
class ST_OROMEL_EXPORT OWebInfoWidget : public QWidget
{
	Q_OBJECT

	QWebView *WebView;
	QProgressBar* ProgBar;
	QString LoadErrorHtml;

public:
	OWebInfoWidget(QWidget* _Parent = 0);
	void openUrl(const QUrl& _Url);
	void setLoadErrorHtml(const QString& _Value) { LoadErrorHtml = _Value; }

private slots:
	void slotLoadStarted();
	void slotLoadFinished(bool _Ok);
	void slotLinkClicked(const QUrl& _Url);

};

#endif // OWEBINFOWIDGET_H
