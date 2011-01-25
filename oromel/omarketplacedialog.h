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

#ifndef OMARKETPLACEDIALOG_H
#define OMARKETPLACEDIALOG_H

#include <QDialog>
#include "sterror.h"
#include "stxmlpublishersettings.h"
#include "storomelexport.h"

class OMItemListModel;
class QActionGroup;
class QWebView;
class QFrame;
class QLabel;
class QTimer;
class QModelIndex;
class QListView;
class STFtpStatusWidget;
class QDialogButtonBox;
class QToolBar;
class ST_OROMEL_EXPORT OMarketPlaceDialog : public QDialog
{
Q_OBJECT
public:
	enum EnTypeActions
	{
		TANew,
		TAInstalled,
		TAUpdate
	};

	enum EnState
	{
		StateShowWebInfo,
		StateNoInfo,
		StateGettingInfo,
		StateTemplatesEmpty
	};


	QWebView* WebView;
	OMItemListModel* Model;
	QActionGroup* TypeActions;
	EnTypeActions SelectedTypeAction;
	QFrame* NoInfoFrame;
	QFrame* BottomFrame;
	QLabel* NoInfoLabel;
	QLabel* LabelInfoPixmap;
	QLabel* NoTemplatesLabel;
	QLabel* DescriptionLabel;
	QTimer* InetgetTimer;
	QListView* LView;
	QPushButton* InstallButton;
	QDialogButtonBox* ButtonBox;
	QToolBar* TBFilter;
	bool InetgetPicture1;
	STFtpStatusWidget* StatusWidget;
	bool ResourcesChanged;

public:
	ST_DECLARE_ERRORCLASS();


private:
	void updateItemsModel();
	void setCurrentState(OMarketPlaceDialog::EnState _State);
	void uninstallCurrent();
	void updateCurrent();
	void installCurrent();
	void setActionsEnabled(bool _Enabled);


public:
    explicit OMarketPlaceDialog(QWidget *parent = 0);
	void sync();
	bool resourcesChanged() const { return ResourcesChanged; }
	void checkForMarketPlaceUpdates();

public slots:
	int exec();


private slots:
	void slotTypeActionTriggered(QAction* _Action);
	void slotTemplateIndexClicked(const QModelIndex& _Index);
	void slotWebLoadStarted();
	void slotWebLoadFinished(bool _Ok);
	void slotInetgetBlinkTimeout();
	void slotInstallCliked();
};

#endif // OMARKETPLACEDIALOG_H
