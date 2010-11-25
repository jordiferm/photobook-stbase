/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-11
 * Description : a digiKam image editor plugin to correct
 *               image white balance
 *
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Guillaume Castagnino <casta at xwing dot info>
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


#include "whitebalancetool.h"

// Qt includes.

#include <QButtonGroup>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QUrl> 
#include <QFileDialog> 
#include <QDesktopServices> 
#include <QMessageBox> 
#if QT_VERSION < 0x040400
#include <QDir> 
#endif 



// KDcraw includes.

#include <rnuminput.h>
#include <rcombobox.h>

// Local includes.

#include "colorgradientwidget.h"
#include "dcolor.h"
#include "dimg.h"
#include "dimgimagefilters.h"
#include "editortoolsettings.h"
#include "histogramwidget.h"
#include "histogrambox.h"
#include "imagehistogram.h"
#include "imageiface.h"
#include "imagewidget.h"
#include "version.h"
#include "whitebalance.h"

using namespace KDcrawIface;
using namespace Digikam;

namespace DigikamWhiteBalanceImagesPlugin
{

	WhiteBalanceTool::WhiteBalanceTool(QObject* parent)
			: EditorTool(parent)
	{
		setObjectName("whitebalance");
		setToolName(tr("White Balance"));
		setToolIcon(QPixmap(":/digikam/whitebalance.png"));

		m_destinationPreviewData = 0;

		// -------------------------------------------------------------

		m_previewWidget = new ImageWidget("whitebalance Tool", 0,
		                                  tr("You can see here the image's white-balance "
		                                     "adjustments preview. You can pick color on image to "
		                                     "see the color level corresponding on histogram."));
		setToolView(m_previewWidget);

		// -------------------------------------------------------------

		m_gboxSettings = new EditorToolSettings(EditorToolSettings::Default |
		                                        EditorToolSettings::Load |
		                                        EditorToolSettings::SaveAs |
		                                        EditorToolSettings::Ok |
		                                        EditorToolSettings::Cancel,
		                                        EditorToolSettings::Histogram);

		// -------------------------------------------------------------

		QGridLayout *grid = new QGridLayout(m_gboxSettings->plainPage());

		m_temperatureLabel = new QLabel(tr("<a href='http://en.wikipedia.org/wiki/Color_temperature'>"
		                                   "Color Temperature</a> (K): "), m_gboxSettings->plainPage());
		m_temperatureLabel->setOpenExternalLinks(true);

		m_adjTemperatureLabel = new QLabel(tr("Adjustment:"), m_gboxSettings->plainPage());
		m_temperatureInput    = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_temperatureInput->setDecimals(1);
		m_temperatureInput->setRange(1750.0, 12000.0, 10.0);
		m_temperatureInput->setDefaultValue(6500.0);
		m_temperatureInput->setWhatsThis(tr("Set here the white balance color temperature in Kelvin."));

		m_temperaturePresetLabel = new QLabel(tr("Preset:"), m_gboxSettings->plainPage());
		m_temperaturePresetCB    = new RComboBox(m_gboxSettings->plainPage());
		m_temperaturePresetCB->addItem(tr("Candle"));
		m_temperaturePresetCB->addItem(tr("40W Lamp"));
		m_temperaturePresetCB->addItem(tr("100W Lamp"));
		m_temperaturePresetCB->addItem(tr("200W Lamp"));
		m_temperaturePresetCB->addItem(tr("Sunrise"));
		m_temperaturePresetCB->addItem(tr("Studio Lamp"));
		m_temperaturePresetCB->addItem(tr("Moonlight"));
		m_temperaturePresetCB->addItem(tr("Neutral"));
		m_temperaturePresetCB->addItem(tr("Daylight D50"));
		m_temperaturePresetCB->addItem(tr("Photo Flash"));
		m_temperaturePresetCB->addItem(tr("Sun"));
		m_temperaturePresetCB->addItem(tr("Xenon Lamp"));
		m_temperaturePresetCB->addItem(tr("Daylight D65"));
		m_temperaturePresetCB->addItem(tr("no temperature preset"));
		m_temperaturePresetCB->setDefaultIndex(DaylightD65);
		m_temperaturePresetCB->setWhatsThis(tr("<p>Select the white balance color temperature "
		                                       "preset to use here:</p>"
		                                       "<p><b>Candle</b>: candle light (1850K).</p>"
		                                       "<p><b>40W Lamp</b>: 40 Watt incandescent lamp (2680K).</p>"
		                                       "<p><b>100W Lamp</b>: 100 Watt incandescent lamp (2800K).</p>"
		                                       "<p><b>200W Lamp</b>: 200 Watt incandescent lamp (3000K).</p>"
		                                       "<p><b>Sunrise</b>: sunrise or sunset light (3200K).</p>"
		                                       "<p><b>Studio Lamp</b>: tungsten lamp used in photo studio or "
		                                       "light at 1 hour from dusk/dawn (3400K).</p>"
		                                       "<p><b>Moonlight</b>: moon light (4100K).</p>"
		                                       "<p><b>Neutral</b>: neutral color temperature (4750K).</p>"
		                                       "<p><b>Daylight D50</b>: sunny daylight around noon (5000K).</p>"
		                                       "<p><b>Photo Flash</b>: electronic photo flash (5500K).</p>"
		                                       "<p><b>Sun</b>: effective sun temperature (5770K).</p>"
		                                       "<p><b>Xenon Lamp</b>: xenon lamp or light arc (6420K).</p>"
		                                       "<p><b>Daylight D65</b>: overcast sky light (6500K).</p>"
		                                       "<p><b>None</b>: no preset value.</p>"));
		m_pickTemperature = new QToolButton(m_gboxSettings->plainPage());
		m_pickTemperature->setIcon(QIcon(":/digikam/color_picker.png"));
		m_pickTemperature->setCheckable(true);
		m_pickTemperature->setToolTip(tr("Temperature tone color picker."));
		m_pickTemperature->setWhatsThis(tr("With this button, you can pick the color from original "
		                                   "image used to set white color balance temperature and "
		                                   "green component."));

		//KSeparator *line = new KSeparator(Qt::Horizontal, m_gboxSettings->plainPage());
		QFrame* Line = new QFrame(m_gboxSettings->plainPage()); 
		Line->setFrameShape(QFrame::HLine); 

		// -------------------------------------------------------------

		m_blackLabel = new QLabel(tr("Black point:"), m_gboxSettings->plainPage());
		m_blackInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_blackInput->setDecimals(2);
		m_blackInput->setRange(0.0, 0.05, 0.01, true);
		m_blackInput->setWhatsThis(tr("Set here the black level value."));
		m_blackInput->setDefaultValue(0.0);

		m_darkLabel = new QLabel(tr("Shadows:"), m_gboxSettings->plainPage());
		m_darkInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_darkInput->setDecimals(2);
		m_darkInput->setRange(0.0, 1.0, 0.01, true);
		m_darkInput->setDefaultValue(0.5);
		m_darkInput->setWhatsThis(tr("Set here the shadows noise suppression level."));

		m_saturationLabel = new QLabel(tr("Saturation:"), m_gboxSettings->plainPage());
		m_saturationInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_saturationInput->setDecimals(2);
		m_saturationInput->setRange(0.0, 2.0, 0.01, true);
		m_saturationInput->setDefaultValue(1.0);
		m_saturationInput->setWhatsThis(tr("Set here the saturation value."));

		m_gammaLabel = new QLabel(tr("Gamma:"), m_gboxSettings->plainPage());
		m_gammaInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_gammaInput->setDecimals(2);
		m_gammaInput->setRange(0.1, 3.0, 0.01, true);
		m_gammaInput->setDefaultValue(1.0);
		m_gammaInput->setWhatsThis(tr("Set here the gamma correction value."));

		m_greenLabel = new QLabel(tr("Green:"), m_gboxSettings->plainPage());
		m_greenInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_greenInput->setDecimals(2);
		m_greenInput->setRange(0.2, 2.5, 0.01, true);
		m_greenInput->setDefaultValue(1.0);
		m_greenInput->setWhatsThis(tr("Set here the green component to set magenta color "
		                              "cast removal level."));

		//KSeparator *line2 = new KSeparator(Qt::Horizontal, m_gboxSettings->plainPage());
		QFrame* Line2 = new QFrame(m_gboxSettings->plainPage()); 
		Line2->setFrameShape(QFrame::HLine); 

		// -------------------------------------------------------------

		m_exposureLabel = new QLabel(tr("<a href='http://en.wikipedia.org/wiki/Exposure_value'>"
		                                "Exposure Compensation</a> (E.V): "), m_gboxSettings->plainPage());
		m_exposureLabel->setOpenExternalLinks(true);

		m_mainExposureLabel  = new QLabel(tr("Main:"), m_gboxSettings->plainPage());
		m_autoAdjustExposure = new QToolButton(m_gboxSettings->plainPage());
		m_autoAdjustExposure->setIcon(QIcon(":/digikam/system_run.png"));
		m_autoAdjustExposure->setToolTip(tr("Auto exposure adjustments"));
		m_autoAdjustExposure->setWhatsThis(tr("With this button, you can automatically adjust Exposure "
		                                      "and Black Point values."));
		m_mainExposureInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_mainExposureInput->setDecimals(2);
		m_mainExposureInput->setRange(-6.0, 8.0, 0.1, true);
		m_mainExposureInput->setDefaultValue(0.0);
		m_mainExposureInput->setWhatsThis(tr("Set here the main exposure compensation value in E.V."));

		m_fineExposureLabel = new QLabel(tr("Fine:"), m_gboxSettings->plainPage());
		m_fineExposureInput = new RDoubleNumInput(m_gboxSettings->plainPage());
		m_fineExposureInput->setDecimals(2);
		m_fineExposureInput->setRange(-0.5, 0.5, 0.01, true);
		m_fineExposureInput->setDefaultValue(0.0);
		m_fineExposureInput->setWhatsThis(tr("This value in E.V will be added to main exposure "
		                                     "compensation value to set fine exposure adjustment."));

		// -------------------------------------------------------------

		grid->setMargin(m_gboxSettings->spacingHint());
		grid->setSpacing(m_gboxSettings->spacingHint());
		grid->addWidget(m_temperatureLabel,        0, 0, 1, 6);
		grid->addWidget(m_adjTemperatureLabel,     1, 0, 1, 1);
		grid->addWidget(m_pickTemperature,         1, 1, 1, 1);
		grid->addWidget(m_temperatureInput,        1, 2, 1, 4);
		grid->addWidget(m_temperaturePresetLabel,  2, 0, 1, 1);
		grid->addWidget(m_temperaturePresetCB,     2, 2, 1, 4);
		grid->addWidget(Line,                      3, 0, 1, 6);
		grid->addWidget(m_blackLabel,              4, 0, 1, 1);
		grid->addWidget(m_blackInput,              4, 1, 1, 5);
		grid->addWidget(m_darkLabel,               5, 0, 1, 1);
		grid->addWidget(m_darkInput,               5, 1, 1, 5);
		grid->addWidget(m_saturationLabel,         6, 0, 1, 1);
		grid->addWidget(m_saturationInput,         6, 1, 1, 5);
		grid->addWidget(m_gammaLabel,              7, 0, 1, 1);
		grid->addWidget(m_gammaInput,              7, 1, 1, 5);
		grid->addWidget(m_greenLabel,              8, 0, 1, 1);
		grid->addWidget(m_greenInput,              8, 1, 1, 5);
		grid->addWidget(Line2,                     9, 0, 1, 6);
		grid->addWidget(m_exposureLabel,          10, 0, 1, 6);
		grid->addWidget(m_mainExposureLabel,      11, 0, 1, 1);
		grid->addWidget(m_autoAdjustExposure,     11, 1, 1, 1);
		grid->addWidget(m_mainExposureInput,      11, 2, 1, 4);
		grid->addWidget(m_fineExposureLabel,      12, 0, 1, 2);
		grid->addWidget(m_fineExposureInput,      12, 2, 1, 4);
		grid->setRowStretch(13, 10);

		setToolSettings(m_gboxSettings);
		init();

		// -------------------------------------------------------------

		connect(m_previewWidget, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor &, const QPoint &)),
		        this, SLOT(slotColorSelectedFromOriginal(const Digikam::DColor &)));

		connect(m_previewWidget, SIGNAL(spotPositionChangedFromTarget(const Digikam::DColor &, const QPoint &)),
		        this, SLOT(slotColorSelectedFromTarget(const Digikam::DColor &)));

		connect(m_previewWidget, SIGNAL(signalResized()),
		        this, SLOT(slotEffect()));

		// -------------------------------------------------------------
		// Correction Filter Slider controls.

		connect(m_temperaturePresetCB, SIGNAL(activated(int)),
		        this, SLOT(slotTemperaturePresetChanged(int)));

		connect(m_temperatureInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTemperatureChanged(double)));

		connect(m_darkInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		connect(m_blackInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		connect(m_mainExposureInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		connect(m_fineExposureInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		connect(m_gammaInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		connect(m_saturationInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		connect(m_greenInput, SIGNAL(valueChanged(double)),
		        this, SLOT(slotTimer()));

		// -------------------------------------------------------------
		// Buttons slots.

		connect(m_autoAdjustExposure, SIGNAL(clicked()),
		        this, SLOT(slotAutoAdjustExposure()));

		connect(m_pickTemperature, SIGNAL(released()),
		        this, SLOT(slotPickerColorButtonActived()));
	}

	WhiteBalanceTool::~WhiteBalanceTool()
	{
		if (m_destinationPreviewData)
			delete [] m_destinationPreviewData;
	}

	void WhiteBalanceTool::slotTemperatureChanged(double temperature)
	{
		switch ((uint) temperature)
		{
			case 1850:
				m_temperaturePresetCB->setCurrentIndex(Candle);
				break;

			case 2680:
				m_temperaturePresetCB->setCurrentIndex(Lamp40W);
				break;

			case 2800:
				m_temperaturePresetCB->setCurrentIndex(Lamp100W);
				break;

			case 3000:
				m_temperaturePresetCB->setCurrentIndex(Lamp200W);
				break;

			case 3200:
				m_temperaturePresetCB->setCurrentIndex(Sunrise);
				break;

			case 3400:
				m_temperaturePresetCB->setCurrentIndex(StudioLamp);
				break;

			case 4100:
				m_temperaturePresetCB->setCurrentIndex(MoonLight);
				break;

			case 4750:
				m_temperaturePresetCB->setCurrentIndex(Neutral);
				break;

			case 5000:
				m_temperaturePresetCB->setCurrentIndex(DaylightD50);
				break;

			case 5500:
				m_temperaturePresetCB->setCurrentIndex(Flash);
				break;

			case 5770:
				m_temperaturePresetCB->setCurrentIndex(Sun);
				break;

			case 6420:
				m_temperaturePresetCB->setCurrentIndex(XeonLamp);
				break;

			case 6500:
				m_temperaturePresetCB->setCurrentIndex(DaylightD65);
				break;

			default:
				m_temperaturePresetCB->setCurrentIndex(None);
				break;
		}

		slotTimer();
	}

	void WhiteBalanceTool::slotTemperaturePresetChanged(int tempPreset)
	{
		switch (tempPreset)
		{
			case Candle:
				m_temperatureInput->setValue(1850.0);
				break;

			case Lamp40W:
				m_temperatureInput->setValue(2680.0);
				break;

			case Lamp100W:
				m_temperatureInput->setValue(2800.0);
				break;

			case Lamp200W:
				m_temperatureInput->setValue(3000.0);
				break;

			case Sunrise:
				m_temperatureInput->setValue(3200.0);
				break;

			case StudioLamp:
				m_temperatureInput->setValue(3400.0);
				break;

			case MoonLight:
				m_temperatureInput->setValue(4100.0);
				break;

			case Neutral:
				m_temperatureInput->setValue(4750.0);
				break;

			case DaylightD50:
				m_temperatureInput->setValue(5000.0);
				break;

			case Flash:
				m_temperatureInput->setValue(5500.0);
				break;

			case Sun:
				m_temperatureInput->setValue(5770.0);
				break;

			case XeonLamp:
				m_temperatureInput->setValue(6420.0);
				break;

			case DaylightD65:
				m_temperatureInput->setValue(6500.0);
				break;

			default: // None.
				break;
		}

		slotEffect();
	}

	void WhiteBalanceTool::slotPickerColorButtonActived()
	{
		// Save previous rendering mode and toggle to original image.
		m_currentPreviewMode = m_previewWidget->getRenderingPreviewMode();
		m_previewWidget->setRenderingPreviewMode(ImageGuideWidget::PreviewOriginalImage);
	}

	void WhiteBalanceTool::slotColorSelectedFromOriginal(const DColor &color)
	{
		if (m_pickTemperature->isChecked())
		{
			DColor dc = color;
			QColor tc          = dc.getQColor();
			double temperatureLevel, greenLevel;

			WhiteBalance::autoWBAdjustementFromColor(tc, temperatureLevel, greenLevel);

			m_temperatureInput->setValue(temperatureLevel);
			m_greenInput->setValue(greenLevel);
			m_pickTemperature->setChecked(false);
		}
		else
			return;

		// restore previous rendering mode.
		m_previewWidget->setRenderingPreviewMode(m_currentPreviewMode);

		slotEffect();
	}

	void WhiteBalanceTool::slotColorSelectedFromTarget(const DColor &color)
	{
		m_gboxSettings->histogramBox()->histogram()->setHistogramGuideByColor(color);
	}

	void WhiteBalanceTool::slotAutoAdjustExposure()
	{
		qApp->activeWindow()->setCursor(Qt::WaitCursor);

		ImageIface* iface = m_previewWidget->imageIface();
		uchar *data                = iface->getOriginalImage();
		int width                  = iface->originalWidth();
		int height                 = iface->originalHeight();
		bool sb                    = iface->originalSixteenBit();

		double blackLevel;
		double exposureLevel;

		WhiteBalance::autoExposureAdjustement(data, width, height, sb, blackLevel, exposureLevel);
		delete [] data;

		m_blackInput->setValue(blackLevel);
		m_mainExposureInput->setValue(exposureLevel);
		m_fineExposureInput->setValue(0.0);

		qApp->activeWindow()->unsetCursor();
		slotEffect();
	}

	void WhiteBalanceTool::slotEffect()
	{
		ImageIface* iface = m_previewWidget->imageIface();
		uchar *data                = iface->getPreviewImage();
		int w                      = iface->previewWidth();
		int h                      = iface->previewHeight();
		bool sb                    = iface->previewSixteenBit();

		// Create the new empty destination image data space.
		m_gboxSettings->histogramBox()->histogram()->stopHistogramComputation();

		if (m_destinationPreviewData)
			delete [] m_destinationPreviewData;

		m_destinationPreviewData = new uchar[w*h*(sb ? 8 : 4)];

		double temperature  = m_temperatureInput->value();
		double dark         = m_darkInput->value();
		double black        = m_blackInput->value();
		double mainExposure = m_mainExposureInput->value();
		double fineExposure = m_fineExposureInput->value();
		double gamma        = m_gammaInput->value();
		double saturation   = m_saturationInput->value();
		double green        = m_greenInput->value();

		WhiteBalance wbFilter(sb);
		wbFilter.whiteBalance(data, w, h, sb,
		                      black, mainExposure + fineExposure,
		                      temperature, green, dark,
		                      gamma, saturation);

		iface->putPreviewImage(data);
		m_previewWidget->updatePreview();

		// Update histogram.
		memcpy(m_destinationPreviewData, data, w*h*(sb ? 8 : 4));
		m_gboxSettings->histogramBox()->histogram()->updateData(m_destinationPreviewData, w, h, sb, 0, 0, 0, false);
		delete [] data;
	}

	void WhiteBalanceTool::finalRendering()
	{
		qApp->setOverrideCursor(Qt::WaitCursor);
		ImageIface* iface = m_previewWidget->imageIface();
		uchar *data                = iface->getOriginalImage();
		int w                      = iface->originalWidth();
		int h                      = iface->originalHeight();
		bool sb                    = iface->originalSixteenBit();

		double temperature  = m_temperatureInput->value();
		double dark         = m_darkInput->value();
		double black        = m_blackInput->value();
		double mainExposure = m_mainExposureInput->value();
		double fineExposure = m_fineExposureInput->value();
		double gamma        = m_gammaInput->value();
		double saturation   = m_saturationInput->value();
		double green        = m_greenInput->value();

		WhiteBalance wbFilter(sb);
		wbFilter.whiteBalance(data, w, h, sb,
		                      black, mainExposure + fineExposure,
		                      temperature, green, dark,
		                      gamma, saturation);

		iface->putOriginalImage(tr("White Balance"), data);
		delete [] data;
		qApp->restoreOverrideCursor();
	}

	void WhiteBalanceTool::slotResetSettings()
	{
		m_blackInput->blockSignals(true);
		m_darkInput->blockSignals(true);
		m_fineExposureInput->blockSignals(true);
		m_gammaInput->blockSignals(true);
		m_greenInput->blockSignals(true);
		m_mainExposureInput->blockSignals(true);
		m_saturationInput->blockSignals(true);
		m_temperatureInput->blockSignals(true);
		m_temperaturePresetCB->blockSignals(true);

		// Neutral color temperature settings is D65.
		m_blackInput->slotReset();
		m_darkInput->slotReset();
		m_fineExposureInput->slotReset();
		m_gammaInput->slotReset();
		m_greenInput->slotReset();
		m_mainExposureInput->slotReset();
		m_saturationInput->slotReset();
		m_temperaturePresetCB->slotReset();
		slotTemperaturePresetChanged(m_temperaturePresetCB->defaultIndex());
		m_temperatureInput->slotReset();

		m_previewWidget->resetSpotPosition();
		m_gboxSettings->histogramBox()->setChannel(EditorToolSettings::LuminosityChannel);
		m_gboxSettings->histogramBox()->histogram()->reset();

		m_blackInput->blockSignals(false);
		m_darkInput->blockSignals(false);
		m_fineExposureInput->blockSignals(false);
		m_gammaInput->blockSignals(false);
		m_greenInput->blockSignals(false);
		m_mainExposureInput->blockSignals(false);
		m_saturationInput->blockSignals(false);
		m_temperatureInput->blockSignals(false);
		m_temperaturePresetCB->blockSignals(false);

		slotEffect();
	}

	void WhiteBalanceTool::readSettings()
	{
		//TODO: Read from QSettings.
/*		KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group = config->group("whitebalance Tool");

		m_gboxSettings->histogramBox()->setChannel(group.readEntry("Histogram Channel",
		        (int)EditorToolSettings::LuminosityChannel));
		m_gboxSettings->histogramBox()->setScale(group.readEntry("Histogram Scale",
		        (int)HistogramWidget::LogScaleHistogram));

		m_blackInput->setValue(group.readEntry("Black", m_blackInput->defaultValue()));
		m_mainExposureInput->setValue(group.readEntry("MainExposure", m_mainExposureInput->defaultValue()));
		m_fineExposureInput->setValue(group.readEntry("FineExposure", m_fineExposureInput->defaultValue()));
		m_gammaInput->setValue(group.readEntry("Gamma", m_gammaInput->defaultValue()));
		m_saturationInput->setValue(group.readEntry("Saturation", m_saturationInput->defaultValue()));
		m_greenInput->setValue(group.readEntry("Green", m_greenInput->defaultValue()));
		m_temperatureInput->setValue(group.readEntry("Temperature", m_temperatureInput->defaultValue()));

		slotTemperatureChanged(m_temperatureInput->value());*/
	}

	void WhiteBalanceTool::writeSettings()
	{
		//TODO: Write to QSettings.
		/*KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group = config->group("whitebalance Tool");
		group.writeEntry("Histogram Channel", m_gboxSettings->histogramBox()->channel());
		group.writeEntry("Histogram Scale", m_gboxSettings->histogramBox()->scale());

		group.writeEntry("Dark", m_darkInput->value());
		group.writeEntry("Black", m_blackInput->value());
		group.writeEntry("MainExposure", m_mainExposureInput->value());
		group.writeEntry("FineExposure", m_fineExposureInput->value());
		group.writeEntry("Gamma", m_gammaInput->value());
		group.writeEntry("Saturation", m_saturationInput->value());
		group.writeEntry("Green", m_greenInput->value());
		group.writeEntry("Temperature", m_temperatureInput->value());
		m_previewWidget->writeSettings();
		config->sync();*/
	}

// Load all settings.
	void WhiteBalanceTool::slotLoadSettings()
	{
	#if QT_VERSION >= 0x040400
		QUrl loadWhiteBalanceFile = QFileDialog::getOpenFileName(qApp->activeWindow(), tr("White Color Balance Settings File to Load"),
			QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),QString("*"));
	#else 
		QUrl loadWhiteBalanceFile = QFileDialog::getOpenFileName(qApp->activeWindow(), tr("White Color Balance Settings File to Load"),
			QDir::homePath(),QString("*"));
	#endif 
		if (loadWhiteBalanceFile.isEmpty())
			return;

		QFile file(loadWhiteBalanceFile.path());

		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&file);

			if (stream.readLine() != "# White Color Balance Configuration File V2")
			{
				QMessageBox::critical(qApp->activeWindow(), tr("Error reading file"),
				                   tr("\"%1\" is not a White Color Balance settings text file.").arg(
				                      loadWhiteBalanceFile.path()));
				file.close();
				return;
			}

			blockSignals(true);
			m_temperatureInput->setValue(stream.readLine().toDouble());
			m_darkInput->setValue(stream.readLine().toDouble());
			m_blackInput->setValue(stream.readLine().toDouble());
			m_mainExposureInput->setValue(stream.readLine().toDouble());
			m_fineExposureInput->setValue(stream.readLine().toDouble());
			m_gammaInput->setValue(stream.readLine().toDouble());
			m_saturationInput->setValue(stream.readLine().toDouble());
			m_greenInput->setValue(stream.readLine().toDouble());
			m_gboxSettings->histogramBox()->histogram()->reset();
			blockSignals(false);
			slotEffect();
		}
		else
			QMessageBox::critical(qApp->activeWindow(), tr("Error reading file"),
			                   tr("Cannot load settings from the White Color Balance text file."));

		file.close();
	}

// Save all settings.
	void WhiteBalanceTool::slotSaveAsSettings()
	{
	#if QT_VERSION >= 0x040400
		QUrl saveWhiteBalanceFile = QFileDialog::getSaveFileName(qApp->activeWindow(), tr("White Color Balance Settings File to Save") , 
											QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),QString("*"));
	#else 
		QUrl saveWhiteBalanceFile = QFileDialog::getSaveFileName(qApp->activeWindow(), tr("White Color Balance Settings File to Save") , 
											QDir::homePath(),QString("*"));
	#endif 
		if (saveWhiteBalanceFile.isEmpty())
			return;

		QFile file(saveWhiteBalanceFile.path());

		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&file);
			stream << "# White Color Balance Configuration File V2\n";
			stream << m_temperatureInput->value() << "\n";
			stream << m_darkInput->value() << "\n";
			stream << m_blackInput->value() << "\n";
			stream << m_mainExposureInput->value() << "\n";
			stream << m_fineExposureInput->value() << "\n";
			stream << m_gammaInput->value() << "\n";
			stream << m_saturationInput->value() << "\n";
			stream << m_greenInput->value() << "\n";
		}
		else
			QMessageBox::critical(qApp->activeWindow(), tr("Error saving file"),
			                   tr("Cannot save settings to the White Color Balance text file."));

		file.close();
	}

}  // namespace DigikamWhiteBalanceImagesPlugin
