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

#include "smessagebox.h"
#include "sprocessstatuswidget.h"
#include "stimage.h"
#include "stutils.h"
#include "qxtgroupbox.h"

#include "stftpordertransfer.h"
#include "printjobmodel.h"


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
// class OPWProdChooseWidget
//_____________________________________________________________________________

OPWSingleProdChooseWidget::OPWSingleProdChooseWidget(QWidget* _Parent) : QWidget(_Parent)
{
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	MCBox = new QComboBox(this); 
	connect(MCBox, SIGNAL(currentIndexChanged( int )), this, SIGNAL(changed())); 
	MLayout->addWidget(MCBox);


	QHBoxLayout* AddRemoveButLayout = new QHBoxLayout; 
	MLayout->addLayout(AddRemoveButLayout); 

	AddButton = new QToolButton(this); 
	AddButton->setIcon(QIcon(":/st/wizards/viewmag+.png"));
	AddRemoveButLayout->addWidget(AddButton); 
	connect(AddButton, SIGNAL(clicked( bool )), this, SLOT(buttonClicked()));

	RemoveButton = new QToolButton(this); 
	RemoveButton->setIcon(QIcon(":/st/wizards/viewmag-.png"));
	AddRemoveButLayout->addWidget(RemoveButton); 
	connect(RemoveButton, SIGNAL(clicked( bool )), this, SLOT(buttonClicked()));

	ResetButton = new QToolButton(this); 
	ResetButton->setIcon(QIcon(":/st/wizards/excluded.png"));
	AddRemoveButLayout->addWidget(ResetButton); 
	connect(ResetButton, SIGNAL(clicked( bool )), this, SLOT(buttonClicked()));
	
	//MSpinBox = new QSpinBox(this);
	//connect(MSpinBox, SIGNAL(valueChanged( int )), this, SIGNAL(changed())); 
	//MLayout->addWidget(MSpinBox); 
}


void OPWSingleProdChooseWidget::setModel(QAbstractItemModel* _Model)
{
	MCBox->setModel(_Model); 
}

void OPWSingleProdChooseWidget::setProductIndex(int _Index) 
{
	MCBox->setCurrentIndex(qMin(_Index, MCBox->maxCount())); 
}

int OPWSingleProdChooseWidget::productIndex() const
{
	return MCBox->currentIndex();
}

void OPWSingleProdChooseWidget::buttonClicked()
{
	if ( sender() == AddButton )
		emit incProduct(productIndex(), +1); 
	else 
	if ( sender() == RemoveButton )
		emit incProduct(productIndex(), -1); 
	else 
	if ( sender() == ResetButton )
		emit resetProduct(productIndex()); 
}


//_____________________________________________________________________________
//
// class OPWProdChooseWidget
//_____________________________________________________________________________

OPWProdChooseWidget::OPWProdChooseWidget(int _NumRows, QWidget* _Parent) : QWidget(_Parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setSpacing(0); 
	for (int Vfor = 0; Vfor < _NumRows; Vfor++)
	{
		OPWSingleProdChooseWidget* NSSelWidg = new OPWSingleProdChooseWidget(this);
		connect(NSSelWidg, SIGNAL(incProduct(int, int)), this, SIGNAL(incProduct(int, int))); 
		connect(NSSelWidg, SIGNAL(resetProduct(int)), this, SIGNAL(resetProduct(int))); 
		MLayout->addWidget(NSSelWidg); 
	}
}

void OPWProdChooseWidget::setModel(QAbstractItemModel* _Model)
{
	QList<OPWSingleProdChooseWidget*> ProdSelWList = findChildren<OPWSingleProdChooseWidget*>();
	QList<OPWSingleProdChooseWidget*>::const_iterator it;
	int Cnt = 0; 
	for (it = ProdSelWList.begin(); it != ProdSelWList.end(); ++it)
	{
		(*it)->setModel(_Model);
		(*it)->setProductIndex(Cnt++);
	}
}


int OPWProdChooseWidget::firstProdIndex() const
{
	int Res = -1; 
	QList<OPWSingleProdChooseWidget*> ProdSelWList = findChildren<OPWSingleProdChooseWidget*>();
	if (ProdSelWList.size() > 0)
		Res = (ProdSelWList.first())->productIndex(); 
	
	return Res; 
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
	QSqlDatabase::cloneDatabase(STDom::PublisherDatabase(PubDBFile), STOrderPrintsWizard::PublisherDBConnectionName);
	STDom::PublisherDatabase PubDatabase(QSqlDatabase::database(STOrderPrintsWizard::PublisherDBConnectionName));
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
/*	DocModel = new STDom::DDocModel(this);
	PJobModel = new STDom::PrintJobModel(this);
	ImageModel = new OPhotoCollectionImageModel(this);
	ImageModel->setNoImagePixmap(QPixmap(":/st/wizards/nopreview.png")); 
	MProxyModel = new STProductPrintsProxyModel(this);	
	MProxyModel->setSourceModel(ImageModel); 
	MProxyModel->setThumbnailSize(QSize(90, 60));*/
}

void OPWAbstractChooseProduct::initialize(const QFileInfoList& _Images)
{
	/*ImageModel->setImages(_Images);
	ImageModel->invalidateThumbnails();*/
}

int OPWAbstractChooseProduct::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseCropMode;
}

STProductPrintsProxyModel* OPWAbstractChooseProduct::prodPrintsModel() const
{
	//return MProxyModel;
}


//_____________________________________________________________________________
//
// class OPWChooseDigiprintProduct
//_____________________________________________________________________________


OPWChooseDigiprintProduct::OPWChooseDigiprintProduct(OPWChoosePublisher* _PublisherPage, QWidget* _Parent) : 
	OPWAbstractChooseProduct(_PublisherPage, _Parent), LView(0)
{
	setTitle(tr("<h1>Product selection</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Please select a product from the list bellow."));
	QGridLayout* MLayout = new QGridLayout(this); 
	QLabel* ProdLabel = new QLabel(tr("Products :"), this);
	MLayout->addWidget(ProdLabel, 0, 1); 
	MLayout->setAlignment(ProdLabel, Qt::AlignBottom);
	//CBProducts = new QComboBox(this); 
	//MLayout->addWidget(CBProducts, 0, 1); 	
	WProducts = new OPWProdChooseWidget(4, this); 
	connect(WProducts, SIGNAL(incProduct(int, int)), this, SLOT(incSelectedProduct(int, int))); 
	connect(WProducts, SIGNAL(incProduct(int, int)), this, SIGNAL(completeChanged()));
	connect(WProducts, SIGNAL(resetProduct(int)), this, SLOT(resetSelectedProduct(int))); 
	connect(WProducts, SIGNAL(resetProduct(int)), this, SIGNAL(completeChanged()));
	
	MLayout->addWidget(WProducts, 1, 1); 	
	LView = new QListView(this);
	//LView->setViewMode(QListView::IconMode); 
	//LView->setFlow(QListView::LeftToRight); 
	LView->setWrapping(false);
	LView->setSelectionMode(QAbstractItemView::ExtendedSelection); 
	LView->setSelectionBehavior(QAbstractItemView::SelectRows); 
	//LView->setTextElideMode(Qt::ElideNone);
	MLayout->addWidget(LView, 0, 0, 3, 1); 

	LView->setModel(MProxyModel); 
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


QSize OPWChooseDigiprintProduct::sizeHint() const
{
	return QSize(800, 500);
}

void OPWChooseDigiprintProduct::initializePage()
{
	try
	{
		MProxyModel->clearProductCopies();
		LView->selectAll();
		syncPublisherData();
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error in sync process"), _Error.description());
	}
	try
	{
		getPublisherData();
		WProducts->setModel(PModel);
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
	if (MProxyModel) //Defensive 
		Res = MProxyModel->hasCopies(); 
	return Res;
}

void OPWChooseDigiprintProduct::resetSelectedProduct(int _Index)
{
/*	STPublisherDatabase PubDatabase = QSqlDatabase::database(STOrderPrintsWizard::PublisherDBConnectionName);
	KPhotoProduct CurrProduct =  PubDatabase.getProduct(PModel, _Index); 
	Assert(!CurrProduct.isNull(), Error(QString(tr("Could not get product for reference %1")).arg(_Index)));
	QModelIndexList SelIndexes = LView->selectionModel()->selectedRows();
	QModelIndexList::iterator sit; 
	for (sit = SelIndexes.begin(); sit != SelIndexes.end(); ++sit)
	{
		MProxyModel->setProductCopies(*sit, CurrProduct, 0);  
	}
	LView->setSpacing(2);*/
}


void OPWChooseDigiprintProduct::incSelectedProduct(int _Index, int _Quantity)
{
/*	STPublisherDatabase PubDatabase = QSqlDatabase::database(STOrderPrintsWizard::PublisherDBConnectionName);
	KPhotoProduct CurrProduct =  PubDatabase.getProduct(PModel, _Index); 
	Assert(!CurrProduct.isNull(), Error(QString(tr("Could not get product for reference %1")).arg(_Index)));
	QModelIndexList SelIndexes = LView->selectionModel()->selectedRows();
	QModelIndexList::iterator sit; 
	for (sit = SelIndexes.begin(); sit != SelIndexes.end(); ++sit)
	{
		int CValue = 0; 
		KProductList PCMap = MProxyModel->productCopies(*sit);
		if (PCMap.contains(CurrProduct))
			CValue = PCMap[CurrProduct];
		CValue += _Quantity; 
		MProxyModel->setProductCopies(*sit, CurrProduct, CValue);  
	}
	LView->setSpacing(2);*/
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
		MProxyModel->clearProductCopies(); 
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
/*	if (PModel)
	{
		STPublisherDatabase PubDatabase = QSqlDatabase::database(STOrderPrintsWizard::PublisherDBConnectionName);
		KPhotoProduct CurrProduct =  PubDatabase.getProduct(PModel, CBoxModel->currentIndex()); 
		Assert(!CurrProduct.isNull(), Error(QString(tr("Error getting product"))));

		MProxyModel->clearProductCopies(); 
		MProxyModel->incProductCopiesAll(SBoxNCopies->value(), CurrProduct);
	}	*/
}



//_____________________________________________________________________________
//
// class OPWChooseCropModePage
//_____________________________________________________________________________


OPWChooseCropModePage::OPWChooseCropModePage(QWidget* _Parent) : ChooseCropModePage(_Parent)
{
	QLabel* NoCropPixLabel = new QLabel(this); 
	NoCropPixLabel->setPixmap(QPixmap(":/st/wizards/nocrop.png")); 
	NoCropPixLabel->setAlignment(Qt::AlignCenter); 
	NoCropPixLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred); 
	MLayout->addWidget(NoCropPixLabel, MLayout->rowCount(), 0);
	RBNoModify = new QRadioButton(tr("Leave image as is. Do not cut neither modify image."), this);
	MLayout->addWidget(RBNoModify, MLayout->rowCount() -1, 1);
}


int OPWChooseCropModePage::nextId() const
{
	int Res; 

	if (RBNoCut->isChecked() || RBNoModify->isChecked() || RBSideNoCut->isChecked())
		Res = STOrderPrintsWizard::Page_ChooseShipMethod;
	else 
		Res = STOrderPrintsWizard::Page_SelectCrops;

	return Res; 
}

OPWChooseCropModePage::EnCropMode OPWChooseCropModePage::cropMode()
{

	OPWChooseCropModePage::EnCropMode Res; 
	if (RBNoCut->isChecked())
		Res = ModeWhiteMarginCentered;
	else 
	if (RBSideNoCut->isChecked())
		Res = ModeWhiteMargin;
	else 
	if (RBNoModify->isChecked())
		Res = ModeNoModify;
	else 
		Res = ModeUserSelectCrop;
	return Res; 	
}

//_____________________________________________________________________________
//
// class OPWSelectCropsPage
//_____________________________________________________________________________

int OPWSelectCropsPage::nextId() const
{
	return STOrderPrintsWizard::Page_ChooseShipMethod;
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
	STDom::PublisherDatabase PubDatabase = QSqlDatabase::database(STOrderPrintsWizard::PublisherDBConnectionName);
	if (PModel)
		delete PModel;
	PModel = PubDatabase.newShippingMethodModel(this);
	CBSMethods->setModel(PModel);
	CBSMethods->setModelColumn(PModel->record().indexOf("description")); 
}

bool OPWChooseShippingMethod::forgetMe() 
{
	initializePage();
	return PModel->rowCount() == 1; 	
}


int OPWChooseShippingMethod::nextId() const
{
	return STOrderPrintsWizard::Page_ChoosePayMethod; 
}

QSqlRecord OPWChooseShippingMethod::currentShippingMethod() const
{
	return PModel->record(CBSMethods->currentIndex());
}

//_____________________________________________________________________________
//
// class OPWChoosePayMethod
//_____________________________________________________________________________

OPWChoosePayMethod::OPWChoosePayMethod(OPWChoosePublisher* _PublisherPage, QWidget* _Parent) : STOWizardPage(_Parent), PublisherPage(_PublisherPage)
{
	setTitle(tr("<h1>Pay method selection</h1>"));
	//TODO Fetch and show html from a remote URL stored in database.
	setSubTitle(tr("Please choose between the following pay methods."));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	BillLabel = new QLabel(this); 
	MLayout->addWidget(BillLabel);

	RBCOnDel = new QRadioButton(tr("Cash On delivery"), this); 
	MLayout->addWidget(RBCOnDel); 
	connect(RBCOnDel, SIGNAL(toggled( bool )), this, SIGNAL(completeChanged()));  

	RBCredCard = new QRadioButton(tr("Credit Card"), this); 
	MLayout->addWidget(RBCredCard); 
	QGroupBox* GBCreditCard = new QGroupBox(tr("Credit card settings"), this); 
	MLayout->addWidget(GBCreditCard); 
	
	QGridLayout* GLCCard = new QGridLayout(GBCreditCard); 
	GLCCard->addWidget(new QLabel(tr("Credit Card Owner:"), GBCreditCard), 0, 0);
	QLineEdit* NewLEdit = new QLineEdit(GBCreditCard); 
	connect(NewLEdit, SIGNAL(textChanged( const QString& )), this, SIGNAL(completeChanged()));
	GLCCard->addWidget(NewLEdit, 0, 1); 
	registerField("ccowner", NewLEdit);

	GLCCard->addWidget(new QLabel(tr("Credit Card Number:"), GBCreditCard), 1, 0);
	NewLEdit = new QLineEdit(GBCreditCard); 
	connect(NewLEdit, SIGNAL(textChanged( const QString& )), this, SIGNAL(completeChanged()));
	GLCCard->addWidget(NewLEdit, 1, 1); 
	registerField("ccnumber", NewLEdit);
	
	GLCCard->addWidget(new QLabel(tr("Credit Expiry Date:"), GBCreditCard), 2, 0);
	NewLEdit = new QLineEdit(GBCreditCard); 
	connect(NewLEdit, SIGNAL(textChanged( const QString& )), this, SIGNAL(completeChanged()));
	GLCCard->addWidget(NewLEdit, 2, 1); 
	registerField("ccexpdate", NewLEdit);
	
	GBCreditCard->setVisible(false); 
	connect(RBCredCard, SIGNAL(toggled( bool )), GBCreditCard, SLOT(setVisible( bool )));
	connect(RBCredCard, SIGNAL(toggled( bool )), this, SIGNAL(completeChanged()));  

	RBPayPal = new QRadioButton(tr("PayPal"), this); 
	MLayout->addWidget(RBPayPal); 
	QGroupBox* GBPayPal = new QGroupBox(tr("Paypal settings"), this); 
	MLayout->addWidget(GBPayPal); 
	
	QGridLayout* GLPaypal = new QGridLayout(GBPayPal); 
	GLPaypal->addWidget(new QLabel(tr("EMail:"), GBCreditCard), 0, 0);
	NewLEdit = new QLineEdit(GBPayPal); 
	connect(NewLEdit, SIGNAL(textChanged( const QString& )), this, SIGNAL(completeChanged()));
	GLPaypal->addWidget(NewLEdit, 0, 1); 
	registerField("paypalmail", NewLEdit);

	GBPayPal->setVisible(false); 
	connect(RBPayPal, SIGNAL(toggled( bool )), GBPayPal, SLOT(setVisible( bool ))); 
	connect(RBPayPal, SIGNAL(toggled( bool )), this, SIGNAL(completeChanged()));  

}

void OPWChoosePayMethod::initializePage()
{
	RBCOnDel->setChecked(true); 
	STDom::STXmlPublisherSettings PXmlS;
	Assert(PXmlS.loadXml(PublisherPage->publisherInfo().publisherXmlFile().absoluteFilePath()), Error(QString(tr("Could not load settings file: %1")).arg(PublisherPage->publisherInfo().publisherXmlFile().absoluteFilePath())));
	RBCOnDel->setEnabled(PXmlS.cODPayment()); 
	RBCredCard->setEnabled(PXmlS.cCPayment()); 
	RBPayPal->setEnabled(PXmlS.payPalPayment()); 
}


bool OPWChoosePayMethod::isComplete () const
{
	bool Res = true; 
	if (RBPayPal->isChecked())
		Res = !field("paypalmail").toString().isEmpty();
	else 
	if (RBCredCard->isChecked())
		Res = !field("ccowner").toString().isEmpty() && !field("ccnumber").toString().isEmpty() && !field("ccexpdate").toString().isEmpty();
	return Res;
}

bool OPWChoosePayMethod::forgetMe() 
{
	STDom::STXmlPublisherSettings PXmlS;
	Assert(PXmlS.loadXml(PublisherPage->publisherInfo().publisherXmlFile().absoluteFilePath()), Error(QString(tr("Could not load settings file: %1")).arg(PublisherPage->publisherInfo().publisherXmlFile().absoluteFilePath())));
	int NOptionsEnabled = 0; 
	if (PXmlS.cODPayment())
		NOptionsEnabled++;
	if (PXmlS.cCPayment())
		NOptionsEnabled++;
	if (PXmlS.payPalPayment())
		NOptionsEnabled++;

	return NOptionsEnabled == 1; 
}

int OPWChoosePayMethod::nextId() const
{
	return STOrderPrintsWizard::Page_ConfirmOrder;  
}

//_____________________________________________________________________________
//
// class STOrderPrintsWizard
//_____________________________________________________________________________

OPWConfirmOrder::OPWConfirmOrder(OPWSelectCropsPage* _CropsPage, QWidget* _Parent) : STOWizardPage(_Parent), CropsPage(_CropsPage)
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

void OPWConfirmOrder::calcBill(STProductPrintsProxyModel* _ProdPrModel, const QSqlRecord& _ShippingMethod)
{
	/*STDom::PublisherDatabase PubDatabase = QSqlDatabase::database(STOrderPrintsWizard::PublisherDBConnectionName);
	BillLabel->setText(PubDatabase.billRitchText(ProdPrModel, _ShippingMethod )); */
}

void OPWConfirmOrder::initialize(STProductPrintsProxyModel* _ProdPrModel, const STDom::STCollectionPublisherInfo& _PubInfo, OPWChooseCropModePage::EnCropMode _CropMode)
{
	SenderOrderTE->clear();
	StatusWidg->setVisible(false);
	ProdPrModel = _ProdPrModel;
	CropMode = _CropMode; 
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

bool OPWConfirmOrder::storeImages()
{
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
/*	try
	{
		//if (ImagesToSend.isEmpty())
		//	emit getImagesToSend(ImagesToSend, StatusWidg);
		if (ProdPrModel->rowCount() > 0)
		{
			Res = Res && validatePayment(); 	
			
			//Cropping and storing images: 
			Res = Res && storeImages(); 
		}
		else 
			SMessageBox::critical(this, tr("Error storing images"), tr("There was problems getting images to send. Please try again.")); 
	}	
	catch (const STError& _Error)
	{
		Res = false; 
		SMessageBox::critical(this, tr("Error storing images"), _Error.description()); 
	}*/

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

	SelCropModePage = new OPWChooseCropModePage(this);
	setPage(Page_ChooseCropMode, SelCropModePage);
	
	SelCropPage = new OPWSelectCropsPage(this);
	setPage(Page_SelectCrops, SelCropPage);

	SMethPage = new OPWChooseShippingMethod(this);
	setPage(Page_ChooseShipMethod, SMethPage); 

	SPayMPage = new OPWChoosePayMethod(PublisherPage, this); 
	setPage(Page_ChoosePayMethod, SPayMPage);

	ConfirmOrderPage = new OPWConfirmOrder(SelCropPage, this); 
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
			case Page_SelectCrops:
				//if (ImagesToSend.isEmpty())
				//	emit getImagesToSend(ImagesToSend, 0);
				//SelCropPage->initialize(ProductPage->prodPrintsModel()->imagesWithCopies(), ProductPage->prodPrintsModel()->referenceFormatList(DPIS));
			break;  
			case Page_ConfirmOrder : 
			{
				if (AtomicOrder)
					ConfirmOrderPage->initialize(ProductPage->prodPrintsModel(), PublisherPage->publisherInfo(), OPWChooseCropModePage::ModeNoModify );  
				else 
					ConfirmOrderPage->initialize(ProductPage->prodPrintsModel(), PublisherPage->publisherInfo(), SelCropModePage->cropMode());  

				ConfirmOrderPage->calcBill(ProductPage->prodPrintsModel(), SMethPage->currentShippingMethod()); 
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

