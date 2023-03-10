/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : save JPEG 2000 image options.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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


#include "jp2ksettings.h"

// Qt includes.

#include <QString>
#include <QLabel>
#include <QCheckBox>
#include <QLayout>
#include <QGridLayout>
#include <QSpinBox>


namespace Digikam
{

	class JP2KSettingsPriv
	{

	public:

		JP2KSettingsPriv()
		{
			JPEG2000Grid             = 0;
			labelJPEG2000compression = 0;
			JPEG2000compression      = 0;
			JPEG2000LossLess         = 0;
		}

		QGridLayout  *JPEG2000Grid;

		QLabel       *labelJPEG2000compression;

		QCheckBox    *JPEG2000LossLess;

		QSpinBox *JPEG2000compression;
	};

	JP2KSettings::JP2KSettings(QWidget *parent)
			: QWidget(parent), d(new JP2KSettingsPriv)
	{
		setAttribute(Qt::WA_DeleteOnClose);

		d->JPEG2000Grid     = new QGridLayout(this);
		d->JPEG2000LossLess = new QCheckBox(tr("Lossless JPEG 2000 files"), this);

		d->JPEG2000LossLess->setWhatsThis(tr("<p>Toggle lossless compression for JPEG 2000 images.</p>"
		                                     "<p>If you enable this option, you will use a lossless method "
		                                     "to compress JPEG 2000 pictures.</p>"));

		d->JPEG2000compression = new QSpinBox(this);
		d->JPEG2000compression->setValue(75);
		d->JPEG2000compression->setRange(1, 100);
		//d->JPEG2000compression->setSliderEnabled(true);
		d->labelJPEG2000compression = new QLabel(tr("JPEG 2000 quality:"), this);

		d->JPEG2000compression->setWhatsThis(tr("<p>The quality value for JPEG 2000 images:</p>"
		                                        "<p><b>1</b>: low quality (high compression and small "
		                                        "file size)<br/>"
		                                        "<b>50</b>: medium quality<br/>"
		                                        "<b>75</b>: good quality (default)<br/>"
		                                        "<b>100</b>: high quality (no compression and "
		                                        "large file size)</p>"
		                                        "<p><b>Note: JPEG 2000 is not a lossless image "
		                                        "compression format when you use this setting.</b></p>"));

		d->JPEG2000Grid->addWidget(d->JPEG2000LossLess,         0, 0, 1, 2);
		d->JPEG2000Grid->addWidget(d->labelJPEG2000compression, 1, 0, 1, 2);
		d->JPEG2000Grid->addWidget(d->JPEG2000compression,      2, 0, 1, 2);
		d->JPEG2000Grid->setColumnStretch(1, 10);
		d->JPEG2000Grid->setRowStretch(3, 10);
		//d->JPEG2000Grid->setMargin(KDialog::spacingHint());
		//d->JPEG2000Grid->setSpacing(KDialog::spacingHint());

		connect(d->JPEG2000LossLess, SIGNAL(toggled(bool)),
		        this, SLOT(slotToggleJPEG2000LossLess(bool)));

		connect(d->JPEG2000LossLess, SIGNAL(toggled(bool)),
		        this, SIGNAL(signalSettingsChanged()));

		connect(d->JPEG2000compression, SIGNAL(valueChanged(int)),
		        this, SIGNAL(signalSettingsChanged()));
	}

	JP2KSettings::~JP2KSettings()
	{
		delete d;
	}

	void JP2KSettings::setCompressionValue(int val)
	{
		d->JPEG2000compression->setValue(val);
	}

	int JP2KSettings::getCompressionValue()
	{
		return d->JPEG2000compression->value();
	}

	void JP2KSettings::setLossLessCompression(bool b)
	{
		d->JPEG2000LossLess->setChecked(b);
		slotToggleJPEG2000LossLess(d->JPEG2000LossLess->isChecked());
	}

	bool JP2KSettings::getLossLessCompression()
	{
		return d->JPEG2000LossLess->isChecked();
	}

	void JP2KSettings::slotToggleJPEG2000LossLess(bool b)
	{
		d->JPEG2000compression->setEnabled(!b);
		d->labelJPEG2000compression->setEnabled(!b);
	}

}  // namespace Digikam
