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

//Template info
#include <QtWebKit/QWebView>
#include <QApplication>

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


//_____________________________________________________________________________
//
// class PhotoBookNamePage
//_____________________________________________________________________________

PhotoBookNamePage::PhotoBookNamePage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>PhotoBook properties</h1>"));
	setSubTitle(tr("Please, enter the new <em>Photo Book</em> name. You will use this name later to identify the <em>Photo Book</em> in the collection. You can also set a description related to it."));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	MLayout->addWidget(new QLabel(tr("<h3>New Photo Book Name: </h3>"), this));
	LEName = new QLineEdit(this); 
	MLayout->addWidget(LEName);
	registerField("photobookname*", LEName);
	MLayout->addWidget(new QLabel(tr("Description: "), this));
	TEDescription = new QTextEdit(this); 
	MLayout->addWidget(TEDescription);
	registerField("photobookdescription", TEDescription, "plainText", SIGNAL(textChanged()));
	MLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
}

int PhotoBookNamePage::nextId() const
{
	return STAlbumWizard::Page_ChooseTemplateMode;
}

void PhotoBookNamePage::initializePage()
{
	LEName->setText(tr("NewPhotoBook")); 
	TEDescription->setPlainText(""); 
}

bool PhotoBookNamePage::validatePage()
{
	bool Res = false; 
	STPhotobookCollectionInfo PCInfo(field("photobookname").toString()); 
	QDir PBDir(PCInfo.photoBookPath()); 
	if (PBDir.exists())
	{
		if (SMessageBox::question(this, tr("New PhotoBook Wizard"), QString(tr("The PhotoBook with name %1 already exists in the collection."
				  "  Do you want to overwrite it ?")).arg(field("photobookname").toString())) == QMessageBox::Yes)
		{
			//Deletes the existing album:
			if (!STUtils::rmDir(PBDir))
			{
				SMessageBox::critical(this, tr("New PhotoBook Wizard"), QString(tr("The directory: %1 could not be deleted.")).arg(PBDir.absolutePath()));
			}
			else 
			{
				STRecentFiles::removeFile(PBDir.absolutePath());
				Res = true; 
			}
		}
	}
	else 
		Res = true; 
	
	return Res; 
}
		

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
		WebView->setVisible(!NoInfoFrame->isVisible());
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


ChooseTemplatePage::ChooseTemplatePage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Template selection</h1>"));
	setSubTitle(tr("The <em>Photo Book</em> template defines the photobook features like size, number of pages, layouts, etc... </br> Use the following list to choose the template that you want for your <em>Photo Book</em>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	QHBoxLayout* TopLayout = new QHBoxLayout;
	MLayout->addLayout(TopLayout);

	QToolBar* TBType = new QToolBar(this);
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
	TopLayout->addWidget(CBSize);

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

	QVBoxLayout* RightLayout = new QVBoxLayout;
	BottomLayout->addLayout(RightLayout);
	RightLayout->addWidget(new QLabel(tr("Information:"), this));

	WebView = new QWebView(this);
	RightLayout->addWidget(WebView);
	//WebView->load(QUrl("http://sites.google.com/site/orometemplates/km_magazinea4"));
	connect(WebView, SIGNAL(loadStarted()), this, SLOT(slotWebLoadStarted()));
	connect(WebView, SIGNAL(loadFinished(bool)), this, SLOT(slotWebLoadFinished(bool)));
	WebView->setMinimumWidth(460);
	WebView->setMinimumHeight(500);

	NoInfoFrame = new QFrame(this);
	NoInfoFrame->setMinimumHeight(WebView->minimumHeight());
	NoInfoFrame->setMinimumWidth(WebView->minimumWidth());
	RightLayout->addWidget(NoInfoFrame);

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
	if (TypeActions->checkedAction())
		Res = static_cast<STPhotoLayout::EnLayoutType>(TypeActions->checkedAction()->data().toInt());
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
	//TODO: Set the current locale.
	return true; 
}

bool ChooseTemplatePage::isComplete() const
{
	return Model->rowCount() > 0 && View->currentIndex().isValid();
}

bool ChooseTemplatePage::typeSelected()
{
	return TypeActions->checkedAction() != 0;
}

void ChooseTemplatePage::slotTemplateIndexClicked(const QModelIndex& _Index)
{
	WebView->stop();
	setCurrentState(StateGettingInfo);

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
	QString DebugUrl = InfoUrl.toString();
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
	qDebug(PBDir.absolutePath().toLatin1()); 

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

ChooseCreationModePage::ChooseCreationModePage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Creation mode selection</h1>"));
	setSubTitle(tr("<p>How do you want to create your <em>Photo Book</em>?</p> You have 2 options:"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	QFont RBFont = font(); 
	RBFont.setBold(true); 
	RBFont.setPointSize(12); 
	RBAutomaticFill = new QRadioButton(tr("Automatic picture fill"), this); 
	RBAutomaticFill->setFont(RBFont); 
	RBAutomaticFill->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred); 
	MLayout->addWidget(RBAutomaticFill); 
	RBAutomaticFill->setChecked(true); 			
	MLayout->addWidget(new QLabel(tr("<b>Automatically</b> fills your book with selected source media. <br/> <em> Note:Once the book is created you can modify everything you want.</em>"), this)); 

	QRadioButton* RBManualFill = new QRadioButton(tr("Manual picture fill"), this); 
	RBManualFill->setFont(RBFont); 
	MLayout->addWidget(RBManualFill); 
	MLayout->addWidget(new QLabel(tr("Lets the picture filling up to you."), this)); 

}


int ChooseCreationModePage::nextId() const
{
	if (RBAutomaticFill->isChecked())
		return STAlbumWizard::Page_BuildOptions;
	else 
		return STAlbumWizard::Page_End;
}


//_____________________________________________________________________________
//
// class BuildOptionsPage
//_____________________________________________________________________________

BuildOptionsPage::BuildOptionsPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Build options</h1>"));
	setSubTitle(tr("<p>Please, configure your <em>Photo Book</em>?</p> automatic options:"));
	QVBoxLayout* MLayout = new QVBoxLayout(this);

}

int BuildOptionsPage::nextId() const
{
	return STAlbumWizard::Page_SelectDiskFolder;

}


void BuildOptionsPage::setTemplateType(STPhotoLayout::EnLayoutType _Type)
{

}


//_____________________________________________________________________________
//
// class SelectDiskFolderPage
//_____________________________________________________________________________


SelectDiskFolderPage::SelectDiskFolderPage(QWidget* _Parent) : QWizardPage(_Parent)
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
	MiddleLayout->addWidget(ImageBoxListView );

	ImageBoxListView->model()->setReportDropsMd5(true);
	connect(ImageBoxListView->model(), SIGNAL(imageDropped(QString,QString)), ImagesListView->model(), SLOT(slotCheckImage(QString, QString)));
	connect(ImageBoxListView->model(), SIGNAL(imageRemoved(QString,QString)), ImagesListView->model(), SLOT(slotUncheckImage(QString,QString)));
	connect(ImageBoxListView->model(), SIGNAL(rowsInserted(const QModelIndex&, int , int )), this, SIGNAL(completeChanged()));
	connect(ImageBoxListView->model(), SIGNAL(rowsRemoved(const QModelIndex&, int , int )), this, SIGNAL(completeChanged()));
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
	setPage(Page_PhotoBookName, new PhotoBookNamePage(this)); 
	setPage(Page_ChooseTemplateMode, new ChooseTemplateModePage(this)); 
	CCustomSizesPage = new CustomSizesPage(this);
	setPage(Page_CustomSizes, CCustomSizesPage); 
	CTemplatePage = new ChooseTemplatePage(this);
	setPage(Page_ChooseTemplate, CTemplatePage);
	setPage(Page_CooseCreationMode, new ChooseCreationModePage(this));
	SDFolderPage = new SelectDiskFolderPage(this);
	setPage(Page_SelectDiskFolder, SDFolderPage);
	setPage(Page_End, new AlbumWizardEndPage(this)); 
	
	setStartId(Page_PhotoBookName);
	
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif

	setOption(HaveHelpButton, true);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/albumwizard.png").scaled(64,64));

	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));

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
	return hasVisitedPage(Page_SelectDiskFolder);
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
	return Res; 
}

STDom::DDocModel* STAlbumWizard::selectedImages() const
{
	return SDFolderPage->selectedImages();
}


//_____________________________________________________________________________
//
// class STAlbumMiniWizard
//_____________________________________________________________________________

STAlbumMiniWizard::STAlbumMiniWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags)
{
	setPage(Page_PhotoBookName, new PhotoBookNamePage(this)); 
	setPage(Page_End, new AlbumWizardEndPage(this)); 
	
	setStartId(Page_PhotoBookName);
	
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/albumwizard.png"));
	setWindowTitle(QObject::tr("PhotoBook Wizard"));
}


int STAlbumMiniWizard::nextId() const
{
	int Res = QWizard::nextId();
	if (Res == STAlbumWizard::Page_ChooseTemplateMode)
		Res = Page_End;
	return Res; 
}

