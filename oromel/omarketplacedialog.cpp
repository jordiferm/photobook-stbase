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

#include "omarketplacedialog.h"
#include <QLabel>
#include <QLayout>
#include <QToolBar>
#include <QAction>
#include <QListView>
#include <QWebView>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTimer>
#include <QFrame>
#include <QDebug>

#include "stcollectionpublishermodel.h"
#include "stxmlpublishersettings.h"
#include "omarketplace.h"
#include "smessagebox.h"
#include "stftpstatuswidget.h"
#include "siconfactory.h"

void OMarketPlaceDialog::updateItemsModel()
{
	OMarketPlace MarketPlace;

	switch(SelectedTypeAction)
	{
		case OMarketPlaceDialog::TANew:
			Model->showInstalledVer(false);
			Model->setItemList(MarketPlace.newItems());
			DescriptionLabel->setText(tr("<h3>New items to install</h3>"));
			InstallButton->setText(tr("Install"));
			InstallButton->setIcon(QIcon(":/st/oromel/install.png"));
		break;
		case OMarketPlaceDialog::TAInstalled:
			Model->showInstalledVer(true);
			Model->setItemList(MarketPlace.installed());
			DescriptionLabel->setText(tr("<h3>Installed items</h3>"));
			InstallButton->setText(tr("Uninstall"));
			InstallButton->setIcon(QIcon(":/st/oromel/uninstall.png"));
		break;
		case OMarketPlaceDialog::TAUpdate:
			Model->showInstalledVer(false);
			Model->setItemList(MarketPlace.updates());
			DescriptionLabel->setText(tr("<h3>Updates for installed items</h3>"));
			InstallButton->setText(tr("Update"));
			InstallButton->setIcon(QIcon(":/st/oromel/update.png"));
		break;
	}
	if (Model->rowCount() == 0)
		setCurrentState(OMarketPlaceDialog::StateTemplatesEmpty);
	else
	{
		QModelIndex FirstItem = Model->index(0, 0);
		//LView->selectionModel()->select(FirstItem, QItemSelectionModel::SelectCurrent);
		LView->setCurrentIndex(FirstItem);
		slotTemplateIndexClicked(Model->index(0, 0));
	}
}

void OMarketPlaceDialog::setCurrentState(OMarketPlaceDialog::EnState _State)
{
	InstallButton->setVisible(_State != StateTemplatesEmpty);
	BottomFrame->setVisible( _State != StateTemplatesEmpty);
	if (BottomFrame->isVisible())
	{
		NoInfoFrame->setVisible(_State != StateShowWebInfo);
		WebView->setVisible(!NoInfoFrame->isVisible());
	}
	InetgetTimer->stop();

	LView->setVisible(_State != StateTemplatesEmpty);
	NoTemplatesLabel->setVisible(_State == StateTemplatesEmpty);
	switch (_State)
	{
		case StateGettingInfo:
			NoInfoLabel->setText(tr("<h1>Getting info from internet.</h1>"));
			LabelInfoPixmap->setPixmap(QPixmap(":/inetget.png"));
			InetgetPicture1 = false;
			InetgetTimer->start();
		break;
		case StateNoInfo:
			NoInfoLabel->setText(tr("<h1>There is no information for this item.</h1>"));
			LabelInfoPixmap->setPixmap(QPixmap(":/dialog-information.png"));
		break;
	}
}

void OMarketPlaceDialog::uninstallCurrent()
{
	OMarketPlaceItem CurrItem = Model->item(LView->currentIndex());
	if (SMessageBox::yesNo(this, tr("Uninstalling Marketplace items"),
						   tr("Are you sure that you want to remove the package %1 from your computer ?").arg(CurrItem.Description)) == QMessageBox::Yes)
	{
		try
		{
			OMarketPlace MarketPlace;
			MarketPlace.uninstall(CurrItem);
			updateItemsModel();
			ResourcesChanged = true;
			SMessageBox::information(this, tr("Installing Marketplace items"), tr("Package has been successfull removed."));
		}
		catch(STError& _Error)
		{
			SMessageBox::critical(this, tr("Error installing Marketplace items"), _Error.description());
		}
	}
}

void OMarketPlaceDialog::updateCurrent()
{

}

void OMarketPlaceDialog::installCurrent()
{
	qApp->setOverrideCursor( QCursor(Qt::WaitCursor));
	setActionsEnabled(false);
	try
	{
		STDom::STXmlPublisherSettings PXmlS =  STDom::STXmlPublisherSettings::getDefaultSettings();

		OMarketPlace MarketPlace;

		OMarketPlaceItem CurrItem = Model->item(LView->currentIndex());
		StatusWidget->setStatusText(tr("<h2>Installing items, please wait...</h2>"));
		StatusWidget->setVisible(true);
		MarketPlace.install(CurrItem, PXmlS.dbHost(), PXmlS.dbPort(), PXmlS.dbUser(),
							PXmlS.dbPassword(), PXmlS.dbDir(),static_cast<QFtp::TransferMode>(PXmlS.dbTransferMode()),
							StatusWidget);
		ResourcesChanged = true;
		StatusWidget->setVisible(false);
		updateItemsModel();
		SMessageBox::information(this, tr("Installing marketplace items"), tr("Package has been successfull installed."));
	}
	catch(STError& _Error)
	{
		StatusWidget->setVisible(false);
		SMessageBox::critical(this, tr("Error installing marketplace items"), _Error.description());
	}
	qApp->restoreOverrideCursor();
	setActionsEnabled(true);
}

void OMarketPlaceDialog::setActionsEnabled(bool _Enabled)
{
	InstallButton->setEnabled(_Enabled);
	ButtonBox->setEnabled(_Enabled);
	TBFilter->setEnabled(_Enabled);
}


OMarketPlaceDialog::OMarketPlaceDialog(QWidget *parent) :
	QDialog(parent), SelectedTypeAction(OMarketPlaceDialog::TANew), ResourcesChanged(false)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	QLabel* HeaderLabel = new QLabel(this);
	HeaderLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	HeaderLabel->setText("<img src="":/st/oromel/starphobmarketplace64.png""><font size=""26"">Starphob MarketPlace</font></img>");
	MLayout->addWidget(HeaderLabel);

	TBFilter = new QToolBar(this);
	MLayout->addWidget(TBFilter);
	TBFilter->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	QAction* NewAction;
	TypeActions = new QActionGroup(this);
	TypeActions->setExclusive(true);

	NewAction = TypeActions->addAction(QIcon(":/st/oromel/mpnew.png"), tr("New"));
	NewAction->setCheckable(true);
	NewAction->setData(TANew);
	NewAction->setChecked(true);

	NewAction = TypeActions->addAction(QIcon(":/st/oromel/mpinstalled.png"), tr("Installed"));
	NewAction->setData(TAInstalled);
	NewAction->setCheckable(true);

	NewAction = TypeActions->addAction(QIcon(":/st/oromel/mpupdate.png"), tr("Update"));
	NewAction->setData(TAUpdate);
	NewAction->setCheckable(true);

	TBFilter->addActions(TypeActions->actions());
	connect(TypeActions, SIGNAL(triggered(QAction*)), this, SLOT(slotTypeActionTriggered(QAction*)));

	TBFilter->addSeparator();
	DescriptionLabel = new QLabel(this);
	TBFilter->addWidget(DescriptionLabel);

	NoTemplatesLabel = new QLabel(this);
	NoTemplatesLabel->setText(tr("<h2>There are no items.</h2>"));
	NoTemplatesLabel->setAlignment(Qt::AlignTop);
	NoTemplatesLabel->setWordWrap(true);
	MLayout->addWidget(NoTemplatesLabel);

	BottomFrame = new QFrame(this);
	MLayout->addWidget(BottomFrame);

	QHBoxLayout* ViewLayout = new QHBoxLayout(BottomFrame);

	LView = new QListView(this);
	LView->setIconSize(QSize(64, 64));
	LView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	ViewLayout->addWidget(LView);

	Model = new OMItemListModel(this);
	LView->setModel(Model);
	connect(LView, SIGNAL(clicked( const QModelIndex& )), this, SLOT(slotTemplateIndexClicked(const QModelIndex& )));

	WebView = new QWebView(this);
	WebView->setVisible(false);
	ViewLayout->addWidget(WebView);
	connect(WebView, SIGNAL(loadStarted()), this, SLOT(slotWebLoadStarted()));
	connect(WebView, SIGNAL(loadFinished(bool)), this, SLOT(slotWebLoadFinished(bool)));

	StatusWidget = new STFtpStatusWidget(this);
	MLayout->addWidget(StatusWidget);
	StatusWidget->setVisible(false);

	InstallButton = new QPushButton(this);
	InstallButton->setText(tr("Install"));
	InstallButton->setIconSize(QSize(64,64));
	InstallButton->setIcon(QIcon(":/st/oromel/install.png"));
	connect(InstallButton, SIGNAL(clicked()), this, SLOT(slotInstallCliked()));

	QHBoxLayout* BottomLayout = new QHBoxLayout;
	MLayout->addLayout(BottomLayout);
	BottomLayout->addWidget(InstallButton);

	ButtonBox = new QDialogButtonBox();
	QPushButton* ButOk = ButtonBox->addButton(QDialogButtonBox::Ok);
	ButOk->setIcon(stIcon(SIconFactory::Accept));
	ButOk->setIconSize(QSize(64,64));

	connect(ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	BottomLayout->addWidget(ButtonBox);

	NoInfoFrame = new QFrame(this);
	NoInfoFrame->setMinimumHeight(WebView->minimumHeight());
	NoInfoFrame->setMinimumWidth(WebView->minimumWidth());
	ViewLayout->addWidget(NoInfoFrame);

	QVBoxLayout* NoInfoFrameLayout = new QVBoxLayout(NoInfoFrame);
	NoInfoLabel = new QLabel(NoInfoFrame);
	NoInfoLabel->setWordWrap(true);
	NoInfoLabel->setAlignment(Qt::AlignCenter);
	NoInfoFrameLayout->addWidget(NoInfoLabel);
	LabelInfoPixmap = new QLabel(NoInfoFrame);
	LabelInfoPixmap->setAlignment(Qt::AlignCenter);
	NoInfoFrameLayout->addWidget(LabelInfoPixmap);
	InetgetTimer = new QTimer(this);
	InetgetTimer->setInterval(1000);
	connect(InetgetTimer, SIGNAL(timeout()), this, SLOT(slotInetgetBlinkTimeout()));

	setCurrentState(StateNoInfo);
}

int OMarketPlaceDialog::exec()
{
	ResourcesChanged = false;
	return QDialog::exec();
}

void OMarketPlaceDialog::sync()
{
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor));
	try
	{
		STDom::STXmlPublisherSettings PXmlS = STDom::STXmlPublisherSettings::getDefaultSettings();
		if (!PXmlS.isEmpty())
		{
			OMarketPlace MarketPlace;
			MarketPlace.sync(PXmlS.dbDir(), PXmlS.dbHost(), PXmlS.dbPort(), PXmlS.dbUser(), PXmlS.dbPassword(), PXmlS.dbDir(),static_cast<QFtp::TransferMode>(PXmlS.dbTransferMode()));

			updateItemsModel();
		}
		QApplication::restoreOverrideCursor();
	}
	catch(...)
	{
		QApplication::restoreOverrideCursor();
		throw;
	}
}

void OMarketPlaceDialog::slotTypeActionTriggered(QAction* _Action)
{
	SelectedTypeAction = static_cast<OMarketPlaceDialog::EnTypeActions>(_Action->data().toInt());
	updateItemsModel();
}


void OMarketPlaceDialog::slotTemplateIndexClicked(const QModelIndex& _Index)
{
	WebView->stop();
	setCurrentState(StateGettingInfo);

	//Get info url from model and display it.
	OMarketPlaceItem MPItem = Model->item(_Index);
	QUrl InfoUrl = QUrl(QString(MARKETPLACEINFO_BASE_URL) + "/" +  MPItem.UrlInfo);
	WebView->load(InfoUrl);
}

void OMarketPlaceDialog::slotWebLoadStarted()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void OMarketPlaceDialog::slotWebLoadFinished(bool _Ok)
{
	QApplication::restoreOverrideCursor();
	if (_Ok)
		setCurrentState(StateShowWebInfo);
	else
		setCurrentState(StateNoInfo);
}

void OMarketPlaceDialog::slotInetgetBlinkTimeout()
{
	if (InetgetPicture1)
		LabelInfoPixmap->setPixmap(QPixmap(":/inetget.png"));
	else
		LabelInfoPixmap->setPixmap(QPixmap(":/inetget_2.png"));

	InetgetPicture1 = !InetgetPicture1;
}


void OMarketPlaceDialog::slotInstallCliked()
{
	//Install current item
	switch(SelectedTypeAction)
	{
		case OMarketPlaceDialog::TANew:
			installCurrent();
		break;
		case OMarketPlaceDialog::TAInstalled:
			uninstallCurrent();
		break;
		case OMarketPlaceDialog::TAUpdate:
			installCurrent();
		break;
	}
}

void OMarketPlaceDialog::checkForMarketPlaceUpdates()
{
	bool Res = false;
	QDate LastSync = OMarketPlace::lastSyncDate();
	if (LastSync.daysTo(QDate::currentDate()) > 0)
	{
		try
		{
			sync();
		}
		catch(STError& _Error)
		{
			qDebug() << "Error syncing marketplace:" << _Error.description();
		}
	}

	if (OMarketPlace::isThereChanges())
	{
		QMessageBox MsgBox(QMessageBox::Information, QObject::tr("OMarketplace Info"), QObject::tr("There are new items in the market. Do you want to view them ?"));
		MsgBox.setIconPixmap(QPixmap(":/st/oromel/starphobmarketplace64.png"));
		QPushButton* ButYes = MsgBox.addButton(QMessageBox::Yes);
		ButYes->setIcon(stIcon(SIconFactory::Accept));
		QPushButton* ButNotNow = MsgBox.addButton(QObject::tr("Not now"), QMessageBox::AcceptRole);
		QPushButton* ButIgnore = MsgBox.addButton(QMessageBox::Ignore);
		ButIgnore->setIcon(stIcon(SIconFactory::Cancel));

		if (MsgBox.exec())
		{
			if (MsgBox.clickedButton() != ButNotNow)
			{
				if (MsgBox.clickedButton() == ButIgnore)
					OMarketPlace::clearChanges();
				else
				{
					OMarketPlace::clearChanges();
					updateItemsModel();
					showFullScreen();
					exec();
				}
			}
		}
	}
	QApplication::restoreOverrideCursor();//Problem in sync when app is not in main loop.
}
