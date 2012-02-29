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
#include "stphotoindexwizard.h"

#include <QSpinBox> 
#include <QLabel> 
#include <QLayout> 
#include <QGroupBox> 
#include <QCheckBox> 
#include <QLineEdit> 
#include <QDir> 
#include <QButtonGroup> 
#include <QToolButton> 
#include <QTextStream> 

#include "fileselectlineedit.h" 
#include "stcolorbutton.h"
#include "smappablecombobox.h"

#include "qxtgroupbox.h"

//_____________________________________________________________________________
//
// class STPhotoIndexWizard
//_____________________________________________________________________________

QSpinBox* STPIWIndexPropertiesPage::newSizeSpinBox(QWidget* _Parent, const QString& _FieldName)
{
	QSpinBox* Res = new QSpinBox(_Parent); 
	connect(Res, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	Res->setSuffix(tr(" mm")); 
	Res->setRange(0, 9999); 
	registerField(_FieldName, Res);
	return Res; 	
} 

QLabel* STPIWIndexPropertiesPage::newLabel(const QString& _Text, QWidget* _Parent)
{
	QLabel* Res = new QLabel(_Text + ":", _Parent); 
	Res->setAlignment(Qt::AlignRight); 
	QFont LFont = Res->font(); 
	LFont.setBold(true); 
	Res->setFont(LFont); 
	return Res; 
}


STPIWIndexPropertiesPage::STPIWIndexPropertiesPage(QWidget* _Parent): QWizardPage(_Parent)
{
	setTitle(tr("<h1>Photo Index properties</h1>"));
	setSubTitle(tr("<p>Please specify index sizes and other parameters</p>"));
	setPixmap(QWizard::WatermarkPixmap, QPixmap(":/watermark.png"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	
	QGroupBox* GBBasic = new QGroupBox(tr("Basic"), this);
	MLayout->addWidget(GBBasic); 

	QHBoxLayout* BasicLayout = new QHBoxLayout(GBBasic); 
	BasicLayout->addWidget(newLabel(tr("Index size"), this)); 

	CBSizes = new QComboBox(this); 

	CBSizes->addItem(tr("A0"), SizeA0);
	CBSizes->addItem(tr("A0+"), SizeA0Plus);
	CBSizes->addItem(tr("A1"), SizeA1);
	CBSizes->addItem(tr("A1+"), SizeA1Plus); 
	CBSizes->addItem(tr("A2"), SizeA2); 
	CBSizes->addItem(tr("A3"), SizeA3); 
	CBSizes->addItem(tr("A3+"), SizeA3Plus); 
	CBSizes->addItem(tr("A4"), SizeA4); 
	CBSizes->addItem(tr("A5"), SizeA5); 
	CBSizes->addItem(tr("A6"), SizeA6); 
	CBSizes->addItem(tr("20x30"), Size20x30);
	CBSizes->addItem(tr("15x22"), Size15x22); 
	CBSizes->addItem(tr("15x20"), Size15x20); 
	CBSizes->addItem(tr("10x15"), Size10x15);
	CBSizes->setCurrentIndex(CBSizes->findData(SizeA4)); 
	connect(CBSizes, SIGNAL(activated( int )), this, SLOT(slotSizeSelected(int )));

	BasicLayout->addWidget(CBSizes); 

	//--- Advanced parameters ---
	QxtGroupBox* AdvancedGBox = new QxtGroupBox(tr("Advanced"), this); 
	AdvancedGBox->setChecked(false); 
	MLayout->addWidget(AdvancedGBox); 

	QVBoxLayout* AdvLayout = new QVBoxLayout(AdvancedGBox); 

	
	QGroupBox* GBSize = new QGroupBox(tr("Size"), this); 
	AdvLayout->addWidget(GBSize); 
	QHBoxLayout* GBCoverLayout = new QHBoxLayout(GBSize); 
	
	GBCoverLayout->addWidget(newLabel(tr("Width"), GBSize)); 
	GBCoverLayout->addWidget(newSizeSpinBox(GBSize, "width")); 
	
	GBCoverLayout->addWidget(newLabel(tr("Height"), GBSize)); 
	GBCoverLayout->addWidget(newSizeSpinBox(GBSize, "height")); 

	GBCoverLayout->addWidget(newLabel(tr("DPIS"), GBSize)); 
	QSpinBox* NewSPBox = new QSpinBox(GBSize); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(1, 999); 
	GBCoverLayout->addWidget(NewSPBox); 
	registerField("dpis", NewSPBox); 

	QCheckBox* NewCheck = new QCheckBox(tr("Draw Frames ?"), this); 
	AdvLayout->addWidget(NewCheck); 
	registerField("drawframes", NewCheck); 	

	NewCheck = new QCheckBox(tr("Draw File Numbers ?"), this); 
	AdvLayout->addWidget(NewCheck); 
	registerField("drawfilenumbers", NewCheck); 	

	NewCheck = new QCheckBox(tr("Draw File Names ?"), this); 
	AdvLayout->addWidget(NewCheck); 
	registerField("drawfilenames", NewCheck); 	

	QHBoxLayout* NumImgLayout = new QHBoxLayout;
	AdvLayout->addLayout(NumImgLayout); 
	NumImgLayout->addWidget(new QLabel(tr("Estimated num of images per page:"), this)); 
	NewSPBox = new QSpinBox(this); 
	connect(NewSPBox, SIGNAL(valueChanged( int )), this, SIGNAL(completeChanged())); 
	NewSPBox->setRange(2, 999); 
	NumImgLayout->addWidget(NewSPBox); 
	registerField("numimages", NewSPBox); 

 	QHBoxLayout* BottomLayout = new QHBoxLayout; 
 	AdvLayout->addLayout(BottomLayout); 
	BottomLayout->addWidget(new QLabel(tr("Background Color:"), this)); 
	STColorButton* ColButton = new STColorButton(this); 
	ColButton->setText(tr("Click to Select")); 
	BottomLayout->addWidget(ColButton); 
	registerField("backgroundcolor", ColButton, "colorStr", SIGNAL(valueChanged(const QString&))); 


}


void STPIWIndexPropertiesPage::initializePage()
{
	setField("width", 210); 
	setField("height", 297); 
	setField("dpis", 150); 
	setField("numimages", 40); 
	setField("drawframes", true); 
	setField("drawfilenumbers", true); 
	setField("drawfilenames", true); 
	setField("backgroundcolor", "#FFFFFF"); 
}

bool STPIWIndexPropertiesPage::isComplete () const
{
	return field("width").toInt() > 0 &&
		field("height").toInt() > 0 &&
		field("dpis").toInt() > 10;
}


void STPIWIndexPropertiesPage::slotSizeSelected(int _Index)
{
	switch (CBSizes->itemData(_Index).toInt())
	{
		case SizeA0 :
			setField("width", 814); 
			setField("height", 1189); 			
		break;
		case SizeA0Plus :
			setField("width", 914); 
			setField("height", 1292); 			
		break;
		case SizeA1 :
			setField("width", 594); 
			setField("height", 841); 			
		break;
		case SizeA1Plus :
			setField("width", 609); 
			setField("height", 914); 			
		break;
		case SizeA2 :
			setField("width", 420); 
			setField("height", 594); 			
		break;
		case SizeA3 :
			setField("width", 297); 
			setField("height", 420); 			
		break;
		case SizeA3Plus :
			setField("width", 329); 
			setField("height", 483); 			
		break;
		case SizeA4 :
			setField("width", 210); 
			setField("height", 297); 			
		break;
		case SizeA5 :
			setField("width", 148); 
			setField("height", 210); 			
		break;
		case SizeA6 :
			setField("width", 105); 
			setField("height", 148); 			
		break;
		case Size20x30 :
			setField("width", 203); 
			setField("height", 305); 			
		break;
		case Size15x22 :
			setField("width", 152); 
			setField("height", 229); 			
		break;
		case Size15x20 :
			setField("width", 152); 
			setField("height", 203); 			
		break;
		case Size10x15 :
			setField("width", 102); 
			setField("height", 152); 			
		break;
	}
}



//_____________________________________________________________________________
//
// class STPISelectOutputPage
//_____________________________________________________________________________

STPISelectOutputPage::STPISelectOutputPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Select index output</h1>"));
	setSubTitle(tr("<p>Where do you want to send the Photo index ?</p>"));
	QHBoxLayout* MLayout = new QHBoxLayout(this); 
	
	QButtonGroup* MButtGroup = new QButtonGroup(this); 
	MButtGroup->setExclusive(true); 
	
	TBFolder = new QToolButton(this); 
	connect(TBFolder, SIGNAL(clicked()), this, SIGNAL(completeChanged())); 
	TBFolder->setCheckable(true); 
	TBFolder->setIcon(QIcon(":/st/wizards/folder_image.png")); 
	TBFolder->setIconSize(QSize(128, 128));
	TBFolder->setText(tr("Save to folder")); 
	TBFolder->setChecked(true); 
	MButtGroup->addButton(TBFolder); 
	MLayout->addWidget(TBFolder); 
	
	QToolButton* TBPrint = new QToolButton(this); 
	TBPrint->setCheckable(true); 
	TBPrint->setIcon(QIcon(":/st/wizards/printer.png")); 
	TBPrint->setIconSize(QSize(128, 128));
	TBPrint->setText(tr("Print")); 
	MButtGroup->addButton(TBPrint); 
	MLayout->addWidget(TBPrint); 
}

int STPISelectOutputPage::nextId() const 
{
	if (TBFolder->isChecked())
		return STPhotoIndexWizard::Page_DestinationProperties; 
	else 
		return STPhotoIndexWizard::Page_End; 
}

//_____________________________________________________________________________
//
// class DestinationPropertiesPage
//_____________________________________________________________________________


DestinationPropertiesPage::DestinationPropertiesPage(QWidget* _Parent) : QWizardPage(_Parent)
{
	setTitle(tr("<h1>Select disk folder</h1>"));
	setSubTitle(tr("<p>The program generates images inside this folder.</p>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 

 	QHBoxLayout* TopLayout = new QHBoxLayout; 
 	MLayout->addLayout(TopLayout); 

	TopLayout->addWidget(new QLabel(tr("Output format: ")));
	SMappableComboBox* ComboFormat = new SMappableComboBox(this); 
	ComboFormat->addItem("JPG, Joint Photographic Experts Group", "JPG");
	ComboFormat->addItem("PNG, Portable Network Graphics", "PNG");
	ComboFormat->addItem("PBM, Portable Bitmap", "PBM");
	ComboFormat->addItem("BMP, Windows Bitmap", "BMP");
	TopLayout->addWidget(ComboFormat); 
	registerField("format", ComboFormat, "value", SIGNAL(currentIndexChanged ( int )));
	

	MLayout->addWidget(new QLabel(tr("Folder name:"), this));
	MFileLEdit = new FileSelectLineEdit(FileSelectLineEdit::GetExistingDirectory, this);
	MLayout->addWidget(MFileLEdit); 
	
	registerField("diskfolderpath", MFileLEdit->lineEdit());	 
	setField("diskfolderpath", QDir::home().absolutePath()); 
}

void DestinationPropertiesPage::initializePage()
{
	MFileLEdit->setInitDir(field("diskfolderpath").toString()); 
}

//_____________________________________________________________________________
//
// class STPIEndPage
//_____________________________________________________________________________

STPIEndPage::STPIEndPage(QWidget* _Parent)
{
	setTitle(tr("<h1>Ready to build photoindex</h1>"));
	setSubTitle(tr("<p>Click on 'Finish' button to start building.</p>"));
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	LabInfo = new QLabel(this); 
	ParentWizard = qobject_cast<QWizard*> (_Parent);
	MLayout->addWidget(LabInfo); 
}

void STPIEndPage::initializePage()
{
	if (ParentWizard)
	{
		QString Info; 
		QTextStream Strm(&Info);
		//Strm << "<table bgcolor=#00FF00 border=0 >";
		Strm << "<table border=0 >";
		Strm << "<tr><th colspan='7'><span style=\" font-size:20pt; font-family:'Arial'\">"  << tr("PhotoIndex information") << "</font></span></th></tr>";
		Strm << "<tr><td><b>Width</b></td>" << "<td>" << ParentWizard->field("width").toString() << "mm </td></tr>";
		Strm << "<tr><td><b>Height</b></td>" << "<td>" << ParentWizard->field("height").toString() << "mm </td></tr>";
		Strm << "<tr><td><b>Dpis</b></td>" << "<td>" << ParentWizard->field("dpis").toString() << "</td></tr>";
		Strm << "<tr><td><b>Page images</b></td>" << "<td>" << ParentWizard->field("numimages").toString() << "</td></tr>";

		if (!ParentWizard->hasVisitedPage(STPhotoIndexWizard::Page_DestinationProperties))
			Strm << "<tr><td><b>Index destination to Printer.</b></td>" << "<td>" << "</td></tr>";
		else
		{
			Strm << "<tr><td><b>Index destination to Folder:</b></td><td>" << ParentWizard->field("diskfolderpath").toString() << "</td></tr>";
			Strm << "<tr><td><b>Output format</b></td><td>" << ParentWizard->field("format").toString() << "</td></tr>";
		}


		Strm << "</table>";
		LabInfo->setText(Info); 
	}
}

//_____________________________________________________________________________
//
// class STPhotoIndexWizard
//_____________________________________________________________________________

STPhotoIndexWizard::STPhotoIndexWizard(QWidget* _Parent, Qt::WindowFlags _Flags) : QWizard(_Parent, _Flags)
{
	setPage(Page_IndexProperties, new STPIWIndexPropertiesPage(this)); 
	setPage(Page_SelectOutput, new STPISelectOutputPage(this)); 
	setPage(Page_DestinationProperties, new DestinationPropertiesPage(this)); 
	setPage(Page_End, new STPIEndPage(this)); 
	
	setStartId(Page_IndexProperties);
	
#ifndef Q_WS_MAC
	setWizardStyle(ModernStyle);
#endif
	setPixmap(QWizard::LogoPixmap, QPixmap(":/albumwizard.png"));
	setWindowTitle(QObject::tr("PhotoIndex Wizard"));
}

bool STPhotoIndexWizard::sendToPrinter() const 
{
	return !hasVisitedPage(Page_DestinationProperties);
}
