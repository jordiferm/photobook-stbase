/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-06
 * Description : Black and White conversion tool.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#include "bwsepiatool.h"

// Qt includes.

#include <QColor>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QListWidget>
#include <QPixmap>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>
#include <QToolButton>
#include <QSpinBox> 
#include <QApplication> 
#include <QUrl> 
#include <QFileDialog> 
#include <QDesktopServices> 
#include <QMessageBox> 
#if QT_VERSION < 0x040400
#include <QDir> 
#endif 


// LibKDcraw includes.

#include <rnuminput.h>

// Local includes.

#include "bcgmodifier.h"
#include "colorgradientwidget.h"
#include "curveswidget.h"
#include "dimg.h"
#include "dimgimagefilters.h"
#include "editortoolsettings.h"
#include "histogramwidget.h"
#include "histogrambox.h"
#include "imagecurves.h"
#include "imagehistogram.h"
#include "imageiface.h"
#include "imagewidget.h"

using namespace KDcrawIface;
using namespace Digikam;

namespace DigikamImagesPluginCore
{

	class PreviewPixmapFactory : public QObject
	{
	public:

		PreviewPixmapFactory(BWSepiaTool* bwSepia, const QSize& previewSize);
		~PreviewPixmapFactory();

		void invalidate()
		{
			m_previewPixmapMap.clear();
		}

		const QSize previewSize()
		{
			return m_previewSize;
		};
		const QPixmap* pixmap(int id);

	private:

		QPixmap makePixmap(int id);

	private:

		QHash<int, QPixmap*> m_previewPixmapMap;
		QSize                m_previewSize;
		BWSepiaTool *m_bwSepia;
	};

	PreviewPixmapFactory::PreviewPixmapFactory(BWSepiaTool* bwSepia, const QSize& previewSize)
			: QObject(bwSepia)
	{
		m_bwSepia     = bwSepia;
		m_previewSize = previewSize;
	}

	PreviewPixmapFactory::~PreviewPixmapFactory()
	{
		// Delete all hash items
		while (!m_previewPixmapMap.isEmpty())
		{
			QPixmap *value = *m_previewPixmapMap.begin();
			m_previewPixmapMap.erase(m_previewPixmapMap.begin());
			delete value;
		}
	}

	const QPixmap* PreviewPixmapFactory::pixmap(int id)
	{
		if (m_previewPixmapMap.find(id) == m_previewPixmapMap.end())
		{
			QPixmap pix = makePixmap(id);
			m_previewPixmapMap.insert(id, new QPixmap(pix));
		}

		QPixmap* res = m_previewPixmapMap[id];

		return res;
	}

	QPixmap PreviewPixmapFactory::makePixmap(int id)
	{
		return m_bwSepia->getThumbnailForEffect(id);
	}

// -----------------------------------------------------------------------------------

	class ListWidgetBWPreviewItem : public QListWidgetItem
	{

	public:

		ListWidgetBWPreviewItem(QListWidget *parent, const QString& text,
		                        PreviewPixmapFactory* factory, int id)
				: QListWidgetItem(QIcon(QPixmap(factory->previewSize())), text, parent)
		{
			m_previewPixmapFactory = factory;
			m_id                   = id;
		};

		void updatePreview()
		{
			setIcon(QIcon(*m_previewPixmapFactory->pixmap(m_id)));
		}

	private:

		int                   m_id;
		PreviewPixmapFactory* m_previewPixmapFactory;
	};

// -----------------------------------------------------------------------------------

	BWSepiaTool::BWSepiaTool(QObject* parent)
			: EditorTool(parent)
	{
		setObjectName("convertbw");
		setToolName(tr("Black && White"));
		//setToolIcon(SmallIcon("bwtonal"));
		setToolHelp("blackandwhitetool.anchor");

		m_destinationPreviewData = 0;

		ImageIface iface(0, 0);
		m_originalImage  = iface.getOriginalImg();
		m_thumbnailImage = m_originalImage->smoothScale(128, 128, Qt::KeepAspectRatio);

		// -------------------------------------------------------------

		m_previewWidget = new ImageWidget("convertbw Tool", 0,
		                                  tr("Here you can see the black and white conversion "
		                                     "tool preview. You can pick color on image "
		                                     "to see the color level corresponding on histogram."));
		setToolView(m_previewWidget);

		// -------------------------------------------------------------

		m_gboxSettings = new EditorToolSettings(EditorToolSettings::Default |
		                                        EditorToolSettings::Load |
		                                        EditorToolSettings::SaveAs |
		                                        EditorToolSettings::Ok |
		                                        EditorToolSettings::Cancel,
		                                        EditorToolSettings::Histogram);

		QGridLayout* gridSettings = new QGridLayout(m_gboxSettings->plainPage());

		// -------------------------------------------------------------

		m_tab = new QTabWidget(m_gboxSettings->plainPage());

		m_bwFilm = new QListWidget(m_tab);
		m_bwFilm->setIconSize(m_thumbnailImage.size());
		m_previewPixmapFactory = new PreviewPixmapFactory(this, m_thumbnailImage.size());

		int type = BWGeneric;

		ListWidgetBWPreviewItem *item = new ListWidgetBWPreviewItem(m_bwFilm,
		        tr("generic black and white film",
		           "Generic"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Generic</b>:"
		                      "<p>Simulate a generic black and white film</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Agfa 200X"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Agfa 200X</b>:"
		                      "<p>Simulate the Agfa 200X black and white film at 200 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Agfa Pan 25"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Agfa Pan 25</b>:"
		                      "<p>Simulate the Agfa Pan black and white film at 25 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Agfa Pan 100"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Agfa Pan 100</b>:"
		                      "<p>Simulate the Agfa Pan black and white film at 100 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Agfa Pan 400"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Agfa Pan 400</b>:"
		                      "<p>Simulate the Agfa Pan black and white film at 400 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford Delta 100"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford Delta 100</b>:"
		                      "<p>Simulate the Ilford Delta black and white film at 100 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford Delta 400"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford Delta 400</b>:"
		                      "<p>Simulate the Ilford Delta black and white film at 400 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford Delta 400 Pro 3200"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford Delta 400 Pro 3200</b>:"
		                      "<p>Simulate the Ilford Delta 400 Pro black and white film at 3200 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford FP4 Plus"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford FP4 Plus</b>:"
		                      "<p>Simulate the Ilford FP4 Plus black and white film at 125 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford HP5 Plus"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford HP5 Plus</b>:"
		                      "<p>Simulate the Ilford HP5 Plus black and white film at 400 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford PanF Plus"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford PanF Plus</b>:"
		                      "<p>Simulate the Ilford PanF Plus black and white film at 50 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Ilford XP2 Super"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Ilford XP2 Super</b>:"
		                      "<p>Simulate the Ilford XP2 Super black and white film at 400 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Kodak Tmax 100"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Kodak Tmax 100</b>:"
		                      "<p>Simulate the Kodak Tmax black and white film at 100 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Kodak Tmax 400"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Kodak Tmax 400</b>:"
		                      "<p>Simulate the Kodak Tmax black and white film at 400 ISO</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilm, tr("Kodak TriX"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Kodak TriX</b>:"
		                      "<p>Simulate the Kodak TriX black and white film at 400 ISO</p>"));

		// -------------------------------------------------------------

		QWidget *vbox = new QWidget(m_tab);
		QVBoxLayout* VBoxLayout = new QVBoxLayout(vbox); 

		VBoxLayout->setSpacing(m_gboxSettings->spacingHint());

		m_bwFilters = new QListWidget(vbox);
		VBoxLayout->addWidget(m_bwFilters); 
		m_bwFilters->setIconSize(m_thumbnailImage.size());

		type = BWNoFilter;

		item = new ListWidgetBWPreviewItem(m_bwFilters, tr("No Lens Filter"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>No Lens Filter</b>:"
		                      "<p>Do not apply a lens filter when rendering the image.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilters, tr("Green Filter"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Green Filter</b>:"
		                      "<p>Simulate black and white film exposure using a green filter. "
		                      "This is useful for all scenic shoots, especially "
		                      "portraits photographed against the sky.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilters, tr("Orange Filter"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Orange Filter</b>:"
		                      "<p>Simulate black and white film exposure using an orange filter. "
		                      "This will enhance landscapes, marine scenes and aerial "
		                      "photography.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilters, tr("Red Filter"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Red Filter</b>:"
		                      "<p>Simulate black and white film exposure using a red filter. "
		                      "This creates dramatic sky effects, and simulates moonlight scenes "
		                      "in the daytime.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwFilters, tr("Yellow Filter"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Yellow Filter</b>:"
		                      "<p>Simulate black and white film exposure using a yellow filter. "
		                      "This has the most natural tonal correction, and improves contrast. Ideal for "
		                      "landscapes.</p>"));

		m_strengthInput = new RIntNumInput(vbox);
		VBoxLayout->addWidget(m_strengthInput); 
		m_strengthInput->setLabel(tr("Strength:"), Qt::AlignLeft | Qt::AlignVCenter);
		m_strengthInput->setRange(1, 5, 1);
		m_strengthInput->setSliderEnabled(true);
		m_strengthInput->setDefaultValue(1);
		m_strengthInput->setWhatsThis(tr("Here, set the strength adjustment of the lens filter."));

		// -------------------------------------------------------------

		m_bwTone = new QListWidget(m_tab);
		m_bwTone->setIconSize(m_thumbnailImage.size());

		type = BWNoTone;

		item = new ListWidgetBWPreviewItem(m_bwTone, tr("No Tone Filter"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>No Tone Filter</b>:"
		                      "<p>Do not apply a tone filter to the image.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwTone, tr("Sepia Tone"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Sepia Tone</b>:"
		                      "<p>Gives a warm highlight and mid-tone while adding a bit of coolness to "
		                      "the shadows - very similar to the process of bleaching a print and "
		                      "re-developing in a sepia toner.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwTone, tr("Brown Tone"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Brown Tone</b>:"
		                      "<p>This filter is more neutral than the Sepia Tone "
		                      "filter.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwTone, tr("Cold Tone"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Cold Tone</b>:"
		                      "<p>Start subtly and replicate printing on a cold tone black and white "
		                      "paper such as a bromide enlarging "
		                      "paper.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwTone, tr("Selenium Tone"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Selenium Tone</b>:"
		                      "<p>This effect replicates traditional selenium chemical toning done "
		                      "in the darkroom.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwTone, tr("Platinum Tone"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with Platinum Tone</b>:"
		                      "<p>This effect replicates traditional platinum chemical toning done "
		                      "in the darkroom.</p>"));

		++type;
		item = new ListWidgetBWPreviewItem(m_bwTone, tr("Green Tone"), m_previewPixmapFactory, type);
		item->setWhatsThis(tr("<b>Black & White with greenish tint</b>:"
		                      "<p>This effect is also known as Verdante.</p>"));

		// -------------------------------------------------------------

		QWidget *curveBox     = new QWidget(m_tab);
		QGridLayout *gridTab2 = new QGridLayout(curveBox);

		ColorGradientWidget* vGradient = new ColorGradientWidget(Qt::Vertical, 10, curveBox);
		vGradient->setColors(QColor("white"), QColor("black"));

		QLabel *spacev = new QLabel(curveBox);
		spacev->setFixedWidth(1);

		m_curvesWidget = new CurvesWidget(256, 256, m_originalImage->bits(), m_originalImage->width(),
		                                  m_originalImage->height(), m_originalImage->sixteenBit(),
		                                  curveBox);
		m_curvesWidget->setWhatsThis(tr("This is the curve adjustment of the image luminosity"));

		QLabel *spaceh = new QLabel(curveBox);
		spaceh->setFixedHeight(1);

		ColorGradientWidget *hGradient = new ColorGradientWidget(Qt::Horizontal, 10, curveBox);
		hGradient->setColors(QColor("black"), QColor("white"));

		m_cInput = new RIntNumInput(curveBox);
		m_cInput->setLabel(tr("Contrast:"), Qt::AlignLeft | Qt::AlignVCenter);
		m_cInput->setRange(-100, 100, 1);
		m_cInput->setSliderEnabled(true);
		m_cInput->setDefaultValue(0);
		m_cInput->setWhatsThis(tr("Set here the contrast adjustment of the image."));

		gridTab2->addWidget(vGradient,      0, 0, 1, 1);
		gridTab2->addWidget(spacev,         0, 1, 1, 1);
		gridTab2->addWidget(m_curvesWidget, 0, 2, 1, 1);
		gridTab2->addWidget(spaceh,         1, 2, 1, 1);
		gridTab2->addWidget(hGradient,      2, 2, 1, 1);
		gridTab2->addWidget(m_cInput,       4, 0, 1, 3);
		gridTab2->setRowMinimumHeight(3, m_gboxSettings->spacingHint());
		gridTab2->setRowStretch(5, 10);
		gridTab2->setMargin(m_gboxSettings->spacingHint());
		gridTab2->setSpacing(0);

		// -------------------------------------------------------------

		m_tab->insertTab(FilmTab,       m_bwFilm, tr("Film"));
		m_tab->insertTab(BWFiltersTab,  vbox,     tr("Lens Filters"));
		m_tab->insertTab(ToneTab,       m_bwTone, tr("Tone"));
		m_tab->insertTab(LuminosityTab, curveBox, tr("Lightness"));

		gridSettings->addWidget(m_tab, 0, 0, 1, 5);
		gridSettings->setRowStretch(0, 10);
		gridSettings->setMargin(m_gboxSettings->spacingHint());
		gridSettings->setSpacing(m_gboxSettings->spacingHint());

		setToolSettings(m_gboxSettings);
		updatePreviews();
		init();

		// -------------------------------------------------------------

		connect(m_previewWidget, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor&, const QPoint&)),
		        this, SLOT(slotSpotColorChanged(const Digikam::DColor&)));

		connect(m_previewWidget, SIGNAL(spotPositionChangedFromTarget(const Digikam::DColor&, const QPoint&)),
		        this, SLOT(slotColorSelectedFromTarget(const Digikam::DColor&)));

		connect(m_bwFilters, SIGNAL(itemSelectionChanged()),
		        this, SLOT(slotFilterSelected()));

		connect(m_strengthInput, SIGNAL(valueChanged(int)),
		        this, SLOT(slotTimer()));

		connect(m_bwFilm, SIGNAL(itemSelectionChanged()),
		        this, SLOT(slotEffect()));

		connect(m_bwTone, SIGNAL(itemSelectionChanged()),
		        this, SLOT(slotEffect()));

		connect(m_curvesWidget, SIGNAL(signalCurvesChanged()),
		        this, SLOT(slotTimer()));

		connect(m_cInput, SIGNAL(valueChanged(int)),
		        this, SLOT(slotTimer()));

		connect(m_previewWidget, SIGNAL(signalResized()),
		        this, SLOT(slotEffect()));
	}

	BWSepiaTool::~BWSepiaTool()
	{
		delete [] m_destinationPreviewData;
	}

	void BWSepiaTool::updatePreviews()
	{
		for (int i = 0 ; i < m_bwFilters->count(); i++)
		{
			ListWidgetBWPreviewItem* item = dynamic_cast<ListWidgetBWPreviewItem*>(m_bwFilters->item(i));
			item->updatePreview();
		}

		for (int i = 0 ; i < m_bwFilm->count(); i++)
		{
			ListWidgetBWPreviewItem* item = dynamic_cast<ListWidgetBWPreviewItem*>(m_bwFilm->item(i));
			item->updatePreview();
		}

		for (int i = 0 ; i < m_bwTone->count(); i++)
		{
			ListWidgetBWPreviewItem* item = dynamic_cast<ListWidgetBWPreviewItem*>(m_bwTone->item(i));
			item->updatePreview();
		}
	}

	void BWSepiaTool::slotFilterSelected()
	{
		int filter = m_bwFilters->currentRow();
		if (filter == BWNoFilter)
			m_strengthInput->setEnabled(false);
		else
			m_strengthInput->setEnabled(true);

		slotEffect();
	}

	QPixmap BWSepiaTool::getThumbnailForEffect(int type)
	{
		DImg thumb          = m_thumbnailImage.copy();
		int w               = thumb.width();
		int h               = thumb.height();
		bool sb             = thumb.sixteenBit();
		bool a              = thumb.hasAlpha();

		if (type < BWGeneric)
		{
			// In Filter view, we will render a preview of the B&W filter with the generic B&W film.
			blackAndWhiteConversion(thumb.bits(), w, h, sb, type);
			blackAndWhiteConversion(thumb.bits(), w, h, sb, BWGeneric);
		}
		else
		{
			// In Film and Tone view, we will render the preview without to use the B&W Filter
			blackAndWhiteConversion(thumb.bits(), w, h, sb, type);
		}

		if (m_curvesWidget->curves())   // in case we're called before the creator is done
		{
			uchar *targetData = new uchar[w*h*(sb ? 8 : 4)];
			m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);
			m_curvesWidget->curves()->curvesLutProcess(thumb.bits(), targetData, w, h);

			DImg preview(w, h, sb, a, targetData);
			BCGModifier cmod;
			cmod.setContrast((double)(m_cInput->value() / 100.0) + 1.00);
			cmod.applyBCG(preview);

			thumb.putImageData(preview.bits());

			delete [] targetData;
		}
		return (thumb.convertToPixmap());
	}

	void BWSepiaTool::slotScaleChanged()
	{
		m_curvesWidget->m_scaleType = m_gboxSettings->histogramBox()->scale();
		m_curvesWidget->repaint();
	}

	void BWSepiaTool::slotSpotColorChanged(const DColor &color)
	{
		m_curvesWidget->setCurveGuide(color);
	}

	void BWSepiaTool::slotColorSelectedFromTarget(const DColor &color)
	{
		m_gboxSettings->histogramBox()->histogram()->setHistogramGuideByColor(color);
	}

	void BWSepiaTool::readSettings()
	{
		//TODO: Do it with QSettings; 
		/*KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group        = config->group("convertbw Tool");

		m_tab->setCurrentIndex(group.readEntry("Settings Tab", (int)BWFiltersTab));

		m_bwFilters->setCurrentRow(group.readEntry("BW Filter", 0));
		m_bwFilm->setCurrentRow(group.readEntry("BW Film", 0));
		m_bwTone->setCurrentRow(group.readEntry("BW Tone", 0));
		m_cInput->setValue(group.readEntry("ContrastAdjustment", m_cInput->defaultValue()));
		m_strengthInput->setValue(group.readEntry("StrengthAdjustment", m_strengthInput->defaultValue()));

		for (int i = 0 ; i < 5 ; i++)
			m_curvesWidget->curves()->curvesChannelReset(i);

		m_curvesWidget->curves()->setCurveType(m_curvesWidget->m_channelType, ImageCurves::CURVE_SMOOTH);
		m_curvesWidget->reset();

		for (int j = 0 ; j < 17 ; j++)
		{
			QPoint disable(-1, -1);
			QPoint p = group.readEntry(QString("CurveAdjustmentPoint%1").arg(j), disable);

			if (m_originalImage->sixteenBit() && p.x() != -1)
			{
				p.setX(p.x()*255);
				p.setY(p.y()*255);
			}

			m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, j, p);
		}

		for (int i = 0 ; i < 5 ; i++)
			m_curvesWidget->curves()->curvesCalculateCurve(i);

		// we need to call the set methods here, otherwise the curve will not be updated correctly
		m_gboxSettings->histogramBox()->setChannel(group.readEntry("Histogram Channel",
		        (int)EditorToolSettings::LuminosityChannel));
		m_gboxSettings->histogramBox()->setScale(group.readEntry("Histogram Scale",
		        (int)CurvesWidget::LogScaleHistogram));

		slotFilterSelected();*/
	}

	void BWSepiaTool::writeSettings()
	{
		//TODO: Do it with QSettings; 
		/*KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group        = config->group("convertbw Tool");
		group.writeEntry("Settings Tab", m_tab->currentIndex());
		group.writeEntry("Histogram Channel", m_gboxSettings->histogramBox()->channel());
		group.writeEntry("Histogram Scale", m_gboxSettings->histogramBox()->scale());
		group.writeEntry("BW Filter", m_bwFilters->currentRow());
		group.writeEntry("BW Film", m_bwFilm->currentRow());
		group.writeEntry("BW Tone", m_bwTone->currentRow());
		group.writeEntry("ContrastAdjustment", m_cInput->value());
		group.writeEntry("StrengthAdjustment", m_strengthInput->value());

		for (int j = 0 ; j < 17 ; j++)
		{
			QPoint p = m_curvesWidget->curves()->getCurvePoint(ImageHistogram::ValueChannel, j);

			if (m_originalImage->sixteenBit() && p.x() != -1)
			{
				p.setX(p.x() / 255);
				p.setY(p.y() / 255);
			}

			group.writeEntry(QString("CurveAdjustmentPoint%1").arg(j), p);
		}

		m_previewWidget->writeSettings();
		group.sync();*/
	}

	void BWSepiaTool::slotResetSettings()
	{
		m_bwFilm->blockSignals(true);
		m_bwFilters->blockSignals(true);
		m_bwTone->blockSignals(true);
		m_cInput->blockSignals(true);
		m_strengthInput->blockSignals(true);

		m_bwFilters->setCurrentRow(0);
		m_bwFilm->setCurrentRow(0);
		m_bwTone->setCurrentRow(0);

		m_cInput->slotReset();
		m_strengthInput->slotReset();

		for (int channel = 0 ; channel < 5 ; channel++)
			m_curvesWidget->curves()->curvesChannelReset(channel);

		m_curvesWidget->reset();

		m_bwFilm->blockSignals(false);
		m_bwFilters->blockSignals(false);
		m_bwTone->blockSignals(false);
		m_cInput->blockSignals(false);
		m_strengthInput->blockSignals(false);

		m_gboxSettings->histogramBox()->histogram()->reset();
		m_previewPixmapFactory->invalidate();
		updatePreviews();
		m_bwFilters->update();
		m_bwTone->update();

		slotEffect();
	}

	void BWSepiaTool::slotEffect()
	{
		qApp->setOverrideCursor(Qt::WaitCursor);

		m_gboxSettings->histogramBox()->histogram()->stopHistogramComputation();

		delete [] m_destinationPreviewData;

		ImageIface* iface = m_previewWidget->imageIface();
		m_destinationPreviewData   = iface->getPreviewImage();
		int w                      = iface->previewWidth();
		int h                      = iface->previewHeight();
		bool a                     = iface->previewHasAlpha();
		bool sb                    = iface->previewSixteenBit();

		// Apply black and white filter.

		blackAndWhiteConversion(m_destinationPreviewData, w, h, sb, m_bwFilters->currentRow());

		// Apply black and white film type.

		blackAndWhiteConversion(m_destinationPreviewData, w, h, sb, m_bwFilm->currentRow() + BWGeneric);

		// Apply color tone filter.

		blackAndWhiteConversion(m_destinationPreviewData, w, h, sb, m_bwTone->currentRow() + BWNoTone);

		// Calculate and apply the curve on image.

		uchar *targetData = new uchar[w*h*(sb ? 8 : 4)];
		m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);
		m_curvesWidget->curves()->curvesLutProcess(m_destinationPreviewData, targetData, w, h);

		// Adjust contrast.

		DImg preview(w, h, sb, a, targetData);
		BCGModifier cmod;
		cmod.setContrast((double)(m_cInput->value() / 100.0) + 1.00);
		cmod.applyBCG(preview);
		iface->putPreviewImage(preview.bits());

		m_previewWidget->updatePreview();

		// Update histogram.

		memcpy(m_destinationPreviewData, preview.bits(), preview.numBytes());
		m_gboxSettings->histogramBox()->histogram()->updateData(m_destinationPreviewData, w, h, sb, 0, 0, 0, false);
		delete [] targetData;

		qApp->restoreOverrideCursor();
	}

	void BWSepiaTool::finalRendering()
	{
		qApp->setOverrideCursor(Qt::WaitCursor);
		ImageIface* iface = m_previewWidget->imageIface();
		uchar *data                = iface->getOriginalImage();
		int w                      = iface->originalWidth();
		int h                      = iface->originalHeight();
		bool a                     = iface->originalHasAlpha();
		bool sb                    = iface->originalSixteenBit();

		if (data)
		{
			// Apply black and white filter.

			blackAndWhiteConversion(data, w, h, sb, m_bwFilters->currentRow());

			// Apply black and white film type.

			blackAndWhiteConversion(data, w, h, sb, m_bwFilm->currentRow() + BWGeneric);

			// Apply color tone filter.

			blackAndWhiteConversion(data, w, h, sb, m_bwTone->currentRow() + BWNoTone);

			// Calculate and apply the curve on image.

			uchar *targetData = new uchar[w*h*(sb ? 8 : 4)];
			m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);
			m_curvesWidget->curves()->curvesLutProcess(data, targetData, w, h);

			// Adjust contrast.

			DImg img(w, h, sb, a, targetData);
			BCGModifier cmod;
			cmod.setContrast((double)(m_cInput->value() / 100.0) + 1.00);
			cmod.applyBCG(img);

			iface->putOriginalImage(tr("Convert to Black && White"), img.bits());

			delete [] data;
			delete [] targetData;
		}

		qApp->restoreOverrideCursor();
	}

	void BWSepiaTool::blackAndWhiteConversion(uchar *data, int w, int h, bool sb, int type)
	{
		// Value to multiply RGB 8 bits component of mask used by changeTonality() method.
		int mul = sb ? 255 : 1;
		DImgImageFilters filter;
		double strength = 1.0 + ((double)m_strengthInput->value() - 1.0) * (1.0 / 3.0);

		switch (type)
		{
			case BWNoFilter:
				m_redAttn   = 0.0;
				m_greenAttn = 0.0;
				m_blueAttn  = 0.0;
				break;

			case BWGreenFilter:
				m_redAttn   = -0.20 * strength;
				m_greenAttn = + 0.11 * strength;
				m_blueAttn  = + 0.09 * strength;
				break;

			case BWOrangeFilter:
				m_redAttn   = + 0.48 * strength;
				m_greenAttn = -0.37 * strength;
				m_blueAttn  = -0.11 * strength;
				break;

			case BWRedFilter:
				m_redAttn   = + 0.60 * strength;
				m_greenAttn = -0.49 * strength;
				m_blueAttn  = -0.11 * strength;
				break;

			case BWYellowFilter:
				m_redAttn   = + 0.30 * strength;
				m_greenAttn = -0.31 * strength;
				m_blueAttn  = + 0.01 * strength;
				break;

				// --------------------------------------------------------------------------------

			case BWGeneric:
			case BWNoTone:
				m_redMult   = 0.24;
				m_greenMult = 0.68;
				m_blueMult  = 0.08;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWAgfa200X:
				m_redMult   = 0.18;
				m_greenMult = 0.41;
				m_blueMult  = 0.41;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWAgfapan25:
				m_redMult   = 0.25;
				m_greenMult = 0.39;
				m_blueMult  = 0.36;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWAgfapan100:
				m_redMult   = 0.21;
				m_greenMult = 0.40;
				m_blueMult  = 0.39;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWAgfapan400:
				m_redMult   = 0.20;
				m_greenMult = 0.41;
				m_blueMult  = 0.39;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordDelta100:
				m_redMult   = 0.21;
				m_greenMult = 0.42;
				m_blueMult  = 0.37;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordDelta400:
				m_redMult   = 0.22;
				m_greenMult = 0.42;
				m_blueMult  = 0.36;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordDelta400Pro3200:
				m_redMult   = 0.31;
				m_greenMult = 0.36;
				m_blueMult  = 0.33;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordFP4:
				m_redMult   = 0.28;
				m_greenMult = 0.41;
				m_blueMult  = 0.31;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordHP5:
				m_redMult   = 0.23;
				m_greenMult = 0.37;
				m_blueMult  = 0.40;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordPanF:
				m_redMult   = 0.33;
				m_greenMult = 0.36;
				m_blueMult  = 0.31;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWIlfordXP2Super:
				m_redMult   = 0.21;
				m_greenMult = 0.42;
				m_blueMult  = 0.37;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWKodakTmax100:
				m_redMult   = 0.24;
				m_greenMult = 0.37;
				m_blueMult  = 0.39;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWKodakTmax400:
				m_redMult   = 0.27;
				m_greenMult = 0.36;
				m_blueMult  = 0.37;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

			case BWKodakTriX:
				m_redMult   = 0.25;
				m_greenMult = 0.35;
				m_blueMult  = 0.40;
				filter.channelMixerImage(data, w, h, sb, true, true,
				                         m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
				                         0.0, 1.0, 0.0,
				                         0.0, 0.0, 1.0);
				break;

				// --------------------------------------------------------------------------------

			case BWSepiaTone:
				filter.changeTonality(data, w, h, sb, 162*mul, 132*mul, 101*mul);
				break;

			case BWBrownTone:
				filter.changeTonality(data, w, h, sb, 129*mul, 115*mul, 104*mul);
				break;

			case BWColdTone:
				filter.changeTonality(data, w, h, sb, 102*mul, 109*mul, 128*mul);
				break;

			case BWSeleniumTone:
				filter.changeTonality(data, w, h, sb, 122*mul, 115*mul, 122*mul);
				break;

			case BWPlatinumTone:
				filter.changeTonality(data, w, h, sb, 115*mul, 110*mul, 106*mul);
				break;

			case BWGreenTone:
				filter.changeTonality(data, w, h, sb, 108*mul, 116*mul, 100*mul);
				break;
		}
	}

//-- Load all settings from file --------------------------------------

	void BWSepiaTool::slotLoadSettings()
	{
	#if QT_VERSION >= 0x040400
		QUrl loadFile = QFileDialog::getOpenFileName(qApp->activeWindow(), tr("Black & White Settings File to Load"),
			QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),QString("*"));
	#else
		QUrl loadFile = QFileDialog::getOpenFileName(qApp->activeWindow(), tr("Black & White Settings File to Load"),
			QDir::homePath(),QString("*"));
	#endif 
		if (loadFile.isEmpty())
			return;

		QFile file(loadFile.path());

		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&file);

			if (stream.readLine() != "# Black & White Configuration File")
			{
				QMessageBox::critical(qApp->activeWindow(), tr("Error loading file"), 
				                   tr("\"%1\" is not a Black & White settings text file.").arg(
				                      loadFile.path()));
				file.close();
				return;
			}

			m_bwFilters->blockSignals(true);
			m_bwTone->blockSignals(true);
			m_cInput->blockSignals(true);

			m_bwFilters->setCurrentRow(stream.readLine().toInt());
			m_bwTone->setCurrentRow(stream.readLine().toInt());
			m_cInput->setValue(stream.readLine().toInt());

			for (int i = 0 ; i < 5 ; i++)
				m_curvesWidget->curves()->curvesChannelReset(i);

			m_curvesWidget->curves()->setCurveType(m_curvesWidget->m_channelType, ImageCurves::CURVE_SMOOTH);
			m_curvesWidget->reset();

			for (int j = 0 ; j < 17 ; j++)
			{
				QPoint disable(-1, -1);
				QPoint p;
				p.setX(stream.readLine().toInt());
				p.setY(stream.readLine().toInt());

				if (m_originalImage->sixteenBit() && p != disable)
				{
					p.setX(p.x()*255);
					p.setY(p.y()*255);
				}

				m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, j, p);
			}

			for (int i = 0 ; i < 5 ; i++)
				m_curvesWidget->curves()->curvesCalculateCurve(i);

			m_bwFilters->blockSignals(false);
			m_bwTone->blockSignals(false);
			m_cInput->blockSignals(false);

			m_gboxSettings->histogramBox()->histogram()->reset();
			m_previewPixmapFactory->invalidate();
			updatePreviews();
			m_bwFilters->update();
			m_bwTone->update();

			slotEffect();
		}
		else
			QMessageBox::critical(qApp->activeWindow(), tr("Error loading file"), tr("Cannot load settings from the Black & White text file."));

		file.close();
	}

//-- Save all settings to file ---------------------------------------

	void BWSepiaTool::slotSaveAsSettings()
	{
	#if QT_VERSION >= 0x040400
		QUrl saveFile = QFileDialog::getSaveFileName(qApp->activeWindow(), tr("Black & White Settings File to Save") , 
											QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),QString("*"));
	#else 
		QUrl saveFile = QFileDialog::getSaveFileName(qApp->activeWindow(), tr("Black & White Settings File to Save") , 
											QDir::homePath(),QString("*"));
	#endif 
		if (saveFile.isEmpty())
			return;

		QFile file(saveFile.path());

		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&file);
			stream << "# Black & White Configuration File\n";
			stream << m_bwFilters->currentItem() << "\n";
			stream << m_bwTone->currentItem() << "\n";
			stream << m_cInput->value() << "\n";

			for (int j = 0 ; j < 17 ; j++)
			{
				QPoint p = m_curvesWidget->curves()->getCurvePoint(ImageHistogram::ValueChannel, j);
				if (m_originalImage->sixteenBit())
				{
					p.setX(p.x() / 255);
					p.setY(p.y() / 255);
				}
				stream << p.x() << "\n";
				stream << p.y() << "\n";
			}
		}
		else
			QMessageBox::critical(qApp->activeWindow(), tr("Error saving file"), 
			                   tr("Cannot save settings to the Black & White text file."));

		file.close();
	}

}  // namespace DigikamImagesPluginCore
