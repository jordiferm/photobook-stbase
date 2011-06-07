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
#include <QDebug>

#include "smessagebox.h"
#include "sprocessstatuswidget.h"
#include "stimage.h"
#include "stutils.h"
#include "qxtgroupbox.h"

#include "stftpordertransfer.h"
#include "printjobmodel.h"
#include "tpphotoselwidget.h"

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

void OPWUserDataPage::initializePage()
{
	//loadSettings();
	STOWizardSettings Settings; 
	//Settings.beginGroup("orderprintswizard");
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
	setField("id", Settings.value("id")); 
	setField("transportroute", Settings.value("transportroute")); 
	//Settings.endGroup(); 
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
	Settings.setValue("id", field("id").toString()); 
	Settings.setValue("transportroute", field("transportroute").toString()); 
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
	return STOrderPrintsWizard::Page_ChoosePublisher;
}


//_____________________________________________________________________________
//
// class ChooseTemplatePage
//_____________________________________________________________________________


OPWChoosePublisher::OPWChoosePublisher(QWidget* _Parent) : 
	STOWizardPage(_Parent)
{
	setTitle(tr("<h1>Publisher selection</h1>"));
	setSubTitle(tr("Please select the publisher you want to order your prints from the list bellow."));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	Model = new STDom::STCollectionPublisherModel(this);
	MLayout->addWidget(new QLabel(tr("Template list:"), this));
	View = new QListView(this); 
	View->setModel(Model);
	View->setIconSize(QSize(64, 64)); 
	connect(View, SIGNAL(clicked( const QModelIndex& )), this, SIGNAL(completeChanged())); 
	MLayout->addWidget(View); 
}

bool OPWChoosePublisher::forgetMe() 
{
	Model->loadPublishers();
	PublisherInfo = Model->publisherInfo(Model->index(0,0)); 
	return Model->rowCount() == 1; 	
}

int OPWChoosePublisher::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseProduct;
}

void OPWChoosePublisher::initializePage()
{
	View->selectionModel()->select(Model->index(0, 0), QItemSelectionModel::SelectCurrent);
	View->setCurrentIndex(Model->index(0, 0)); 
}

bool OPWChoosePublisher::validatePage()
{
	PublisherInfo = Model->publisherInfo(View->currentIndex()); 
	//TODO: Set the current locale.
	return true; 
}

bool OPWChoosePublisher::isComplete() const
{
	return View->currentIndex().isValid();
}




//_____________________________________________________________________________
//
// class OPWAbstractChooseProduct
//_____________________________________________________________________________

void OPWAbstractChooseProduct::syncPublisherData()
{
	qApp->setOverrideCursor( QCursor(Qt::WaitCursor));

	try
	{
		QFileInfo PublXml = PublisherPage->publisherInfo().publisherXmlFile(); 
		//Lets sync products database: 
		QString PublisherPath = PublXml.dir().absolutePath(); 
		STDom::STXmlPublisherSettings PXmlS;
		Assert(PXmlS.loadXml(PublXml.absoluteFilePath()), Error(QString(tr("Could not load settings file: %1")).arg(PublXml.absoluteFilePath())));
		SavedPubSettings = PXmlS;
		if (FtpTrans)
			delete FtpTrans; 
		FtpTrans = new STDom::STFtpOrderTransfer(this);

		FtpTrans->syncRemoteDir(PublisherPath, PXmlS.dbHost(), PXmlS.dbPort(), PXmlS.dbUser(), PXmlS.dbPassword(),  PXmlS.dbDir(), static_cast<QFtp::TransferMode>(PXmlS.dbTransferMode()));
		qApp->restoreOverrideCursor();
	}
	catch (...)
	{
		qApp->restoreOverrideCursor();
		throw; 
	}
}

void OPWAbstractChooseProduct::getPublisherData()
{
	QString PubDBFile = PublisherPage->publisherInfo().publisherDatabaseFile().absoluteFilePath(); 
	STDom::PublisherDatabase PubDatabase = STDom::PublisherDatabase::addDatabase(PubDBFile);
	Assert(PubDatabase.open(), Error(QString(tr("Could not open publisher database file %1")).arg(PubDBFile)));

	if (PModel)
		delete PModel;

	QString Filter;
	if (!TemplateRef.isEmpty())
		Filter = QString("templates_ref='%1'").arg(TemplateRef);
	PModel = PubDatabase.newProductsModel(this, ProductType, Filter);

	STDom::STXmlPublisherSettings PXmlS;
	Assert(PXmlS.loadXml(PublisherPage->publisherInfo().publisherXmlFile().absoluteFilePath()), Error(QString(tr("Could not load settings file: %1")).arg(PublisherPage->publisherInfo().publisherXmlFile().absoluteFilePath())));
	Assert(PModel->rowCount() > 0, Error(tr("This publisher could not provide this product. Please contact him at <a href=\"mailto:%1\">%1</a>").arg(
			PXmlS.email())));
}

OPWAbstractChooseProduct::OPWAbstractChooseProduct(OPWChoosePublisher* _PublisherPage, QWidget* _Parent) : 
		STOWizardPage(_Parent), PublisherPage(_PublisherPage), PModel(0), FtpTrans(0), ProductType(STDom::PublisherDatabase::AllProducts),
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
	return STOrderPrintsWizard::Page_ChooseShipMethod;
}

STDom::PrintJob OPWAbstractChooseProduct::printJob() const
{
	return PJModel->printJob();
}


//_____________________________________________________________________________
//
// class OPWChooseDigiprintProduct
//_____________________________________________________________________________


OPWChooseDigiprintProduct::OPWChooseDigiprintProduct(OPWChoosePublisher* _PublisherPage, QWidget* _Parent) : 
	OPWAbstractChooseProduct(_PublisherPage, _Parent), PhotoSelW(0)
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
		syncPublisherData();
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error in sync process"), _Error.description());
	}
	try
	{
		getPublisherData();
		PhotoSelW->setProductsModel(PModel);
		//WProducts->setModel(PModel);
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error getting publisher data"), _Error.description());	
		wizard()->close();
	}
}

bool OPWChooseDigiprintProduct::isComplete() const
{
	bool Res = true; 
	if (PJModel) //Defensive
		Res = PJModel->hasCopies();
	return Res;
}


//_____________________________________________________________________________
//
// class OPWChooseAtomicProduct
//_____________________________________________________________________________

OPWChooseAtomicProduct::OPWChooseAtomicProduct(OPWChoosePublisher* _PublisherPage, QWidget* _Parent) :
	OPWAbstractChooseProduct(_PublisherPage, _Parent)
{
	setTitle(tr("<h1>PhotoBook selection</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Please select a photobook from the list bellow. And the number of photobook copies."));
	QGridLayout* MLayout = new QGridLayout(this); 
	
	MLayout->addWidget(new QLabel(tr("Number of copies:"), this), MLayout->rowCount(), 0); 
	SBoxNCopies = new QSpinBox(this); 
	connect(SBoxNCopies, SIGNAL(valueChanged( int )), this, SLOT(updateSelectedProduct())); 
	SBoxNCopies->setRange(1, 10); 
	MLayout->addWidget(SBoxNCopies,  MLayout->rowCount() -1, 1); 
	
	MLayout->addWidget(new QLabel(tr("Modelo:"), this), MLayout->rowCount(), 0); 
	CBoxModel = new QComboBox(this); 
	connect(CBoxModel, SIGNAL(currentIndexChanged( int )), this, SLOT(updateSelectedProduct())); 
	MLayout->addWidget(CBoxModel,  MLayout->rowCount() -1, 1); 
}

int OPWChooseAtomicProduct::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseShipMethod;
}

void OPWChooseAtomicProduct::initializePage()
{
	try
	{
		//MProxyModel->clearProductCopies();
		syncPublisherData();
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error in sync process"), _Error.description());
	}
	try
	{
		getPublisherData();
		CBoxModel->setModel(PModel);
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error getting publisher data"), _Error.description());	
		wizard()->close();
	}
}

bool OPWChooseAtomicProduct::isComplete() const
{
	return true; 
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


//_____________________________________________________________________________
//
// class OPWChooseShippingMethod
//_____________________________________________________________________________

OPWChooseShippingMethod::OPWChooseShippingMethod(QWidget* _Parent) : STOWizardPage(_Parent), PModel(0)
{
	setTitle(tr("<h1>Shipping method selection</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Please select a shipping method from the list bellow."));
	QGridLayout* MLayout = new QGridLayout(this); 
	MLayout->addWidget(new QLabel(tr("Shipping methods :"), this), 0, 0);
	CBSMethods = new QComboBox(this);
	MLayout->addWidget(CBSMethods, 0, 1);
}

void OPWChooseShippingMethod::initializePage()
{
	STDom::PublisherDatabase PubDatabase;

	if (PModel)
		delete PModel;
	PModel = PubDatabase.newShippingMethodModel(this);
	CBSMethods->setModel(PModel);
	CBSMethods->setModelColumn(PModel->record().indexOf("description")); 
}

bool OPWChooseShippingMethod::forgetMe() 
{
	initializePage();
	return PModel->rowCount() <= 1;
}


int OPWChooseShippingMethod::nextId() const
{
	return STOrderPrintsWizard::Page_ConfirmOrder;
}

QSqlRecord OPWChooseShippingMethod::currentShippingMethod() const
{
	return PModel->record(CBSMethods->currentIndex());
}


//_____________________________________________________________________________
//
// class STOrderPrintsWizard
//_____________________________________________________________________________

OPWConfirmOrder::OPWConfirmOrder(QWidget* _Parent) : STOWizardPage(_Parent)
{
	setTitle(tr("<h1>Please confirm your order</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Below you can see your order bill. Please check it and if all its ok click confirm button. Once you confirm your order all the data will be sent to your provider. Thank you."));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	BillLabel = new QLabel(this); 
	MLayout->addWidget(BillLabel);

	MLayout->addWidget(new QLabel(tr("Order comments:"), this)); 
	SenderOrderTE = new QTextEdit(this); 
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


	StatusWidg = new SProcessStatusWidget(this); 
	MLayout->addWidget(StatusWidg); 	
}

void OPWConfirmOrder::calcBill(const STDom::PrintJob& _Job, const QSqlRecord& _ShippingMethod)
{
	STDom::PublisherDatabase PublDB;
	STDom::PublisherBill Bill;
	Bill = PublDB.calcBill(_Job, _ShippingMethod);
	BillLabel->setText(Bill.ritchText());
}

void OPWConfirmOrder::initialize(const STDom::PrintJob& _Job, const STDom::STCollectionPublisherInfo& _PubInfo)
{
	SenderOrderTE->clear();
	StatusWidg->setVisible(false);
	PrintJob = _Job;
	PublisherXmlFile = _PubInfo.publisherXmlFile(); 
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
	Res.setId(field("id").toString()); 
	Res.setTransportRoute(field("transportroute").toString()); 
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
	Printer.storePublisherXmlFile(PublisherXmlFile);
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

	//Publisher info
	STDom::STXmlPublisherSettings PXmlSettings;
	PXmlSettings.loadXml(PublisherXmlFile.absoluteFilePath());
	STDom::XmlOrderDealer Publisher(PXmlSettings.id(), PXmlSettings.name());
	Publisher.setEmail(PXmlSettings.email());
	XmlOrder.setPublisher(Publisher);

	StatusWidg->setVisible(true);
	StatusWidg->showProgressBar(tr("Storing images..."), 100);
	QApplication::processEvents();
	Printer.clearErrorStack();
	Printer.store(PrintJob, XmlOrder, true, StatusWidg->progressBar());
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

STOrderPrintsWizard::STOrderPrintsWizard(bool _AtomicOrder, QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags),
							AtomicOrder(_AtomicOrder), NumImages(0)
{
	setPage(Page_Welcome, new OPWWelcomePage(this));
	setPage(Page_UserData, new OPWUserDataPage(this));

	PublisherPage = new OPWChoosePublisher(this); 
	if (_AtomicOrder)
		ProductPage = new OPWChooseAtomicProduct(PublisherPage, this);
	else
		ProductPage = new OPWChooseDigiprintProduct(PublisherPage, this);

	setPage(Page_ChoosePublisher, PublisherPage); 
	setPage(Page_ChooseProduct, ProductPage);

	SMethPage = new OPWChooseShippingMethod(this);
	setPage(Page_ChooseShipMethod, SMethPage); 

	ConfirmOrderPage = new OPWConfirmOrder(this);
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
	try
	{
		switch (_Id)
		{	
			case Page_ChooseProduct:
				if (ImagesToSend.isEmpty()) 
					emit getImagesToSend(ImagesToSend, 0);
				ProductPage->initialize(ImagesToSend);
				
			break;
			case Page_ConfirmOrder : 
			{
				ConfirmOrderPage->initialize(ProductPage->printJob(), PublisherPage->publisherInfo());
				ConfirmOrderPage->calcBill(ProductPage->printJob(), SMethPage->currentShippingMethod());
			}
			break; 
		}
	}
	catch(const STError& _Error)
	{	
		SMessageBox::critical(this, tr("Error getting publisher data"), _Error.description());	
	}
	QWizard::initializePage(_Id); 
}

int STOrderPrintsWizard::nextId() const
{
	return nonForgetId(QWizard::nextId()); 
}

STDom::STXmlPublisherSettings STOrderPrintsWizard::publisherSettings() const
{
	return ProductPage->publisherSettings();
}

