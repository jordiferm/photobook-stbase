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

#ifndef TPINPUTDIALOG_H
#define TPINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "sttpopslexport.h"


/**
Dialeg per a demanar una entrada a l'usuari. Amb un teclat per pantalla.

@author Shadow
*/
class QToolButton;
class QFrame;
class QLabel;
class ST_TPOPSL_EXPORT TPInputDialog : public QDialog
{
Q_OBJECT
	QLineEdit* LEUserInput;
	QFrame* BottomFrame;
	QPushButton* BEnter;
	QLabel* TitleLabel;
	QToolButton* newBottomActionButton(const QIcon& _Icon, QWidget* _Parent);

public:
	TPInputDialog(const QString _LabelText, QWidget* _Parent = 0);
	QString strResult() const;
	void setEchoMode(QLineEdit::EchoMode _EchoMode);
	void setText(const QString& _Text);
	void setCancelEnabled(bool _Enabled);
	void setTitle(const QString& _Title);
};

#endif
