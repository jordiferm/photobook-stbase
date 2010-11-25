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

#ifndef TSINPUTDIALOG_H
#define TSINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "stwidgetsexport.h"


/**
Input dialog with touchscreen keyboard.

@author Shadow
*/
class ST_WIDGETS_EXPORT TSInputDialog : public QDialog
{
Q_OBJECT
	QLineEdit* LEUserInput;

public:
	TSInputDialog(const QString _LabelText, QWidget* _Parent);
	QString strResult() const;
	void setEchoMode(QLineEdit::EchoMode _EchoMode);
	void setText(const QString& _Text);
};

#endif
