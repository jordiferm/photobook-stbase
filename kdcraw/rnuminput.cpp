/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-08-16
 * Description : Integer and double num input widget
 *               re-implemented with a reset button to switch to
 *               a default value.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QToolButton>
#include <QSpinBox>
#include <QDoubleSpinBox> 
#include <QSlider> 
#include <QLayout>
#include <QLabel> 


// Local includes.

#include "rnuminput.h"

namespace KDcrawIface
{

	class RIntNumInputPriv
	{

	public:

		RIntNumInputPriv()
		{
			defaultValue = 0;
			resetButton  = 0;
			input        = 0;
		}

		int           defaultValue;

		QToolButton  *resetButton;

		QSlider			 *slider; 
		QSpinBox*		input;
		QLabel*			label;
		QHBoxLayout*	layout;  
	};

	RIntNumInput::RIntNumInput(QWidget *parent)
			: QWidget(parent)
	{
		QHBoxLayout* MLayout = new QHBoxLayout(this); 
		
		d = new RIntNumInputPriv;
		d->layout = MLayout; 
		d->label = 0; 
		d->slider = new QSlider(this); 
		d->slider->setOrientation(Qt::Horizontal); 
		MLayout->addWidget(d->slider); 
		d->input       = new QSpinBox(this);
		MLayout->addWidget(d->input); 
		connect(d->slider, SIGNAL(valueChanged( int )), this, SLOT(slotSliderValueChange(int))); 

		d->resetButton = new QToolButton(this);
		MLayout->addWidget(d->resetButton); 
		d->resetButton->setAutoRaise(true);
		d->resetButton->setFocusPolicy(Qt::NoFocus);
		d->resetButton->setIcon(QIcon(":/kdcraw/document-revert.png"));
		d->resetButton->setToolTip(tr("Reset to default value"));

		MLayout->setStretchFactor(d->input, 10);
		MLayout->setMargin(0);
		MLayout->setSpacing(1);

		// -------------------------------------------------------------

		connect(d->resetButton, SIGNAL(clicked()),
		        this, SLOT(slotReset()));

		connect(d->input, SIGNAL(valueChanged(int)),
		        this, SLOT(slotValueChanged(int)));
	}

	RIntNumInput::~RIntNumInput()
	{
		delete d;
	}

	QSpinBox* RIntNumInput::input() const
	{
		return d->input;
	}

	void RIntNumInput::setSliderEnabled(bool b)
	{
		d->slider->setEnabled(b);
	}

	void RIntNumInput::setRange(int min, int max, int step)
	{
		d->input->setRange(min, max);
		d->input->setSingleStep(step); 
		d->slider->setRange(min, max); 
		d->slider->setSingleStep(step); 
	}

	void RIntNumInput::setLabel(const QString& _Text, Qt::Alignment _Alignment)
	{
		if (!d->label)
		{
			d->label = new QLabel(this); 
			d->layout->insertWidget(0, d->label); 
		}
		d->label->setText(_Text); 
		d->label->setAlignment(_Alignment); 
	}


	int RIntNumInput::value()
	{
		return d->input->value();
	}

	void RIntNumInput::setValue(int v)
	{
		d->input->setValue(v);
	}

	int RIntNumInput::defaultValue()
	{
		return d->defaultValue;
	}

	void RIntNumInput::setDefaultValue(int v)
	{
		d->defaultValue = v;
		d->input->setValue(d->defaultValue);
		slotValueChanged(v);
	}

	void RIntNumInput::slotReset()
	{
		d->input->setValue(d->defaultValue);
		d->resetButton->setEnabled(false);
		emit reset();
	}

	void RIntNumInput::slotValueChanged(int v)
	{
		if (v != d->defaultValue)
			d->resetButton->setEnabled(true);
		d->slider->setValue(v); 
		emit valueChanged(v);
	}

	void RIntNumInput::slotSliderValueChange( int _Value )
	{
		setValue(_Value);
	}


// ----------------------------------------------------

	class RDoubleNumInputPriv
	{

	public:

		RDoubleNumInputPriv()
		{
			defaultValue = 0.0;
			resetButton  = 0;
			input        = 0;
		}

		double           defaultValue;

		QToolButton     *resetButton;
		QSlider			 *slider; 

		QDoubleSpinBox*input;
	};

	RDoubleNumInput::RDoubleNumInput(QWidget *parent)
			: QWidget(parent)
	{
		QHBoxLayout* MLayout = new QHBoxLayout(this); 
		
		d = new RDoubleNumInputPriv;

		d->slider = new QSlider(this); 
		d->slider->setOrientation(Qt::Horizontal);
		MLayout->addWidget(d->slider); 
		d->input       = new QDoubleSpinBox(this);
		MLayout->addWidget(d->input); 
		d->resetButton = new QToolButton(this);
		MLayout->addWidget(d->resetButton); 
		d->resetButton->setAutoRaise(true);
		d->resetButton->setFocusPolicy(Qt::NoFocus);
		d->resetButton->setIcon(QIcon(":/kdcraw/document-revert.png"));
		d->resetButton->setToolTip(tr("Reset to default value"));

		connect(d->slider, SIGNAL(valueChanged( int )), this, SLOT(slotSliderValueChange( int)));

		MLayout->setStretchFactor(d->input, 10);
		MLayout->setMargin(0);
		MLayout->setSpacing(1);

		// -------------------------------------------------------------

		connect(d->resetButton, SIGNAL(clicked()),
		        this, SLOT(slotReset()));

		connect(d->input, SIGNAL(valueChanged(double)),
		        this, SLOT(slotValueChanged(double)));
	}

	RDoubleNumInput::~RDoubleNumInput()
	{
		delete d;
	}

	QDoubleSpinBox* RDoubleNumInput::input() const
	{
		return d->input;
	}

	void RDoubleNumInput::setDecimals(int p)
	{
		d->input->setDecimals(p);
	}

	void RDoubleNumInput::setRange(double min, double max, double step, bool slider)
	{
		//d->input->setRange(min, max, step, slider);
		d->input->setRange(min, max); 
		d->input->setSingleStep(step); 
		d->slider->setRange(intValue(min) , intValue(max)); 
		d->slider->setSingleStep(intValue(step)); 
	}

	double RDoubleNumInput::value()
	{
		return d->input->value();
	}

	void RDoubleNumInput::setValue(double v)
	{
		d->input->setValue(v);
	}

	double RDoubleNumInput::defaultValue()
	{
		return d->defaultValue;
	}

	void RDoubleNumInput::setDefaultValue(double v)
	{
		d->defaultValue = v;
		d->input->setValue(d->defaultValue);
		slotValueChanged(v);
	}

	void RDoubleNumInput::slotReset()
	{
		d->input->setValue(d->defaultValue);
		d->resetButton->setEnabled(false);
		emit reset();
	}

	void RDoubleNumInput::slotValueChanged(double v)
	{
		if (v != d->defaultValue)
			d->resetButton->setEnabled(true);
		d->slider->setValue(intValue(v)); 
		emit valueChanged(v);
	}

	void RDoubleNumInput::slotSliderValueChange( int _Value )
	{
		setValue( doubleValue(_Value));
	}

	int RDoubleNumInput::intValue(double _Value)
	{
		int Res; 
		if (d->input->decimals() > 0)
			Res = _Value * (d->input->decimals() * 10); 
		else 
			Res = static_cast<int>(_Value); 
		return Res;
	}

	double RDoubleNumInput::doubleValue(int _Value)
	{
		double Res; 
		if (d->input->decimals() > 0)
			Res = static_cast<double>(_Value ) / (d->input->decimals() * 10);
		else 
			Res = static_cast<double>(_Value); 
		return Res; 
	}


}  // namespace KDcrawIface
