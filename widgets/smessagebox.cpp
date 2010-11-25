/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Starblitz reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "smessagebox.h"
#include <QAbstractButton>

void SMessageBox::setIcon(QAbstractButton* Button, SIconFactory::IconType _Icon)
{
	Button->setIcon(stIcon(_Icon));
	Button->setIconSize(QSize(32, 32));
}


void SMessageBox::setButtonIcons(QMessageBox* MBox)
{
	if (QAbstractButton* CButton = MBox->button(QMessageBox::Ok))
		setIcon(CButton, SIconFactory::Accept);
	if (QAbstractButton* CButton = MBox->button(QMessageBox::Cancel))
		setIcon(CButton, SIconFactory::Cancel);
	if (QAbstractButton* CButton = MBox->button(QMessageBox::Yes) )
		setIcon(CButton, SIconFactory::Accept);
	if (QAbstractButton* CButton = MBox->button(QMessageBox::No) )
		setIcon(CButton, SIconFactory::Cancel);
}

SMessageBox::StandardButton SMessageBox::stdMessageBox( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton, const QPixmap& _Pixmap, const QString& _DetailedText )
{
	QMessageBox MBox(QMessageBox::Information, title, text, buttons, parent);
	MBox.setIconPixmap(_Pixmap);
	if (!_DetailedText.isEmpty())
		MBox.setDetailedText(_DetailedText);
#if QT_VERSION >= 0x040300
	MBox.setDefaultButton(defaultButton);
#endif
	setButtonIcons(&MBox);
	if (MBox.exec() == -1)
		return QMessageBox::Cancel;
	
	return MBox.standardButton(MBox.clickedButton());
}


SMessageBox::StandardButton SMessageBox::stdMessageBox( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton, SIconFactory::IconType _Icon, const QString& _DetailedText )
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, stIcon(_Icon).pixmap(64, 64), _DetailedText);
}

SMessageBox::SMessageBox(QWidget *parent)
 : QMessageBox(parent)
{
}

SMessageBox::StandardButton SMessageBox::critical(QWidget * parent, const QString & title, const QString & text, 
											StandardButtons buttons, StandardButton defaultButton)
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Error);
}

SMessageBox::StandardButton SMessageBox::criticalDetailed(QWidget * parent, const QString & title, const QString & text, const QString& _DetailedText,
											StandardButtons buttons, StandardButton defaultButton)
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Error, _DetailedText);
}

SMessageBox::StandardButton SMessageBox::information ( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton )
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Info);
}

SMessageBox::StandardButton SMessageBox::question ( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton )
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Question);
}

SMessageBox::StandardButton SMessageBox::warning ( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton )
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Warning);
}

SMessageBox::StandardButton SMessageBox::retryAbort ( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton )
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Error);
}

SMessageBox::StandardButton SMessageBox::yesNo ( QWidget * parent, const QString & title, const QString & text, 
										StandardButtons buttons , StandardButton defaultButton )
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, SIconFactory::Error);
}

SMessageBox::StandardButton SMessageBox::imageIcon(QWidget * parent, const QPixmap& _Pixmap, const QString & title, const QString & text,
	                                  StandardButtons buttons, StandardButton defaultButton)
{
	return stdMessageBox(parent, title, text, buttons, defaultButton, _Pixmap);
}
