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

#ifndef METAINFOWIDGET_H
#define METAINFOWIDGET_H
#include <QWidget>
#include <QLineEdit>
#include "stphotobookexport.h"
#include "metainfo.h"
#include "templateinfolist.h"

class QToolBar;
class FPixmapSelector;
class QwwRichTextEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;
class QxtGroupBox;
class STRectEditWidget;
namespace SPhotoBook
{

class ST_PHOTOBOOK_EXPORT MetaInfoWidget : public QWidget
{
    Q_OBJECT

	QToolBar* ToolBar;

	//General
	QComboBox* CBName;
	QComboBox* CBDesignName;
	FPixmapSelector* ISImage;
	QwwRichTextEdit* TEDescription;
	QLineEdit* TESizeAlias;
	QComboBox* CBType;
	QSpinBox* SBWidth;
	QSpinBox* SBHeight;
	QSpinBox* SBVersion;
	QCheckBox* CBCyphered;

	//Render
	QSpinBox* SBPPageWidth;
	QSpinBox* SBPPageHeight;
	QSpinBox* SBDpis;
        QSpinBox* SBOverlayMargin;
	QComboBox* CBPreProcType;

	//Behavior
	QSpinBox* SBModPages;
	QSpinBox* SBMaxPages;
	QSpinBox* SBMinPages;
	QCheckBox* CBMultiPhoto;
	QCheckBox* CBPreferMinPages;
	QCheckBox* CBAtomic;
	QSpinBox* SBOptImagesPerPage;
	QCheckBox* CBAutoGenerate;

	QCheckBox* CBSnapToBounds;
	QCheckBox* CBExpandImagesToFillFrames;
	QCheckBox* CBAutodetectImageOrientation;
	QCheckBox* CBAutoAdjustFrames;
	QCheckBox* UseImagesAsBackgrounds;
	QSpinBox* SBFixedOutMargin;

	//GUI
	QxtGroupBox* GBCoverMargin;
	QxtGroupBox* GBCoverSpineMargin;
	QxtGroupBox* GBPageMargin;
	QxtGroupBox* GBPageSpineMargin;
	STRectEditWidget* RECover;
	STRectEditWidget* RECoverSpine;
	STRectEditWidget* REPage;
	STRectEditWidget* REPageSpine;

	TemplateInfoList TInfoList;

	void createActions();
	QWidget* createGeneralWidget();
	QWidget* createRenderWidget();
	QWidget* createBehaviorWidget();
	QWidget* createUIWidget();
	void loadTemplatesInfo(const SPhotoBook::TemplateInfoList& _TInfoList);

public:
    explicit MetaInfoWidget(QWidget *parent = 0);
	void setMetaInfo(const MetaInfo& _MetaInfo);
	MetaInfo metaInfo() const;

signals:

private slots:
	void slotNameActivated(const QString& _TemplateName);
};
}

#endif // METAINFOWIDGET_H
