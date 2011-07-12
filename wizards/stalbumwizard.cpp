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
#include "stalbumwizard.h"
#include <QLayout> 
#include <QLabel> 
#include <QListView> 
#include <QRadioButton> 
#include <QLineEdit> 
#include <QTextEdit> 
#include <QDir> 
#include <QGroupBox> 
#include <QSpinBox> 
#include <QButtonGroup>
#include <QCheckBox> 
#include <QCoreApplication>
#include <QComboBox>
#include <QTimer>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QFormLayout>
#include <QDateTimeEdit>
#include <QTextBrowser>

//Template info
#include <QtWebKit/QWebView>
#include <QApplication>

//GUI Other
#include "qxtgroupbox.h"
#include "qxtpushbutton.h"

//Models
#include "stphotobookcollectionmodel.h"
#include "stphotobook.h"

//Image Selection
#include "spimageslistview.h"
#include "spimageboxlistview.h"
#include "stphotobookcheckedproxymodel.h"
#include "qxtlabel.h"

#include "stcollectiontemplatemodel.h"
#include "fileselectlineedit.h"
#include "stphotobookcollectioninfo.h"
#include "smessagebox.h"
#include "stutils.h"
#include "stphotolayout.h" 
#include "disksourceselectionwidget.h" 
#include "strecentfiles.h"
#include "sterror.h"


//_____________________________________________________________________________
//
// class hooseTemplateModePage
//_____________________________________________________________________________

ChooseTemplateModePage::ChooseTemplateModePage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Photo book size</h1>"));
	setSubTitle(tr("<p>Which are the <em>Photo Book</em> sizes?</p> You have 2 options:"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	QFont RBFont = font(); 
	RBFont.setBold(true); 
	RBFont.setPointSize(12); 
	RBCustomSizes = new QRadioButton(tr("Create custom size PhotoBook"), this); 
	RBCustomSizes->setFont(RBFont); 
	RBCustomSizes->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred); 
	MLayout->addWidget(RBCustomSizes); 
	RBCustomSizes->setChecked(true); 			
	MLayout->addWidget(new QLabel(tr("<b>Specify</b> a custom photobook size. <br/> <em> Note:Suitabe for template creation too.</em>"), this)); 

	QRadioButton* RBFromTemplate = new QRadioButton(tr("Create from template"), this); 
	RBFromTemplate->setFont(RBFont); 
	MLayout->addWidget(RBFromTemplate); 
	MLayout->addWidget(new QLabel(tr("Create photobook using template sizes."), this)); 

}


int ChooseTemplateModePage::nextId() const
{
	if (RBCustomSizes->isChecked())
		return STAlbumWizard::Page_CustomSizes; 
	else 
		return STAlbumWizard::Page_ChooseTemplate;
}



//_____________________________________________________________________________
//
// class ChooseTemplatePage
//_____________________________________________________________________________

void ChooseTemplatePage::setCurrentState(ChooseTemplatePage::EnState _State)
{
	BottomFrame->setVisible(_State != StateNoTypeSelected && _State != StateTemplatesEmpty);
	LabSize->setVisible(BottomFrame->isVisible());
	CBSize->setVisible(BottomFrame->isVisible());
	if (BottomFrame->isVisible())
	{
		NoInfoFrame->setVisible(_State != StateShowWebInfo);
		//WebView->setVisible(!NoInfoFrame->isVisible());
	}
	InetgetTimer->stop();

	View->setVisible(_State != StateTemplatesEmpty);
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


ChooseTemplatePage::ChooseTemplatePage(QWidget* _Parent) : QWizardPage(_Parent), HasPreselection(false)
{
	setTitle(tr("<h1>Template selection</h1>"));
	setSubTitle(tr("The <em>Photo Book</em> template defines the photobook features like size, number of pages, layouts, etc... </br> Use the following list to choose the template that you want for your <em>Photo Book</em>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	QHBoxLayout* TopLayout = new QHBoxLayout;
	MLayout->addLayout(TopLayout);

	TBType = new QToolBar(this);
	TopLayout->addWidget(TBType);
	TBType->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	TBType->setIconSize(QSize(48, 48));

	QAction* NewAction;
	TypeActions = new QActionGroup(this);
	TypeActions->setExclusive(true);

	NewAction = TypeActions->addAction(QIcon(":/st/wizards/typephotobook.png"), tr("PhotoBook"));
	NewAction->setData(STPhotoLayout::TypePhotoBook);
	NewAction->setCheckable(true);

	NewAction = TypeActions->addAction(QIcon(":/st/wizards/typecalendar.png"), tr("Calendar"));
	NewAction->setData(STPhotoLayout::TypeCalendar);
	NewAction->setCheckable(true);

	NewAction = TypeActions->addAction(QIcon(":/st/wizards/typecard.png"), tr("Card"));
	NewAction->setData(STPhotoLayout::TypeCard);
	NewAction->setCheckable(true);

	NewAction = TypeActions->addAction(QIcon(":/st/wizards/typeidphoto.png"), tr("IdPhoto"));
	NewAction->setData(STPhotoLayout::TypeIdPhoto);
	NewAction->setCheckable(true);

	NewAction = TypeActions->addAction(QIcon(":/st/wizards/typemultiphoto.png"), tr("MultiPhoto"));
	NewAction->setData(STPhotoLayout::TypeMultiPhoto);
	NewAction->setCheckable(true);

	TBType->addActions(TypeActions->actions());
	connect(TypeActions, SIGNAL(triggered(QAction*)), this, SLOT(reloadTemplates()));

	TopLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Preferred));

	LabSize = new QLabel(tr("Size:"), this);
	LabSize->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	TopLayout->addWidget(LabSize);
	CBSize = new QComboBox(this);
	CBSize->setMinimumWidth(250);
	TopLayout->addWidget(CBSize);

	TopLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));

	BottomFrame = new QFrame(this);
	MLayout->addWidget(BottomFrame);

	QHBoxLayout* BottomLayout = new QHBoxLayout(BottomFrame);

	QVBoxLayout* LeftLayout = new QVBoxLayout;
	BottomLayout->addLayout(LeftLayout);
	LeftLayout->addWidget(new QLabel(tr("Themes:"), this));
	View = new QListView(this); 
	Model = new STCollectionTemplateModel(this);
	View->setModel(Model);

	View->setIconSize(QSize(64, 64)); 
	View->setMinimumWidth(340);
	View->setVisible(false);
	connect(View, SIGNAL(clicked( const QModelIndex& )), this, SLOT(slotTemplateIndexClicked(const QModelIndex& )));
	connect(View, SIGNAL(clicked( const QModelIndex& )), this, SIGNAL(completeChanged()));
	LeftLayout->addWidget(View);

	NoTemplatesLabel = new QLabel(this);
	NoTemplatesLabel->setText(tr("<h2>There is no templates of this type.</h2>"));
	NoTemplatesLabel->setWordWrap(true);
	MLayout->addWidget(NoTemplatesLabel);

	QGridLayout* RightLayout = new QGridLayout;
	BottomLayout->addLayout(RightLayout);
	RightLayout->addWidget(new QLabel(tr("Information:"), this), 0, 0);

	WebView = new QWebView(this);
	RightLayout->addWidget(WebView, 1, 0);
	//WebView->load(QUrl("http://sites.google.com/site/orometemplates/km_magazinea4"));
	connect(WebView, SIGNAL(loadStarted()), this, SLOT(slotWebLoadStarted()));
	connect(WebView, SIGNAL(loadFinished(bool)), this, SLOT(slotWebLoadFinished(bool)));
	//WebView->setMinimumWidth(460);
	//WebView->setMinimumHeight(500);
	WebView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


	NoInfoFrame = new QFrame(this);
	NoInfoFrame->setMinimumHeight(460);
	NoInfoFrame->setMinimumWidth(500);
	RightLayout->addWidget(NoInfoFrame, 1, 0);

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
	connect(InetgetTimer, SIGNAL(timeout()), this, SLOT(inetgetBlinkTimeout()));

	setCurrentState(StateNoInfo);

}

int ChooseTemplatePage::nextId() const
{
	return STAlbumWizard::Page_CooseCreationMode;
}


void ChooseTemplatePage::selectFirstIndex()
{
	if (Model->rowCount() > 0)
	{
		View->setCurrentIndex(Model->index(0,0));
		slotTemplateIndexClicked(Model->index(0,0));
	}
	completeChanged();
}

void ChooseTemplatePage::initializePage()
{
	QAction* CAction;
	foreach(CAction, TypeActions->actions())
		CAction->setChecked(false);
	setCurrentState(StateNoTypeSelected);
	reloadTemplates();
}

STPhotoLayout::EnLayoutType ChooseTemplatePage::currentType() const
{
	STPhotoLayout::EnLayoutType Res;
	if (HasPreselection)
		Res = PreselectedType;
	else
	{
		if (TypeActions->checkedAction())
			Res = static_cast<STPhotoLayout::EnLayoutType>(TypeActions->checkedAction()->data().toInt());
	}
	return Res;
}

STDom::DDocFormat ChooseTemplatePage::currentSize() const
{
	STDom::DDocFormat Res;
	if (CBSize->currentIndex() != -1)
		Res = CBSize->itemData(CBSize->currentIndex()).value<STDom::DDocFormat>();
	return Res;
}

bool ChooseTemplatePage::validatePage()
{
	TemplateFileInfo = Model->photoLayoutFileInfo(View->currentIndex(), currentSize());
	emit templateSelected();
	//TODO: Set the current locale.
	return true; 
}

bool ChooseTemplatePage::isComplete() const
{
	return Model->rowCount() > 0 && View->currentIndex().isValid();
}

bool ChooseTemplatePage::typeSelected()
{
	return TypeActions->checkedAction() != 0 || HasPreselection;
}

void ChooseTemplatePage::setTemplateType(STPhotoLayout::EnLayoutType _Type)
{
	PreselectedType = _Type;
	HasPreselection = true;
	TBType->setVisible(false);
	reloadTemplates();
}

void ChooseTemplatePage::slotTemplateIndexClicked(const QModelIndex& _Index)
{
	WebView->stop();

	//Load available sizes.
	CBSize->clear();
	STDom::DDocFormatList AvailableFormats = Model->availableFormats(_Index);
	STDom::DDocFormatList::iterator it;
	for (it = AvailableFormats.begin(); it != AvailableFormats.end(); ++it)
	{
		QVariant MVariant;
		MVariant.setValue(*it);
		CBSize->addItem(it->toString(), MVariant);
	}

	//Get info url from model and display it.
	QUrl InfoUrl = Model->photoLayoutInfoUrl(_Index);
	WebView->setHtml("");
	setCurrentState(StateGettingInfo);
	WebView->load(InfoUrl);
}

void ChooseTemplatePage::slotWebLoadStarted()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void ChooseTemplatePage::slotWebLoadFinished(bool _Ok)
{
	QApplication::restoreOverrideCursor();
	if (_Ok)
		setCurrentState(StateShowWebInfo);
	else
		setCurrentState(StateNoInfo);
}

void ChooseTemplatePage::reloadTemplates()
{
	if (typeSelected())
	{
		Model->loadTemplates(currentType());
		if (Model->rowCount() == 0)
			setCurrentState(StateTemplatesEmpty);
		else
			setCurrentState(StateGettingInfo);
		selectFirstIndex();
	}
}

void ChooseTemplatePage::inetgetBlinkTimeout()
{
	if (InetgetPicture1)
		LabelInfoPixmap->setPixmap(QPixmap(":/inetget.png"));
	else
		LabelInfoPixmap->setPixmap(QPixmap(":/inetget_2.png"));

	InetgetPicture1 = !InetgetPicture1;
}

//_____________________________________________________________________________
//
// class CustomSizesPage
//_____________________________________________________________________________

QSpinBox* CustomSizesPage::newSizeSpinBox(QWidget* _Parent, const QString& _FieldName)
{
	QSpinBox* Res = new QSpinBox(_Parent); 
	connect(Res, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	Res->setSuffix(tr(" mm")); 
	Res->setRange(0, 9999); 
	registerField(_FieldName, Res);
	return Res; 	
} 

QLabel* CustomSizesPage::newLabel(const QString& _Text, QWidget* _Parent)
{
	QLabel* Res = new QLabel(_Text + ":", _Parent); 
	Res->setAlignment(Qt::AlignRight); 
	QFont LFont = Res->font(); 
	LFont.setBold(true); 
	Res->setFont(LFont); 
	return Res; 
}

CustomSizesPage::CustomSizesPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Photo Book sizes</h1>"));
	setSubTitle(tr("<p>Please specify the new <em>Photo Book</em> sizes and features</p>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	MLayout->addWidget(new QLabel(tr("<h3>New Photo Book Name: </h3>"), this));
	QLineEdit* LEName = new QLineEdit(this);
	MLayout->addWidget(LEName);
	registerField("photobookname*", LEName);

	QGroupBox* GBCover = new QGroupBox(tr("Cover"), this); 
	MLayout->addWidget(GBCover); 
	QHBoxLayout* GBCoverLayout = new QHBoxLayout(GBCover); 
	
	GBCoverLayout->addWidget(newLabel(tr("Width"), GBCover)); 
	GBCoverLayout->addWidget(newSizeSpinBox(GBCover, "coverwidth")); 
	
	GBCoverLayout->addWidget(newLabel(tr("Height"), GBCover)); 
	GBCoverLayout->addWidget(newSizeSpinBox(GBCover, "coverheight")); 

	QSpinBox* NewSPBox = new QSpinBox(GBCover); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(0, 100); 
	NewSPBox->setSuffix(tr(" mm")); 
	GBCoverLayout->addWidget(newLabel(tr("Margin"), GBCover)); 
	registerField("covermargin", NewSPBox); 
	GBCoverLayout->addWidget(NewSPBox); 
	

	QGroupBox* GBPage = new QGroupBox(tr("Page"), this); 
	MLayout->addWidget(GBPage); 
	QHBoxLayout* GBPageLayout = new QHBoxLayout(GBPage); 
	
	GBPageLayout->addWidget(newLabel(tr("Width"), GBPage)); 
	GBPageLayout->addWidget(newSizeSpinBox(GBPage, "pagewidth")); 
	
	GBPageLayout->addWidget(newLabel(tr("Height"), GBPage)); 
	GBPageLayout->addWidget(newSizeSpinBox(GBPage, "pageheight")); 

	NewSPBox = new QSpinBox(GBPage); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(0, 100); 
	NewSPBox->setSuffix(tr(" mm")); 
	GBPageLayout->addWidget(newLabel(tr("Margin"), GBPage)); 
	registerField("pagemargin", NewSPBox); 
	GBPageLayout->addWidget(NewSPBox); 

	QGroupBox* GBLayout = new QGroupBox(tr("Layout"), this); 
	MLayout->addWidget(GBLayout); 
	QHBoxLayout* GBLayoutLayout = new QHBoxLayout(GBLayout); 
	QButtonGroup* BGLayout = new QButtonGroup(this); 

	QRadioButton* RBSinglePage = new QRadioButton(tr("Single Page"), this); 	
	BGLayout->addButton(RBSinglePage); 
	registerField("singlepagelayout", RBSinglePage); 
	GBLayoutLayout->addWidget(RBSinglePage); 

	QRadioButton* RBDoublePage = new QRadioButton(tr("Double Page"), this); 	
	BGLayout->addButton(RBDoublePage); 
	registerField("doublepagelayout", RBDoublePage); 
	GBLayoutLayout->addWidget(RBDoublePage); 
	
	QGroupBox* GBValidPages = new QGroupBox(tr("Valid pages"), this); 
	MLayout->addWidget(GBValidPages); 
	QHBoxLayout* GBValidPagesLayout = new QHBoxLayout(GBValidPages); 

	QCheckBox* CBFirstPage = new QCheckBox(tr("First page is editable"), GBValidPages); 
	registerField("firstpageeditable", CBFirstPage); 
	GBValidPagesLayout->addWidget(CBFirstPage); 

	QCheckBox* CBLastPage = new QCheckBox(tr("Last page is editable"), GBValidPages); 
	registerField("lastpageeditable", CBLastPage); 
	GBValidPagesLayout->addWidget(CBLastPage); 
	
	QHBoxLayout* BottomLayout = new QHBoxLayout; 
	MLayout->addLayout(BottomLayout); 

	BottomLayout->addWidget(newLabel(tr("Min. Pages"), this)); 
	NewSPBox = new QSpinBox(this); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(0, 999); 
	BottomLayout->addWidget(NewSPBox); 
	registerField("minpages", NewSPBox); 
	
	BottomLayout->addWidget(newLabel(tr("Max. Pages"), this)); 
	NewSPBox = new QSpinBox(this); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(0, 999); 
	BottomLayout->addWidget(NewSPBox); 
	registerField("maxpages", NewSPBox); 

	BottomLayout->addWidget(newLabel(tr("DPIS"), this)); 
	NewSPBox = new QSpinBox(this); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(0, 999); 
	BottomLayout->addWidget(NewSPBox); 
	registerField("dpis", NewSPBox); 

}

void CustomSizesPage::initializePage()
{
	setField("doublepagelayout", true); 
	setField("firstpageeditable", true); 
	setField("lastpageeditable", true); 
	setField("minpages", 2); 
	setField("maxpages", 20); 
	setField("dpis", 300); 
}

bool CustomSizesPage::isComplete () const
{
	return field("pagewidth").toInt() > 0 && 
		field("pageheight").toInt() > 0 &&
		field("pagemargin").toInt() > 0 &&
		field("coverwidth").toInt() > 0 &&
		field("coverheight").toInt() > 0 &&
		field("covermargin").toInt() > 0 &&
		field("dpis").toInt() > 0 &&
		field("minpages").toInt() <= field("maxpages").toInt(); 
}

bool CustomSizesPage::validatePage()
{
	STPhotobookCollectionInfo PCInfo(field("photobookname").toString()); 
	QDir PBDir(PCInfo.photoBookPath()); 

	STPhotoBookTemplate PhotoBook; 
	PhotoBook.setSize(QSizeF(field("pagewidth").toInt(), field("pageheight").toInt())); 
	PhotoBook.setDpis(field("dpis").toInt()); 
	PhotoBook.setMinPages(field("minpages").toInt()); 
	PhotoBook.setMaxPages(field("maxpages").toInt()); 

	//Dummy cover frame.
	STPhotoLayoutTemplate CoverTemplate; 
	CoverTemplate.setSize(QSizeF(field("coverwidth").toInt(), field("coverheight").toInt()));
	CoverTemplate.setIsFirstPage(true); 
	CoverTemplate.addFrame(STPhotoLayoutTemplate::Frame(QRectF(QPointF(0,0), CoverTemplate.size())));
	PhotoBook.templates().push_back(CoverTemplate); 

	STPhotoBookTemplate::TMarginRectList PageMarginRects; 
	PageMarginRects.push_back(QRectF(field("pagemargin").toInt(), field("pagemargin").toInt(),
			PhotoBook.size().width() - (field("pagemargin").toInt() * 2), PhotoBook.size().height() - (field("pagemargin").toInt() * 2)));
	if (field("doublepagelayout").toBool())
		PageMarginRects.push_back(QRectF(PhotoBook.size().width() / 2, 0, 1, PhotoBook.size().height()));
	PhotoBook.setPageMarginRects(PageMarginRects); 

	STPhotoBookTemplate::TMarginRectList CoverMarginRects; 
	CoverMarginRects.push_back(QRectF(field("covermargin").toInt(), field("covermargin").toInt(),
			CoverTemplate.size().width() - (field("covermargin").toInt() * 2), CoverTemplate.size().height() - (field("covermargin").toInt() * 2)));
	PhotoBook.setCoverMarginRects(CoverMarginRects); 


	PhotoBook.setBGImageDirPath("./backgrounds");
	PhotoBook.setClipartDirPath("./clipart"); 
	PhotoBook.setMaskDirPath("./masks"); 
	PhotoBook.setAuthorOrganization("Starblitz");
	PhotoBook.setAuthorName(QCoreApplication::applicationName()); 
	if (field("doublepagelayout").toBool())
		PhotoBook.setPrintPageWidth(field("pagewidth").toInt() / 2); 
	else 
		PhotoBook.setPrintPageWidth(field("pagewidth").toInt()); 



	bool Res = false; 
	if (!PBDir.exists())
	{
		PBDir.mkpath(PBDir.absolutePath()); 
		PBDir.mkpath(PBDir.absolutePath() + "/backgrounds"); 
		PBDir.mkpath(PBDir.absolutePath() + "/clipart"); 
		PBDir.mkpath(PBDir.absolutePath() + "/masks"); 
	}
	try
	{
		TemplateFileInfo.setFile(PBDir.absolutePath() + "/template.kpspltml" );
		PhotoBook.save(TemplateFileInfo.absoluteFilePath());
		Res = true; 
	}
	catch (STPhotoBookTemplate::Error& _Error)
	{
		SMessageBox::critical(this, tr("Error saving photobook template"), _Error.description()); 
	}
	return Res; 
}

int CustomSizesPage::nextId() const
{
	return STAlbumWizard::Page_End;
}

//_____________________________________________________________________________
//
// class ChooseCreationModePage
//_____________________________________________________________________________

ChooseCreationModePage::ChooseCreationModePage(QWidget* _Parent) : QWizardPage(_Parent), PredesignPhotoItems(0)
{
	setTitle(tr("<h1>Creation mode selection</h1>"));
	setSubTitle(tr("How do you want to create your <em>Photo Book</em>?:"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	

	QGridLayout* TopLayout = new QGridLayout;
	MLayout->addLayout(TopLayout);

	QButtonGroup* BGCreationMode = new QButtonGroup(this);
	PBAutomatic = new QxtPushButton(tr("<table><tr>"
									   "<td><h1>Automatic picture fill</h1><p>"
									   "<b>Automatically</b> fills your book with selected source media. <br/> <em> Note:Once the book is created you can modify everything you want.</em></td>"
									   "<td><center><img src=:/st/wizards/automatic.png /></center></p></td></tr></table>"), this);
	PBAutomatic->setCheckable(true);
	PBAutomatic->setTextFormat(Qt::RichText);
	PBAutomatic->setObjectName("OptionPushButton");
	TopLayout->addWidget(PBAutomatic, 0, 0);
	BGCreationMode->addButton(PBAutomatic);

	QxtPushButton* PBManual = new QxtPushButton(tr("<table><tr>"
												   "<td><h1>Manual picture fill</h1><p>"
												   "Lets the picture filling up to you.</td>"
												   "<td><center><img src=:/st/wizards/manual.png /></center></p></td></tr></table>"), this);
	PBManual->setCheckable(true);
	PBManual->setTextFormat(Qt::RichText);
	PBManual->setObjectName("OptionPushButton");
	TopLayout->addWidget(PBManual, 0, 1);
	BGCreationMode->addButton(PBManual);



	GBUsePredesign = new QxtGroupBox(tr("Use a predesign"), this);
	MLayout->addWidget(GBUsePredesign);
	GBUsePredesign->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

	QHBoxLayout* GBLayout = new QHBoxLayout(GBUsePredesign);

	LVPredesigns = new QListView(GBUsePredesign);
	LVPredesigns->setIconSize(QSize(32, 32));
	LVPredesigns->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

	GBLayout->addWidget(LVPredesigns);

	PredesignModel = new STPhotoBookCollectionModel(this);
	PredesignModel->setThumbnailSize(LVPredesigns->iconSize());
	LVPredesigns->setModel(PredesignModel);
	connect(LVPredesigns->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotPredesignChanged(QModelIndex)));

	TBDescription = new QTextBrowser(this);
	GBLayout->addWidget(TBDescription);
	TBDescription->setHtml("<h1>This is a sample description</h1><p>Here comes the description for each selected item. It depends upon a description that user enters while predesign creation.</p><img src=./automatic.png/>");

	PBAutomatic->setChecked(true);
}


int ChooseCreationModePage::nextId() const
{
//	if (RBAutomaticFill->isChecked())
	return STAlbumWizard::Page_BuildOptions;
//	else
//		return STAlbumWizard::Page_End;
}

void ChooseCreationModePage::setPhotoBookTemplateFileInfo(const QFileInfo& _FileInfo)
{
	//Fill predesigns of template.
	STCollectionTemplateInfo TemplateInfo(_FileInfo.dir());
	PredesignModel->setRootDir(TemplateInfo.predesignRootDir());
	PredesignModel->load(true);
	GBUsePredesign->setEnabled(true);
	if (PredesignModel->rowCount()==0)
	{
		GBUsePredesign->setChecked(false);
		GBUsePredesign->setEnabled(false);
	}
	else
	{
		LVPredesigns->setCurrentIndex(PredesignModel->index(0, 0));
		GBUsePredesign->setChecked(true);
	}

}

bool ChooseCreationModePage::validatePage()
{
	PredesignPhotoItems = 0;
	if (usePredesign())
	{
		try
		{
			STPhotoBook CurrentPB;
			CurrentPB.load(predesignDir());
			PredesignPhotoItems = CurrentPB.numPhotoFrames();
		}
		catch (STError& _Error)
		{
			SMessageBox::critical(this, tr("Error loading predesign"), _Error.description());
		}
	}
	return true;
}

bool ChooseCreationModePage::autoBuildModeSelected() const
{
	return PBAutomatic->isChecked();
}

bool ChooseCreationModePage::usePredesign() const
{
	return GBUsePredesign->isChecked();
}

QDir ChooseCreationModePage::predesignDir() const
{
	QDir Res;
	if (usePredesign())
		Res = QDir(PredesignModel->photoBookPath(LVPredesigns->currentIndex()));

	return Res;
}

void ChooseCreationModePage::slotPredesignChanged(const QModelIndex& _Index)
{
	if (_Index.isValid()) //Defensive
	{
		//Lets load the photobook
		QDir PhotoBookDir(PredesignModel->photoBookPath(_Index));
		STPhotoBook CurrPhotoBook;
		try
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			STPhotobookCollectionInfo PBInfo(PhotoBookDir);
			CurrPhotoBook.load(PhotoBookDir);
			TBDescription->setHtml(QString("<h1>%1</h1><p>%2</p><center><img src=""%3""/></center>").arg(CurrPhotoBook.name()).arg(CurrPhotoBook.description()).arg(
					QString(QUrl::fromLocalFile(PBInfo.thumbnailFileName()).toEncoded())));
		}
		catch(STError& _Error)
		{
			SMessageBox::critical(this, tr("Error loading predesign"), _Error.description());
		}
		QApplication::restoreOverrideCursor();

	}
}

//_____________________________________________________________________________
//
// class BuildOptionsPage
//_____________________________________________________________________________

BuildOptionsPage::BuildOptionsPage(QWidget* _Parent) : QWizardPage(_Parent), AutoBuildMode(false)
{
	setTitle(tr("<h1>Build options</h1>"));
	setSubTitle(tr("<p>Please, configure your <em>Photo Book</em>?</p> automatic options:"));
	QVBoxLayout* MLayout = new QVBoxLayout(this);


	// ----  General  ----
	GBGeneral = new QGroupBox(tr("General"), this);
	MLayout->addWidget(GBGeneral);
	QVBoxLayout* GenLayout = new QVBoxLayout(GBGeneral);
	QCheckBox* CBAutoAdjust = new QCheckBox(tr("Auto adjust images to frames"), GBGeneral);
	GenLayout->addWidget(CBAutoAdjust);
	registerField("autoadjust", CBAutoAdjust);

	QCheckBox* CBUseBackgrounds = new QCheckBox(tr("Use photos as background images"), GBGeneral);
	GenLayout->addWidget(CBUseBackgrounds);
	registerField("autofillbackgrounds", CBUseBackgrounds);

	QCheckBox* CBDetectRotations = new QCheckBox(tr("Auto detect image orientation"), GBGeneral);
	GenLayout->addWidget(CBDetectRotations);
	registerField("autodetectrotation", CBDetectRotations);


	// ----  PhotoBook  ----
	GBPhotoBook = new QGroupBox(tr("PhotoBoook"), this);
	MLayout->addWidget(GBPhotoBook);
	QVBoxLayout* PhotoBookLayout = new QVBoxLayout(GBPhotoBook);
	QCheckBox* CBIncludeTexts = new QCheckBox(tr("Include layouts with texts"), GBPhotoBook);
	PhotoBookLayout->addWidget(CBIncludeTexts);
	registerField("includetexts", CBIncludeTexts);

	QFormLayout* FormLayout = new QFormLayout();
	PhotoBookLayout->addLayout(FormLayout);
	QLineEdit* LETitle = new QLineEdit(GBPhotoBook);
	registerField("title", LETitle);
	FormLayout->addRow(tr("Title"), LETitle);

	SBNumPages = new QSpinBox(GBPhotoBook);
	registerField("numpages", SBNumPages);
	FormLayout->addRow(tr("Number of pages"), SBNumPages);
	SBNumPages->setRange(1, 999);


	// ----  Calendar  ----
	GBCalendar = new QGroupBox(tr("Calendar"), this);
	MLayout->addWidget(GBCalendar);
	QFormLayout* CalFormLayout = new QFormLayout(GBCalendar);

	QDateTimeEdit* FromMonth = new QDateTimeEdit(GBCalendar);
	registerField("fromdate", FromMonth);
	FromMonth->setDisplayFormat("MMMM yyyy");
	FromMonth->setCalendarPopup(true);
	CalFormLayout->addRow(tr("From month"), FromMonth);
	connect(FromMonth, SIGNAL(dateChanged(QDate)), this, SIGNAL(completeChanged()));

	QDateTimeEdit* ToMonth = new QDateTimeEdit(GBCalendar);
	registerField("todate", ToMonth);
	ToMonth->setDisplayFormat("MMMM yyyy");
	ToMonth->setCalendarPopup(true);
	CalFormLayout->addRow(tr("To month"), ToMonth);
	connect(ToMonth, SIGNAL(dateChanged(QDate)), this, SIGNAL(completeChanged()));
}

int BuildOptionsPage::nextId() const
{
	int Res;
	if (AutoBuildMode)
		Res = STAlbumWizard::Page_SelectDiskFolder;
	else
		Res = STAlbumWizard::Page_End;
	return Res;
}

void BuildOptionsPage::initializePage()
{
}


bool BuildOptionsPage::isComplete() const
{
	STPhotoBookBuildOptions BOptions = getBuildOptions();
	bool Res = true;
	Res = Res && BOptions.fromDate() < BOptions.toDate();
	return Res;
}


void BuildOptionsPage::setBuildOptions(const STPhotoBookBuildOptions& _Options)
{
	setField("autoadjust", _Options.autoadjustFrames());
	setField("autofillbackgrounds", _Options.autoFillBackgrounds());
	setField("autodetectrotation", !_Options.ignoreExifRotation());
	setField("includetexts", _Options.useTexts());
	setField("title", _Options.title());
	setField("numpages", _Options.pagesToFill());
	setField("fromdate", _Options.fromDate());
	setField("todate", _Options.toDate());
}


STPhotoBookBuildOptions BuildOptionsPage::getBuildOptions() const
{
	STPhotoBookBuildOptions Res;
	Res.setAutoadjustFrames(field("autoadjust").toBool());
	Res.setAutoFillBackgrounds(field("autofillbackgrounds").toBool());
	Res.setIgnoreExifRotation(!field("autodetectrotation").toBool());
	Res.setUseTexts(field("includetexts").toBool());
	Res.setTitle(field("title").toString());
	Res.setPagesToFill(field("numpages").toInt());
	Res.setFromDate(field("fromdate").toDate());
	Res.setToDate(field("todate").toDate());
	return Res;
}

void BuildOptionsPage::setTemplate(const STPhotoBookTemplate& _Template, STPhotoLayout::EnLayoutType _Type)
{
	SBNumPages->setRange(_Template.minPages(), _Template.maxPages());

	LayoutType = _Type;
	STPhotoBookBuildOptions InitOpts;
	InitOpts.setDefaults(LayoutType, _Template.minPages());
	setBuildOptions(InitOpts);
	GBPhotoBook->setVisible(_Type == STPhotoLayout::TypePhotoBook);
	GBCalendar->setVisible(_Type == STPhotoLayout::TypeCalendar);

}

void BuildOptionsPage::setAutoBuildMode(bool _Value)
{
	AutoBuildMode = _Value;
	GBGeneral->setVisible(_Value);
}

void BuildOptionsPage::setUsePredesign(bool _Value)
{
	if (GBPhotoBook->isVisible())
		GBPhotoBook->setVisible(!_Value);
}


//_____________________________________________________________________________
//
// class SelectDiskFolderPage
//_____________________________________________________________________________


void SelectDiskFolderPage::updateInfo()
{
	//ImageBoxListView->model()->rowCount()
	QString FontColor="#000000";
	int SelectedImages = ImageBoxListView->model()->rowCount();
	int OptimalImageNum = 0;
	if (AbsoluteImageCount)
		OptimalImageNum = AbsoluteImageCount;
	else
		OptimalImageNum = OptimalImagesPerPage * PagesToFill;
	if ( SelectedImages < OptimalImageNum)
		FontColor="#FF0000";
	InfoLabel->setText(tr("Optimal image number: <span style=\"font-size:20px;\"><font color=\%1><b>%2</b></font></span>, Images per page: <span style=\"font-size:20px;\"><font  color=\%1><b>%3</b></font></span>").arg(
			FontColor).arg(OptimalImageNum).arg(SelectedImages / qMax(PagesToFill, 1)));

}

SelectDiskFolderPage::SelectDiskFolderPage(QWidget* _Parent) : QWizardPage(_Parent), PagesToFill(0), OptimalImagesPerPage(0), AbsoluteImageCount(0)
{
	setTitle(tr("<h1>Image selection</h1>"));
	setSubTitle(tr("<p>Please choose your PhotoBook images. Drag&Drop them to the list at the bottom.</p>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->setMargin(0);
	MLayout->setSpacing(0);

	QHBoxLayout* TopLayout = new QHBoxLayout;
	TopLayout->setMargin(0);
	TopLayout->setSpacing(0);
	MLayout->addLayout(TopLayout);

	QxtLabel* SourceLabel = new QxtLabel(tr("Source Images"), this);
	SourceLabel->setMinimumWidth(30);
	SourceLabel->setRotation(Qxt::CounterClockwise);
	TopLayout->addWidget(SourceLabel);
	//MLayout->addWidget(new QLabel(tr("Folder name:"), this));
	SPImagesListView* ImagesListView = new SPImagesListView(this);
	ImagesListView->model()->setNumMatchLimit(1);
	TopLayout->addWidget(ImagesListView);
	ImagesListView->showSelectAllAction(true);

	QHBoxLayout* MiddleLayout = new QHBoxLayout;
	MiddleLayout->setMargin(0);
	MiddleLayout->setSpacing(0);
	MLayout->addLayout(MiddleLayout);

	QxtLabel* DestinationLabel = new QxtLabel(tr("PhotoBook Images"), this);
	DestinationLabel->setMinimumWidth(30);
	DestinationLabel->setRotation(Qxt::CounterClockwise);
	MiddleLayout->addWidget(DestinationLabel);

	ImageBoxListView = new SPImageBoxListView(this);
	InfoLabel = new QLabel(this);
	ImageBoxListView->toolBar()->addSeparator();
	ImageBoxListView->toolBar()->addWidget(InfoLabel);
	MiddleLayout->addWidget(ImageBoxListView );

	ImageBoxListView->model()->setReportDropsMd5(true);
	connect(ImageBoxListView->model(), SIGNAL(imageDropped(QString,QString)), ImagesListView->model(), SLOT(slotCheckImage(QString, QString)));
	connect(ImageBoxListView->model(), SIGNAL(imageRemoved(QString,QString)), ImagesListView->model(), SLOT(slotUncheckImage(QString,QString)));
	connect(ImageBoxListView->model(), SIGNAL(rowsInserted(const QModelIndex&, int , int )), this, SIGNAL(completeChanged()));
	connect(ImageBoxListView->model(), SIGNAL(rowsRemoved(const QModelIndex&, int , int )), this, SIGNAL(completeChanged()));
	connect(this, SIGNAL(completeChanged()), this, SLOT(slotUpdateInfo()));

	updateInfo();
}

void SelectDiskFolderPage::setTemplate(const STPhotoBookTemplate& _Template, const STPhotoBookBuildOptions& _Options)
{
	PagesToFill = _Options.pagesToFill();
	OptimalImagesPerPage = _Template.numOptimalImagesPerPage();
	AbsoluteImageCount = 0;
	updateInfo();
}

void SelectDiskFolderPage::setAbsoluteImageCount(int _Value)
{
	AbsoluteImageCount = _Value;
	updateInfo();
}

void SelectDiskFolderPage::clearSelection()
{
	ImageBoxListView->model()->sourceModel()->clear();
}

int SelectDiskFolderPage::nextId() const
{
	return STAlbumWizard::Page_End; 
}

bool SelectDiskFolderPage::isComplete() const
{
	return ImageBoxListView->model()->rowCount() > 0;
}

STDom::DDocModel* SelectDiskFolderPage::selectedImages() const
{
	return ImageBoxListView->model()->sourceModel();
}

void SelectDiskFolderPage::slotUpdateInfo()
{
	updateInfo();
}


//_____________________________________________________________________________
//
// class AlbumWizardEndPage
//_____________________________________________________________________________

AlbumWizardEndPage::AlbumWizardEndPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Ready to create your Photobook.</h1>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	//setPixmap(QWizard::WatermarkPixmap, QPixmap(":/st/wizards/watermark.png"));
	
	QLabel* DescLabel = new QLabel(tr("<p>When you press <em>Finish</em> your Photobook will be created. Then you can modify everything you need. Look in menu and icon bar to find all your operations.</p><p>Remember to save your changes and don't hessitate to read help pages and manuals if you need some help.</p>"));
	DescLabel->setWordWrap(true); 
	MLayout->addWidget(DescLabel); 
	
	QLabel* LabEnjoy = new QLabel(tr("<h1>Enjoy Starblitz Photo Book !</h1>"), this);
	LabEnjoy->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	LabEnjoy->setAlignment(Qt::AlignCenter);
	MLayout->addWidget(LabEnjoy);
	QLabel* LabPixmap = new QLabel(this);
	LabPixmap->setPixmap(QPixmap(":/st/wizards/albumwizard.png"));
	LabPixmap->setAlignment(Qt::AlignCenter);
	MLayout->addWidget(LabPixmap);
}
	
//_____________________________________________________________________________
//
// class STAlbumWizard
//_____________________________________________________________________________

STAlbumWizard::STAlbumWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), CustomSizesEnabled(false)
{
	setPage(Page_ChooseTemplateMode, new ChooseTemplateModePage(this));
	CCustomSizesPage = new CustomSizesPage(this);
	setPage(Page_CustomSizes, CCustomSizesPage); 
	CTemplatePage = new ChooseTemplatePage(this);
	setPage(Page_ChooseTemplate, CTemplatePage);
	CCreationModePage = new ChooseCreationModePage(this);
	setPage(Page_CooseCreationMode, CCreationModePage);
	PBuildOptions = new BuildOptionsPage(this);
	setPage(Page_BuildOptions, PBuildOptions);
	SDFolderPage = new SelectDiskFolderPage(this);
	setPage(Page_SelectDiskFolder, SDFolderPage);
	setPage(Page_End, new AlbumWizardEndPage(this)); 
	
	if (CustomSizesEnabled)
		setStartId(Page_ChooseTemplateMode);
	else
		setStartId(Page_ChooseTemplate);

#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif

	setOption(HaveHelpButton, true);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/albumwizard.png").scaled(64,64));

	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
	connect(CTemplatePage, SIGNAL(templateSelected()), this, SLOT(slotLoadTemplate()));

	setWindowTitle(QObject::tr("PhotoBook Wizard"));
}

STAlbumWizard::~STAlbumWizard()
{
}


QFileInfo STAlbumWizard::photoBookTemplateFileInfo() const
{
	QFileInfo Res; 
	//TODO Dependrà de si el creem d'un template o especifiquem les mides.
	if (hasVisitedPage(Page_ChooseTemplate))
		Res = CTemplatePage->photoBookTemplateFileInfo();
	else 
		Res = CCustomSizesPage->photoBookTemplateFileInfo();
	return Res; 
}

bool STAlbumWizard::autoFillSelected() const
{
	return CCreationModePage->autoBuildModeSelected();
}
	
//! DEPRECARED !!!
bool STAlbumWizard::imagesFromCollection() const
{
	//return hasVisitedPage(Page_SelCollectionFolder);
	return false;
}

//! DEPRECARED !!!
quint64 STAlbumWizard::collectionFolderKey() const
{
	//return ColFolderSelectPage->collectionFolderKey();
	return -1;
}

int STAlbumWizard::nextId() const
{

	int Res = QWizard::nextId();
	if (!CustomSizesEnabled && Res == Page_ChooseTemplateMode)
		Res = Page_ChooseTemplate;

	if (Res ==  Page_BuildOptions)
	{
		STPhotoLayout::EnLayoutType TemplType = CTemplatePage->currentType();
		PBuildOptions->setTemplate(PhotoBookTemplate, TemplType);
		PBuildOptions->setAutoBuildMode(CCreationModePage->autoBuildModeSelected());
		PBuildOptions->setUsePredesign(CCreationModePage->usePredesign());
		if (TemplType != STPhotoLayout::TypeCalendar && TemplType != STPhotoLayout::TypePhotoBook ||
				CCreationModePage->usePredesign())
			Res = PBuildOptions->nextId();
	}
	if (Res == Page_CooseCreationMode)
	{
		CCreationModePage->setPhotoBookTemplateFileInfo(CTemplatePage->photoBookTemplateFileInfo());
		SDFolderPage->clearSelection();
	}
	if (Res == Page_SelectDiskFolder)
	{
		if (CCreationModePage->usePredesign())
			SDFolderPage->setAbsoluteImageCount(CCreationModePage->predesignPhotoItems());
		else
			SDFolderPage->setTemplate(PhotoBookTemplate, PBuildOptions->getBuildOptions());
	}

	return Res; 
}

STPhotoLayout::EnLayoutType STAlbumWizard::templateType() const
{
	return CTemplatePage->currentType();
}

STDom::DDocModel* STAlbumWizard::selectedImages() const
{
	return SDFolderPage->selectedImages();
}

STPhotoBookBuildOptions STAlbumWizard::buildOptions() const
{
	return PBuildOptions->getBuildOptions();
}

bool STAlbumWizard::usePredesign() const
{
	return CCreationModePage->usePredesign();
}

QDir STAlbumWizard::predesignDir() const
{
	return CCreationModePage->predesignDir();
}

void STAlbumWizard::setTemplateType(STPhotoLayout::EnLayoutType _Type)
{
	CTemplatePage->setTemplateType(_Type);
}

void STAlbumWizard::slotLoadTemplate()
{
	try
	{
		PhotoBookTemplate.load(photoBookTemplateFileInfo().absoluteFilePath());
	}
	catch (STError& _Error)
	{
		SMessageBox::critical(this, tr("Error loading template"), _Error.description());
	}
}



