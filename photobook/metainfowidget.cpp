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
#include "qwwrichtextedit.h"
#include "fpixmapselector.h"
#include "templateinfo.h"
#include "rendersettings.h"

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
	CBName->setEditable(true);
	LeftFormLayout->addRow(tr("Name"), CBName);

	CBDesignName = new QComboBox(this);
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
	CBType->addItem(tr("PhotoBook"), TemplateInfo::TypePhotoBook);
	CBType->addItem(tr("Calendar"), TemplateInfo::TypeCalendar);
	CBType->addItem(tr("Card"), TemplateInfo::TypeCard);
	CBType->addItem(tr("IdPhoto"), TemplateInfo::TypeIdPhoto);
	CBType->addItem(tr("MultiPhoto"), TemplateInfo::TypeMultiPhoto);
	RightFormLayout->addRow(tr("Type"), CBType);


	SBWidth = new QSpinBox(this);http://www.wysota.eu.org/wwwidgets/
	SBWidth->setRange(0, 9999);
	RightFormLayout->addRow(tr("Width"), SBWidth);

	SBHeight = new QSpinBox(this);
	SBHeight->setRange(0, 9999);
	RightFormLayout->addRow(tr("Height"), SBHeight);

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

	CBPreProcType = new QComboBox(this);
	CBPreProcType->addItem(tr("None"), RenderSettings::TypeNone);
	CBPreProcType->addItem(tr("BookLet"), RenderSettings::TypeBooklet);
	FormLayout->addRow(tr("Print Preprocess type"), CBPreProcType);

	CBCutPages = new QCheckBox(this);
	FormLayout->addRow(tr("Cut pages on print"), CBCutPages);

	return Widget;
}

QWidget* MetaInfoWidget::createBehaviorWidget()
{
	QWidget* Widget = new QWidget(this);
	QVBoxLayout* MLayout = new QVBoxLayout(Widget);

	QFormLayout* FormLayout = new QFormLayout;
	MLayout->addLayout(FormLayout);

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

	CBPreferMinImages = new QCheckBox(this);
	FormLayout->addRow(tr("Prefer min images"), CBPreferMinImages);

	CBAtomic = new QCheckBox(this);
	FormLayout->addRow(tr("Is Atomic"), CBAtomic);

	SBOptImagesPerPage = new QSpinBox(this);
	SBOptImagesPerPage->setRange(0, 9999);
	FormLayout->addRow(tr("Optimal images per page"), SBOptImagesPerPage);

	CBAutoGenerate = new QCheckBox(this);
	FormLayout->addRow(tr("Auto generate layouts"), CBAutoGenerate);

	return Widget;

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

	setMinimumWidth(600);
}

void MetaInfoWidget::setMetaInfo(const MetaInfo& _MetaInfo)
{
	//General
	CBName->lineEdit()->setText(_MetaInfo.name());
	CBDesignName->lineEdit()->setText(_MetaInfo.designName());
	ISImage->setPixmapFileName(_MetaInfo.imagePath());
	TEDescription->setText(_MetaInfo.description());
	if (int Index = CBType->findData(_MetaInfo.templateType()) != -1)
		CBType->setCurrentIndex(Index);

	SBWidth->setValue(_MetaInfo.size().width());
	SBHeight->setValue(_MetaInfo.size().height());
	SBVersion->setValue(_MetaInfo.version());
	CBCyphered->setChecked(_MetaInfo.cyphered());

	//Render

	SBPPageWidth->setValue(_MetaInfo.printPageSize().width());
	SBPPageHeight->setValue(_MetaInfo.printPageSize().height());
	SBDpis->setValue(_MetaInfo.dpis());

	if (int Index = CBPreProcType->findData(_MetaInfo.printPreprocessType()) != -1)
		CBPreProcType->setCurrentIndex(Index);
	CBCutPages->setChecked(_MetaInfo.cutPagesOnPrint());

	//Behavior
	SBModPages->setValue(_MetaInfo.modPages());
	SBMaxPages->setValue(_MetaInfo.maxPages());
	SBMinPages->setValue(_MetaInfo.minPages());
	CBMultiPhoto->setChecked(_MetaInfo.multiPhoto());
	CBPreferMinImages->setCheckable(_MetaInfo.preferMinPages());
	CBAtomic->setCheckable(_MetaInfo.atomic());
	SBOptImagesPerPage->setValue(_MetaInfo.numOptimalImagesPerPage());
	CBAutoGenerate->setChecked(_MetaInfo.autogenerateLayouts());

}

MetaInfo MetaInfoWidget::metaInfo() const
{
	MetaInfo Res;

	Res.setName(CBName->currentText());
	Res.setDesignName(CBDesignName->currentText());
	Res.setImagePath(ISImage->pixmapFileName());
	Res.setDescription(TEDescription->toHtml());
	Res.setTemplateType(static_cast<TemplateInfo::EnTemplateType>(CBType->itemData(CBType->currentIndex()).toInt()));
	Res.setSize(QSizeF(SBWidth->value(), SBHeight->value()));
	Res.setVersion(SBVersion->value());
	Res.setCyphered(CBCyphered->isChecked());

	//Render
	Res.setPrintPageSize(QSizeF(SBPPageWidth->value(), SBPPageHeight->value()));
	Res.setDpis(SBDpis->value());
	Res.setPrintPreprocessType(static_cast<RenderSettings::EnPrintPreProcessType>(CBPreProcType->itemData(CBType->currentIndex()).toInt()));
	Res.setCutPagesOnPrint(CBCutPages->isChecked());

	//Behavior
	Res.setModPages(SBModPages->value());
	Res.setMaxPages(SBMaxPages->value());
	Res.setMinPages(SBMinPages->value());
	Res.setMultiPhoto(CBMultiPhoto->isChecked());
	Res.setPreferMinPages(CBPreferMinImages->isChecked());
	Res.setAtomic(CBAtomic->isChecked());
	Res.setNumOptimalImagesPerPage(SBOptImagesPerPage->value());
	Res.setAutogenerateLayouts(CBAutoGenerate->isChecked());

	return Res;
}
