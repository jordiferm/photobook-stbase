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

#ifndef GKEYBOARD2_H
#define GKEYBOARD2_H

#include "keyboardw2.h"

#include <QString>
#include <QFrame>
#include <QKeyEvent>
#include "stwidgetsexport.h"


/**
@author Shadow
Teclat per casademont, conté més funcionalitats que el teclat convencional.
*/
class QSignalMapper;
class ST_WIDGETS_EXPORT GKeyboard2 : public QWidget
{
	Q_OBJECT
	
	QObject* ReceiberObj;
	uint LastPressedKey;
	uint BeforeLastPressedKey;

public:
	Ui::KeyboardW2 KbUi;
	
private: 	
	void mapMapper(QSignalMapper* __SignalMapper);
	void init();
	QKeyEvent* getNewKeyEvent ( int _key, const QString& _text = "" );
	
public:
	GKeyboard2(QWidget *parent = 0);
	~GKeyboard2();
	void setReceiver(QObject* _Receiver); 
	QString keyStr(int _Key);
	
	QFrame* getKeyBoard() { return KbUi.FrameAlphaKeys; }
	QFrame* getKeyPad() { return KbUi.FrameKeyNum; }
	
	
public slots:
	void keyReleased(int _Key);
	void showHideAlphaNumericKeyboard();
	void showHideNumericKeyboard();
	void setAlphaNumericKeyboardVisible(bool _Visible);
	void setNumericKeyboardVisible(bool _Visible);

signals:
	void enterPressed();

};

#endif
