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

class STPWWelcomePage : public QWizardPage
{
Q_OBJECT
public:
	STPWWelcomePage(QWidget* _Parent = 0);
	int nextId() const;
};

class QLabel; 
class QPrinter; 
class SelectPrinterPage : public QWizardPage
{
Q_OBJECT
	QPrinter* Printer;
	QLabel* PNameInfo;
	QLabel* PSizeInfo;
	QLabel* POrientatioInfo;
	int ImagesToPrint; 
	
	void updatePrinterInfo(const QPrinter& _Printer);
	
public:
	SelectPrinterPage(QPrinter* _Printer, QWidget* _Parent = 0);
	void setNumImagesToPrint(int _Value) { ImagesToPrint = _Value; }
	int nextId() const;
	void initializePage();

private slots:
	void getPrinterSettings();
};

class SProcessStatusWidget; 
class STNCopiesProxyModel;
class QListView;
class QModelIndex;
class STThumbnailView; 
class ChooseImagesPage : public QWizardPage
{
	Q_OBJECT

public: 
	typedef struct TImagePrint
	{
		public: 
		int NCopies; 
		QFileInfo FInfo; 
	};

	typedef QList<TImagePrint> TImagePrintList; 

private: 
	STThumbnailView* LView;
	QFileInfoList Images; 
	SProcessStatusWidget* StatusWidg;
	STDom::DDocModel* ImageModel;
	STNCopiesProxyModel* MProxyModel;

public:
	ChooseImagesPage(QWidget* _Parent = 0);
	int nextId() const;
	void setImages(const QFileInfoList& _Images) { Images = _Images; }
	void initializePage();
	bool isComplete() const;
	TImagePrintList imagesToPrint();
	static QFileInfoList imagesToPrintFileInfoList(const TImagePrintList& _PrnList);

public slots: 
	void toggleChecked(const QModelIndex& _Index);
	void toggleCheckedCurrent();
};


class QRadioButton;
class QGridLayout;
class ChooseCropModePage : public QWizardPage
{
	Q_OBJECT 

public:
	enum EnCropMode
	{
		CropModeCut, 
		CropModeNoCut,
		CropModelNoCutCentered
	};

protected:
	QRadioButton* RBNoCut;
	QRadioButton* RBSideNoCut; 
	QGridLayout* MLayout;

public:
	ChooseCropModePage(QWidget* _Parent = 0);
	int nextId() const;
	EnCropMode cropMode() const;
};

class SelectionInterface;
class QLabel;
class SelectCropsPage : public QWizardPage
{
public: 
	typedef QList<QRect> TCropRectList; 

private:

	Q_OBJECT 
	QRadioButton* RBNoCut;
	QFileInfoList ImagesToCrop; 
	TCropRectList CropRects; 
	SelectionInterface* MSelIface;
	QAction* PreviousAct;
	QAction* NextAct;
	QLabel* LabInfo;
	
	int CurrentIndex; 
	QRect PageRect; 
	TCropRectList PageRectList;
	bool IsComplete; 

	void updateActionStatus();
	void updateInfo();
	void updateSelection();
	void setCurrentIndex(int _Index); 
	void storeCurrentCrop();
	void initialize(const QFileInfoList& _ImagesToCrop);

public:
	SelectCropsPage(QWidget* _Parent = 0);
	int nextId() const;
	void initialize(const QFileInfoList& _ImagesToCrop, const QRect& _PageRect);
	void initialize(const QFileInfoList& _ImagesToCrop, const TCropRectList& _PageRectList);
	bool validatePage(); 
	TCropRectList cropRects() const { return CropRects; } 
	bool isComplete() const;
	

private slots:
	void first(); 
	void next(); 
	void previous(); 
	void rotate();
};

class PrintingPage : public QWizardPage
{
	Q_OBJECT 
	SProcessStatusWidget* StatusWidg; 
	ChooseImagesPage* CIPage;
	QPrinter* Printer;
	ChooseCropModePage* ChooseCropPage;
	SelectCropsPage* CropsPage;

public:
	PrintingPage(ChooseImagesPage* _CIPage, QPrinter* _Printer, ChooseCropModePage* _ChooseCropPage, SelectCropsPage* _CropsPage, QWidget* _Parent = 0);
	void initializePage();
	bool validatePage();
};

/**
Wizard to print collections of images.

	@author
*/
class ST_WIZARDS_EXPORT STPrintWizard : public QWizard
{
	Q_OBJECT
	QPrinter Printer; 
	ChooseImagesPage* ChooseImgPage;
	SelectCropsPage* SelCropPage;
	SelectPrinterPage* SelPrinterPage;

public:	
	enum { Page_Welcome, Page_SelectPrinter, Page_ChooseImages, Page_ChooseCropMode, Page_SelectCrops, 
		Page_Printing};
			
public:
	STPrintWizard(QWidget* parent, Qt::WindowFlags flags = 0);
	void setImages(const QFileInfoList& _Images); 

private slots:
	void slotCurrentIdChanged(int _Id);

};

#endif
