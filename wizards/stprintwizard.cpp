/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Aili Image reserves all rights not expressly granted herein.
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
#include <QPrintDialog> 
#include <QApplication> 
#include <QRadioButton> 
#include <QToolButton>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QDebug>

#include "sprocessstatuswidget.h"
#include "tpphotoselwidget.h"

//stdom
#include "printjob.h"
#include "printjobmodel.h"
#include "printjobprinter.h"
#include "smessagebox.h"

//______________________________________________________________________________________________________________________________________________________________-



//_____________________________________________________________________________
//
// class PrintingPage
//_____________________________________________________________________________

STPWPrintingPage::STPWPrintingPage( QWidget* _Parent)
	: QWizardPage(_Parent), PDialog(0), AtomicPrint(false)
{
	setTitle(tr("<h1>Printing photos</h1>"));
	setSubTitle(tr("Press the 'Finish' button to print your photos."));
	QVBoxLayout* MLayout = new QVBoxLayout(this);
	StatusWidg = new SProcessStatusWidget(this);
	MLayout->addWidget(StatusWidg);

	QToolButton* PreviewButton = new QToolButton(this);
	MLayout->addWidget(PreviewButton);
	PreviewButton->setText(tr("Preview"));
	PreviewButton->setIcon(QIcon(":/st/wizards/printpreview.png"));
	PreviewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	PreviewButton->setIconSize(QSize(64, 64));
	connect(PreviewButton, SIGNAL(clicked()), this, SLOT(slotRunPreview()));

}


void STPWPrintingPage::initializePage()
{
	StatusWidg->setVisible(false);
}

bool STPWPrintingPage::print(QPrinter* _Printer)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	StatusWidg->showProgressBar(tr("Printing..."), PrintJob.totalCopies() -1 );
	StatusWidg->setVisible(true);
	QApplication::processEvents();
	STDom::PrintJobPrinter PJPrinter;
	PJPrinter.setFitMode(FitMode);
	PJPrinter.setAtomicPrint(AtomicPrint);
	PJPrinter.clearErrorStack();
	PJPrinter.printToPrinter(*_Printer, PrintJob, "st-print-wizard", StatusWidg->progressBar());
	if (!PJPrinter.errorStack().isEmpty())
		SMessageBox::criticalDetailed(this, tr("Print Wizard"), tr("There were errors while printing"), PJPrinter.errorStack().errorString().join("\n"));

	StatusWidg->setVisible(false);
	QApplication::restoreOverrideCursor();
	return PJPrinter.errorStack().isEmpty();
}

bool STPWPrintingPage::validatePage()
{
	return print(Printer);
}

void STPWPrintingPage::slotRunPreview()
{
	if (PDialog)
		delete PDialog;

	PDialog = new QPrintPreviewDialog(Printer, this);
	connect(PDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(slotPreviewPaintRequested(QPrinter* )));

	PDialog->exec();
}

void STPWPrintingPage::slotPreviewPaintRequested(QPrinter* _Printer)
{
	print(_Printer);
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
	return STPrintJobWizard::Page_Printing;
}

STDom::PrintJobPrinter::EnFitMode ChooseCropModePage::fitMode() const
{
	STDom::PrintJobPrinter::EnFitMode Res;
	if (RBNoCut->isChecked())
		Res = STDom::PrintJobPrinter::FitModeWhiteMarginCenter ;
	else if (RBSideNoCut->isChecked())
		Res = STDom::PrintJobPrinter::FitModeWhiteMarginRight;
	else
		Res = STDom::PrintJobPrinter::FitModeCrop;
	return Res;
}



//_____________________________________________________________________________
//
// class STPWImageSelectionPage
//_____________________________________________________________________________

STPWImageSelectionPage::STPWImageSelectionPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Image selection page</h1>"));
	setSubTitle(tr("Here you can select and edit printed images."));
	QVBoxLayout* MLayout = new QVBoxLayout(this);


	PJModel = new STDom::PrintJobModel(this);
	STDom::DDocModel* DocModel = new STDom::DDocModel(this);
	PJModel->setSourceModel(DocModel);
	PJModel->setThumbnailSize(QSize(128, 128));
	PJModel->setRitchTextDisplay(true);

	PhotoSelW = new TPPhotoSelWidget(this, false);
	PhotoSelW->setModel(PJModel);
	PhotoSelW->setBillEnabled(false);
	PhotoSelW->setFilterEnabled(false);
	MLayout->addWidget(PhotoSelW);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void STPWImageSelectionPage::setPrintJob(const STDom::PrintJob& _PrintJob)
{
	PJModel->setPrintJob(_PrintJob);
}

void STPWImageSelectionPage::setSingleProduct(const STDom::DDocProduct& _Product)
{
	PhotoSelW->setSingleProduct(_Product);
}

void STPWImageSelectionPage::setAtomicPrint(bool _Value)
{
	PhotoSelW->setAtomicPrintSelection(_Value);
}

int STPWImageSelectionPage::nextId() const
{
	return STPrintJobWizard::Page_ChooseCropMode;
}

STDom::PrintJob STPWImageSelectionPage::printJob() const
{
	return PJModel->printJob();
}


//_____________________________________________________________________________
//
// class STPrintJobWizard
//_____________________________________________________________________________

STPrintJobWizard::STPrintJobWizard(QWidget* _Parent) : QWizard(_Parent)
{
	setWindowTitle(tr("Print Wizard"));

	ImageSelectPage = new STPWImageSelectionPage(this);
	setPage(Page_ImageSelection, ImageSelectPage);
	CropModePage = new ChooseCropModePage(this);
	setPage(Page_ChooseCropMode, CropModePage);
	PrnPage = new STPWPrintingPage(this);
	setPage(Page_Printing, PrnPage);

	setStartId(Page_ImageSelection);

#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif

	setOption(HaveHelpButton, false);
	setPixmap(QWizard::LogoPixmap, QPixmap(":/st/wizards/printwizard.png"));
	setButtonText(QWizard::FinishButton, tr("Print"));

	connect(this, SIGNAL(currentIdChanged( int )), this, SLOT(slotCurrentIdChanged(int)));
}

void STPrintJobWizard::setPrintJob(const STDom::PrintJob& _PrintJob)
{
	ImageSelectPage->setPrintJob(_PrintJob); 
}

void STPrintJobWizard::setSingleProduct(const STDom::DDocProduct& _Product)
{
	ImageSelectPage->setSingleProduct(_Product);
}


void STPrintJobWizard::setAtomicPrint(bool _Value)
{
	ImageSelectPage->setAtomicPrint(_Value);
	PrnPage->setAtomicPrint(_Value);
}


int STPrintJobWizard::getPrinter()
{
	QPrintDialog PDialog(&Printer, this);
	PDialog.setOption(QAbstractPrintDialog::PrintToFile,false);
	PDialog.setOption(QAbstractPrintDialog::PrintSelection,false);
	PDialog.setOption(QAbstractPrintDialog::PrintPageRange,false);
	if (PDialog.exec() != QDialog::Accepted)
		return QDialog::Rejected;

#ifdef Q_OS_MAC
	QPageSetupDialog PSDialog(&Printer, this);
	if (PSDialog.exec() != QDialog::Accepted)
		return QDialog::Rejected;
#endif
	return QDialog::Accepted;
}

int STPrintJobWizard::printImages(const QFileInfoList& _Images)
{
	if (getPrinter() == QDialog::Accepted)
	{
		// Create a PrintJob with Images
		QSizeF ProdSize = Printer.paperSize(QPrinter::Millimeter);
		STDom::DDocProduct NewProduct("noref", "Prints", STDom::DDocFormat(ProdSize.width(), ProdSize.height()));
		setSingleProduct(NewProduct);
		STDom::PrintJob PJob;
		QFileInfoList::const_iterator it;
		for (it = _Images.begin(); it != _Images.end(); ++it)
		{
			PJob.addCopies(NewProduct, *it, Printer.numCopies());
		}
		setPrintJob(PJob);
		restart();
		return exec();
	}
	else
		return QDialog::Rejected;
}

int STPrintJobWizard::printPrintJobProduct(const STDom::PrintJob& _SourcePrintJob, const STDom::DDocProduct& _Product)
{
	int Res = QDialog::Rejected;
	if (_SourcePrintJob.prints(_Product).size() > 0)
	{
		if (getPrinter() == QDialog::Accepted)
		{
			// Create a PrintJob with Images
			QSizeF ProdSize = Printer.paperSize(QPrinter::Millimeter);
			STDom::DDocProduct NewProduct("noref", "Prints", STDom::DDocFormat(ProdSize.width(), ProdSize.height()));
			setSingleProduct(NewProduct);
			STDom::PrintJob PJob;
			QFileInfoList Images = _SourcePrintJob.files();
			QFileInfoList::const_iterator it;
			for (it = Images.begin(); it != Images.end(); ++it)
			{
				PJob.setCopies(NewProduct, *it, _SourcePrintJob.copies(_Product, *it));
			}
			setPrintJob(PJob);
			restart();
			Res = exec();
		}
	}
	else
		SMessageBox::warning(this, tr("Print Wizard"), tr("There are no selected copies of '%1' product, please select a different product").arg(
				_Product.description()));
	return Res;
}

void STPrintJobWizard::slotCurrentIdChanged(int _Id)
{
	if (_Id == Page_Printing)
	{
		PrnPage->setPrinter(&Printer);
		PrnPage->setFitMode(CropModePage->fitMode());
		PrnPage->setPrintJob(ImageSelectPage->printJob());
	}
}
