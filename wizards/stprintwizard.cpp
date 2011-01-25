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
#include "stprintwizard.h"
#include <QLayout> 
#include <QLabel> 
#include <QLineEdit> 
#include <QToolButton> 
#include <QVariant> 
#include <QPrintDialog> 
#include <QToolBar>
#include <QListView>
#include <QApplication> 
#include <QAction> 
#include <QRadioButton> 
#include <QPainter> 

#include "imageTools.h"
#include "sprocessstatuswidget.h"
#include "stncopiesproxymodel.h"
#include "selectionInterface.h"
#include "siconfactory.h"
#include "stimage.h"
#include "stthumbnailview.h" 
#include "plusminusitemdelegate.h" 

#ifdef USE_PRINTKEEPER
	#include "printkeeper.h"
#endif

//_____________________________________________________________________________
//
// class STPWWelcomePage
//_____________________________________________________________________________

STPWWelcomePage::STPWWelcomePage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Welcome to Starblitz Print wizard</h1>"));
	setPixmap(QWizard::WatermarkPixmap, QPixmap(":/st/wizards/watermark.png"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	QLabel* DescLabel = new QLabel(tr("This wizard will walk you throw the process of printing your photos.\n Click the 'Next' button to begin."));
	DescLabel->setWordWrap(true); 
	MLayout->addWidget(DescLabel); 
}

int STPWWelcomePage::nextId() const
{
	return STPrintWizard::Page_SelectPrinter;
}


//_____________________________________________________________________________
//
// class SelectPrinterPage
//_____________________________________________________________________________

void SelectPrinterPage::updatePrinterInfo(const QPrinter& _Printer)
{
	PNameInfo->setText(_Printer.printerName()); 
	QRect SizeRect = _Printer.pageRect(); 
	PSizeInfo->setText(QString(tr("width: <b>%1</b> mm, height:<b>%2</b> mm")).arg(pixelsTomm(SizeRect.width(), _Printer.resolution())).arg(pixelsTomm(SizeRect.height(), _Printer.resolution()))); 
	if (_Printer.orientation() == Qt::Horizontal)
		POrientatioInfo->setText(tr("Horizontal"));
	else 
		POrientatioInfo->setText(tr("Vertical"));	
}

SelectPrinterPage::SelectPrinterPage(QPrinter* _Printer, QWidget* _Parent) : QWizardPage(_Parent), Printer(_Printer), ImagesToPrint(0)
{
	setTitle(tr("<h1>Printer Settings</h1>"));
	setSubTitle(tr("Click the button to select printer and printer settings. At the bottom of this page you can read the information for the printer to use to print your photos. Once this are correct you can press the 'Next' button."));
	QGridLayout* MLayout = new QGridLayout(this); 
	QToolButton* TBGetPrinter = new QToolButton(this); 
	connect(TBGetPrinter, SIGNAL(clicked( bool )), this, SLOT(getPrinterSettings())); 
	TBGetPrinter->setIconSize(QSize(64, 64));
	TBGetPrinter->setIcon(QIcon(":/st/wizards/printer.png")); 
	TBGetPrinter->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	TBGetPrinter->setText(tr("Click to select printer")); 
	MLayout->addWidget(TBGetPrinter, 0, 0);
	
	MLayout->addWidget(new QLabel(tr("Print images to the following printer:"), this), MLayout->rowCount(), 0, 1, 2); 
	MLayout->addWidget(new QLabel(tr("<b>Printer Name:</b>"), this), MLayout->rowCount(), 0);
	PNameInfo = new QLabel(this); 
	MLayout->addWidget(PNameInfo, MLayout->rowCount() - 1, 1); 

	MLayout->addWidget(new QLabel(tr("<b>Paper Size:</b>"), this), MLayout->rowCount(), 0);
	PSizeInfo = new QLabel(this); 
	MLayout->addWidget(PSizeInfo, MLayout->rowCount() - 1, 1); 

	MLayout->addWidget(new QLabel(tr("<b>Orientation:</b>"), this), MLayout->rowCount(), 0);
	POrientatioInfo = new QLabel(this); 
	MLayout->addWidget(POrientatioInfo, MLayout->rowCount() - 1, 1); 
}

int SelectPrinterPage::nextId() const
{
	return STPrintWizard::Page_ChooseImages;
}

void SelectPrinterPage::initializePage()
{
	updatePrinterInfo(*Printer); 
}

void SelectPrinterPage::getPrinterSettings()
{
	QPrintDialog Dialog(Printer, this); 
	Dialog.exec(); 
	updatePrinterInfo(*Printer); 
}

//_____________________________________________________________________________
//
// class ChooseImagesPage
//_____________________________________________________________________________

ChooseImagesPage::ChooseImagesPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Image prints selection</h1>"));
	setSubTitle(tr("Click 'Add' button to add copies of all checked images. You can also select individual number of copies clicking '+' and '-' buttons that appears when your mouse is over the image. Click on each image to check and uncheck it. Only checked images will be printed. By default all the images are checked. When you have finished, please click 'Next' button."));

	QLayout* MLayout = new QVBoxLayout(this); 

	StatusWidg = new SProcessStatusWidget(this); 
	StatusWidg->setVisible(false);
	MLayout->addWidget(StatusWidg);

	LView = new STThumbnailView(this);
	PlusMinusItemDelegate* Delegate = new PlusMinusItemDelegate(LView);
	LView->setItemDelegate(Delegate); 
	LView->setViewMode(QListView::IconMode); 
	LView->setFlow(QListView::LeftToRight); 
	LView->setWrapping(false); 
	connect(LView, SIGNAL(clicked( const QModelIndex& )), this, SLOT(toggleChecked(const QModelIndex& )));
	MLayout->addWidget(LView); 

	QToolBar* MToolBar = new QToolBar(this); 
	MToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	MLayout->addWidget(MToolBar); 

	ImageModel = new STDom::DDocModel(this);
	ImageModel->setNoImagePixmap(QPixmap(":/st/wizards/nopreview.png")); 
	MProxyModel = new STNCopiesProxyModel(this, QImage(":/st/wizards/checked.png"));	
	connect(MProxyModel, SIGNAL(checksChanged()), this, SIGNAL(completeChanged()));
	MProxyModel->setSourceModel(ImageModel); 
	MProxyModel->setThumbnailSize(QSize(150, 100));
	LView->setModel(MProxyModel); 

	connect(Delegate, SIGNAL(plusButtonClicked(const QModelIndex&)), MProxyModel, SLOT(incNCopies(const QModelIndex&))); 
	connect(Delegate, SIGNAL(minusButtonClicked(const QModelIndex&)), MProxyModel, SLOT(decNCopies(const QModelIndex&))); 


	QAction* AddCopiesAct = new QAction(QIcon(":/st/edit_add.png"), tr("Add copies"), this);
	AddCopiesAct->setToolTip(tr("Add 1 more copy to all checked images."));
	connect(AddCopiesAct, SIGNAL(triggered()), MProxyModel, SLOT(incCheckedCopies()));	
	MToolBar->addAction(AddCopiesAct); 

	QAction* CheckAllAct = new QAction(tr("Check All"), this);
	connect(CheckAllAct, SIGNAL(triggered()), MProxyModel, SLOT(checkAll())); 
	MToolBar->addAction(CheckAllAct); 

	QAction* UnCheckAllAct = new QAction(tr("UnCheck All"), this);
	connect(UnCheckAllAct, SIGNAL(triggered()), MProxyModel, SLOT(clearChecked())); 
	MToolBar->addAction(UnCheckAllAct); 

	QAction* InvertCheckAct = new QAction(QIcon(":/st/wizards/invert.png"), tr("Invert checks"), this);
	connect(InvertCheckAct, SIGNAL(triggered()), MProxyModel, SLOT(invertChecked())); 
	MToolBar->addAction(InvertCheckAct); 

	QAction* CheckAct = new QAction(QIcon(":/st/wizards/checked.png"), tr("Check/Uncheck"), this);
	CheckAct->setShortcut(tr("Alt+C")); 
	CheckAct->setToolTip(tr("Toggles exclusion to import the current picture."));
	connect(CheckAct, SIGNAL(triggered()), this, SLOT(toggleCheckedCurrent()));	
	MToolBar->addAction(CheckAct); 
	
}

int ChooseImagesPage::nextId() const
{
	return STPrintWizard::Page_ChooseCropMode;
}

void ChooseImagesPage::initializePage()
{
	//Set images to a model.
	ImageModel->setDocs(Images);
	MProxyModel->checkAll(); 
}

bool ChooseImagesPage::isComplete() const
{
	return MProxyModel->numCheckedImages() > 0; 
}

/*!
	O(n)
*/
ChooseImagesPage::TImagePrintList ChooseImagesPage::imagesToPrint()
{	
	TImagePrintList Res; 
	for (int Vfor=0; Vfor < ImageModel->rowCount(); Vfor++)
	{	
		QModelIndex CIndex = MProxyModel->index(Vfor, 0); 
		if (MProxyModel->isChecked(CIndex))
		{
			STDom::DDoc* Doc = ImageModel->doc(CIndex);
			TImagePrint IPrint; 
			IPrint.FInfo = Doc->fileInfo();
			IPrint.NCopies = MProxyModel->nCopies(CIndex); 
			Res.push_back(IPrint);
		}
	}
	return Res; 
}

QFileInfoList ChooseImagesPage::imagesToPrintFileInfoList(const ChooseImagesPage::TImagePrintList& _PrnList)
{
	QFileInfoList Res; 
	ChooseImagesPage::TImagePrintList::const_iterator it; 
	for (it = _PrnList.begin(); it != _PrnList.end(); ++it)
	{
		Res.push_back(it->FInfo); 
	}
	return Res; 
}



void ChooseImagesPage::toggleChecked(const QModelIndex& _Index)
{
	MProxyModel->setChecked(_Index, !MProxyModel->isChecked(_Index)); 
}

void ChooseImagesPage::toggleCheckedCurrent()
{
	toggleChecked(LView->currentIndex()); 
}

//_____________________________________________________________________________
//
// class ChooseCropMode
//_____________________________________________________________________________


ChooseCropModePage::ChooseCropModePage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Image adjustment to paper size</h1>"));
	setSubTitle(tr("Please, select the desired policy to fit your images to paper size and press 'Next' button."));

	MLayout = new QGridLayout(this); 

	QLabel* CutPixLabel = new QLabel(this); 
	CutPixLabel->setPixmap(QPixmap(":/st/wizards/propcut.png")); 
	CutPixLabel->setAlignment(Qt::AlignCenter); 
	CutPixLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred); 
	MLayout->addWidget(CutPixLabel, 0, 0);

	QRadioButton* RBCut = new QRadioButton(tr("Crop images that don't fit to media"), this);
	MLayout->addWidget(RBCut, 0, 1);	


	QLabel* SideNoCutPixLabel = new QLabel(this); 
	SideNoCutPixLabel->setPixmap(QPixmap(":/st/wizards/sidewhitemargin.png")); 
	SideNoCutPixLabel->setAlignment(Qt::AlignCenter); 
	SideNoCutPixLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred); 
	MLayout->addWidget(SideNoCutPixLabel, 1, 0);
	RBSideNoCut = new QRadioButton(tr("Leave space to spare as white margins and image aligned to one side"), this);
	MLayout->addWidget(RBSideNoCut, 1, 1);

	QLabel* NoCutPixLabel = new QLabel(this); 
	NoCutPixLabel->setPixmap(QPixmap(":/st/wizards/whitemargin.png")); 
	NoCutPixLabel->setAlignment(Qt::AlignCenter); 
	NoCutPixLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred); 
	MLayout->addWidget(NoCutPixLabel, 2, 0);
	RBNoCut = new QRadioButton(tr("Leave space to spare as white margins and image centered"), this);
	MLayout->addWidget(RBNoCut, 2, 1);

	RBNoCut->setChecked(true); 
}

int ChooseCropModePage::nextId() const
{
	int Res; 

	if (RBNoCut->isChecked() || RBSideNoCut->isChecked())
		Res = STPrintWizard::Page_Printing;
	else 
		Res = STPrintWizard::Page_SelectCrops;

	return Res; 
}

ChooseCropModePage::EnCropMode ChooseCropModePage::cropMode() const
{
	EnCropMode Res; 
	if (RBNoCut->isChecked())
		Res = CropModelNoCutCentered;
	else if (RBSideNoCut->isChecked())
		Res = CropModeNoCut;
	else 
		Res = CropModeCut;
	return Res; 
}


//_____________________________________________________________________________
//
// class SelectCropsPage
//_____________________________________________________________________________

void SelectCropsPage::updateActionStatus()
{
	PreviousAct->setEnabled(CurrentIndex > 0); 
	NextAct->setEnabled(CurrentIndex < ImagesToCrop.size() -1);
}

void SelectCropsPage::updateInfo()
{
	LabInfo->setText(QString(tr("Photo %1 of %2")).arg(CurrentIndex + 1).arg(ImagesToCrop.size())); 
}

void SelectCropsPage::updateSelection()
{
	MSelIface->selectMaxByAspectRatio(PageRect.size());
}

void SelectCropsPage::setCurrentIndex(int _Index)
{
	if (_Index >= 0 && _Index < ImagesToCrop.size() && _Index < PageRectList.size())
	{
		PageRect = PageRectList[qMin(_Index, PageRectList.size() -1)];
		QImage CPhoto(ImagesToCrop[_Index].absoluteFilePath());
		if ((CPhoto.width() > CPhoto.height() && PageRect.width() < PageRect.height()) ||
			(CPhoto.width() < CPhoto.height() && PageRect.width() > PageRect.height()))
		{
			QSize PRSize = PageRect.size();
			PRSize.transpose(); 
			PageRect.setSize(PRSize); 
		}
		storeCurrentCrop();
		CurrentIndex = _Index; 
		MSelIface->setPhoto(CPhoto); 
		updateSelection();
		updateActionStatus(); 
		updateInfo(); 
		if (_Index == (ImagesToCrop.size() -1))
		{
			IsComplete = true; 
			emit completeChanged(); 
		}
	}
}

void SelectCropsPage::storeCurrentCrop()
{
	if (CurrentIndex >=0 && CurrentIndex < CropRects.size())
		CropRects[CurrentIndex] = MSelIface->getSelectionRect();
}

void SelectCropsPage::initialize(const QFileInfoList& _ImagesToCrop)
{
	IsComplete = false; 

	ImagesToCrop = _ImagesToCrop; 
	//Initialize rects, Null rects means that the default crop will be used.
	CropRects.clear(); 
	for (int Vfor=0; Vfor < ImagesToCrop.size(); Vfor++)
	{
		CropRects.push_back(QRect()); 
	}
	first(); 
}


SelectCropsPage::SelectCropsPage(QWidget* _Parent) : QWizardPage(_Parent), CurrentIndex(0), IsComplete(false)
{
	setTitle(tr("<h1>Crop photos</h1>"));
	setSubTitle(tr("Move the box bellow to tell to this wizard how to crop the photos to fit inside the photo sizes tou have given. You can crop each image differently, or just click 'Next' to use the default center cropping for each photo."));

	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	MLayout->addWidget(new QLabel(tr("Click and drag the mouse"), this)); 	
	MSelIface = new SelectionInterface(this); 
	MSelIface->setMinimumHeight(300);
	MSelIface->setDrawSelectionAllowed(false);
	MLayout->addWidget(MSelIface);

	QToolBar* MToolBar = new QToolBar(this); 
	MToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	MLayout->addWidget(MToolBar); 

	QAction* RotateAct = new QAction(QIcon(":/st/wizards/rotateright.png"), tr("Rotate"), this);
	connect(RotateAct, SIGNAL(triggered()), this, SLOT(rotate()));
	MToolBar->addAction(RotateAct); 

	MToolBar->addSeparator(); 

	PreviousAct = new QAction(stIcon(SIconFactory::Rew), tr("Previous Photo"), this);
	connect(PreviousAct, SIGNAL(triggered()), this, SLOT(previous()));
	MToolBar->addAction(PreviousAct); 

	NextAct = new QAction(stIcon(SIconFactory::Fwd), tr("Next Photo"), this);
	connect(NextAct, SIGNAL(triggered()), this, SLOT(next()));
	MToolBar->addAction(NextAct); 

	MToolBar->addSeparator(); 
	LabInfo = new QLabel(this); 
	MToolBar->addWidget(LabInfo);
}

int SelectCropsPage::nextId() const
{
	return STPrintWizard::Page_Printing;
}


void SelectCropsPage::initialize(const QFileInfoList& _ImagesToCrop, const TCropRectList& _PageRectList)
{
	PageRectList = _PageRectList; 	
	initialize(_ImagesToCrop); 
}


void SelectCropsPage::initialize(const QFileInfoList& _ImagesToCrop, const QRect& _PageRect)
{
	PageRectList.clear(); 
	for (int Vfor=0; Vfor < _ImagesToCrop.size(); Vfor++)
	{
		PageRectList.push_back(_PageRect); 
	}
	initialize(_ImagesToCrop); 
}


bool SelectCropsPage::validatePage()
{
	storeCurrentCrop();
}

bool SelectCropsPage::isComplete() const
{
	return IsComplete; 
}

void SelectCropsPage::first()
{
	setCurrentIndex(0); 
}

void SelectCropsPage::next()
{
	setCurrentIndex(CurrentIndex + 1); 
}

void SelectCropsPage::previous()
{
	setCurrentIndex(CurrentIndex - 1); 
}

void SelectCropsPage::rotate()
{
	int WAnt = PageRect.width();
	PageRect.setWidth(PageRect.height()); 
	PageRect.setHeight(WAnt); 
	updateSelection(); 
}

//_____________________________________________________________________________
//
// class PrintingPage
//_____________________________________________________________________________

PrintingPage::PrintingPage(ChooseImagesPage* _CIPage, QPrinter* _Printer, ChooseCropModePage* _ChooseCropPage, SelectCropsPage* _CropsPage, QWidget* _Parent)
	: QWizardPage(_Parent), CIPage(_CIPage), Printer(_Printer), ChooseCropPage(_ChooseCropPage), CropsPage(_CropsPage)
{
	setTitle(tr("<h1>Printing photos</h1>"));
	setSubTitle(tr("Press the 'Finish' button to print your photos."));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	StatusWidg = new SProcessStatusWidget(this); 
	MLayout->addWidget(StatusWidg); 	
}


void PrintingPage::initializePage()
{
	StatusWidg->setVisible(false);
}

bool PrintingPage::validatePage()
{
	ChooseImagesPage::TImagePrintList ImagesToPrint = CIPage->imagesToPrint(); 
	SelectCropsPage::TCropRectList CropRectList; 
	//QSize ResSize = Printer->pageRect().size(); 
	//qDebug("ResSize: %d, %d", ResSize.width(), ResSize.height()); 
	bool CropImages = ChooseCropPage->cropMode() == ChooseCropModePage::CropModeCut; 
	if (CropImages)
	{
		CropRectList = CropsPage->cropRects(); 
	}
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	StatusWidg->showProgressBar(tr("Printing..."), ImagesToPrint.size());
	StatusWidg->setVisible(true);
	StatusWidg->incrementProgress(); 
	QApplication::processEvents();
	ChooseImagesPage::TImagePrintList::iterator it; 
	int Cnt = 0;

	bool PrintAccess = true;
#ifdef USE_PRINTKEEPER
	PrintAccess = PrintKeeper::printAcces(Printer->printerName());
#endif
	if (PrintAccess)
	{
		QPainter Painter(Printer);
		it = ImagesToPrint.begin();
		while(it != ImagesToPrint.end())
		{
			STImage CImage;
			if (CImage.load(it->FInfo.absoluteFilePath()))
			{
				if (CropImages)
					CImage = CImage.copy(CropRectList[Cnt]);//Do no scale images at the moment.

				//CImage.print(Printer, ChooseCropPage->cropMode() == ChooseCropModePage::CropModelNoCutCentered);
				Printer->setNumCopies(it->NCopies);
				CImage.print(Painter, ChooseCropPage->cropMode() == ChooseCropModePage::CropModelNoCutCentered);
				++it;
				if (it != ImagesToPrint.end())
					Printer->newPage();
				StatusWidg->incrementProgress();
				QApplication::processEvents();
			}
			else
				++it;
			//TODO: Error Stack !
			Cnt++;
		}
	}
#ifdef USE_PRINTKEEPER
	else
		PrintKeeper::accesDeniedWarning(Printer->printerName());
#endif

	QApplication::restoreOverrideCursor();
	return true;
}



//_____________________________________________________________________________
//
// class STPrintWizard
//_____________________________________________________________________________

STPrintWizard::STPrintWizard(QWidget* parent, Qt::WindowFlags flags): QWizard(parent, flags), Printer(QPrinter::HighResolution)
{
	Printer.setFullPage(true); 
	setPage(Page_Welcome, new STPWWelcomePage(this));
	setStartId(Page_Welcome);
	SelPrinterPage = new SelectPrinterPage(&Printer, this);
	setPage(Page_SelectPrinter, SelPrinterPage);
	ChooseImgPage = new ChooseImagesPage(this);
	setPage(Page_ChooseImages, ChooseImgPage);
	ChooseCropModePage* ChCropModePage = new ChooseCropModePage(this);
	setPage(Page_ChooseCropMode, ChCropModePage);
	SelCropPage = new SelectCropsPage(this);
	setPage(Page_SelectCrops, SelCropPage);
	setPage(Page_Printing, new PrintingPage(ChooseImgPage, &Printer, ChCropModePage, SelCropPage, this));
	
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif

	setOption(HaveHelpButton, false);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/printwizard.png"));
	setButtonText(QWizard::FinishButton, tr("Print")); 

	connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
	connect(this, SIGNAL(currentIdChanged( int )), this, SLOT(slotCurrentIdChanged(int)));

	setWindowTitle(tr("Print Wizard"));
}

void STPrintWizard::setImages(const QFileInfoList& _Images)
{
	ChooseImgPage->setImages(_Images); 
	SelPrinterPage->setNumImagesToPrint(_Images.size()); 
}

void STPrintWizard::slotCurrentIdChanged(int _Id)
{
	if (_Id == Page_SelectCrops)
	{
		//QPainter Painter(&Printer);
		SelCropPage->initialize(ChooseImagesPage::imagesToPrintFileInfoList(ChooseImgPage->imagesToPrint()), Printer.paperRect()); //Painter.viewport()); 
	}
}
