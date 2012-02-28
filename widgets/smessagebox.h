/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef SMESSAGEBOX_H
#define SMESSAGEBOX_H

#include <QMessageBox>
#include "siconfactory.h"
#include "stwidgetsexport.h"

/**
	@author Shadow
	A Message box with icons on buttons.
*/
class ST_WIDGETS_EXPORT SMessageBox : public QMessageBox
{
	Q_OBJECT

	static void setIcon(QAbstractButton* Button, SIconFactory::IconType _Icon);
	static void setButtonIcons(QMessageBox* MBox);
	static StandardButton stdMessageBox( QWidget * parent, const QString & title, const QString & text, 
											StandardButtons buttons , StandardButton defaultButton, const QPixmap& _Pixmap, const QString& _DetailedText = "" );
	static StandardButton stdMessageBox(QWidget * parent, const QString & title, const QString & text,
			StandardButtons buttons , StandardButton defaultButton, SIconFactory::IconType _Icon, const QString& _DetailedText = "");

public:
	SMessageBox(QWidget *parent = 0);

	static StandardButton ST_WIDGETS_EXPORT critical(QWidget * parent, const QString & title, const QString & text, 
											 StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
	static StandardButton ST_WIDGETS_EXPORT criticalDetailed(QWidget * parent, const QString & title, const QString & text, const QString& _DetailedText,
											 StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
	static StandardButton ST_WIDGETS_EXPORT information(QWidget * parent, const QString & title, const QString & text,
	                                  StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
	static StandardButton ST_WIDGETS_EXPORT question(QWidget * parent, const QString & title, const QString & text,
	                                  StandardButtons buttons = Yes | No , StandardButton defaultButton = NoButton);
	static StandardButton ST_WIDGETS_EXPORT warning(QWidget * parent, const QString & title, const QString & text,
	                                  StandardButtons buttons = Ok, StandardButton defaultButton = NoButton);
	static StandardButton ST_WIDGETS_EXPORT retryAbort(QWidget * parent, const QString & title, const QString & text,
	                                  StandardButtons buttons = Retry | Abort , StandardButton defaultButton = Retry);
	static StandardButton ST_WIDGETS_EXPORT yesNo(QWidget * parent, const QString & title, const QString & text,
	                                  StandardButtons buttons = Yes | No, StandardButton defaultButton = No);
	static StandardButton ST_WIDGETS_EXPORT imageIcon(QWidget * parent, const QPixmap& _Pixmap, const QString & title, const QString & text,
	                                  StandardButtons buttons = Yes | No, StandardButton defaultButton = No);
};

#endif
