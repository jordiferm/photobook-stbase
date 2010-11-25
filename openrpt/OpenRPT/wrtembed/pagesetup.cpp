/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2008 by OpenMFG, LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "pagesetup.h"

#include <QVariant>

#include <labelsizeinfo.h>

/*
 *  Constructs a PageSetup as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
PageSetup::PageSetup(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : QDialog(parent, name, modal, fl)
{
    setupUi(this);


    // signals and slots connections
    connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(cbPaperSize, SIGNAL(activated(const QString&)), this, SLOT(cbPaperSize_activated(const QString&)));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
PageSetup::~PageSetup()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PageSetup::languageChange()
{
    retranslateUi(this);
}

void PageSetup::cbPaperSize_activated( const QString & str)
{
    // the user selected a different paper size
    // if it's a predefined size make the custom boxes disabled
    // otherwise enable them if it's custom
    if(str == "Custom") {
	enableCustom(TRUE);
    } else {
	enableCustom(FALSE);
    }
    if(str == "Labels") {
	enableLabels(TRUE);
    } else {
	enableLabels(FALSE);
    }
}

void PageSetup::enableCustom( bool yes )
{
    labelCustom->setEnabled(yes);
    labelWidth->setEnabled(yes);
    labelHeight->setEnabled(yes);
    labelInInches->setEnabled(yes);
    tbPaperWidth->setEnabled(yes);
    tbPaperHeight->setEnabled(yes);
}

void PageSetup::enableLabels( bool yes )
{
    gbLabels->setEnabled(yes);
    gbMargins->setEnabled(!yes);
}

QString PageSetup::getPaperSize()
{
    return cbPaperSize->currentText();
}

double PageSetup::getPaperWidth()
{
    return tbPaperWidth->text().toDouble();
}

double PageSetup::getPaperHeight()
{
    return tbPaperHeight->text().toDouble();
}

void PageSetup::setPaperSize( const QString & str )
{
    enableCustom(FALSE);
    enableLabels(FALSE);
    if(str == "Letter") {
	cbPaperSize->setCurrentItem(0);
    } else if(str == "Legal") {
	cbPaperSize->setCurrentItem(1);
    } else if(str == "A4") {
	cbPaperSize->setCurrentItem(2);
    } else if(str == "Labels") {
	cbPaperSize->setCurrentItem(4);
	enableLabels(TRUE);
    } else {
	cbPaperSize->setCurrentItem(3);
        enableCustom(TRUE);
    }
}

void PageSetup::setPaperSize( double w, double h )
{
    QString s = QString("Custom");
    setPaperSize(s);
    tbPaperWidth->setText(QString::number(w,'f',3));
    tbPaperHeight->setText(QString::number(h,'f',3));
}

double PageSetup::getMarginBottom()
{
    return tbBottom->text().toDouble();
}

double PageSetup::getMarginLeft()
{
    return tbLeft->text().toDouble();
}

double PageSetup::getMarginRight()
{
    return tbRight->text().toDouble();
}

double PageSetup::getMarginTop()
{
    return tbTop->text().toDouble();
}

void PageSetup::setMargins( double t, double b, double l, double r )
{
    tbTop->setText(QString::number(t,'f',3));
    tbBottom->setText(QString::number(b,'f',3));
    tbLeft->setText(QString::number(l,'f',3));
    tbRight->setText(QString::number(r,'f',3));

}

bool PageSetup::isPortrait()
{
    return rbPortrait->isChecked();
}

void PageSetup::setPortrait( bool yes )
{
    if(yes == TRUE) {
	    rbPortrait->setChecked(TRUE);
    } else {
	    rbLandscape->setChecked(TRUE);
    }
}

const QString PageSetup::getLabelType()
{
    return cbLabelType->currentText();
}

void PageSetup::setLabelType( const QString & type)
{
    if(type == QString::null) {
	    cbLabelType->setCurrentItem(0);
    } else {
	    for(int i = 0; i < cbLabelType->count(); i++) {
	        if(cbLabelType->text(i) == type) {
		        cbLabelType->setCurrentItem(i);
	        }
	    }
    }
}

void PageSetup::init()
{
    cbLabelType->clear();
    cbLabelType->insertStringList(LabelSizeInfo::getLabelNames());
    
}
