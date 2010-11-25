/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : save JPEG image options.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "jpegsettings.h"

// Qt includes.

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QString>
#include <QComboBox>
#include <QDialog>
#include <QSpinBox>


namespace Digikam
{

	class JPEGSettingsPriv
	{

	public:

		JPEGSettingsPriv()
		{
			JPEGGrid             = 0;
			labelJPEGcompression = 0;
			JPEGcompression      = 0;
			labelWarning         = 0;
			labelSubSampling     = 0;
			subSamplingCB        = 0;
		}

		QGridLayout  *JPEGGrid;

		QLabel       *labelJPEGcompression;
		QLabel       *labelWarning;
		QLabel       *labelSubSampling;

		QComboBox    *subSamplingCB;

		QSpinBox *JPEGcompression;
	};

	JPEGSettings::JPEGSettings(QWidget *parent)
			: QWidget(parent), d(new JPEGSettingsPriv)
	{
		setAttribute(Qt::WA_DeleteOnClose);

		d->JPEGGrid             = new QGridLayout(this);
		d->JPEGcompression      = new QSpinBox(this);
		d->JPEGcompression->setValue(75);
		d->JPEGcompression->setRange(1, 100);
		//d->JPEGcompression->setSliderEnabled(true);
		d->labelJPEGcompression = new QLabel(tr("JPEG quality:"), this);

		d->JPEGcompression->setWhatsThis(tr("<p>The JPEG quality:</p>"
		                                      "<p><b>1</b>: low quality (high compression and small "
		                                      "file size)<br/>"
		                                      "<b>50</b>: medium quality<br/>"
		                                      "<b>75</b>: good quality (default)<br/>"
		                                      "<b>100</b>: high quality (no compression and "
		                                      "large file size)</p>"
		                                      "<p><b>Note: JPEG always uses lossy compression.</b></p>"));

		d->labelWarning = new QLabel(tr("<font size='-1' color='red'><i>"
		                                  "Warning: <a href='http://en.wikipedia.org/wiki/JPEG'>JPEG</a> is a "
		                                  "lossy compression image format!"
		                                  "</i></font>"), this);

		d->labelWarning->setOpenExternalLinks(true);
		d->labelWarning->setFrameStyle(QFrame::Box | QFrame::Plain);
		d->labelWarning->setLineWidth(1);
		d->labelWarning->setFrameShape(QFrame::Box);

		d->labelSubSampling = new QLabel(tr("Chroma subsampling:"), this);

		d->subSamplingCB = new QComboBox(this);
		d->subSamplingCB->insertItem(0, tr("None"));    // 1x1, 1x1, 1x1 (4:4:4)
		d->subSamplingCB->insertItem(1, tr("Medium"));  // 2x1, 1x1, 1x1 (4:2:2)
		d->subSamplingCB->insertItem(2, tr("High"));    // 2x2, 1x1, 1x1 (4:1:1)
		d->subSamplingCB->setWhatsThis(tr("<p>JPEG Chroma subsampling level \n(color is saved with less resolution "
		                                    "than luminance):</p>"
		                                    "<p><b>None</b>=best: uses 4:4:4 ratio. Does not employ chroma "
		                                    "subsampling at all. This preserves edges and contrasting "
		                                    "colors, whilst adding no additional compression</p>"
		                                    "<p><b>Medium</b>: uses 4:2:2 ratio. Medium compression: reduces "
		                                    "the color resolution by one-third with little to "
		                                    "no visual difference</p>"
		                                    "<p><b>High</b>: use 4:1:1 ratio. High compression: suits "
		                                    "images with soft edges but tends to alter colors</p>"
		                                    "<p><b>Note: JPEG always uses lossy compression.</b></p>"));

		d->JPEGGrid->addWidget(d->labelJPEGcompression, 0, 0, 1, 2);
		d->JPEGGrid->addWidget(d->JPEGcompression,      1, 0, 1, 2);
		d->JPEGGrid->addWidget(d->labelSubSampling,     2, 0, 1, 2);
		d->JPEGGrid->addWidget(d->subSamplingCB,        3, 0, 1, 2);
		d->JPEGGrid->addWidget(d->labelWarning,         4, 0, 1, 1);
		d->JPEGGrid->setColumnStretch(1, 10);
		d->JPEGGrid->setRowStretch(5, 10);
		//d->JPEGGrid->setMargin(KDialog::spacingHint());
		//d->JPEGGrid->setSpacing(KDialog::spacingHint());

		connect(d->JPEGcompression, SIGNAL(valueChanged(int)),
		        this, SIGNAL(signalSettingsChanged()));

		connect(d->subSamplingCB, SIGNAL(activated(int)),
		        this, SIGNAL(signalSettingsChanged()));
	}

	JPEGSettings::~JPEGSettings()
	{
		delete d;
	}

	void JPEGSettings::setCompressionValue(int val)
	{
		d->JPEGcompression->setValue(val);
	}

	int JPEGSettings::getCompressionValue()
	{
		return d->JPEGcompression->value();
	}

	void JPEGSettings::setSubSamplingValue(int val)
	{
		d->subSamplingCB->setCurrentIndex(val);
	}

	int JPEGSettings::getSubSamplingValue()
	{
		return d->subSamplingCB->currentIndex();
	}

}  // namespace Digikam
