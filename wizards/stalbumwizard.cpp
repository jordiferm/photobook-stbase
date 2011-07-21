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
#include "designinfomodel.h"
#include "templateinfomodel.h"
#include "document.h"

//Image Selection
#include "imageslistview.h"
#include "imageboxlistview.h"
#include "doccheckedproxymodel.h"
#include "qxtlabel.h"

#include "templateinfomodel.h"
#include "metainfo.h"
#include "fileselectlineedit.h"
#include "collectioninfo.h"
#include "smessagebox.h"
#include "stutils.h"
#include "disksourceselectionwidget.h" 
#include "strecentfiles.h"
#include "sterror.h"

//_____________________________________________________________________________
//
// class ChooseTemplatePage
//_____________________________________________________________________________

void ChooseTemplatePage::setCurrentState(ChooseTemplatePage::EnState _State)
{
	BottomFrame->setVisible(_State != StateTemplatesEmpty);
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
	Model = new SPhotoBook::TemplateInfoModel(this);
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
	return STAlbumWizard::Page_ChooseCreationMode;
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
}

SPhotoBook::TemplateInfo ChooseTemplatePage::selectedTemplateInfo() const
{
	SPhotoBook::TemplateInfo Res;

	int ItemRow = CBSize->itemData(CBSize->currentIndex()).toInt();
	QModelIndex ItemIndex = Model->index(ItemRow, 0);
	if (ItemIndex.isValid())
		Res = Model->templateInfo(ItemIndex);
	return Res;
}

bool ChooseTemplatePage::validatePage()
{
	return true; 
}

bool ChooseTemplatePage::isComplete() const
{
	return Model->rowCount() > 0 && View->currentIndex().isValid();
}

void ChooseTemplatePage::setTemplateList(const SPhotoBook::TemplateInfoList& _TemplateList )
{
	Model->setTemplateList(_TemplateList);
	selectFirstIndex();
}

void ChooseTemplatePage::slotTemplateIndexClicked(const QModelIndex& _Index)
{
	WebView->stop();

	//Load available sizes.
	CBSize->clear();
	QModelIndexList SizesList = Model->sizes(_Index);
	QModelIndexList::iterator it;
	for (it = SizesList.begin(); it != SizesList.end(); ++it)
	{
		SPhotoBook::TemplateInfo CTemplateInfo = Model->templateInfo(*it);
		QSizeF CSize = CTemplateInfo.size();
		CBSize->addItem(QString("%1x%2 mm").arg(CSize.width(), CSize.height()), it->row());
	}

	//Get info url from model and display it.
	QUrl InfoUrl = Model->templateInfo(_Index).infoUrl();
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



	QGroupBox* GBUsePredesign = new QGroupBox(tr("Use a predesign"), this);
	MLayout->addWidget(GBUsePredesign);
	GBUsePredesign->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

	QHBoxLayout* GBLayout = new QHBoxLayout(GBUsePredesign);

	LVDesigns = new QListView(GBUsePredesign);
	LVDesigns->setIconSize(QSize(32, 32));
	LVDesigns->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

	GBLayout->addWidget(LVDesigns);

	DesignModel = new SPhotoBook::DesignInfoModel(this);
	LVDesigns->setModel(DesignModel);
	connect(LVDesigns->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotPredesignChanged(QModelIndex)));

	TBDescription = new QTextBrowser(this);
	GBLayout->addWidget(TBDescription);
	TBDescription->setHtml("<h1>This is a sample description</h1><p>Here comes the description for each selected item. It depends upon a description that user enters while predesign creation.</p><img src=./automatic.png/>");

	PBAutomatic->setChecked(true);
}


int ChooseCreationModePage::nextId() const
{
	return STAlbumWizard::Page_BuildOptions;
}

void ChooseCreationModePage::setTemplateInfo(const SPhotoBook::TemplateInfo& _TemplateInfo)
{
	//Fill predesigns of template.
	DesignModel->setDesignInfoList(_TemplateInfo.designs());
	if (DesignModel->rowCount() > 0)
	{
		LVDesigns->setCurrentIndex(DesignModel->index(0, 0));
	}

}

bool ChooseCreationModePage::validatePage()
{
	return true;
}

bool ChooseCreationModePage::autoBuildModeSelected() const
{
	return PBAutomatic->isChecked();
}

SPhotoBook::DesignInfo ChooseCreationModePage::designInfo() const
{
	SPhotoBook::DesignInfo Res;
	if (DesignModel->rowCount() > 0)
	{
		QModelIndex CIndex = LVDesigns->currentIndex();
		if (CIndex.isValid())
			Res = DesignModel->designInfo(CIndex);
	}
	return Res;
}



void ChooseCreationModePage::slotPredesignChanged(const QModelIndex& _Index)
{
	if (_Index.isValid()) //Defensive
	{
		//Lets load the photobook
		SPhotoBook::DesignInfo DInfo = designInfo();
		TBDescription->setHtml(QString("<h1>%1</h1><p>%2</p><center><img src=""%3""/></center>").arg(DInfo.name()).arg(DInfo.description()).arg(
				QString(QUrl::fromLocalFile(DInfo.imageFile()).toEncoded())));
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
	SPhotoBook::BuildOptions BOptions = getBuildOptions();
	bool Res = true;
	Res = Res && BOptions.fromDate() < BOptions.toDate();
	return Res;
}


void BuildOptionsPage::setBuildOptions(const SPhotoBook::BuildOptions& _Options)
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


SPhotoBook::BuildOptions BuildOptionsPage::getBuildOptions() const
{
	SPhotoBook::BuildOptions Res;
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

void BuildOptionsPage::setTemplateMetaInfo(const SPhotoBook::MetaInfo& _MInfo)
{
	SBNumPages->setRange(_MInfo.minPages(), _MInfo.maxPages());

	LayoutType = _MInfo.templateType();
	SPhotoBook::BuildOptions InitOpts;
	InitOpts.setDefaults(LayoutType, _MInfo.minPages());
	setBuildOptions(InitOpts);
	GBPhotoBook->setVisible(LayoutType ==  SPhotoBook::MetaInfo::TypePhotoBook);
	GBCalendar->setVisible(LayoutType == SPhotoBook::MetaInfo::TypeCalendar);

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
	int SelectedImages = IBListView->model()->rowCount();
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
	SPhotoBook::ImagesListView* IListView = new SPhotoBook::ImagesListView(this);
	IListView->model()->setNumMatchLimit(1);
	TopLayout->addWidget(IListView);
	IListView->showSelectAllAction(true);

	QHBoxLayout* MiddleLayout = new QHBoxLayout;
	MiddleLayout->setMargin(0);
	MiddleLayout->setSpacing(0);
	MLayout->addLayout(MiddleLayout);

	QxtLabel* DestinationLabel = new QxtLabel(tr("PhotoBook Images"), this);
	DestinationLabel->setMinimumWidth(30);
	DestinationLabel->setRotation(Qxt::CounterClockwise);
	MiddleLayout->addWidget(DestinationLabel);

	IBListView = new SPhotoBook::ImageBoxListView(this);
	InfoLabel = new QLabel(this);
	IBListView->toolBar()->addSeparator();
	IBListView->toolBar()->addWidget(InfoLabel);
	MiddleLayout->addWidget(IBListView );

	IBListView->model()->setReportDropsMd5(true);
	connect(IBListView->model(), SIGNAL(imageDropped(QString,QString)), IListView->model(), SLOT(slotCheckImage(QString, QString)));
	connect(IBListView->model(), SIGNAL(imageRemoved(QString,QString)), IListView->model(), SLOT(slotUncheckImage(QString,QString)));
	connect(IBListView->model(), SIGNAL(rowsInserted(const QModelIndex&, int , int )), this, SIGNAL(completeChanged()));
	connect(IBListView->model(), SIGNAL(rowsRemoved(const QModelIndex&, int , int )), this, SIGNAL(completeChanged()));
	connect(this, SIGNAL(completeChanged()), this, SLOT(slotUpdateInfo()));

	updateInfo();
}

//void SelectDiskFolderPage::setTemplate(const STPhotoBookTemplate& _Template, const STPhotoBookBuildOptions& _Options)
void SelectDiskFolderPage::setMetaInfo(const SPhotoBook::MetaInfo& _Info , const SPhotoBook::BuildOptions& _Options)
{
	PagesToFill = _Options.pagesToFill();
	OptimalImagesPerPage = _Info.numOptimalImagesPerPage();
	AbsoluteImageCount = 0;
	updateInfo();
}

void SelectDiskFolderPage::clearSelection()
{
	IBListView->model()->sourceModel()->clear();
}

int SelectDiskFolderPage::nextId() const
{
	return STAlbumWizard::Page_End; 
}

bool SelectDiskFolderPage::isComplete() const
{
	return IBListView->model()->rowCount() > 0;
}

STDom::DDocModel* SelectDiskFolderPage::selectedImages() const
{
	return IBListView->model()->sourceModel();
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

STAlbumWizard::STAlbumWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags)
{
	CTemplatePage = new ChooseTemplatePage(this);
	setPage(Page_ChooseTemplate, CTemplatePage);
	CCreationModePage = new ChooseCreationModePage(this);
	setPage(Page_ChooseCreationMode, CCreationModePage);
	PBuildOptions = new BuildOptionsPage(this);
	setPage(Page_BuildOptions, PBuildOptions);
	SDFolderPage = new SelectDiskFolderPage(this);
	setPage(Page_SelectDiskFolder, SDFolderPage);
	setPage(Page_End, new AlbumWizardEndPage(this)); 
	
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

SPhotoBook::DesignInfo STAlbumWizard::designInfo() const
{
	return CCreationModePage->designInfo();
}


SPhotoBook::TemplateInfo STAlbumWizard::templateInfo() const
{
	return CTemplatePage->selectedTemplateInfo();
}

bool STAlbumWizard::autoFillSelected() const
{
	return CCreationModePage->autoBuildModeSelected();
}
	

int STAlbumWizard::nextId() const
{

	int Res = QWizard::nextId();

	if (Res ==  Page_BuildOptions)
	{
		SPhotoBook::MetaInfo::EnTemplateType TemplType = CTemplatePage->selectedTemplateInfo().type();
		PBuildOptions->setTemplateMetaInfo(CCreationModePage->designInfo().metaInfo());
		PBuildOptions->setAutoBuildMode(CCreationModePage->autoBuildModeSelected());
		if (TemplType != SPhotoBook::MetaInfo::TypeCalendar && TemplType != SPhotoBook::MetaInfo::TypePhotoBook)
			Res = PBuildOptions->nextId();
	}
	if (Res == Page_ChooseCreationMode)
	{
		CCreationModePage->setTemplateInfo(CTemplatePage->selectedTemplateInfo());
		SDFolderPage->clearSelection();
	}
	if (Res == Page_SelectDiskFolder)
	{
		SDFolderPage->setMetaInfo(CCreationModePage->designInfo().metaInfo(), PBuildOptions->getBuildOptions());
	}

	return Res; 
}

STDom::DDocModel* STAlbumWizard::selectedImages() const
{
	return SDFolderPage->selectedImages();
}

SPhotoBook::BuildOptions STAlbumWizard::buildOptions() const
{
	return PBuildOptions->getBuildOptions();
}

void STAlbumWizard::setTemplateType(const SPhotoBook::TemplateInfoList& _TemplateList)
{
	CTemplatePage->setTemplateList(_TemplateList);
}

