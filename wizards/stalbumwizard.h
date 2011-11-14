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
#include "ddocmodel.h"
#include "buildoptions.h"
#include "metainfo.h"

namespace SPhotoBook
{
	class MetaInfo;
	class TemplateInfoList;
	class TemplateInfoModel;
	class DesignInfo;
	class DesignInfoModel;
	class ImageBoxListView;
}

class STCollectionTemplateModel;
class QListView; 
class QWebView;
class QFrame;
class QComboBox;
class QTimer;
class QLabel;
class QActionGroup;
class QToolBar;
class StarlabAbstractManager;

class ChooseTemplatePage : public QWizardPage
{
Q_OBJECT
	enum EnState
	{
		StateShowWebInfo,
		StateNoInfo,
		StateGettingInfo,
		StateTemplatesEmpty
	};

	SPhotoBook::TemplateInfoModel* Model;
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
	bool HasPreselection;
	SPhotoBook::MetaInfo::EnTemplateType  PreselectedType;
	StarlabAbstractManager* Manager;


	void setCurrentState(EnState _State);

public:
	ChooseTemplatePage(StarlabAbstractManager* _Manager, QWidget* _Parent = 0);
	int nextId() const;
	void selectFirstIndex();
	void initializePage();
	SPhotoBook::TemplateInfo templateInfo(const QModelIndex& _Index, const QSizeF& _Size) const;
	SPhotoBook::TemplateInfo selectedTemplateInfo() const;
	bool validatePage();
	bool isComplete() const; 
	void setTemplateList(const SPhotoBook::TemplateInfoList& _TemplateList, SPhotoBook::MetaInfo::EnTemplateType _Type);


private slots:
	void slotTemplateIndexClicked(const QModelIndex& );
	void slotWebLoadStarted();
	void slotWebLoadFinished(bool _Error);
	void inetgetBlinkTimeout();
signals:
	void templateSelected();

};


class QRadioButton; 
class QxtGroupBox;
class QTextBrowser;
class QxtPushButton;
class ChooseCreationModePage : public QWizardPage
{
Q_OBJECT
	QxtPushButton* PBAutomatic;
	SPhotoBook::DesignInfoModel* DesignModel;
	QListView* LVDesigns;
	QTextBrowser* TBDescription;
	int PredesignPhotoItems;

public:
	ChooseCreationModePage(QWidget* _Parent = 0);
	int nextId() const;
	void setTemplateInfo(const SPhotoBook::TemplateInfo& _TemplateInfo);
	bool validatePage();
	bool autoBuildModeSelected() const;
	SPhotoBook::DesignInfo designInfo() const;

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
	SPhotoBook::MetaInfo::EnTemplateType  LayoutType;
	bool AutoBuildMode;


public:
	BuildOptionsPage(QWidget* _Parent = 0);
	int nextId() const;
	virtual void initializePage ();
	virtual bool isComplete() const;
	void setBuildOptions(const SPhotoBook::BuildOptions& _Options);
	SPhotoBook::BuildOptions getBuildOptions() const;
	void setTemplateMetaInfo(const SPhotoBook::MetaInfo& _MInfo);
	void setAutoBuildMode(bool _Value);
	void setUsePredesign(bool _Value);
};


class SelectDiskFolderPage : public QWizardPage
{
Q_OBJECT

	SPhotoBook::ImageBoxListView* IBListView;
	QLabel* InfoLabel;
	int PagesToFill, OptimalImagesPerPage, AbsoluteImageCount;
	void updateInfo();

public:
	SelectDiskFolderPage(QWidget* _Parent = 0);
	void setMetaInfo(const SPhotoBook::MetaInfo& _Info, const SPhotoBook::BuildOptions& _Options);
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
	enum { Page_ChooseTemplate, Page_ChooseCreationMode,
			Page_BuildOptions, Page_SelectDiskFolder, Page_End };
		
private: 		
	ChooseTemplatePage* CTemplatePage; 
	SelectDiskFolderPage* SDFolderPage;
	BuildOptionsPage* PBuildOptions;
	ChooseCreationModePage* CCreationModePage;
	StarlabAbstractManager* Manager;

public:
	STAlbumWizard(StarlabAbstractManager* _Manager, QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~STAlbumWizard();
	SPhotoBook::DesignInfo designInfo() const;
	SPhotoBook::TemplateInfo templateInfo() const;
	bool autoFillSelected() const; 
	int nextId() const;
	STDom::DDocModel* selectedImages() const;
	SPhotoBook::BuildOptions buildOptions() const;
	void setTemplateList(const SPhotoBook::TemplateInfoList& _TemplateList, SPhotoBook::MetaInfo::EnTemplateType _Type);
};


#endif
