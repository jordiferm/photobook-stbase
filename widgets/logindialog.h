/****************************************************************************
**
** Copyright (C) 2006-2010 SoftTopia. All rights reserved.
**
** This file is part of SoftTopia Software.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** SoftTopia reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QUrl>

class QLineEdit;
class QLabel;
class LoginDialog : public QDialog
{
Q_OBJECT
	QLineEdit* LEUser;
	QLineEdit* LEPassword;
	QLineEdit* LEKey;
	QLabel* NewAccountLabel;
	QLabel* LostPasswordLabel;

public:
	explicit LoginDialog(bool _AskForKey = false, QWidget *parent = 0);
	void setNewAccountUrl(const QUrl& _Url);
	void setForgetPassword(const QUrl& _Url);
	QString userName() const;
	void setUserName(const QString& _Value);
	QString password() const;
	void setPassword(const QString& _Value);
	QString key() const;
	void setKey(const QString& _Value);

signals:

public slots:

};

#endif // USERPASSWORDDIALOG_H
