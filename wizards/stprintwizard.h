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
#ifndef STPRINTWIZARD_H
#define STPRINTWIZARD_H

#include <QWizard>
#include <QPrinter> 
#include <QFileInfoList>
#include "stwizardsexport.h"
#include "ddocmodel.h"
#include "printjobprinter.h"

/**
Print process page
*/
class QPrintPreviewDialog;
class SProcessStatusWidget;
class STPWPrintingPage : public QWizardPage
{
	Q_OBJECT
	SProcessStatusWidget* StatusWidg;
	QPrinter* Printer;
	STDom::PrintJob PrintJob;
	STDom::PrintJobPrinter::EnFitMode FitMode;
	QPrintPreviewDialog* PDialog;
	bool AtomicPrint;

public:
	STPWPrintingPage(QWidget* _Parent = 0);
	void setPrinter(QPrinter* _Printer) { Printer = _Printer; }
	void setPrintJob(const STDom::PrintJob& _PrintJob) { PrintJob = _PrintJob; }
	void setFitMode(STDom::PrintJobPrinter::EnFitMode _FitMode) { FitMode = _FitMode; }
	void setAtomicPrint(bool _Value) { AtomicPrint = _Value; }
	void initializePage();
	bool print(QPrinter* _Printer);
	bool validatePage();

private slots:
	void slotRunPreview();
	void slotPreviewPaintRequested(QPrinter* _Printer);

};


/**
Page to choose crop mode
*/

class QRadioButton;
class QGridLayout;
class ChooseCropModePage : public QWizardPage
{
	Q_OBJECT

protected:
	QRadioButton* RBNoCut;
	QRadioButton* RBSideNoCut;
	QGridLayout* MLayout;

public:
	ChooseCropModePage(QWidget* _Parent = 0);
	int nextId() const;
	STDom::PrintJobPrinter::EnFitMode fitMode() const;
};


/**
Page to select, edit and cut images.

	@author Jordi Fernandez
*/

namespace STDom
{
	class PrintJobModel;
	class PrintJob;
}
class TPPhotoSelWidget;
class STPWImageSelectionPage : public QWizardPage
{
Q_OBJECT
	STDom::PrintJobModel* PJModel;
	TPPhotoSelWidget* PhotoSelW;

public:
	STPWImageSelectionPage(QWidget* _Parent = 0);
	void setPrintJob(const STDom::PrintJob& _PrintJob);
	void setAtomicPrint(bool _Value);
	int nextId() const;
	virtual QSize sizeHint () const { return QSize(750, 550); }
	STDom::PrintJob printJob() const;
};


/**
Wizard to print images using a printjob printer.

	@author
*/

class ST_WIZARDS_EXPORT STPrintJobWizard : public QWizard
{
	Q_OBJECT
	QWidget* Parent;
	QPrinter Printer;
	STPWImageSelectionPage* ImageSelectPage;
	ChooseCropModePage* CropModePage;
	STPWPrintingPage* PrnPage;

public:
	enum { Page_ImageSelection, Page_ChooseCropMode, Page_Printing};

public:
	STPrintJobWizard(QWidget* _Parent);
	void setPrintJob(const STDom::PrintJob& _PrintJob);
	void setAtomicPrint(bool _Value);
	int printImages(const QFileInfoList& _Images);

private slots:
	void slotCurrentIdChanged(int _Id);

};

#endif
