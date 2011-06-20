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
#ifndef STALBUMWIZARD_H
#define STALBUMWIZARD_H

#include <QWizard>
#include <QFileInfo> 

#include "stwizardsexport.h"
#include "ddoc.h"
#include "stphotolayout.h"
#include "ddocmodel.h"
#include "stphotobookbuildoptions.h"


class QTextEdit; 
class QLineEdit;
class PhotoBookNamePage : public QWizardPage
{
Q_OBJECT
	QTextEdit* TEDescription; 
	QLineEdit* LEName;
	
public:
	PhotoBookNamePage(QWidget* _Parent = 0);
	int nextId() const;
	void initializePage();
	bool validatePage();
};

class QRadioButton; 
class ChooseTemplateModePage : public QWizardPage
{
Q_OBJECT
	QRadioButton* RBCustomSizes;

public:
	ChooseTemplateModePage(QWidget* _Parent = 0);
	int nextId() const;
};

class STCollectionTemplateModel;
class QListView; 
class QWebView;
class QFrame;
class QComboBox;
class QTimer;
class QLabel;
class QActionGroup;
class ChooseTemplatePage : public QWizardPage
{
Q_OBJECT
	enum EnState
	{
		StateNoTypeSelected,
		StateShowWebInfo,
		StateNoInfo,
		StateGettingInfo,
		StateTemplatesEmpty
	};

	STCollectionTemplateModel* Model;
	QListView* View;
	QFileInfo TemplateFileInfo;
	QWebView* WebView;
	QFrame* NoInfoFrame;
	QComboBox* CBSize;
	QLabel* NoInfoLabel;
	QLabel* LabelInfoPixmap;
	QLabel* LabSize;
	bool InetgetPicture1;
	QTimer* InetgetTimer;
	QActionGroup* TypeActions;
	QFrame* BottomFrame;
	QLabel* NoTemplatesLabel;

	void setCurrentState(EnState _State);

public:
	ChooseTemplatePage(QWidget* _Parent = 0);
	int nextId() const;
	void selectFirstIndex();
	void initializePage();
	STDom::DDocFormat currentSize() const;
	STPhotoLayout::EnLayoutType currentType() const;
	bool validatePage();
	bool isComplete() const; 
	bool typeSelected();
	QFileInfo photoBookTemplateFileInfo() const { return TemplateFileInfo; }

private slots:
	void slotTemplateIndexClicked(const QModelIndex& );
	void slotWebLoadStarted();
	void slotWebLoadFinished(bool _Error);
	void reloadTemplates();
	void inetgetBlinkTimeout();
signals:
	void templateSelected();

};


/**
Wizard to specify album sizes.

	@author 
*/
class QSpinBox; 
class QLabel; 
class CustomSizesPage : public QWizardPage 
{
Q_OBJECT
	
	QFileInfo TemplateFileInfo; 

	QSpinBox* newSizeSpinBox(QWidget* _Parent, const QString& _FieldName);
	QLabel* newLabel(const QString& _Text, QWidget* _Parent);

public:
	CustomSizesPage(QWidget* _Parent = 0);
	void initializePage();
	bool isComplete () const;
	bool validatePage();
	int nextId() const;
	QFileInfo photoBookTemplateFileInfo() const { return TemplateFileInfo; }
};


class QRadioButton; 
class STPhotoBookCollectionModel;
class QxtGroupBox;
class QTextBrowser;
class QxtPushButton;
class ChooseCreationModePage : public QWizardPage
{
Q_OBJECT
	QxtPushButton* PBAutomatic;
	STPhotoBookCollectionModel* PredesignModel;
	QxtGroupBox* GBUsePredesign;
	QListView* LVPredesigns;
	QTextBrowser* TBDescription;
	int PredesignPhotoItems;

public:
	ChooseCreationModePage(QWidget* _Parent = 0);
	int nextId() const;
	void setPhotoBookTemplateFileInfo(const QFileInfo& _FileInfo);
	bool validatePage();
	bool autoBuildModeSelected() const;
	bool usePredesign() const;
	int predesignPhotoItems() const { return PredesignPhotoItems; }
	QDir predesignDir() const;
private slots:
	void slotPredesignChanged(const QModelIndex& _Index);

};


class QGroupBox;
class QSpinBox;
class BuildOptionsPage : public QWizardPage
{
	Q_OBJECT

	QGroupBox* GBGeneral;
	QGroupBox* GBPhotoBook;
	QGroupBox* GBCalendar;
	QSpinBox* SBNumPages;
	STPhotoLayout::EnLayoutType LayoutType;
	bool AutoBuildMode;


public:
	BuildOptionsPage(QWidget* _Parent = 0);
	int nextId() const;
	virtual void initializePage ();
	virtual bool isComplete() const;
	void setBuildOptions(const STPhotoBookBuildOptions& _Options);
	STPhotoBookBuildOptions getBuildOptions() const;
	void setTemplate(const STPhotoBookTemplate& _Template, STPhotoLayout::EnLayoutType _Type);
	void setAutoBuildMode(bool _Value);
	void setUsePredesign(bool _Value);
};


class SPImageBoxListView;
class SelectDiskFolderPage : public QWizardPage
{
Q_OBJECT

	SPImageBoxListView* ImageBoxListView;
	QLabel* InfoLabel;
	int PagesToFill, OptimalImagesPerPage, AbsoluteImageCount;
	void updateInfo();

public:
	SelectDiskFolderPage(QWidget* _Parent = 0);
	void setTemplate(const STPhotoBookTemplate& _Template, const STPhotoBookBuildOptions& _Options);
	void setAbsoluteImageCount(int _Value);
	void clearSelection();
	int nextId() const;
	bool isComplete() const;
	STDom::DDocModel* selectedImages() const;
private slots:
	void slotUpdateInfo();
};

class AlbumWizardEndPage : public QWizardPage
{
Q_OBJECT
public:
	AlbumWizardEndPage(QWidget* _Parent = 0);
};

/**
Wizard to create albums.

	@author 
*/
class ST_WIZARDS_EXPORT STAlbumWizard : public QWizard
{

	Q_OBJECT

public:	
	enum { Page_PhotoBookName, Page_ChooseTemplateMode, Page_CustomSizes, Page_ChooseTemplate, Page_CooseCreationMode,
			Page_BuildOptions, Page_SelectDiskFolder, Page_End };
		
private: 		
	ChooseTemplatePage* CTemplatePage; 
	CustomSizesPage* CCustomSizesPage;
	SelectDiskFolderPage* SDFolderPage;
	BuildOptionsPage* PBuildOptions;
	ChooseCreationModePage* CCreationModePage;
	bool CustomSizesEnabled; 
	STPhotoBookTemplate PhotoBookTemplate;

public:
	STAlbumWizard(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~STAlbumWizard();
	QFileInfo photoBookTemplateFileInfo() const;
	bool autoFillSelected() const; 
	//! returns true if we get images from collection.
	bool imagesFromCollection() const; 
	quint64 collectionFolderKey() const;
	int nextId() const;
	STPhotoLayout::EnLayoutType templateType() const;
	void setCustomSizesEnabled(bool _Enabled) { CustomSizesEnabled = _Enabled; }
	bool customSizesEnabled() const { return CustomSizesEnabled; }
	STDom::DDocModel* selectedImages() const;
	STPhotoBookBuildOptions buildOptions() const;
	bool usePredesign() const;
	QDir predesignDir() const;

private slots:
	void slotLoadTemplate();
};


class ST_WIZARDS_EXPORT STAlbumMiniWizard : public QWizard
{

	Q_OBJECT

public:	
	enum { Page_PhotoBookName, Page_End };
		
private: 		
	
public:
	STAlbumMiniWizard(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	int nextId() const;
};

#endif
