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
#ifndef STORDERPRINTSWIZARD_H
#define STORDERPRINTSWIZARD_H

#include <QWizard>
#include <QFileInfo> 
#include <QSqlRecord> 
#include <QWidget> 
#include <QSettings> 

#include "stwizardsexport.h"
#include "sterror.h"
#include "stcollectionpublishermodel.h"
#include "stprintwizard.h"
#include "stxmlpublishersettings.h"
#include "publisherdatabase.h"
#include "stcollectionpublishermodel.h"
#include "xmlorder.h"
#include "publisherinfo.h"


/**
	Wizard Settings.

	@author Shadow
*/


class STOWizardSettings : public QSettings 
{
Q_OBJECT
public:
	STOWizardSettings()  { beginGroup("orderprintswizard"); }
	~STOWizardSettings() { endGroup(); }
	bool areEmpty() const { return !contains("name"); }
};

class STOWizardPage : public QWizardPage
{
Q_OBJECT

public:
	STOWizardPage(QWidget* _Parent = 0) : QWizardPage(_Parent) {}
	virtual bool forgetMe() { return false; }
};


/**
	Welcome page.

	@author Shadow
*/


class OPWWelcomePage : public STOWizardPage
{
Q_OBJECT
public:
	OPWWelcomePage(QWidget* _Parent = 0);
	bool forgetMe();
	int nextId() const;
};

class OPWUserDataPage : public STOWizardPage
{
Q_OBJECT
public:
	OPWUserDataPage(QWidget* _Parent = 0);
	void loadFieldSettings();
	void initializePage();
	bool validatePage();
	bool forgetMe();
	int nextId() const;
};


/**
	Abstract class to choose products.

	@author Shadow
*/


namespace STDom
{
	class STFtpOrderTransfer;
	class PrintJobModel;
	class DDocModel;
}

class OPWAbstractChooseProduct : public STOWizardPage
{
Q_OBJECT
public:
	ST_DECLARE_ERRORCLASS();

	
protected:
	QAbstractItemModel* PModel;
	STDom::PrintJobModel* PJModel;
	STDom::DDocModel* DocModel;
	STDom::PublisherDatabase::EnProductType ProductType;
	QString TemplateRef;
	STDom::PublisherInfo PublisherInfo;
	
	void getPublisherData();

public:
	OPWAbstractChooseProduct(const STDom::PublisherInfo& _PubInfo,  QWidget* _Parent = 0);
	void setProductType(STDom::PublisherDatabase::EnProductType _ProdType) { ProductType = _ProdType; }
	void setTemplateRef(const QString& _TemplateRef) { TemplateRef = _TemplateRef; }
	void initialize(const QFileInfoList& Images);
	STDom::PrintJob printJob() const;
	virtual int nextId() const;
	virtual void initializePage() = 0;
	virtual bool isComplete() const = 0;
	virtual void showError(const STError& _Error) = 0;
};


/**
	Page to choose product of type digiprints.

	@author Shadow
*/

class OPWChooseDigiprintProduct : public OPWAbstractChooseProduct
{
Q_OBJECT

private:
	TPPhotoSelWidget* PhotoSelW;

public:
	OPWChooseDigiprintProduct(const STDom::PublisherInfo& _PubInfo, QWidget* _Parent = 0);
	QSize sizeHint() const;
	virtual void initializePage();
	virtual bool isComplete() const;
	virtual void showError(const STError& _Error);
};


/**
	Page to choose product of type photobook.

	@author Shadow
*/

class QComboBox;
class QSpinBox;
class QFrame;
class OPWChooseAtomicProduct : public OPWAbstractChooseProduct
{
Q_OBJECT

private:	
	QComboBox* CBoxModel;
	QSpinBox* SBoxNCopies;
	bool HasError;
	QFrame* MainFrame;

public:
	OPWChooseAtomicProduct(const STDom::PublisherInfo& _PublisherInfo,  QWidget* _Parent = 0);
	virtual int nextId() const;
	virtual void initializePage();
	virtual bool isComplete() const;
	virtual void showError(const STError& _Error);

private slots: 
	void updateSelectedProduct();
};



/**
	Page to choose send mode.

	@author Shadow
*/

class OPWChooseSendMode : public STOWizardPage
{
Q_OBJECT
	QRadioButton* RBInteSent;
	QRadioButton* RBLocalStore;
	bool AllowInetSend;

public:
	OPWChooseSendMode(QWidget* _Parent = 0);
	void setAllowInetSend(bool _Value);
	void initializePage();
	bool inetSend() const;
	bool forgetMe();
	int nextId() const;
};

/**
	Wizard to order prints, photobooks and gifts.

	@author Shadow
*/

class QPushButton;
class OPWConfirmOrder; 
class ST_WIZARDS_EXPORT STOrderPrintsWizard : public QWizard
{
Q_OBJECT

private:
	QFileInfoList ImagesToSend; 
	OPWAbstractChooseProduct* ProductPage;
	OPWConfirmOrder* ConfirmOrderPage; 
	OPWChooseSendMode* SendModePage;
	QPushButton* ButConfirmAndSend;
	int NumImages;
	bool AtomicOrder;
	static int DPIS; 
	QWidget* createContactWidget();

public:	
	enum { Page_Welcome, Page_UserData, Page_ChooseProduct, Page_ChooseSendMode, Page_ConfirmOrder};

	static const QString PublisherDBConnectionName; 
	int nonForgetId(int _FromId) const;

public:	
	STOrderPrintsWizard(bool _AtomicOrder, const STDom::PublisherInfo& _PublisherInfo , QWidget* parent = 0, Qt::WindowFlags flags = 0);
	void setProductType(STDom::PublisherDatabase::EnProductType _ProdType);
	void setTemplateRef(const QString& _TemplateRef);
	void setImages(const QFileInfoList& _Images); 
	void clearImages();
	//!Usefull if we use getImagesToSend 
	void setNumImages(int _NImages) { NumImages = _NImages; }
	QString sendedOrderRef() const;
	static void setDPIS(int _Value) { DPIS = _Value; }
	static int dpis() { return DPIS; }
	void initializePage(int _Id);
	int nextId () const;
	void setAllowInetSend(bool _Value);
	bool inetSend() const;
	
signals:
	void getImagesToSend(QFileInfoList& ImagesToSend, SProcessStatusWidget* _StatusWidget);
};

/**
	Order confirmation page.

	@author Shadow
*/


class XmlOrderDealer;
class SProcessStatusWidget;
class QTextEdit;
class QLabel;
class QGroupBox;
class OPWConfirmOrder: public STOWizardPage
{
Q_OBJECT

public:
	ST_DECLARE_ERRORCLASS();

	enum EnShipOption
	{
		ShipOptionByPost = 0,
		ShipOptionCollectInStore
	};

private:
	QLabel* BillLabel; 
	SProcessStatusWidget* StatusWidg;
	QString LastOrderRef;
	QTextEdit* SenderOrderTE;
	STDom::PrintJob PrintJob;
	bool SendViaInternet;
	STDom::PublisherInfo PublisherInfo;
	QLabel* ContactLabel;
	QComboBox* CBPaymentType;
	QComboBox* CBShipOption;
	QComboBox* CBShippingMethod;
	QComboBox* CBCollectionPoint;
	QGroupBox* ByPostGBox;
	QGroupBox* CollectInStoreGBox;
	QLabel* ShippingAddressLabel;
	QLabel* CollectionPointAddressLabel;
	OPWUserDataPage* UserDataPage;

	QWidget* createContactWidget();
	QWidget* createShipmentOptionsWidget();
	QWidget* createOtherOptionsWidget();
	QWidget* createBillOptionsWidget();
	void updateContactInfo();
	void updateContactAddress();

public:
	OPWConfirmOrder(const STDom::PublisherInfo& _PublisherInfo, OPWUserDataPage* _UserDataPage, QWidget* _Parent = 0);
	void initialize(const STDom::PrintJob& _Job);
	bool validatePayment();
	void sendViaInternet(bool _Value);
	QString newOrderRef();
	STDom::XmlOrderDealer sender();
	STDom::XmlOrderDealer customer();
	void storeImages();
	bool validatePage();
	QString lastOrderRef() const { return LastOrderRef; }
	EnShipOption currentShipOption();
	STDom::ShippingMethod currentShippingMethod();
	STDom::CollectionPoint currentCollectionPoint();
	STDom::PaymentType currentPaymentType();

private slots:
	void slotShipOptionIndexChanged(int _Index);
	void slotUpdateCollectionPointAddress();
	void slotCalcBill();
	void slotSenderSettings();

signals:
	void getImagesToSend(QFileInfoList& ImagesToSend, SProcessStatusWidget* _StatusWidget);		
};



#endif
