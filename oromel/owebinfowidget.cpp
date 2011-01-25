/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "owebinfowidget.h"
#include <QLayout>
#include <QProgressBar>
#include <QToolBar>
#include <QAction>
#include <QtWebKit/QWebView>
#include <QDesktopServices>
#include "siconfactory.h"


OWebInfoWidget::OWebInfoWidget(QWidget* _Parent) : QWidget(_Parent)
{
	setObjectName("OWebInfoWidget");
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint );
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setSpacing(0);
	MLayout->setMargin(0);

	WebView = new QWebView(this);
	connect(WebView, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
	connect(WebView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool )));
	connect(WebView, SIGNAL(linkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));
	MLayout->addWidget(WebView);

	QHBoxLayout* BottomLayout = new QHBoxLayout;
	MLayout->addLayout(BottomLayout);
	BottomLayout->setMargin(5);

	ProgBar = new QProgressBar(this);
	ProgBar->setRange(0, 100);
	connect(WebView, SIGNAL(loadProgress(int)), ProgBar, SLOT(setValue(int)));
	ProgBar->setVisible(false);
	BottomLayout->addWidget(ProgBar);

	BottomLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));

	QToolBar* TBActions = new QToolBar(this);
	TBActions->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	BottomLayout->addWidget(TBActions);

	QAction* QuitAction = new QAction(stIcon(SIconFactory::Exit), tr("Close"), this);
	connect(QuitAction, SIGNAL(triggered()), this, SLOT(close()));
	TBActions->addAction(QuitAction);
}

void OWebInfoWidget::openUrl(const QUrl& _Url)
{
	WebView->load(_Url);
	WebView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
}

void OWebInfoWidget::slotLoadStarted()
{
	ProgBar->show();
}

void OWebInfoWidget::slotLoadFinished(bool _Ok)
{
	ProgBar->hide();
	if (!_Ok && !LoadErrorHtml.isEmpty())
		WebView->setHtml(LoadErrorHtml);
}

void OWebInfoWidget::slotLinkClicked(const QUrl& _Url)
{
	if (_Url.isValid())
		QDesktopServices::openUrl(_Url);
}
