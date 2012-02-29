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
#ifndef FTSMESSAGEBOX_H
#define FTSMESSAGEBOX_H

#include "ftsdialog.h"
#include "fstdactions.h"
#include "stwidgetsexport.h"


/**
TouchScreen Message Box.
 
@author Shadow
*/
class QActionGroup;
class QLabel;
class ST_WIDGETS_EXPORT FTSMessageBox : public FTSDialog
{

public:
	enum EnIcon
	{
		NoIcon, 
		Question, 
		Information, 
		Warning,
		Critical	
	};

protected: 
	QLabel* LabText;
	QLabel* LabIcon;

private:
	void init(const QActionGroup& _Actions, EnIcon _Icon);

public:
	FTSMessageBox(QWidget* _Parent = 0);
	FTSMessageBox(const QString& caption,
                          const QString &text, EnIcon icon,
                          const QActionGroup& _Actions,
                          QWidget *parent = 0, Qt::WFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	~FTSMessageBox();
	QString text() const; 
	void setText(const QString& _Text); 
	void setIcon(EnIcon _Icon);
	void setIcon(const QPixmap& _Pixmap);

	static FAction::EnActionId information(QWidget* _Parent, const QString& _Caption, const QString& _Text);
	static FAction::EnActionId critical(QWidget* _Parent, const QString& _Caption, const QString& _Text);
	static FAction::EnActionId question(QWidget* _Parent, const QString& _Caption, const QString& _Text);
	static FAction::EnActionId general(QWidget* _Parent, const QString& _Caption, const QString& _Text, EnIcon _Icon, const QActionGroup& _ActGroup);
};

/*!
	Classe de MessageBox Touch Screen amb la opci� d'ensenyar un texte amb informaci� abansada.
*/
class ST_WIDGETS_EXPORT FTSAdvancedMessageBox : public FTSMessageBox
{
Q_OBJECT
	QString AdvancedText;
	QString ShortText;

public:
	FTSAdvancedMessageBox(const QString& caption,
                          const QString &text, FTSMessageBox::EnIcon icon,
                          const QActionGroup& _Actions,
                          QWidget *parent = 0, Qt::WFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	void setAdvancedText(const QString& _AdvancedText);
	QString advancedText() const;
	static FAction::EnActionId advanced(QWidget* _Parent, const QString& _Caption, const QString& _Text, 
		const QString& _AdvancedText, QActionGroup& _ActGroup, FTSMessageBox::EnIcon _Icon = FTSMessageBox::Information);
	//! Sobrec�rrega de l'anterior amb _ActGroup Accept.
	static FAction::EnActionId advanced(QWidget* _Parent, const QString& _Caption, const QString& _Text, 
		const QString& _AdvancedText, FTSMessageBox::EnIcon _Icon = FTSMessageBox::Information);

public slots: 
	void showAdvancedText(bool _Show); 
};


#endif
