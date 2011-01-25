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
	bool areEmpty() const { return !contains("name") || !contains("id"); }
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
	void initializePage();
	bool validatePage();
	bool forgetMe();
	int nextId() const;
};

/**
	Page to choose publisher.

	@author Shadow
*/

class QListView; 
class STDom::STCollectionPublisherModel;
class OPWChoosePublisher : public STOWizardPage
{
Q_OBJECT

	QListView* View; 
	STDom::STCollectionPublisherModel* Model;
	STDom::STCollectionPublisherInfo  PublisherInfo;

public:
	OPWChoosePublisher(QWidget* _Parent = 0);
	void initializePage();
	bool validatePage();
	bool isComplete() const;
	bool forgetMe();
	int nextId() const;
	STDom::STCollectionPublisherInfo publisherInfo() const { return PublisherInfo; }
};


/**
	Special comboBox with 3 buttons for Add, remove, reset actions.

	@author Shadow
*/


class QComboBox;
class QSpinBox; 
class QAbstractItemModel; 
class QToolButton;
class OPWSingleProdChooseWidget : public QWidget 
{
	QComboBox* MCBox;
	QToolButton* AddButton;
	QToolButton* RemoveButton;
	QToolButton* ResetButton;

Q_OBJECT
public:
	OPWSingleProdChooseWidget(QWidget* _Parent = 0); 
	void setModel(QAbstractItemModel* _Model); 
	void setProductIndex(int _Index);
	int productIndex() const; 

private slots:
	void buttonClicked(); 

signals:
	void incProduct(int _Index, int _Quantity); 
	void resetProduct(int _Index); 
};

/**
	Widget with n comboBoxes and buttons to choose digital print copies.

	@author Shadow
*/

class OPWProdChooseWidget : public QWidget 
{

Q_OBJECT
public:
	OPWProdChooseWidget(int _NumRows = 3, QWidget* _Parent = 0); 
	void setModel(QAbstractItemModel* _Model); 
	int firstProdIndex() const; 

signals:
	void incProduct(int, int);
	void resetProduct(int); 
};


/**
	Abstract class to choose products.

	@author Shadow
*/


class OPhotoCollectionImageModel;
class STProductPrintsProxyModel;
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
	OPWChoosePublisher* PublisherPage; 
	QAbstractItemModel* PModel;
	OPhotoCollectionImageModel* ImageModel;
	STDom::PrintJobModel* MProxyModel;
	STDom::STFtpOrderTransfer* FtpTrans;
	STDom::STXmlPublisherSettings SavedPubSettings;
	STDom::PublisherDatabase::EnProductType ProductType;
	QString TemplateRef;
	
	void syncPublisherData();
	void getPublisherData();

public:
	OPWAbstractChooseProduct(OPWChoosePublisher* _PublisherPage,  QWidget* _Parent = 0);
	void setProductType(STDom::PublisherDatabase::EnProductType _ProdType) { ProductType = _ProdType; }
	void setTemplateRef(const QString& _TemplateRef) { TemplateRef = _TemplateRef; }
	void initialize(const QFileInfoList& Images);
	STProductPrintsProxyModel* prodPrintsModel() const;
	virtual int nextId() const;
	virtual void initializePage() = 0;
	virtual bool isComplete() const = 0;
	STDom::STXmlPublisherSettings publisherSettings() const { return SavedPubSettings; }
};


/**
	Page to choose product of type digiprints.

	@author Shadow
*/

class OPWChooseDigiprintProduct : public OPWAbstractChooseProduct
{
Q_OBJECT

private:
	OPWProdChooseWidget* WProducts;
	QListView* LView;
	
public:
	OPWChooseDigiprintProduct(OPWChoosePublisher* _PublisherPage,  QWidget* _Parent = 0);
	QSize sizeHint() const;
	virtual void initializePage();
	virtual bool isComplete() const;

private slots: 
	void resetSelectedProduct(int _Index);
	void incSelectedProduct(int _Index, int _Quantity); 
};


/**
	Page to choose product of type photobook.

	@author Shadow
*/

class OPWChooseAtomicProduct : public OPWAbstractChooseProduct
{
Q_OBJECT

private:	
	QComboBox* CBoxModel;
	QSpinBox* SBoxNCopies;

public:
	OPWChooseAtomicProduct(OPWChoosePublisher* _PublisherPage,  QWidget* _Parent = 0);
	virtual int nextId() const;
	virtual void initializePage();
	virtual bool isComplete() const;

private slots: 
	void updateSelectedProduct();
};

/**
	Page to choose crop mode.

	@author Shadow
*/

class OPWChooseCropModePage : public ChooseCropModePage
{
Q_OBJECT 
public: 
	enum EnCropMode
	{
		ModeUserSelectCrop,
		ModeWhiteMargin,
		ModeWhiteMarginCentered,
		ModeNoModify
	};

private:
	QRadioButton* RBNoModify;

public:
	OPWChooseCropModePage(QWidget* _Parent = 0);
	int nextId() const;
	EnCropMode cropMode(); 
};

/**
	Page to select image crops.

	@author Shadow
*/


class OPWSelectCropsPage : public SelectCropsPage
{
public:
	OPWSelectCropsPage(QWidget* _Parent = 0) : SelectCropsPage(_Parent) {}
	int nextId() const;
};

/**
	Page to choose shipping method.

	@author Shadow
*/

class QSqlTableModel;
class OPWChooseShippingMethod : public STOWizardPage
{
Q_OBJECT

	QComboBox* CBSMethods;
	QSqlTableModel* PModel;

public:
	OPWChooseShippingMethod(QWidget* _Parent = 0);
	void initializePage();
	bool forgetMe();
	int nextId() const;
	QSqlRecord currentShippingMethod() const;
};

/**
	Page to choose pay method.

	@author Shadow
*/

class QSqlTableModel;
class OPWChoosePayMethod : public STOWizardPage
{
Q_OBJECT
public:
	ST_DECLARE_ERRORCLASS();

private:
	QLabel* BillLabel; 
	QRadioButton* RBCOnDel;
	QRadioButton* RBCredCard;
	QRadioButton* RBPayPal;
	OPWChoosePublisher* PublisherPage;

public:
	OPWChoosePayMethod(OPWChoosePublisher* _PublisherPage, QWidget* _Parent = 0);
	void initializePage();
	bool isComplete () const;
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
	OPWSelectCropsPage* SelCropPage;
	OPWAbstractChooseProduct* ProductPage;
	OPWChoosePayMethod* SPayMPage; 
	OPWChooseShippingMethod* SMethPage;
	OPWChoosePublisher* PublisherPage;
	OPWConfirmOrder* ConfirmOrderPage; 
	OPWChooseCropModePage* SelCropModePage;
	QPushButton* ButConfirmAndSend;
	int NumImages;
	bool AtomicOrder;
	static int DPIS; 
	
public:	
	enum { Page_Welcome, Page_UserData, Page_ChoosePublisher, Page_ChooseProduct, Page_ChooseCropMode, Page_SelectCrops, Page_ChooseShipMethod, Page_ChoosePayMethod, Page_ConfirmOrder};

	static const QString PublisherDBConnectionName; 
	int nonForgetId(int _FromId) const;

public:	
	STOrderPrintsWizard(bool _AtomicOrder, QWidget* parent = 0, Qt::WindowFlags flags = 0);
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
	STDom::STXmlPublisherSettings publisherSettings() const;
	
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
class OPWConfirmOrder: public STOWizardPage
{
Q_OBJECT

public:
	ST_DECLARE_ERRORCLASS();

private:
	QLabel* BillLabel; 
	SProcessStatusWidget* StatusWidg;
	OPWSelectCropsPage* CropsPage; 
	STProductPrintsProxyModel* ProdPrModel;
	QFileInfo PublisherXmlFile;
	QString LastOrderRef;
	OPWChooseCropModePage::EnCropMode CropMode;
	QTextEdit* SenderOrderTE;

public:
	OPWConfirmOrder(OPWSelectCropsPage* _CropsPage, QWidget* _Parent = 0);
	void calcBill(STProductPrintsProxyModel* _ProdPrModel, const QSqlRecord& _ShippingMethod); 
	void initialize(STProductPrintsProxyModel* _ProdPrModel, const STDom::STCollectionPublisherInfo& _PubInfo, OPWChooseCropModePage::EnCropMode _CropMode);
	bool validatePayment();
	QString newOrderRef();
	STDom::XmlOrderDealer sender();
	STDom::XmlOrderDealer customer();
	bool storeImages();
	bool validatePage();
	QString lastOrderRef() const { return LastOrderRef; }
	
signals:
	void getImagesToSend(QFileInfoList& ImagesToSend, SProcessStatusWidget* _StatusWidget);		
};



#endif
