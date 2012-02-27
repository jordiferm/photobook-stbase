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
#include "storderprintswizard.h"
#include <QLabel> 
#include <QLayout>
#include <QLineEdit> 
#include <QGroupBox> 
#include <QListView> 
#include <QComboBox>
#include <QDir> 
#include <QProcess>
#include <QProgressDialog>
#include <QApplication>
#include <QRadioButton>
#include <QSqlTableModel>
#include <QTextStream> 
#include <QPainter>
#include <QSpinBox> 
#include <QToolButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QDebug>

#include "smessagebox.h"
#include "sprocessstatuswidget.h"
#include "stimage.h"
#include "stutils.h"
#include "qxtgroupbox.h"

#include "stftpordertransfer.h"
#include "printjobmodel.h"
#include "tpphotoselwidget.h"
#include "stsendersettingsdialog.h"


//_____________________________________________________________________________
//
// class OPWWelcomePage
//_____________________________________________________________________________

OPWWelcomePage::OPWWelcomePage(QWidget* _Parent) : STOWizardPage(_Parent)
{
	setTitle(tr("<h1>Welcome to Starblitz Order Print wizard</h1>"));
	setPixmap(QWizard::WatermarkPixmap, QPixmap(":/st/wizards/watermark.png"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	QLabel* DescLabel = new QLabel(tr("This wizard will walk you throw the process of sending your photos to your publisher.\n Click the 'Next' button to begin."));
	DescLabel->setWordWrap(true); 
	MLayout->addWidget(DescLabel); 
}

bool OPWWelcomePage::forgetMe() 
{ 
	STOWizardSettings Settings; 
	return !Settings.areEmpty(); 
}


int OPWWelcomePage::nextId() const
{
	return STOrderPrintsWizard::Page_UserData;
}


//_____________________________________________________________________________
//
// class OPWUserDataPage
//_____________________________________________________________________________

OPWUserDataPage::OPWUserDataPage(QWidget* _Parent) : STOWizardPage(_Parent)
{
	QGridLayout* MLayout = new QGridLayout(this); 
	setTitle(tr("Enter or modify User Data"));
	setSubTitle(tr("Please enter your personal data or verify if it is correct. This data will be attached to your order to inform the order supplier."));

	MLayout->addWidget(new QLabel(tr("<b>Name:</b>"), this), 0, 0); 
	QLineEdit* NewLEdit = new QLineEdit(this); 
	registerField("name*", NewLEdit);
	MLayout->addWidget(NewLEdit, 0, 1);

	MLayout->addWidget(new QLabel(tr("<b>Surname:</b>"), this), 1, 0); 
	NewLEdit = new QLineEdit(this); 
	registerField("surname*", NewLEdit);
	MLayout->addWidget(NewLEdit, 1, 1);

	//-- Contact --
	QGroupBox* NewGBox = new QGroupBox(tr("Contact"), this); 
	MLayout->addWidget(NewGBox, MLayout->rowCount(), 0, 1, 4);
	QGridLayout* GBLayout = new QGridLayout(NewGBox); 
		
	GBLayout->addWidget(new QLabel(tr("e-mail:"), NewGBox), 0, 0); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("email", NewLEdit);
	GBLayout->addWidget(NewLEdit, 0, 1);

	GBLayout->addWidget(new QLabel(tr("<b>Phone:</b>"), this), 1, 0); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("phone*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 1, 1);

	GBLayout->addWidget(new QLabel(tr("Mobile phone:"), this), 1, 2); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("mobilephone", NewLEdit);
	GBLayout->addWidget(NewLEdit, 1, 3);

	//-- Address --
	NewGBox = new QGroupBox(tr("Send Address"), this); 
	MLayout->addWidget(NewGBox, MLayout->rowCount(), 0, 1, 4);
	GBLayout = new QGridLayout(NewGBox); 
		
	GBLayout->addWidget(new QLabel(tr("<b>Address:</b>"), NewGBox), 0, 0); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("address*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 0, 1);

	GBLayout->addWidget(new QLabel(tr("<b>Cp/Zip:</b>"), NewGBox), 1, 0); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("zip*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 1, 1);
	
	GBLayout->addWidget(new QLabel(tr("<b>City:</b>"), NewGBox), 1, 2); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("city*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 1, 3);

	GBLayout->addWidget(new QLabel(tr("<b>Country:</b>"), NewGBox), 2, 0); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("country*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 2, 1);

	GBLayout->addWidget(new QLabel(tr("<b>State:</b>"), NewGBox), 2, 2); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("state*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 2, 3);

	//-- Other --
#ifdef EFL_SENDERS_HAVE_ID
	NewGBox = new QGroupBox(tr("Other data"), this); 
	MLayout->addWidget(NewGBox, MLayout->rowCount(), 0, 1, 4);
	GBLayout = new QGridLayout(NewGBox); 
		
	GBLayout->addWidget(new QLabel(tr("Publisher account number:"), NewGBox), 0, 0); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("id*", NewLEdit);
	GBLayout->addWidget(NewLEdit, 0, 1);

	GBLayout->addWidget(new QLabel(tr("Publisher transport route:"), NewGBox), 0, 2); 
	NewLEdit = new QLineEdit(NewGBox); 
	registerField("transportroute", NewLEdit);
	GBLayout->addWidget(NewLEdit, 0, 3);
#endif 
}

void OPWUserDataPage::loadFieldSettings()
{
	STOWizardSettings Settings;
	setField("name", Settings.value("name"));
	setField("surname", Settings.value("surname"));
	setField("email", Settings.value("email"));
	setField("phone", Settings.value("phone"));
	setField("mobilephone", Settings.value("mobilephone"));
	setField("address", Settings.value("address"));
	setField("zip", Settings.value("zip"));
	setField("city", Settings.value("city"));
	setField("country", Settings.value("country"));
	setField("state", Settings.value("state"));
	//setField("id", Settings.value("id"));
	//setField("transportroute", Settings.value("transportroute"));
}

void OPWUserDataPage::initializePage()
{
	loadFieldSettings();
}

bool OPWUserDataPage::validatePage()
{
	//storeSettings(); 
	STOWizardSettings Settings; 
	//Settings.beginGroup("orderprintswizard");
	Settings.setValue("name", field("name").toString()); 
	Settings.setValue("surname", field("surname").toString()); 
	Settings.setValue("email", field("email").toString()); 
	Settings.setValue("phone", field("phone").toString()); 
	Settings.setValue("mobilephone", field("mobilephone").toString()); 
	Settings.setValue("address", field("address").toString()); 
	Settings.setValue("zip", field("zip").toString()); 
	Settings.setValue("city", field("city").toString()); 
	Settings.setValue("country", field("country").toString()); 
	Settings.setValue("state", field("state").toString()); 
	//Settings.setValue("id", field("id").toString());
	//Settings.setValue("transportroute", field("transportroute").toString());
	//Settings.endGroup(); 
	return true; 
}


bool OPWUserDataPage::forgetMe() 
{
	initializePage(); 
	STOWizardSettings Settings; 
	return !Settings.areEmpty(); 	
}

int OPWUserDataPage::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseProduct;
}


//_____________________________________________________________________________
//
// class OPWAbstractChooseProduct
//_____________________________________________________________________________


void OPWAbstractChooseProduct::getPublisherData()
{
	// QString PubDBFile = PublisherPage->publisherInfo().publisherDatabaseFile().absoluteFilePath();
	//	STDom::PublisherDatabase PubDatabase = STDom::PublisherDatabase::addDatabase(PubDBFile);
	STDom::PublisherDatabase PubDatabase = PublisherInfo.publisherDatabase();
	Assert(PubDatabase.open(), Error(QString(tr("Could not open publisher database file %1")).arg(PubDatabase.connectionName())));

	if (PModel)
		delete PModel;

	QString Filter;
	if (!TemplateRef.isEmpty())
		PModel = PubDatabase.newProductsTemplateModel(this, ProductType, TemplateRef);
	else
		PModel = PubDatabase.newProductsModel(this, ProductType);

	STDom::STXmlPublisherSettings PXmlS;
	Assert(PModel->rowCount() > 0, Error(tr("This publisher could not provide this product. Please contact him at <a href=\"mailto:%1\">%1</a>").arg(
			PXmlS.email())));
}

OPWAbstractChooseProduct::OPWAbstractChooseProduct(const STDom::PublisherInfo& _PublisherInfo, QWidget* _Parent) :
		STOWizardPage(_Parent), PublisherInfo(_PublisherInfo), PModel(0), ProductType(STDom::PublisherDatabase::AllProducts),
		TemplateRef("")
{
	PJModel = new STDom::PrintJobModel(this);
	DocModel = new STDom::DDocModel(this);
	PJModel->setSourceModel(DocModel);
	PJModel->setThumbnailSize(QSize(128, 128));
	PJModel->setRitchTextDisplay(true);
}

void OPWAbstractChooseProduct::initialize(const QFileInfoList& _Images)
{
	DocModel->setDocs(_Images);
	/*ImageModel->setImages(_Images);
	ImageModel->invalidateThumbnails();*/
}

int OPWAbstractChooseProduct::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseSendMode;
}

STDom::PrintJob OPWAbstractChooseProduct::printJob() const
{
	return PJModel->printJob();
}


//_____________________________________________________________________________
//
// class OPWChooseDigiprintProduct
//_____________________________________________________________________________


OPWChooseDigiprintProduct::OPWChooseDigiprintProduct(const STDom::PublisherInfo& _PublisherInfo, QWidget* _Parent) :
	OPWAbstractChooseProduct(_PublisherInfo, _Parent), PhotoSelW(0)
{
	setTitle(tr("<h1>Image selection page</h1>"));
	setSubTitle(tr("Here you can select and edit printed images."));
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	PhotoSelW = new TPPhotoSelWidget(this, false);
	PhotoSelW->setModel(PJModel);
	PhotoSelW->setBillEnabled(false);
	PhotoSelW->setFilterEnabled(false);
	MLayout->addWidget(PhotoSelW);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	connect(PJModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(completeChanged()));
}


QSize OPWChooseDigiprintProduct::sizeHint() const
{
	return QSize(800, 500);
}

void OPWChooseDigiprintProduct::initializePage()
{
	try
	{
		PJModel->clearProductCopies();
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error in sync process"), _Error.description());
	}
	//try
	//{
		getPublisherData();
		PhotoSelW->setProductsModel(PModel);
		//WProducts->setModel(PModel);
	//}
	//catch (STError& _Error)
	//{
	//	SMessageBox::critical(this, tr("Error getting publisher data"), _Error.description());
	//	wizard()->close();
	//}
}

bool OPWChooseDigiprintProduct::isComplete() const
{
	bool Res = true; 
	if (PJModel) //Defensive
		Res = PJModel->hasCopies();
	return Res;
}


void OPWChooseDigiprintProduct::showError(const STError& _Error)
{
	PhotoSelW->setVisible(false);

	setTitle(tr("<h1>The process could not continue due to an error</h1>"));
	setSubTitle(QString("<center><img src=\":/st/error.png\"/></center><p>%1</p>").arg(_Error.description()));

}


//_____________________________________________________________________________
//
// class OPWChooseAtomicProduct
//_____________________________________________________________________________

OPWChooseAtomicProduct::OPWChooseAtomicProduct(const STDom::PublisherInfo& _PublisherInfo, QWidget* _Parent) :
	OPWAbstractChooseProduct(_PublisherInfo, _Parent), HasError(false)
{
	setTitle(tr("<h1>PhotoBook selection</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Please select a photobook from the list bellow. And the number of photobook copies."));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->setMargin(0);
	MLayout->setSpacing(0);
	MainFrame = new QFrame(this);
	MLayout->addWidget(MainFrame);

	QGridLayout* FrameLayout = new QGridLayout(MainFrame);
	
	FrameLayout->addWidget(new QLabel(tr("Number of copies:"), this), FrameLayout->rowCount(), 0);
	SBoxNCopies = new QSpinBox(this); 
	connect(SBoxNCopies, SIGNAL(valueChanged( int )), this, SLOT(updateSelectedProduct())); 
	SBoxNCopies->setRange(1, 10); 
	FrameLayout->addWidget(SBoxNCopies,  FrameLayout->rowCount() -1, 1);
	
	FrameLayout->addWidget(new QLabel(tr("Modelo:"), this), FrameLayout->rowCount(), 0);
	CBoxModel = new QComboBox(this); 
	connect(CBoxModel, SIGNAL(currentIndexChanged( int )), this, SLOT(updateSelectedProduct())); 
	FrameLayout->addWidget(CBoxModel,  FrameLayout->rowCount() -1, 1);
}

int OPWChooseAtomicProduct::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseSendMode;
}

void OPWChooseAtomicProduct::initializePage()
{
	getPublisherData();
	CBoxModel->setModel(PModel);
}

bool OPWChooseAtomicProduct::isComplete() const
{
	return !HasError;
}

void OPWChooseAtomicProduct::updateSelectedProduct()
{
	if (PModel)
	{
		STDom::PublisherDatabase PubDatabase;
		STDom::DDocProduct CurrProduct = PubDatabase.getProduct(PModel, CBoxModel->currentIndex());
		Assert(!CurrProduct.isNull(), Error(QString(tr("Error getting product"))));

		PJModel->clearProductCopies();
		PJModel->incProductCopiesAll(SBoxNCopies->value(), CurrProduct);
	}
}

void OPWChooseAtomicProduct::showError(const STError& _Error)
{
	//PhotoSelW->setVisible(false);
	MainFrame->setVisible(false);
	HasError = true;
	setTitle(tr("<h1>The process could not continue due to an error</h1>"));
	setSubTitle(QString("<center><img src=\":/st/error.png\"/>%1</center>").arg(_Error.description()));

}


//_____________________________________________________________________________
//
// class OPWChooseSendMode
//_____________________________________________________________________________

OPWChooseSendMode::OPWChooseSendMode(QWidget* _Parent) : STOWizardPage(_Parent), AllowInetSend(true)
{
	setTitle(tr("<h1>Send mode selection</h1>"));
	setSubTitle(tr("<p>How do you want to send your order?</p> You have 2 options:"));
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	QFont RBFont = font();
	RBFont.setBold(true);
	RBFont.setPointSize(12);
	RBLocalStore = new QRadioButton(tr("Store it to a folder"), this);
	RBLocalStore->setIcon(QIcon(":/st/wizards/pendrive.png"));
	RBLocalStore->setIconSize(QSize(128, 128));
	RBLocalStore->setMinimumHeight(150);
	RBLocalStore->setFont(RBFont);
	RBLocalStore->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	MLayout->addWidget(RBLocalStore);
	MLayout->addWidget(new QLabel(tr("Store it to a <b>Pen drive</b> folder and bring it personaly to your publisher store."), this));

	RBInteSent = new QRadioButton(tr("Sent it via internet"), this);
	RBInteSent->setIcon(QIcon(":/st/wizards/inetsend.png"));
	RBInteSent->setIconSize(QSize(128, 128));
	RBInteSent->setMinimumHeight(150);
	RBInteSent->setFont(RBFont);
	RBInteSent->setChecked(true);
	MLayout->addWidget(RBInteSent);
	MLayout->addWidget(new QLabel(tr("Automatically sent it to your store via internet."), this));
}

void OPWChooseSendMode::setAllowInetSend(bool _Value)
{
	AllowInetSend = _Value;
}

void OPWChooseSendMode::initializePage()
{
}

bool OPWChooseSendMode::inetSend() const
{
	return (AllowInetSend && RBInteSent->isChecked());
}


bool OPWChooseSendMode::forgetMe()
{
	return !AllowInetSend;
}


int OPWChooseSendMode::nextId() const
{
	return STOrderPrintsWizard::Page_ConfirmOrder;
}



//_____________________________________________________________________________
//
// class OPWConfirmOrder
//_____________________________________________________________________________

QWidget* OPWConfirmOrder::createContactWidget()
{
	QGroupBox* GBContact = new QGroupBox(tr("Contact"), this);
	GBContact->setFlat(true);

	QVBoxLayout* MLayout = new QVBoxLayout(GBContact);
	ContactLabel = new QLabel(this);
	MLayout->addWidget(ContactLabel);
	QToolButton* ButModify = new QToolButton(this);
	connect(ButModify, SIGNAL(clicked()), this, SLOT(slotSenderSettings()));
	ButModify->setText(tr("Modify"));
	MLayout->addWidget(ButModify);

	return GBContact;
}

QWidget* OPWConfirmOrder::createShipmentOptionsWidget()
{
	QGroupBox* GBox = new QGroupBox(tr("Shippment options"), this);
	GBox->setFlat(true);

	QHBoxLayout* MLayout = new QHBoxLayout(GBox);

	QGridLayout* LeftLayout = new QGridLayout;
	MLayout->addLayout(LeftLayout);

	//------ Payment type ---
	LeftLayout->addWidget(new QLabel(tr("Payment Type"), 0, 0));
	CBPaymentType = new QComboBox(this);
	LeftLayout->addWidget(CBPaymentType, 0, 1);
	STDom::IdDescTableModel* PaymentTypeModel = new STDom::IdDescTableModel(this);
	CBPaymentType->setModel(PaymentTypeModel);
	PaymentTypeModel->setValues(PublisherInfo.paymentTypes());
	if (PaymentTypeModel->rowCount() > 0)
		CBPaymentType->setCurrentIndex(0);

	//------ Shipping Option ---
	LeftLayout->addWidget(new QLabel(tr("Shipping option")), 1, 0);
	CBShipOption = new QComboBox(this);
	LeftLayout->addWidget(CBShipOption, 1, 1);
	CBShipOption->addItem(tr("By Post"), ShipOptionByPost);
	CBShipOption->addItem(tr("Collect In Store"), ShipOptionCollectInStore);
	connect(CBShipOption, SIGNAL(currentIndexChanged(int)), this, SLOT(slotShipOptionIndexChanged(int )));

	// -- By Post --
	ByPostGBox = new QGroupBox(tr("Shipping Address"));
	MLayout->addWidget(ByPostGBox);
	QGridLayout* ByPostLayout = new QGridLayout(ByPostGBox);
	ByPostLayout->addWidget(new QLabel(tr("Shipping Method"), this), 0, 0);

	CBShippingMethod = new QComboBox(this);
	ByPostLayout->addWidget(CBShippingMethod, 0, 1);
	STDom::IdDescTableModel* ShippingMethodModel = new STDom::IdDescTableModel(this);
	CBShippingMethod->setModel(ShippingMethodModel);
	ShippingMethodModel->setValues(PublisherInfo.shippingMethods());

	ShippingAddressLabel = new QLabel(this);
	ByPostLayout->addWidget(ShippingAddressLabel, 1, 0, 1, 2);
	if (ShippingMethodModel->rowCount() > 0)
		CBShippingMethod->setCurrentIndex(0);
	connect(CBShippingMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCalcBill()));

	QToolButton* ButModify = new QToolButton(this);
	connect(ButModify, SIGNAL(clicked()), this, SLOT(slotSenderSettings()));
	ButModify->setText(tr("Modify"));
	ByPostLayout->addWidget(ButModify, 2, 0);


	// -- Collect in Store --
	CollectInStoreGBox = new QGroupBox(tr("Collection Address"));
	MLayout->addWidget(CollectInStoreGBox);
	QGridLayout* InStoreLayout = new QGridLayout(CollectInStoreGBox);
	InStoreLayout->addWidget(new QLabel(tr("Collection Point"), this), 0, 0);

	CBCollectionPoint = new QComboBox(this);
	InStoreLayout->addWidget(CBCollectionPoint , 0, 1);
	STDom::IdDescTableModel* CollectionPointModel = new STDom::IdDescTableModel(this);
	CBCollectionPoint->setModel(CollectionPointModel);
	CollectionPointModel->setValues(PublisherInfo.collectionPoints());
	if (CollectionPointModel->rowCount() > 0)
		CBCollectionPoint->setCurrentIndex(0);
	connect(CBCollectionPoint, SIGNAL(currentIndexChanged(int)), this, SLOT(slotUpdateCollectionPointAddress()));

	CollectionPointAddressLabel = new QLabel(this);
	InStoreLayout->addWidget(CollectionPointAddressLabel, 1, 0, 2, 1);
	CollectInStoreGBox->setVisible(false);

	CBShipOption->setCurrentIndex(0);
	return GBox;
}

QWidget* OPWConfirmOrder::createOtherOptionsWidget()
{
	QxtGroupBox* GBox = new QxtGroupBox(tr("More Options"), this);
	GBox->setChecked(false);
	GBox->setFlat(true);

	QVBoxLayout* MLayout = new QVBoxLayout(GBox );
	MLayout->addWidget(new QLabel(tr("Order comments:"), this));
	SenderOrderTE = new QTextEdit(this);
	SenderOrderTE->setMaximumHeight(80);
	MLayout->addWidget(SenderOrderTE);

	QxtGroupBox* UserDataGBox = new QxtGroupBox(tr("Customer data"), this);
	UserDataGBox->setChecked(false);
	MLayout->addWidget(UserDataGBox);
	QGridLayout* GBLayout = new QGridLayout(UserDataGBox);

	GBLayout->addWidget(new QLabel(tr("<b>Name:</b>"), UserDataGBox), 0, 0);
	QLineEdit* NewLEdit = new QLineEdit(UserDataGBox);
	registerField("customer_name", NewLEdit);
	GBLayout->addWidget(NewLEdit, 0, 1, 1, 7);

	GBLayout->addWidget(new QLabel(tr("<b>Address:</b>"), UserDataGBox), 1, 0);
	NewLEdit = new QLineEdit(UserDataGBox);
	registerField("customer_address", NewLEdit);
	GBLayout->addWidget(NewLEdit, 1, 1, 1, 7);

	GBLayout->addWidget(new QLabel(tr("<b>Cp/Zip:</b>"), UserDataGBox), 2, 0);
	NewLEdit = new QLineEdit(UserDataGBox);
	registerField("customer_zip", NewLEdit);
	GBLayout->addWidget(NewLEdit, 2, 1);

	GBLayout->addWidget(new QLabel(tr("<b>City:</b>"), UserDataGBox), 2, 2);
	NewLEdit = new QLineEdit(UserDataGBox);
	registerField("customer_city", NewLEdit);
	GBLayout->addWidget(NewLEdit, 2, 3);

	GBLayout->addWidget(new QLabel(tr("<b>State:</b>"), UserDataGBox), 2, 4);
	NewLEdit = new QLineEdit(UserDataGBox);
	registerField("customer_state", NewLEdit);
	GBLayout->addWidget(NewLEdit, 2, 5);

	GBLayout->addWidget(new QLabel(tr("<b>Country:</b>"), UserDataGBox), 2, 6);
	NewLEdit = new QLineEdit(UserDataGBox);
	registerField("customer_country", NewLEdit);
	GBLayout->addWidget(NewLEdit, 2, 7);

	return GBox;
}

QWidget* OPWConfirmOrder::createBillOptionsWidget()
{
	QGroupBox* GBox = new QGroupBox(tr("Order invoice"), this);
	GBox->setFlat(true);

	QVBoxLayout* MLayout = new QVBoxLayout(GBox);
	BillLabel = new QLabel(this);
	MLayout->addWidget(BillLabel);

	return GBox;
}

void OPWConfirmOrder::updateContactInfo()
{
	QString ContactStr;
	QTextStream Strm(&ContactStr);
	Strm << "<h2>" << field("name").toString()+ " " + field("surname").toString() << "</h2>";
	Strm << field("email").toString() << "<b>" <<  tr(" Tel:") << "</b>" << field("phone").toString();
	if (!field("mobilephone").toString().isEmpty())
		Strm << "/" << field("mobilephone").toString();
	ContactLabel->setText(ContactStr);


}

void OPWConfirmOrder::updateContactAddress()
{
	QString AddressStr;
	QTextStream Strm(&AddressStr);
	Strm << field("address").toString() << ", " <<  field("zip").toString() << " " << field("city").toString();
	Strm << "(" << field("state").toString() << ") - " << field("country").toString();
	ShippingAddressLabel->setText(AddressStr);
}



OPWConfirmOrder::OPWConfirmOrder(const STDom::PublisherInfo& _PublisherInfo, OPWUserDataPage* _UserDataPage, QWidget* _Parent) :
	PublisherInfo(_PublisherInfo), STOWizardPage(_Parent), SendViaInternet(false), UserDataPage(_UserDataPage)
{
	setTitle(tr("<h1>Please confirm your order</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Below you can see your order bill. Please check it and if all its ok click confirm button. Once you confirm your order all the data will be sent to your provider. Thank you."));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 

	MLayout->addWidget(createContactWidget());

	MLayout->addWidget(createShipmentOptionsWidget());

	MLayout->addWidget(createOtherOptionsWidget());

	MLayout->addWidget(createBillOptionsWidget());


	StatusWidg = new SProcessStatusWidget(this); 
	MLayout->addWidget(StatusWidg); 	

	MLayout->addItem(new QSpacerItem(0, 10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
}

void OPWConfirmOrder::initialize(const STDom::PrintJob& _Job)
{
	SenderOrderTE->clear();
	StatusWidg->setVisible(false);
	updateContactInfo();
	updateContactAddress();
	PrintJob = _Job;
	slotCalcBill();
}

void OPWConfirmOrder::sendViaInternet(bool _Value)
{
	SendViaInternet = _Value;
}

bool OPWConfirmOrder::validatePayment()
{
/*	StatusWidg->showProgressBar(tr("Validating payment..."), ProdPrModel->rowCount() - 1);
	StatusWidg->setVisible(true);*/
	return true; 
}

QString OPWConfirmOrder::newOrderRef()
{
	QString Res;
	//Get it from settings.
	QSettings Settings; 
	Res = Settings.value("orderref", "0").toString(); 
	Settings.setValue("orderref", Res.toInt() + 1);
	return Res; 
}

STDom::XmlOrderDealer OPWConfirmOrder::sender()
{
	STDom::XmlOrderDealer Res;
	Res.setName(field("name").toString() + " " + field("surname").toString()); 
	Res.setAddress(field("address").toString()); 
	Res.setEmail(field("email").toString()); 
	Res.setPhone(field("phone").toString()); 
	Res.setPostalCode(field("zip").toString()); 
	Res.setCity(field("city").toString()); 
	Res.setCountry(field("country").toString()); 
	Res.setState(field("state").toString()); 
	Res.setId(PublisherInfo.publisher().id());
	//Res.setId(field("id").toString());
	//Res.setTransportRoute(field("transportroute").toString());
	return Res; 
}

STDom::XmlOrderDealer OPWConfirmOrder::customer()
{
	STDom::XmlOrderDealer Res;
	Res.setName(field("customer_name").toString()); 
	Res.setAddress(field("customer_address").toString()); 
//	Res.setEmail(field("email").toString()); 
//	Res.setPhone(field("phone").toString()); 
	Res.setPostalCode(field("customer_zip").toString()); 
	Res.setCity(field("customer_city").toString()); 
	Res.setCountry(field("customer_country").toString()); 
	Res.setState(field("customer_state").toString()); 
	return Res; 
}

void OPWConfirmOrder::storeImages()
{
	STDom::PrintJobPrinter Printer;
	Printer.setDpis(STOrderPrintsWizard::dpis());

	QString OrderRef = newOrderRef();
	LastOrderRef = OrderRef;
	STDom::XmlOrder XmlOrder(OrderRef);
	//Setting sender data
	XmlOrder.setSender(sender());
	XmlOrder.setSentDateTime(QDateTime::currentDateTime());
	XmlOrder.setCreationDateTime(QDateTime::currentDateTime());
	XmlOrder.setSenderComment(SenderOrderTE->toPlainText());
	XmlOrder.setCustomer(customer());

	//Collection Point Info if  Collect in Store
	if (currentShipOption() == ShipOptionCollectInStore)
		XmlOrder.setCollectionPoint(currentCollectionPoint());
	else
		XmlOrder.setShippingMethod(currentShippingMethod());
	XmlOrder.setPaymentType(currentPaymentType());

	StatusWidg->setVisible(true);
	StatusWidg->showProgressBar(tr("Storing images..."), 100);
	QApplication::processEvents();
	Printer.clearErrorStack();

	if (SendViaInternet)
		Printer.store(PrintJob, XmlOrder, true, PublisherInfo.publisherDatabaseFilePath(), StatusWidg->progressBar());
	else
	{
		//Choose a folder to store
		QString StoreDirPath = QFileDialog::getExistingDirectory(this, tr("Storage folder"), tr("Please select the folder to store your order"));
		if (!StoreDirPath.isEmpty())
		{
			QDir DestDir(StoreDirPath);
			QString OrderDirName = QString("Order_%1").arg(OrderRef);
			DestDir.mkdir(OrderDirName);
			DestDir.cd(OrderDirName);
			Printer.storeEncoded(PrintJob, XmlOrder, DestDir, StatusWidg->progressBar());
		}
	}

	StatusWidg->hide();
	Assert(Printer.errorStack().isEmpty(), Error("Error storing order.", Printer.errorStack().errorString().join(",")));

	//!!!!!!!!!!!!!!!!!!!! CropRect are now Stored in DDocPrint so in PrintJob !!!!!!!!!!!!!
	//Find a new Order Num
/*	QString OrderRef = newOrderRef();
	LastOrderRef = OrderRef;
	KPSXmlOrder XmlOrder(OrderRef); 
	//Setting sender data 
	XmlOrder.setSender(sender()); 
	XmlOrder.setType(KPSqlCommand::TypeDigitalPrint); 
	XmlOrder.setSentDateTime(QDateTime::currentDateTime()); 
	XmlOrder.setCreationDateTime(QDateTime::currentDateTime()); 
	XmlOrder.setSenderComment(SenderOrderTE->toPlainText());
	XmlOrder.setCustomer(customer()); 

	STOrderStorage OStorage; 
	OStorage.setDpis(STOrderPrintsWizard::dpis()); 
	OStorage.setModel(ProdPrModel); 
	OStorage.setPublisherXmlFileInfo(PublisherXmlFile); 
	STOrderStorage::EnCropMode SCropMode; 
	switch (CropMode)
	{
		case OPWChooseCropModePage::ModeUserSelectCrop :
			SCropMode = STOrderStorage::ModeCrop;
		break; 
		case OPWChooseCropModePage::ModeWhiteMargin :
			SCropMode = STOrderStorage::ModeWhiteMargin;
		break; 
		case OPWChooseCropModePage::ModeWhiteMarginCentered :
			SCropMode = STOrderStorage::ModeWhiteMarginCentered;
		break; 
		case OPWChooseCropModePage::ModeNoModify :
			SCropMode = STOrderStorage::ModeNoModify;
		break; 		
	}
	OStorage.setCropMode(SCropMode); 
	OStorage.setCropRects(CropsPage->cropRects());
	StatusWidg->showProgressBar(tr("Storing images..."), ProdPrModel->rowCount() - 1);
	QApplication::processEvents();
	OStorage.storeOrder(OPhotoCollection::collectionOrdersPath(OrderRef), XmlOrder, StatusWidg->progressBar());
	StatusWidg->hide(); 
	return true; */
}

bool OPWConfirmOrder::validatePage()
{
	bool Res = true; 
	try
	{
		//if (ImagesToSend.isEmpty())
		//	emit getImagesToSend(ImagesToSend, StatusWidg);
		if (PrintJob.totalCopies() > 0)
		{
			Res = Res && validatePayment(); 	
			
			//Cropping and storing images: 
			storeImages();
		}
		else 
			SMessageBox::critical(this, tr("Error storing images"), tr("There was problems getting images to send. Please try again.")); 
	}	
	catch (const STError& _Error)
	{
		Res = false; 
		SMessageBox::critical(this, tr("Error storing images"), _Error.description()); 
	}

	return Res; 
}

OPWConfirmOrder::EnShipOption OPWConfirmOrder::currentShipOption()
{
	return static_cast<OPWConfirmOrder::EnShipOption>(CBShipOption->itemData(CBShipOption->currentIndex()).toInt());
}

STDom::ShippingMethod OPWConfirmOrder::currentShippingMethod()
{
	STDom::ShippingMethod Res;
	if (CBShippingMethod->currentIndex() != -1 && currentShipOption() == ShipOptionByPost)
	{
		if (STDom::IdDescTableModel* Model = qobject_cast<STDom::IdDescTableModel*>(CBShippingMethod->model()))
		{
			STDom::IdDescTableModel::TKey Key = Model->key(Model->index(CBShippingMethod->currentIndex(), 0));
			Res = PublisherInfo.getShippingMethod(Key);
		}
	}
	return Res;
}

STDom::CollectionPoint OPWConfirmOrder::currentCollectionPoint()
{
	STDom::CollectionPoint  Res;
	if (CBCollectionPoint->currentIndex() != -1)
	{
		if (STDom::IdDescTableModel* Model = qobject_cast<STDom::IdDescTableModel*>(CBCollectionPoint->model()))
		{
			STDom::IdDescTableModel::TKey  Key = Model->key(Model->index(CBCollectionPoint->currentIndex(), 0));
			Res = PublisherInfo.getCollectionPoint(Key);
		}
	}
	return Res;
}

STDom::PaymentType OPWConfirmOrder::currentPaymentType()
{
	STDom::PaymentType  Res;
	if (CBPaymentType->currentIndex() != -1)
	{
		if (STDom::IdDescTableModel* Model = qobject_cast<STDom::IdDescTableModel*>(CBPaymentType->model()))
		{
			STDom::IdDescTableModel::TKey  Key = Model->key(Model->index(CBPaymentType->currentIndex(), 0));
			Res = PublisherInfo.getPaymentType(Key);
		}
	}
	return Res;
}

void OPWConfirmOrder::slotShipOptionIndexChanged(int _Index)
{
	ByPostGBox->setVisible(static_cast<OPWConfirmOrder::EnShipOption>(CBShipOption->itemData(_Index).toInt()) == ShipOptionByPost);
	CollectInStoreGBox->setVisible(!ByPostGBox->isVisible());
	if (CollectInStoreGBox->isVisible())
		slotUpdateCollectionPointAddress();
	slotCalcBill();
}

void OPWConfirmOrder::slotUpdateCollectionPointAddress()
{
	STDom::CollectionPoint CCollectionPoint = currentCollectionPoint();
	QString AddressStr;
	QTextStream Strm(&AddressStr);
	Strm << CCollectionPoint.address() << ", " <<  CCollectionPoint.postalcode() << " " << CCollectionPoint.city();
	Strm << "(" << CCollectionPoint.state() << ") - " << CCollectionPoint.country();
	CollectionPointAddressLabel->setText(AddressStr);
}

void OPWConfirmOrder::slotCalcBill()
{
	STDom::PublisherDatabase PublDB = PublisherInfo.publisherDatabase();
	STDom::PublisherBill Bill;
	Bill = PublDB.calcBill(PrintJob, static_cast <STDom::PublisherBill::PublisherShippingMethod>(currentShippingMethod()));
	BillLabel->setText(Bill.ritchText());
}

void OPWConfirmOrder::slotSenderSettings()
{
	STSenderSettingsDialog SettingsDialog;
	SettingsDialog.exec();
	UserDataPage->loadFieldSettings();
	updateContactInfo();
	updateContactAddress();
}



//_____________________________________________________________________________
//
// class STOrderPrintsWizard
//_____________________________________________________________________________

const QString STOrderPrintsWizard::PublisherDBConnectionName = "opw_publishers_db";
int STOrderPrintsWizard::DPIS = 300; //TODO Put it on settings.

int STOrderPrintsWizard::nonForgetId(int _FromId) const
{
	int BaseNextId = _FromId; 
	if (STOWizardPage* CPage = qobject_cast<STOWizardPage*>(page(BaseNextId))) 
	{
		bool ForgetPage = CPage->forgetMe(); 
		while ( ForgetPage && !CPage->isFinalPage())
		{
			BaseNextId++;
			CPage = qobject_cast<STOWizardPage*>(page(BaseNextId)); 
			if (CPage)
				ForgetPage = CPage->forgetMe(); 
		}
	}
	return BaseNextId;
}

STOrderPrintsWizard::STOrderPrintsWizard(bool _AtomicOrder, const STDom::PublisherInfo& _PublisherInfo, QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags),
							AtomicOrder(_AtomicOrder), NumImages(0)
{
	setPage(Page_Welcome, new OPWWelcomePage(this));
	OPWUserDataPage* UserDataPage = new OPWUserDataPage(this);
	setPage(Page_UserData, UserDataPage);

	if (_AtomicOrder)
		ProductPage = new OPWChooseAtomicProduct(_PublisherInfo, this);
	else
		ProductPage = new OPWChooseDigiprintProduct(_PublisherInfo, this);

	setPage(Page_ChooseProduct, ProductPage);


	SendModePage = new OPWChooseSendMode(this);
	setPage(Page_ChooseSendMode, SendModePage);

	ConfirmOrderPage = new OPWConfirmOrder(_PublisherInfo, UserDataPage, this);
	setPage(Page_ConfirmOrder, ConfirmOrderPage);
	

	setStartId(nonForgetId(Page_Welcome));
	
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif

	setOption(HaveHelpButton, false);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/orderprintswizard.png"));
	setButtonText(QWizard::FinishButton, tr("Confirm")); 

	//connect(this, SIGNAL(currentIdChanged( int )), this, SLOT(slotCurrentIdChanged(int)));

	setWindowTitle(tr("Order Prints Wizard"));
}

void STOrderPrintsWizard::setProductType(STDom::PublisherDatabase::EnProductType _ProdType)
{
	ProductPage->setProductType(_ProdType);
}

void STOrderPrintsWizard::setTemplateRef(const QString& _TemplateRef)
{
	ProductPage->setTemplateRef(_TemplateRef);
}

void STOrderPrintsWizard::setImages(const QFileInfoList& _Images)
{
	ImagesToSend = _Images; 
	NumImages = _Images.size();
}

void STOrderPrintsWizard::clearImages()
{
	ImagesToSend.clear(); 
	NumImages = 0; 
}

QString STOrderPrintsWizard::sendedOrderRef() const
{
	return ConfirmOrderPage->lastOrderRef(); 
}

void STOrderPrintsWizard::initializePage(int _Id)
{
	switch (_Id)
	{
		case Page_ChooseProduct:
		{
			try
			{
				if (ImagesToSend.isEmpty())
					emit getImagesToSend(ImagesToSend, 0);
				ProductPage->initialize(ImagesToSend);
				QWizard::initializePage(_Id);
			}
			catch(const STError& _Error)
			{
				ProductPage->showError(_Error);
			}
		}
		break;
		case Page_ConfirmOrder :
		{
			ConfirmOrderPage->initialize(ProductPage->printJob());
			QWizard::initializePage(_Id);
		}
		break;
		default :
			QWizard::initializePage(_Id);
		break;
	}
}

int STOrderPrintsWizard::nextId() const
{
	int Res = nonForgetId(QWizard::nextId());
	if (Res == Page_ConfirmOrder)
		ConfirmOrderPage->sendViaInternet(SendModePage->inetSend());
	return Res;
}

void STOrderPrintsWizard::setAllowInetSend(bool _Value)
{
	SendModePage->setAllowInetSend(_Value);
}

bool STOrderPrintsWizard::inetSend() const
{
	return SendModePage->inetSend();
}



