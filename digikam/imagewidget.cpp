/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-01
 * Description : a widget to display an image preview with some
 *               modes to compare effect results.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "imagewidget.h"

// Qt includes.

#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QLayout>
#include <QPixmap>
#include <QToolButton>
#include <QVBoxLayout>
#include <QLabel> 

// KDE includes.

// #include <kdebug.h>
// #include <kapplication.h>
// #include <kconfig.h>
// #include <kdialog.h>
// #include <kglobal.h>
// #include <kiconloader.h>
// #include <klocale.h>
// #include <ksqueezedtextlabel.h>
// #include <kstandarddirs.h>

namespace Digikam
{

	class ImageWidgetPriv
	{
	public:

		ImageWidgetPriv()
		{
			spotInfoLabel       = 0;
			previewButtons      = 0;
			underExposureButton = 0;
			overExposureButton  = 0;
			previewWidget       = 0;
			prevBBox            = 0;
			expoBBox            = 0;
		}

		QWidget            *prevBBox;
		QWidget            *expoBBox;

		QString             settingsSection;

		QButtonGroup       *previewButtons;

		QToolButton        *underExposureButton;
		QToolButton        *overExposureButton;

		QLabel					*spotInfoLabel;

		ImageGuideWidget   *previewWidget;
	};

	ImageWidget::ImageWidget(const QString& settingsSection, QWidget *parent,
	                         const QString& previewWhatsThis, bool prevModeOptions,
	                         int guideMode, bool guideVisible, bool useImageSelection)
			: QWidget(parent), d(new ImageWidgetPriv)
	{
		d->settingsSection = settingsSection;

		// -------------------------------------------------------------

		QGridLayout* grid = new QGridLayout(this);
		d->spotInfoLabel  = new QLabel(this);
		d->spotInfoLabel->setAlignment(Qt::AlignRight);

		// -------------------------------------------------------------

		d->prevBBox       = new QWidget(this);
		QHBoxLayout *hlay = new QHBoxLayout(d->prevBBox);
		d->previewButtons = new QButtonGroup(d->prevBBox);
		d->previewButtons->setExclusive(true);
		hlay->setSpacing(0);
		hlay->setMargin(0);

		QToolButton *previewOriginalButton = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewOriginalButton, ImageGuideWidget::PreviewOriginalImage);
		hlay->addWidget(previewOriginalButton);
		previewOriginalButton->setIcon(QPixmap(":/digikam/original.png"));
		previewOriginalButton->setCheckable(true);
		previewOriginalButton->setWhatsThis(tr("If you enable this option, you will see "
		                                    "the original image."));

		QToolButton *previewBothButtonVert = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewBothButtonVert, ImageGuideWidget::PreviewBothImagesVertCont);
		hlay->addWidget(previewBothButtonVert);
		previewBothButtonVert->setIcon(QPixmap(":/digikam/bothvert.png"));
		previewBothButtonVert->setCheckable(true);
		previewBothButtonVert->setWhatsThis(tr("If you enable this option, the preview area will "
		                                    "split vertically. "
		                                    "A contiguous area of the image will be shown, "
		                                    "with one half from the original image, "
		                                    "the other half from the target image."));

		QToolButton *previewBothButtonHorz = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewBothButtonHorz, ImageGuideWidget::PreviewBothImagesHorzCont);
		hlay->addWidget(previewBothButtonHorz);
		previewBothButtonHorz->setIcon(QPixmap(":/digikam/bothhorz.png"));
		previewBothButtonHorz->setCheckable(true);
		previewBothButtonHorz->setWhatsThis(tr("If you enable this option, the preview area will "
		                                    "split horizontally. "
		                                    "A contiguous area of the image will be shown, "
		                                    "with one half from the original image, "
		                                    "the other half from the target image."));

		QToolButton *previewDuplicateBothButtonVert = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewDuplicateBothButtonVert, ImageGuideWidget::PreviewBothImagesVert);
		hlay->addWidget(previewDuplicateBothButtonVert);
		previewDuplicateBothButtonVert->setIcon(QPixmap(":/digikam/duplicatebothvert.png"));
		previewDuplicateBothButtonVert->setCheckable(true);
		previewDuplicateBothButtonVert->setWhatsThis(tr("If you enable this option, the preview area will "
		        "split vertically. "
		        "The same part of the original and the target image "
		        "will be shown side by side."));

		QToolButton *previewDupplicateBothButtonHorz = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewDupplicateBothButtonHorz, ImageGuideWidget::PreviewBothImagesHorz);
		hlay->addWidget(previewDupplicateBothButtonHorz);
		previewDupplicateBothButtonHorz->setIcon(QPixmap(":/digikam/duplicatebothhorz.png"));
		previewDupplicateBothButtonHorz->setCheckable(true);
		previewDupplicateBothButtonHorz->setWhatsThis(tr("If you enable this option, the preview area will "
		        "split horizontally. "
		        "The same part of the original and the target image "
		        "will be shown side by side."));

		QToolButton *previewtargetButton = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewtargetButton, ImageGuideWidget::PreviewTargetImage);
		hlay->addWidget(previewtargetButton);
		previewtargetButton->setIcon(QPixmap(":/digikam/target.png"));
		previewtargetButton->setCheckable(true);
		previewtargetButton->setWhatsThis(tr("If you enable this option, you will see "
		                                       "the target image."));

		QToolButton *previewToggleMouseOverButton = new QToolButton(d->prevBBox);
		d->previewButtons->addButton(previewToggleMouseOverButton, ImageGuideWidget::PreviewToggleOnMouseOver);
		hlay->addWidget(previewToggleMouseOverButton);
		previewToggleMouseOverButton->setIcon(QPixmap(":/digikam/togglemouseover.png"));
		previewToggleMouseOverButton->setCheckable(true);
		previewToggleMouseOverButton->setWhatsThis(tr("If you enable this option, you will see "
		        "the original image when the mouse is over image area, "
		        "else the target image."));

		// -------------------------------------------------------------

		d->expoBBox                   = new QWidget(this);
		QHBoxLayout *hlay2            = new QHBoxLayout(d->expoBBox);
		QButtonGroup *exposureButtons = new QButtonGroup(d->expoBBox);
		hlay2->setSpacing(0);
		hlay2->setMargin(0);

		d->underExposureButton = new QToolButton(d->expoBBox);
		exposureButtons->addButton(d->underExposureButton, UnderExposure);
		hlay2->addWidget(d->underExposureButton);
		d->underExposureButton->setIcon(QIcon(":/digikam/underexposure.png"));
		d->underExposureButton->setCheckable(true);
		d->underExposureButton->setWhatsThis(tr("Set this option to display black "
		                                     "overlaid on the preview. This will help you to avoid "
		                                     "under-exposing the image."));

		d->overExposureButton = new QToolButton(d->expoBBox);
		exposureButtons->addButton(d->overExposureButton, OverExposure);
		hlay2->addWidget(d->overExposureButton);
		d->overExposureButton->setIcon(QIcon(":/digikam/overexposure.png"));
		d->overExposureButton->setCheckable(true);
		d->overExposureButton->setWhatsThis(tr("Set this option to display white "
		                                    "overlaid on the preview. This will help you to avoid "
		                                    "over-exposing the image."));

		// -------------------------------------------------------------

		QFrame *frame    = new QFrame(this);
		frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		QVBoxLayout* l   = new QVBoxLayout(frame);
		l->setMargin(5);
		l->setSpacing(0);
		d->previewWidget = new ImageGuideWidget(480, 320, frame, guideVisible,
		                                        guideMode, Qt::red, 1, false,
		                                        useImageSelection);
		d->previewWidget->setWhatsThis(previewWhatsThis);
		l->addWidget(d->previewWidget, 0);

		// -------------------------------------------------------------

		grid->addWidget(d->prevBBox,        1, 0, 1, 1);
		grid->addWidget(d->spotInfoLabel,   1, 1, 1, 1);
		grid->addWidget(d->expoBBox,        1, 3, 1, 1);
		grid->addWidget(frame,              3, 0, 1, 4);
		grid->setColumnMinimumWidth(2, 1);
		grid->setColumnMinimumWidth(1, 1);
		grid->setRowMinimumHeight(0, 1);
		grid->setRowMinimumHeight(2, 1);
		grid->setRowStretch(3, 10);
		grid->setColumnStretch(1, 10);
		grid->setSpacing(0);
		grid->setMargin(0);

		// -------------------------------------------------------------

		connect(d->previewWidget, SIGNAL(signalResized()),
		        this, SIGNAL(signalResized()));

		connect(d->previewWidget, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor&, const QPoint&)),
		        this, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor &, const QPoint &)));

		connect(d->previewWidget, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor&, const QPoint&)),
		        this, SLOT(slotUpdateSpotInfo(const Digikam::DColor&, const QPoint&)));

		connect(d->previewWidget, SIGNAL(spotPositionChangedFromTarget(const Digikam::DColor&, const QPoint&)),
		        this, SIGNAL(spotPositionChangedFromTarget(const Digikam::DColor&, const QPoint&)));

		connect(d->previewWidget, SIGNAL(spotPositionChangedFromTarget(const Digikam::DColor&, const QPoint&)),
		        this, SLOT(slotUpdateSpotInfo(const Digikam::DColor&, const QPoint&)));

		connect(d->previewButtons, SIGNAL(buttonReleased(int)),
		        d->previewWidget, SLOT(slotChangeRenderingPreviewMode(int)));

		connect(d->underExposureButton, SIGNAL(toggled(bool)),
		        d->previewWidget, SLOT(slotToggleUnderExposure(bool)));

		connect(d->overExposureButton, SIGNAL(toggled(bool)),
		        d->previewWidget, SLOT(slotToggleOverExposure(bool)));

		// -------------------------------------------------------------

		if (prevModeOptions)
			readSettings();
		else
		{
			setRenderingPreviewMode(ImageGuideWidget::NoPreviewMode);
			d->spotInfoLabel->hide();
			d->prevBBox->hide();
			d->expoBBox->hide();
		}
	}

	ImageWidget::~ImageWidget()
	{
		writeSettings();
		delete d;
	}

	ImageIface* ImageWidget::imageIface()
	{
		return d->previewWidget->imageIface();
	}

	void ImageWidget::updatePreview()
	{
		d->previewWidget->updatePreview();
	}

	void ImageWidget::slotChangeGuideColor(const QColor &color)
	{
		d->previewWidget->slotChangeGuideColor(color);
	}

	void ImageWidget::slotChangeGuideSize(int size)
	{
		d->previewWidget->slotChangeGuideSize(size);
	}

	void ImageWidget::resetSpotPosition()
	{
		d->previewWidget->resetSpotPosition();
	}

	QPoint ImageWidget::getSpotPosition()
	{
		return (d->previewWidget->getSpotPosition());
	}

	DColor ImageWidget::getSpotColor(int getColorFrom)
	{
		return (d->previewWidget->getSpotColor(getColorFrom));
	}

	void ImageWidget::setSpotVisible(bool spotVisible, bool blink)
	{
		d->previewWidget->setSpotVisible(spotVisible, blink);
	}

	int ImageWidget::getRenderingPreviewMode()
	{
		return (d->previewWidget->getRenderingPreviewMode());
	}

	void ImageWidget::setRenderingPreviewMode(int mode)
	{
		if (d->previewButtons->button(mode))
			d->previewButtons->button(mode)->setChecked(true);

		d->previewWidget->slotChangeRenderingPreviewMode(mode);
	}

	void ImageWidget::slotUpdateSpotInfo(const Digikam::DColor &col, const QPoint &point)
	{
		DColor color = col;
		d->spotInfoLabel->setText(tr("(%1,%2) RGBA:%3,%4,%5,%6").arg(
		                               point.x()).arg(point.y()).arg(
		                               color.red()).arg(color.green()).arg(
		                               color.blue()).arg(color.alpha()));
	}

	void ImageWidget::readSettings()
	{
		//TODO Read from QSettings 
		//KSharedConfig::Ptr config = KGlobal::config();
		//KConfigGroup group = config->group(d->settingsSection);

		//d->underExposureButton->setChecked(group.readEntry("Under Exposure Indicator", false));
		//d->overExposureButton->setChecked(group.readEntry("Over Exposure Indicator", false));
		d->underExposureButton->setChecked(false);
		d->overExposureButton->setChecked(false);

		//int mode = group.readEntry("Separate View", (int)ImageGuideWidget::PreviewBothImagesVertCont);
		int mode =  (int)ImageGuideWidget::PreviewBothImagesVertCont;
		mode = qMax((int)ImageGuideWidget::PreviewOriginalImage, mode);
		mode = qMin((int)ImageGuideWidget::NoPreviewMode, mode);
		setRenderingPreviewMode(mode);
	}

	void ImageWidget::writeSettings()
	{
		//TODO: Write it with QSettings 
		/*KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group = config->group(d->settingsSection);
		group.writeEntry("Separate View", getRenderingPreviewMode());
		group.writeEntry("Under Exposure Indicator", d->underExposureButton->isChecked());
		group.writeEntry("Over Exposure Indicator", d->overExposureButton->isChecked());
		config->sync();*/
	}

}  // namespace Digikam
