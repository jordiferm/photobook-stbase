/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-08-21
 * Description : Editor tool settings template box
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "editortoolsettings.h"

// Qt includes.

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QMap>
#include <QPair>
#include <QString>
#include <QToolButton>
#include <QVariant>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QDesktopWidget> 


// LibKDcraw includes.
//TODO: RAW support
//#include <libkdcraw/rnuminput.h>


// ST includes.
#include "stcolorbutton.h"

// Local includes.

#include "colorgradientwidget.h"
#include "histogramwidget.h"
#include "histogrambox.h"
#include "imagepaniconwidget.h"


#ifdef ST_RAWSUPPORT
using namespace KDcrawIface;
#endif

namespace Digikam
{

	class EditorToolSettingsPriv
	{

	public:

		EditorToolSettingsPriv()
		{
			okBtn           = 0;
			cancelBtn       = 0;
			tryBtn          = 0;
			defaultBtn      = 0;
			settingsArea    = 0;
			plainPage       = 0;
			btnBox1         = 0;
			btnBox2         = 0;
			saveAsBtn       = 0;
			loadBtn         = 0;
			guideBox        = 0;
			guideColorBt    = 0;
			guideSize       = 0;
			panIconView     = 0;
			channelCB       = 0;
			colorsCB        = 0;
			scaleBG         = 0;
			histogramBox    = 0;
			hGradient       = 0;
		}

		QButtonGroup        *scaleBG;

		QToolButton         *linHistoButton;
		QToolButton         *logHistoButton;

		QWidget             *settingsArea;
		QWidget             *plainPage;

		QWidget             *btnBox1;
		QWidget             *btnBox2;
		QWidget             *guideBox;

		QComboBox           *channelCB;
		QComboBox           *colorsCB;

		QPushButton         *okBtn;
		QPushButton         *cancelBtn;
		QPushButton         *tryBtn;
		QPushButton         *defaultBtn;
		QPushButton         *saveAsBtn;
		QPushButton         *loadBtn;

		STColorButton       *guideColorBt;

		ColorGradientWidget *hGradient;

		ImagePanIconWidget  *panIconView;

		HistogramBox        *histogramBox;

		QSpinBox        *guideSize;
	};

	EditorToolSettings::EditorToolSettings(int buttonMask, int toolMask, int histogramType, QWidget *parent)
			: QScrollArea(parent), d(new EditorToolSettingsPriv)
	{
		setFrameStyle(QFrame::NoFrame);
		setWidgetResizable(true);

		d->settingsArea = new QWidget;
		setWidget(d->settingsArea);

		// ---------------------------------------------------------------

		QGridLayout* gridSettings = new QGridLayout(d->settingsArea);

		d->plainPage    = new QWidget(d->settingsArea);
		d->guideBox     = new QWidget(d->settingsArea);
		QHBoxLayout* NLayout = new QHBoxLayout(d->guideBox); 
		d->btnBox1      = new QWidget(d->settingsArea);
		NLayout = new QHBoxLayout(d->btnBox1); 
		d->btnBox2      = new QWidget(d->settingsArea);
		NLayout = new QHBoxLayout(d->btnBox2); 
		d->histogramBox = new HistogramBox(d->settingsArea, histogramType);

		// ---------------------------------------------------------------

		QFrame *frame     = new QFrame(d->settingsArea);
		frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		QVBoxLayout* vlay = new QVBoxLayout(frame);
		d->panIconView    = new ImagePanIconWidget(360, 240, frame);
		d->panIconView->setWhatsThis(tr("<p>Here you can see the original image panel "
		                                  "which can help you to select the clip preview.</p>"
		                                  "<p>Click and drag the mouse cursor in the "
		                                  "red rectangle to change the clip focus.</p>"));
		vlay->addWidget(d->panIconView, 0, Qt::AlignCenter);
		vlay->setSpacing(0);
		vlay->setMargin(5);

		if (!(toolMask & PanIcon))
			frame->hide();

		// ---------------------------------------------------------------

		QLabel* GLabel = new QLabel(tr("Guide:"), d->guideBox);
		d->guideBox->layout()->addWidget(GLabel); 
		QLabel *space4  = new QLabel(d->guideBox);
		d->guideBox->layout()->addWidget(space4); 
		d->guideColorBt = new STColorButton(d->guideBox);
		d->guideColorBt->setColor(Qt::red); 
		d->guideBox->layout()->addWidget(d->guideColorBt); 
		d->guideColorBt->setWhatsThis(tr("Set here the color used to draw guides dashed-lines."));
		d->guideSize    = new QSpinBox(d->guideBox);
		d->guideBox->layout()->addWidget(d->guideSize); 
	
		d->guideSize->setRange(1, 51);
		//d->guideSize->setSliderEnabled(true);
		//d->guideSize->setDefaultValue(1);
		d->guideSize->setValue(1); 
		d->guideSize->setWhatsThis(tr("Set here the width in pixels used to draw guides dashed-lines."));

		static_cast<QBoxLayout*>(d->guideBox->layout())->setStretchFactor(space4, 10);
		d->guideBox->layout()->setSpacing(spacingHint());
		d->guideBox->layout()->setMargin(0);

		if (!(toolMask & ColorGuide))
			d->guideBox->hide();

		// ---------------------------------------------------------------

		d->defaultBtn = new QPushButton(d->btnBox1);
		d->defaultBtn->setText(tr("Defa&ults")); 
		d->btnBox1->layout()->addWidget(d->defaultBtn); 
		//d->defaultBtn->setGuiItem(KStandardGuiItem::defaults());
		d->defaultBtn->setIcon(QIcon(":/digikam/document-revert.png"));
		d->defaultBtn->setToolTip(tr("Reset all settings to their default values."));
		if (!(buttonMask & Default))
			d->defaultBtn->hide();

		QLabel *space2 = new QLabel(d->btnBox1);

		d->okBtn = new QPushButton(d->btnBox1);
		d->btnBox1->layout()->addWidget(d->okBtn); 
		d->okBtn->setText(tr("&Ok")); 
		d->okBtn->setIcon(QIcon(":/digikam/dialog-ok.png")); 
		//d->okBtn->setGuiItem(KStandardGuiItem::ok());
		if (!(buttonMask & Ok))
			d->okBtn->hide();

		d->cancelBtn = new QPushButton(d->btnBox1);
		d->cancelBtn->setText(tr("&Cancel")); 
		d->cancelBtn->setIcon(QIcon(":/digikam/dialog-cancel.png")); 
		d->btnBox1->layout()->addWidget(d->cancelBtn); 
		//d->cancelBtn->setGuiItem(KStandardGuiItem::cancel());
		if (!(buttonMask & Cancel))
			d->cancelBtn->hide();

		static_cast<QBoxLayout*>(d->btnBox1->layout())->setStretchFactor(space2, 10);
		d->btnBox1->layout()->setSpacing(spacingHint());
		d->btnBox1->layout()->setMargin(0);

		if (!(buttonMask & Default) && !(buttonMask & Ok) && !(buttonMask & Cancel))
			d->btnBox1->hide();

		// ---------------------------------------------------------------

		d->loadBtn = new QPushButton(d->btnBox2);
		d->btnBox2->layout()->addWidget(d->loadBtn);
		//d->loadBtn->setGuiItem(KStandardGuiItem::open());
		d->loadBtn->setText(tr("Load..."));
		d->loadBtn->setIcon(QIcon(":/digikam/load.png"));
		d->loadBtn->setToolTip(tr("Load all parameters from settings text file."));
		if (!(buttonMask & Load))
			d->loadBtn->hide();

		d->saveAsBtn = new QPushButton(d->btnBox2);
		d->btnBox2->layout()->addWidget(d->saveAsBtn);
		d->saveAsBtn->setText(tr("Save as"));
		d->saveAsBtn->setIcon(QIcon(":/digikam/saveas.png"));
		//d->saveAsBtn->setGuiItem(KStandardGuiItem::saveAs());
		d->saveAsBtn->setToolTip(tr("Save all parameters to settings text file."));
		if (!(buttonMask & SaveAs))
			d->saveAsBtn->hide();

		QLabel *space3 = new QLabel(d->btnBox2);
		d->btnBox2->layout()->addWidget(space3); 
		d->tryBtn = new QPushButton(d->btnBox2);
		d->btnBox2->layout()->addWidget(d->tryBtn); 
		//d->tryBtn->setGuiItem(KStandardGuiItem::apply());
		d->tryBtn->setText(tr("Try"));
		d->tryBtn->setIcon(QIcon(":/digikam/dialog-apply.png")); 
		d->tryBtn->setToolTip(tr("Try all settings."));
		if (!(buttonMask & Try))
			d->tryBtn->hide();

		static_cast<QBoxLayout*>(d->btnBox2->layout())->setStretchFactor(space3, 10);
		d->btnBox2->layout()->setSpacing(spacingHint());
		d->btnBox2->layout()->setMargin(0);

		if (!(buttonMask & Load) && !(buttonMask & SaveAs) && !(buttonMask & Try))
			d->btnBox2->hide();

		// ---------------------------------------------------------------

		if (!(toolMask & Histogram))
			d->histogramBox->hide();

		// ---------------------------------------------------------------

		gridSettings->addWidget(d->histogramBox,  0, 0, 2, 2);
		gridSettings->addWidget(frame,            2, 0, 1, 2);
		gridSettings->addWidget(d->plainPage,     3, 0, 1, 2);
		gridSettings->addWidget(d->guideBox,      4, 0, 1, 2);
		gridSettings->addWidget(d->btnBox2,       5, 0, 1, 2);
		gridSettings->addWidget(d->btnBox1,       6, 0, 1, 2);
		gridSettings->setSpacing(spacingHint());
		gridSettings->setMargin(0);

		// ---------------------------------------------------------------

		connect(d->okBtn, SIGNAL(clicked()),
		        this, SIGNAL(signalOkClicked()));

		connect(d->cancelBtn, SIGNAL(clicked()),
		        this, SIGNAL(signalCancelClicked()));

		connect(d->tryBtn, SIGNAL(clicked()),
		        this, SIGNAL(signalTryClicked()));

		connect(d->defaultBtn, SIGNAL(clicked()),
		        this, SIGNAL(signalDefaultClicked()));

		connect(d->saveAsBtn, SIGNAL(clicked()),
		        this, SIGNAL(signalSaveAsClicked()));

		connect(d->loadBtn, SIGNAL(clicked()),
		        this, SIGNAL(signalLoadClicked()));

		connect(d->guideColorBt, SIGNAL(changed(const QColor&)),
		        this, SIGNAL(signalColorGuideChanged()));

		connect(d->guideSize, SIGNAL(valueChanged(int)),
		        this, SIGNAL(signalColorGuideChanged()));

		connect(d->histogramBox, SIGNAL(signalChannelChanged()),
		        this, SIGNAL(signalChannelChanged()));

		connect(d->histogramBox, SIGNAL(signalColorsChanged()),
		        this, SIGNAL(signalColorsChanged()));

		connect(d->histogramBox, SIGNAL(signalScaleChanged()),
		        this, SIGNAL(signalScaleChanged()));
	}

	EditorToolSettings::~EditorToolSettings()
	{
		delete d;
	}

	QSize EditorToolSettings::minimumSizeHint() const
	{
		// Editor Tools usually require a larger horizontal space than other widgets in right side bar
		// Set scroll area to a horizontal minimum size sufficient for the settings.
		// Do not touch vertical size hint.
		// Limit to 40% of the desktop width.
		QSize hint = QScrollArea::minimumSizeHint();
		//QRect desktopRect = KGlobalSettings::desktopGeometry(d->settingsArea);
		QDesktopWidget Desktop; 
		QRect desktopRect = Desktop.screenGeometry(d->settingsArea); 
		hint.setWidth(qMin(d->settingsArea->minimumSizeHint().width(), desktopRect.width() * 2 / 5));
		return hint;
	}

	int EditorToolSettings::marginHint()
	{
		//return KDialog::marginHint();
		return 1;
	}

	int EditorToolSettings::spacingHint()
	{
		//return KDialog::spacingHint();
		return 1; 
	}

	QWidget *EditorToolSettings::plainPage() const
	{
		return d->plainPage;
	}

	ImagePanIconWidget* EditorToolSettings::panIconView() const
	{
		return d->panIconView;
	}

	HistogramBox* EditorToolSettings::histogramBox() const
	{
		return d->histogramBox;
	}

	QPushButton* EditorToolSettings::button(int buttonCode) const
	{
		if (buttonCode & Default)
			return d->defaultBtn;

		if (buttonCode & Try)
			return d->tryBtn;

		if (buttonCode & Ok)
			return d->okBtn;

		if (buttonCode & Cancel)
			return d->cancelBtn;

		if (buttonCode & Load)
			return d->loadBtn;

		if (buttonCode & SaveAs)
			return d->saveAsBtn;

		return 0;
	}

	void EditorToolSettings::enableButton(int buttonCode, bool state)
	{
		QPushButton *btn = button(buttonCode);
		if (btn) btn->setEnabled(state);
	}

	QColor EditorToolSettings::guideColor() const
	{
		return d->guideColorBt->color();
	}

	void EditorToolSettings::setGuideColor(const QColor& color)
	{
		d->guideColorBt->setColor(color);
	}

	int EditorToolSettings::guideSize() const
	{
		return d->guideSize->value();
	}

	void EditorToolSettings::setGuideSize(int size)
	{
		d->guideSize->setValue(size);
	}

} // namespace Digikam
