/****************************************************************************
**
** Copyright (C) 2010-2011 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "metainfowidget.h"
#include <QLayout>
#include <QFormLayout>
#include <QToolBar>
#include <QTabWidget>
#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QDebug>
#include "qxtgroupbox.h"
#include "qwwrichtextedit.h"
#include "fpixmapselector.h"
#include "templateinfo.h"
#include "rendersettings.h"
#include "systemtemplates.h"
#include "strecteditwidget.h"

using namespace SPhotoBook;

void MetaInfoWidget::createActions()
{
	QAction* DefaultAct = new QAction(QIcon(":/photobook/defaults.png"), tr("Reset to defaults"), this);
	connect(DefaultAct, SIGNAL(triggered()), this, SLOT(slotHelp()));
	ToolBar->addAction(DefaultAct);

	QAction* HelpAct = new QAction(QIcon(":/photobook/help.png"), tr("&Help"), this);
	connect(HelpAct, SIGNAL(triggered()), this, SLOT(slotHelp()));
	ToolBar->addAction(HelpAct);

	QAction* SaveAct = new QAction(QIcon(":/photobook/save.png"), tr("&Save"), this);
	connect(SaveAct, SIGNAL(triggered()), this, SLOT(slotSave()));
	ToolBar->addAction(SaveAct);

	QAction* LoadAct = new QAction(QIcon(":/photobook/load.png"), tr("&Load"), this);
	connect(LoadAct, SIGNAL(triggered()), this, SLOT(slotLoad()));
	ToolBar->addAction(LoadAct);

}

QWidget* MetaInfoWidget::createGeneralWidget()
{
	QWidget* Widget = new QWidget(this);
	QVBoxLayout* MLayout = new QVBoxLayout(Widget);

	QHBoxLayout* TopLayout = new QHBoxLayout;
	MLayout->addLayout(TopLayout);

	QFormLayout* LeftFormLayout = new QFormLayout;
	TopLayout->addLayout(LeftFormLayout);

	CBName = new QComboBox(this);
	CBName->setMinimumWidth(150);
	CBName->setEditable(true);
	connect(CBName, SIGNAL(activated(QString)), this, SLOT(slotNameActivated(QString)));
	LeftFormLayout->addRow(tr("Name"), CBName);

	CBDesignName = new QComboBox(this);
	CBDesignName->setMinimumWidth(150);
	CBDesignName->setEditable(true);
	LeftFormLayout->addRow(tr("Design name"), CBDesignName);

	ISImage = new FPixmapSelector(this);
	LeftFormLayout->addRow(tr("Image"), ISImage);

	TEDescription = new QwwRichTextEdit(this);
	MLayout->addWidget(new QLabel(tr("Description"), this));
	MLayout->addWidget(TEDescription);


	QFormLayout* RightFormLayout = new QFormLayout;
	TopLayout->addLayout(RightFormLayout);

	CBType = new QComboBox(this);
	CBType->addItem(tr("PhotoBook"), MetaInfo::TypePhotoBook);
	CBType->addItem(tr("Calendar"), MetaInfo::TypeCalendar);
	CBType->addItem(tr("Card"), MetaInfo::TypeCard);
	CBType->addItem(tr("IdPhoto"), MetaInfo::TypeIdPhoto);
	CBType->addItem(tr("MultiPhoto"), MetaInfo::TypeMultiPhoto);
	RightFormLayout->addRow(tr("Type"), CBType);


	SBWidth = new QSpinBox(this);http://www.wysota.eu.org/wwwidgets/
	SBWidth->setRange(0, 9999);
	RightFormLayout->addRow(tr("Width"), SBWidth);

	SBHeight = new QSpinBox(this);
	SBHeight->setRange(0, 9999);
	RightFormLayout->addRow(tr("Height"), SBHeight);

	TESizeAlias = new QLineEdit(this);
	RightFormLayout->addRow(tr("Size Alias"), TESizeAlias);

	SBVersion = new QSpinBox(this);
	RightFormLayout->addRow(tr("Version"), SBVersion);

	CBCyphered = new QCheckBox(this);
	RightFormLayout->addRow(tr("Cyphered"), CBCyphered);


	return Widget;
}

QWidget* MetaInfoWidget::createRenderWidget()
{
	QWidget* Widget = new QWidget(this);
	QVBoxLayout* MLayout = new QVBoxLayout(Widget);

	QFormLayout* FormLayout = new QFormLayout;
	MLayout->addLayout(FormLayout);

	SBPPageWidth = new QSpinBox(this);
	SBPPageWidth->setRange(0, 9999);
	FormLayout->addRow(tr("Print page width"), SBPPageWidth);

	SBPPageHeight = new QSpinBox(this);
	SBPPageHeight->setRange(0, 9999);
	FormLayout->addRow(tr("Print page height"), SBPPageHeight);

	SBDpis = new QSpinBox(this);
	SBDpis->setRange(0, 500);
	FormLayout->addRow(tr("Dpis"), SBDpis);

        SBOverlayMargin = new QSpinBox(this);
        SBOverlayMargin->setRange(0,200);
        FormLayout->addRow(tr("Overlay Margin"), SBOverlayMargin);

	CBPreProcType = new QComboBox(this);
	CBPreProcType->addItem(tr("None"), RenderSettings::TypeNone);
	CBPreProcType->addItem(tr("Horizontal BookLet"), RenderSettings::TypeHBooklet);
	CBPreProcType->addItem(tr("Vertical BookLet"), RenderSettings::TypeVBooklet);
	CBPreProcType->addItem(tr("Multiply"), RenderSettings::TypeMultiply);

	FormLayout->addRow(tr("Print Preprocess type"), CBPreProcType);

	return Widget;
}

QWidget* MetaInfoWidget::createBehaviorWidget()
{
	QWidget* Widget = new QWidget(this);
		QGroupBox* GBGeneral = new QGroupBox(this);

	QHBoxLayout* MLayout = new QHBoxLayout(Widget);
		MLayout->addWidget(GBGeneral);

		// ------ General -------
	QFormLayout* FormLayout = new QFormLayout(GBGeneral);

	SBModPages = new QSpinBox(this);
	FormLayout->addRow(tr("Mod Pages"), SBModPages);

	SBMaxPages = new QSpinBox(this);
	SBMaxPages->setRange(0, 9999);
	FormLayout->addRow(tr("Max Pages"), SBMaxPages);

	SBMinPages = new QSpinBox(this);
	SBMinPages->setRange(0, 9999);
	FormLayout->addRow(tr("Min Pages"), SBMinPages);

	CBMultiPhoto = new QCheckBox(this);
	FormLayout->addRow(tr("MultiPhoto selection"), CBMultiPhoto);

	CBPreferMinPages = new QCheckBox(this);
	FormLayout->addRow(tr("Prefer min pages"), CBPreferMinPages);

	CBAtomic = new QCheckBox(this);
	FormLayout->addRow(tr("Is Atomic"), CBAtomic);

	SBOptImagesPerPage = new QSpinBox(this);
	SBOptImagesPerPage->setRange(0, 9999);
	FormLayout->addRow(tr("Optimal images per page"), SBOptImagesPerPage);

	CBAutoGenerate = new QCheckBox(this);
	FormLayout->addRow(tr("Auto generate layouts"), CBAutoGenerate);

	// ------ Advanced -------
	QGroupBox* GBAdvanced = new QGroupBox(tr("Advanced"), this);
	MLayout->addWidget(GBAdvanced);
	FormLayout = new QFormLayout(GBAdvanced);

	CBSnapToBounds = new QCheckBox(this);
	CBSnapToBounds->setToolTip(tr("Snap photo items to photoitem bounding rect."));
	FormLayout->addRow(tr("Snap to bounds"), CBSnapToBounds );

	CBExpandImagesToFillFrames= new QCheckBox(this);
	FormLayout->addRow(tr("Expand images to fill frames"), CBExpandImagesToFillFrames);

	CBAutodetectImageOrientation = new QCheckBox(this);
	FormLayout->addRow(tr("Autodetect image orientation"), CBAutodetectImageOrientation);

	CBAutoAdjustFrames = new QCheckBox(this);
	FormLayout->addRow(tr("Auto adjust frames to images"), CBAutoAdjustFrames);

	UseImagesAsBackgrounds = new QCheckBox(this);
	FormLayout->addRow(tr("Use images as backgrounds"), UseImagesAsBackgrounds);

	SBFixedOutMargin = new QSpinBox(this);
	SBFixedOutMargin->setToolTip(tr("Fixed margin of image item bounding rect. Not applied if is <= 0"));
	SBFixedOutMargin->setRange(0, 9999);
	FormLayout->addRow(tr("Fixed out margin"), SBFixedOutMargin);

	return Widget;
}

QWidget* MetaInfoWidget::createUIWidget()
{
	QWidget* Widget = new QWidget(this);
	QVBoxLayout* MLayout = new QVBoxLayout(Widget);

	GBCoverMargin = new QxtGroupBox(tr("Cover Margins"), this);
	GBCoverMargin->setChecked(false);
	MLayout->addWidget(GBCoverMargin);
	QHBoxLayout* CMarginLayout = new QHBoxLayout(GBCoverMargin);
	RECover = new STRectEditWidget(this);
	RECover->setEditedRect(QRectF(0,0,0,0));
	RECover->setSuffix(" mm");
	CMarginLayout->addWidget(RECover);

	GBCoverSpineMargin = new QxtGroupBox(tr("Cover Spine Margins"), this);
	GBCoverSpineMargin->setChecked(false);
	MLayout->addWidget(GBCoverSpineMargin);
	QHBoxLayout* CSpineMarginLayout = new QHBoxLayout(GBCoverSpineMargin);
	RECoverSpine = new STRectEditWidget(this);
	RECoverSpine->setEditedRect(QRectF(0,0,0,0));
	RECoverSpine->setSuffix(" mm");
	CSpineMarginLayout->addWidget(RECoverSpine);

	GBPageMargin = new QxtGroupBox(tr("Page Margins"), this);
	GBPageMargin->setChecked(false);
	MLayout->addWidget(GBPageMargin);
	QHBoxLayout* CPageMarginLayout = new QHBoxLayout(GBPageMargin);
	REPage = new STRectEditWidget(this);
	REPage->setEditedRect(QRectF(0,0,0,0));
	REPage->setSuffix(" mm");
	CPageMarginLayout->addWidget(REPage);

	GBPageSpineMargin = new QxtGroupBox(tr("Page Spine Margins"), this);
	GBPageSpineMargin->setChecked(false);
	MLayout->addWidget(GBPageSpineMargin);
	CSpineMarginLayout = new QHBoxLayout(GBPageSpineMargin);
	REPageSpine = new STRectEditWidget(this);
	REPageSpine->setEditedRect(QRectF(0,0,0,0));
	REPageSpine->setSuffix(" mm");
	CSpineMarginLayout->addWidget(REPageSpine);


	return Widget;
}

void MetaInfoWidget::loadTemplatesInfo(const SPhotoBook::TemplateInfoList& _TInfoList)
{
	SPhotoBook::TemplateInfoList::const_iterator it;
	QStringList NewItems;
	for (it = _TInfoList.begin(); it != _TInfoList.end(); ++it)
	{
		TemplateInfo CurrTInfo = *it;
		if (!NewItems.contains(CurrTInfo.name()))
			NewItems.push_back(CurrTInfo.name());
	}
	CBName->addItems(NewItems);
}

MetaInfoWidget::MetaInfoWidget(QWidget *parent) :
    QWidget(parent)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this);

	ToolBar = new QToolBar(this);
	ToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolBar->setIconSize(QSize(32,32));
	MLayout->addWidget(ToolBar);

	createActions();

	QTabWidget* TabWidget = new QTabWidget(this);
	MLayout->addWidget(TabWidget);

	TabWidget->addTab(createGeneralWidget(), QIcon(":/photobook/general.png"), tr("General"));
	TabWidget->addTab(createRenderWidget(), QIcon(":/photobook/render.png"), tr("Render info"));
	TabWidget->addTab(createBehaviorWidget(), QIcon(":/photobook/behavior.png"), tr("Behavior info"));
	TabWidget->addTab(createUIWidget(), QIcon(":/photobook/user-interface.png"), tr("User interface"));

	//Load template info
	TInfoList = SystemTemplates::load();
	loadTemplatesInfo(TInfoList);

	setMinimumWidth(600);
}

void MetaInfoWidget::setMetaInfo(const MetaInfo& _MetaInfo)
{
	//General
	CBName->lineEdit()->setText(_MetaInfo.name());
	CBDesignName->lineEdit()->setText(_MetaInfo.designName());
	ISImage->setPixmapFileName(_MetaInfo.imagePath());
	TEDescription->setText(_MetaInfo.description());
	int Index = CBType->findData(_MetaInfo.templateType());
	if ( Index != -1)
		CBType->setCurrentIndex(Index);

	SBWidth->setValue(_MetaInfo.size().width());
	SBHeight->setValue(_MetaInfo.size().height());
	TESizeAlias->setText(_MetaInfo.sizeAlias());
	SBVersion->setValue(_MetaInfo.version());
	CBCyphered->setChecked(_MetaInfo.cyphered());

	//Render
	SBPPageWidth->setValue(_MetaInfo.printPageSize().width());
	SBPPageHeight->setValue(_MetaInfo.printPageSize().height());
	SBDpis->setValue(_MetaInfo.dpis());
        SBOverlayMargin->setValue(_MetaInfo.overlayMargin());

	Index = CBPreProcType->findData(_MetaInfo.printPreprocessType());
	if ( Index != -1)
		CBPreProcType->setCurrentIndex(Index);

	//Behavior
	SBModPages->setValue(_MetaInfo.modPages());
	SBMaxPages->setValue(_MetaInfo.maxPages());
	SBMinPages->setValue(_MetaInfo.minPages());
	CBMultiPhoto->setChecked(_MetaInfo.multiPhoto());
	CBPreferMinPages->setChecked(_MetaInfo.preferMinPages());
	CBAtomic->setChecked(_MetaInfo.atomic());
	SBOptImagesPerPage->setValue(_MetaInfo.numOptimalImagesPerPage());
	CBAutoGenerate->setChecked(_MetaInfo.autogenerateLayouts());

	CBSnapToBounds->setChecked(_MetaInfo.snapToBounds());
	CBExpandImagesToFillFrames->setChecked(_MetaInfo.expandImagesToFillFrames());
	CBAutodetectImageOrientation->setChecked(_MetaInfo.autodetectImageOrientation());
	CBAutoAdjustFrames->setChecked(_MetaInfo.autoAdjustFrames());
	UseImagesAsBackgrounds->setChecked(_MetaInfo.useImagesAsBackGrounds());
	SBFixedOutMargin->setValue(_MetaInfo.fixedDotMargin());

	//GUI
	QRectF CoverMarginRect = _MetaInfo.coverMarginRect();
	GBCoverMargin->setChecked(!CoverMarginRect.isNull());
	RECover->setEditedRect(CoverMarginRect);

	QRectF SpineMarginRect = _MetaInfo.spineMarginRect();
	GBCoverSpineMargin->setChecked(!SpineMarginRect.isNull());
	RECoverSpine->setEditedRect(SpineMarginRect);

	QRectF PageMarginRect = _MetaInfo.pageMarginRect();
	GBPageMargin->setChecked(!PageMarginRect.isNull());
	REPage->setEditedRect(PageMarginRect);

	QRectF PageSpineMarginRect = _MetaInfo.pageSpineMarginRect();
	GBPageSpineMargin->setChecked(!PageSpineMarginRect.isNull());
	REPageSpine->setEditedRect(PageSpineMarginRect);


}

MetaInfo MetaInfoWidget::metaInfo() const
{
	MetaInfo Res;

	Res.setName(CBName->currentText());
	Res.setDesignName(CBDesignName->currentText());
	Res.setImagePath(ISImage->pixmapFileName());
	Res.setDescription(TEDescription->toHtml());
	Res.setTemplateType(static_cast<MetaInfo::EnTemplateType>(CBType->itemData(CBType->currentIndex()).toInt()));
	Res.setSize(QSizeF(SBWidth->value(), SBHeight->value()));
	Res.setSizeAlias(TESizeAlias->text());
	Res.setVersion(SBVersion->value());
	Res.setCyphered(CBCyphered->isChecked());

	//Render
	Res.setPrintPageSize(QSizeF(SBPPageWidth->value(), SBPPageHeight->value()));
	Res.setDpis(SBDpis->value());
        Res.setOverlayMargin(SBOverlayMargin->value());
	Res.setPrintPreprocessType(static_cast<RenderSettings::EnPrintPreProcessType>(CBPreProcType->itemData(CBPreProcType->currentIndex()).toInt()));

	//Behavior
	Res.setModPages(SBModPages->value());
	Res.setMaxPages(SBMaxPages->value());
	Res.setMinPages(SBMinPages->value());
	Res.setMultiPhoto(CBMultiPhoto->isChecked());
	Res.setPreferMinPages(CBPreferMinPages->isChecked());
	Res.setAtomic(CBAtomic->isChecked());
	Res.setNumOptimalImagesPerPage(SBOptImagesPerPage->value());
	Res.setAutogenerateLayouts(CBAutoGenerate->isChecked());

	Res.setSnapToBounds(CBSnapToBounds->isChecked());
	Res.setExpandImagesToFillFrames(CBExpandImagesToFillFrames->isChecked());
	Res.setAutodetectImageOrientation(CBAutodetectImageOrientation->isChecked());
	Res.setAutoAdjustFrames(CBAutoAdjustFrames->isChecked());
	Res.setUseImagesAsBackgrounds(UseImagesAsBackgrounds->isChecked());
	Res.setFixedDotMargin(SBFixedOutMargin->value());

	//GUI
	if (GBCoverMargin->isChecked())
		Res.setCoverMarginRect(RECover->editedRect());
	if (GBCoverSpineMargin->isChecked())
		Res.setSpineMarginRect(RECoverSpine->editedRect());
	if (GBPageMargin->isChecked())
		Res.setPageMarginRect(REPage->editedRect());
	if (GBPageSpineMargin->isChecked())
		Res.setPageSpineMarginRect(REPageSpine->editedRect());

	return Res;
}

void MetaInfoWidget::slotNameActivated(const QString& _TemplateName)
{
	CBDesignName->clear();
	QStringList DesignList;
	TemplateInfoList::const_iterator it;
	for (it = TInfoList.begin(); it != TInfoList.end(); ++it)
	{
		TemplateInfo CurrTemplate = *it;
		if (CurrTemplate.name() == _TemplateName)
		{
			SPhotoBook::DesignInfoList DesignsList = CurrTemplate.designs();
			SPhotoBook::DesignInfoList::const_iterator dit;
			for (dit = DesignsList.begin(); dit != DesignsList.end(); ++dit)
			{
				if (!DesignList.contains(dit->name()))
					DesignList.push_back(dit->name());
			}
		}
	}
	CBDesignName->addItems(DesignList);
	if (CBDesignName->count() == 0)
		CBDesignName->lineEdit()->setText(tr("noname"));

}
