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
#ifndef STPHOTOINDEXWIZARD_H
#define STPHOTOINDEXWIZARD_H

#include <QWizard>
#include "stwizardsexport.h"


/**
Index properties page.

	@author Jordi Fern�ndez
*/
class QSpinBox; 
class QLabel; 
class QComboBox; 
class STPIWIndexPropertiesPage : public QWizardPage 
{
Q_OBJECT
	
	enum EnStandardSizes
	{
		SizeA0, 
		SizeA0Plus, 
		SizeA1, 
		SizeA1Plus, 
		SizeA2, 
		SizeA3, 
		SizeA3Plus, 
		SizeA4, 
		SizeA5, 
		SizeA6, 
		Size20x30,
		Size15x22, 
		Size15x20, 
		Size10x15
	};

	QSpinBox* newSizeSpinBox(QWidget* _Parent, const QString& _FieldName);
	QLabel* newLabel(const QString& _Text, QWidget* _Parent);
	QComboBox* CBSizes;

public:
	STPIWIndexPropertiesPage(QWidget* _Parent = 0);
	void initializePage();
	bool isComplete () const;

private slots:
	void slotSizeSelected(int _Index);
};


/**
Output selection page.

	@author Jordi Fern�ndez
*/
class QToolButton; 
class STPISelectOutputPage : public QWizardPage
{
	Q_OBJECT
	QToolButton* TBFolder;
public:
	STPISelectOutputPage(QWidget* _Parent = 0); 
	virtual int nextId() const;
};

/**
Destination properties page.

	@author Jordi Fern�ndez
*/
class FileSelectLineEdit; 
class DestinationPropertiesPage : public QWizardPage
{
	Q_OBJECT
	FileSelectLineEdit* MFileLEdit;

public:
	DestinationPropertiesPage(QWidget* _Parent = 0); 
	void initializePage();
};

/**
End Page with some information of the generation.

	@author Jordi Fern�ndez
*/

class STPIEndPage : public QWizardPage
{
	Q_OBJECT
	QLabel* LabInfo;
	QWizard* ParentWizard; 

public:
	STPIEndPage(QWidget* _Parent); 
	virtual void initializePage();
};


/**
Wizard for photoindex generation properties.

	@author
*/
class ST_WIZARDS_EXPORT STPhotoIndexWizard : public QWizard
{
	Q_OBJECT

public:
	enum { Page_IndexProperties, Page_SelectOutput, Page_SelectPrinter, Page_DestinationProperties, Page_End };

public:
	STPhotoIndexWizard(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	bool sendToPrinter() const;
};

#endif
