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

#include "gkeyboard2.h"
#include <qsignalmapper.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QList>
#include <QListIterator>

void GKeyboard2::mapMapper(QSignalMapper* _SignalMapper)
{
// 	_SignalMapper->setMapping(esc,Qt::Key_Escape);
// 	_SignalMapper->setMapping(f1,Qt::Key_F1);
// 	_SignalMapper->setMapping(f2,Qt::Key_F2);
// 	_SignalMapper->setMapping(f3,Qt::Key_F3);
// 	_SignalMapper->setMapping(f4,Qt::Key_F4);
// 	_SignalMapper->setMapping(f5,Qt::Key_F5);
// 	_SignalMapper->setMapping(f6,Qt::Key_F6);
// 	_SignalMapper->setMapping(f7,Qt::Key_F7);
// 	_SignalMapper->setMapping(f8,Qt::Key_F8);
// 	_SignalMapper->setMapping(f9,Qt::Key_F9);
// 	_SignalMapper->setMapping(f10,Qt::Key_F10);
// 	_SignalMapper->setMapping(f11,Qt::Key_F11);
// 	_SignalMapper->setMapping(f12,Qt::Key_F12);

	_SignalMapper->setMapping(KbUi.oa,Qt::Key_masculine);
	_SignalMapper->setMapping(KbUi.b1,Qt::Key_1);
	_SignalMapper->setMapping(KbUi.b2,Qt::Key_2);
	_SignalMapper->setMapping(KbUi.b3,Qt::Key_3);
	_SignalMapper->setMapping(KbUi.b4,Qt::Key_4);
	_SignalMapper->setMapping(KbUi.b5,Qt::Key_5);
	_SignalMapper->setMapping(KbUi.b6,Qt::Key_6);
	_SignalMapper->setMapping(KbUi.b7,Qt::Key_7);
	_SignalMapper->setMapping(KbUi.b8,Qt::Key_8);
	_SignalMapper->setMapping(KbUi.b9,Qt::Key_9);
	_SignalMapper->setMapping(KbUi.b0,Qt::Key_0);
	_SignalMapper->setMapping(KbUi.bint,Qt::Key_Apostrophe );
	_SignalMapper->setMapping(KbUi.bint2,Qt::Key_exclamdown );
	_SignalMapper->setMapping(KbUi.bdel,Qt::Key_Backspace);
	_SignalMapper->setMapping(KbUi.btab,Qt::Key_Tab);
	_SignalMapper->setMapping(KbUi.bq,Qt::Key_Q);
	_SignalMapper->setMapping(KbUi.bw,Qt::Key_W);
	_SignalMapper->setMapping(KbUi.be,Qt::Key_E);
	_SignalMapper->setMapping(KbUi.br,Qt::Key_R);
	_SignalMapper->setMapping(KbUi.bt,Qt::Key_T);
	_SignalMapper->setMapping(KbUi.by,Qt::Key_Y);
	_SignalMapper->setMapping(KbUi.bu,Qt::Key_U);
	_SignalMapper->setMapping(KbUi.bi,Qt::Key_I);
	_SignalMapper->setMapping(KbUi.bo,Qt::Key_O);
	_SignalMapper->setMapping(KbUi.bp,Qt::Key_P);
	//Accent obert
	_SignalMapper->setMapping(KbUi.bp2,Qt::Key_Agrave);
	_SignalMapper->setMapping(KbUi.bp3,Qt::Key_Plus);
	
	_SignalMapper->setMapping(KbUi.benter,Qt::Key_Enter);
	
	_SignalMapper->setMapping(KbUi.bmay,Qt::Key_CapsLock);
	_SignalMapper->setMapping(KbUi.ba,Qt::Key_A);
	_SignalMapper->setMapping(KbUi.bs,Qt::Key_S);
	_SignalMapper->setMapping(KbUi.bd,Qt::Key_D);
	_SignalMapper->setMapping(KbUi.bf,Qt::Key_F);
	_SignalMapper->setMapping(KbUi.bg,Qt::Key_G);
	_SignalMapper->setMapping(KbUi.bh,Qt::Key_H);
	_SignalMapper->setMapping(KbUi.bj,Qt::Key_J);
	_SignalMapper->setMapping(KbUi.bk,Qt::Key_K);
	_SignalMapper->setMapping(KbUi.bl,Qt::Key_L);
	_SignalMapper->setMapping(KbUi.ben,Qt::Key_Ntilde);
	//Accnt tancat
	_SignalMapper->setMapping(KbUi.bcor1,Qt::Key_acute);
	_SignalMapper->setMapping(KbUi.bcor2,Qt::Key_Ccedilla);
	
	_SignalMapper->setMapping(KbUi.blsh,Qt::Key_Shift);
	_SignalMapper->setMapping(KbUi.bmsg,Qt::Key_Greater);
	_SignalMapper->setMapping(KbUi.bz,Qt::Key_Z);
	_SignalMapper->setMapping(KbUi.bx,Qt::Key_X);
	_SignalMapper->setMapping(KbUi.bc,Qt::Key_C);
	_SignalMapper->setMapping(KbUi.bv,Qt::Key_V);
	_SignalMapper->setMapping(KbUi.bb,Qt::Key_B);
	_SignalMapper->setMapping(KbUi.bn,Qt::Key_N);
	_SignalMapper->setMapping(KbUi.bm,Qt::Key_M);
	_SignalMapper->setMapping(KbUi.bcut,Qt::Key_Comma);
	_SignalMapper->setMapping(KbUi.bdot,Qt::Key_Period);
	_SignalMapper->setMapping(KbUi.blmi,Qt::Key_Minus);
	_SignalMapper->setMapping(KbUi.brsh,Qt::Key_Shift);
	
	_SignalMapper->setMapping(KbUi.blctrl,Qt::Key_Control);
	_SignalMapper->setMapping(KbUi.blalt,Qt::Key_Alt);
	_SignalMapper->setMapping(KbUi.bspace,Qt::Key_Space);
	_SignalMapper->setMapping(KbUi.bralt,Qt::Key_AltGr);
	_SignalMapper->setMapping(KbUi.brctrl,Qt::Key_Control);
	_SignalMapper->setMapping(KbUi.bbloq,Qt::Key_NumLock);
	_SignalMapper->setMapping(KbUi.bdiv,Qt::Key_Slash);
	_SignalMapper->setMapping(KbUi.bprod,Qt::Key_Asterisk);
	_SignalMapper->setMapping(KbUi.brmin,Qt::Key_Minus);
	_SignalMapper->setMapping(KbUi.brplus,Qt::Key_Plus);
	_SignalMapper->setMapping(KbUi.brenter,Qt::Key_Enter);
	_SignalMapper->setMapping(KbUi.brdot,Qt::Key_Period);
	_SignalMapper->setMapping(KbUi.br0,Qt::Key_0);
	_SignalMapper->setMapping(KbUi.br1,Qt::Key_1);
	_SignalMapper->setMapping(KbUi.br2,Qt::Key_2);
	_SignalMapper->setMapping(KbUi.br3,Qt::Key_3);
	_SignalMapper->setMapping(KbUi.br4,Qt::Key_4);
	_SignalMapper->setMapping(KbUi.br5,Qt::Key_5);
	_SignalMapper->setMapping(KbUi.br6,Qt::Key_6);
	_SignalMapper->setMapping(KbUi.br7,Qt::Key_7);
	_SignalMapper->setMapping(KbUi.br8,Qt::Key_8);
	_SignalMapper->setMapping(KbUi.br9,Qt::Key_9);
//	_SignalMapper->setMapping(clear_button,Qt::Key_Delete);
}

QKeyEvent* GKeyboard2::getNewKeyEvent ( int _key, const QString& _text )
{
	QString txt;
	if ( _key != 0 ) 
		txt = keyStr( _key );
	else
		txt = _text;
	
	//Comprova que no tinguem el caps lock activat.
	if (KbUi.bmay->isCheckable() && KbUi.bmay->isChecked() )
		txt = txt.toUpper();
	else 
		txt = txt.toLower();
	
	QKeyEvent* ev = NULL;
	ev = new QKeyEvent ( QEvent::KeyPress, _key, Qt::NoModifier, txt.toAscii().data());
	return ev;
}

void GKeyboard2::init()
{
	LastPressedKey = 0;
	BeforeLastPressedKey = 0;
	
	connect ( KbUi.alpha, SIGNAL(clicked()), this, SLOT(showHideAlphaNumericKeyboard()));
	connect ( KbUi.calc, SIGNAL(clicked()), this, SLOT(showHideNumericKeyboard()));

	QSignalMapper * ReleaseSignalMapper = new QSignalMapper(this);
	mapMapper(ReleaseSignalMapper);

	KbUi.bmay->setCheckable(true);
	
	QRect ScrRect = qApp->desktop()->screenGeometry();
					
	QList<QPushButton *> widgets = this->findChildren<QPushButton *>();
	QListIterator<QPushButton *> it2( widgets ); // iterate over the buttons
	QPushButton* obj;
	while (it2.hasNext())
	{
		obj = it2.next();
		obj->setFocusPolicy(Qt::NoFocus);
		connect(obj,SIGNAL(released()),ReleaseSignalMapper,SLOT(map()));
	}
	connect(ReleaseSignalMapper,SIGNAL(mapped(int)),this,SLOT(keyReleased(int)));
}

GKeyboard2::GKeyboard2(QWidget *parent)
		: QWidget(parent), ReceiberObj(parent)
{
	KbUi.setupUi(this);
	init();
}
	
GKeyboard2::~GKeyboard2()
{
}

void GKeyboard2::setReceiver(QObject* _Receiver)
{
	ReceiberObj = _Receiver;
}

/*! Retorna un string amb el caràcter de la tecla.
	_Key Constant de tecla de les Qt. Qt::Key...
*/
QString GKeyboard2::keyStr(int _Key)
{
	QString Res = "";
		
	if ((_Key & 0x1000) == 0)
	{
		Res = static_cast<char>(_Key);
		if (KbUi.bmay->isChecked())
			Res = Res.toUpper();
		else 
			Res = Res.toLower();	
	}
	return Res; 
}

void GKeyboard2::keyReleased(int _Key)
{
	ReceiberObj = qApp->focusWidget();
	if (ReceiberObj)
	{
		if ( _Key ) 
		{
			if (_Key == Qt::Key_Enter)
				emit enterPressed();

			QKeyEvent* keyEvent = NULL;
			//qDebug(QString("\nINIT Last pressed key: %1").arg(LastPressedKey));
			
			//Accent tancat
			if ( LastPressedKey == Qt::Key_acute )
			{
				if ( BeforeLastPressedKey != Qt::Key_Shift )
				{
					if ( _Key == Qt::Key_A )
						keyEvent = getNewKeyEvent ( 0, "á" );
					if ( _Key == Qt::Key_E )
						keyEvent = getNewKeyEvent ( 0, "é");
					if ( _Key == Qt::Key_I )
						keyEvent = getNewKeyEvent ( 0, "í");
					if ( _Key == Qt::Key_O )
						keyEvent = getNewKeyEvent ( 0, "ó");
					if ( _Key == Qt::Key_U )
						keyEvent = getNewKeyEvent ( 0, "ú");
				}
				else
				{
					if ( _Key == Qt::Key_A )
						keyEvent = getNewKeyEvent ( 0, "ä");
					if ( _Key == Qt::Key_E )
						keyEvent = getNewKeyEvent ( 0, "ë");
					if ( _Key == Qt::Key_I )
						keyEvent = getNewKeyEvent ( 0, "ï");
					if ( _Key == Qt::Key_O )
						keyEvent = getNewKeyEvent ( 0, "ö");
					if ( _Key == Qt::Key_U )
						keyEvent = getNewKeyEvent ( 0, "ü");
				}
			}
			//Accent obert.
			if ( LastPressedKey == Qt::Key_Agrave )
			{					
				if ( BeforeLastPressedKey != Qt::Key_Shift )
				{
					if ( _Key == Qt::Key_A )
						keyEvent = getNewKeyEvent ( 0, "à");
					if ( _Key == Qt::Key_E )
						keyEvent = getNewKeyEvent ( 0, "è");
					if ( _Key == Qt::Key_I )
						keyEvent = getNewKeyEvent ( 0, "ì");
					if ( _Key == Qt::Key_O )
						keyEvent = getNewKeyEvent ( 0, "ò");
					if ( _Key == Qt::Key_U )
						keyEvent = getNewKeyEvent ( 0, "ù");
				}
				else
				{
					if ( _Key == Qt::Key_A )
						keyEvent = getNewKeyEvent ( 0, "â");
					if ( _Key == Qt::Key_E )
						keyEvent = getNewKeyEvent ( 0, "ê");
					if ( _Key == Qt::Key_I )
						keyEvent = getNewKeyEvent ( 0, "î");
					if ( _Key == Qt::Key_O )
						keyEvent = getNewKeyEvent ( 0, "ô");
					if ( _Key == Qt::Key_U )
						keyEvent = getNewKeyEvent ( 0, "û");
				}
			}
			
			
			
			if ( LastPressedKey == Qt::Key_AltGr )
			{
				if ( _Key == Qt::Key_masculine )
					keyEvent = getNewKeyEvent(Qt::Key_Backslash);
				if ( _Key == Qt::Key_1 )
					keyEvent = getNewKeyEvent(Qt::Key_Bar);
				if ( _Key == Qt::Key_2 )
					keyEvent = getNewKeyEvent(Qt::Key_At);
				if ( _Key == Qt::Key_3 )
					keyEvent = getNewKeyEvent ( 0, "#");
				if ( _Key == Qt::Key_acute)
					keyEvent = getNewKeyEvent ( 0, "{");
				if ( _Key == Qt::Key_Ccedilla )
					keyEvent = getNewKeyEvent ( 0, "}");
				if ( _Key == Qt::Key_Plus )
					keyEvent = getNewKeyEvent ( 0, "]");
				if ( _Key == Qt::Key_Agrave)
					keyEvent = getNewKeyEvent ( 0, "[");
				
			}
			if ( LastPressedKey == Qt::Key_Shift )
			{
				if ( _Key == Qt::Key_masculine )
					keyEvent = getNewKeyEvent ( 0, "ª");
				if ( _Key == Qt::Key_1 )
					keyEvent = getNewKeyEvent(Qt::Key_Exclam);
				if ( _Key == Qt::Key_2 )
					keyEvent = getNewKeyEvent(Qt::Key_QuoteDbl);
				if ( _Key == Qt::Key_3 )
					keyEvent = getNewKeyEvent ( 0, "·");
				if ( _Key == Qt::Key_4 )
					keyEvent = getNewKeyEvent(Qt::Key_Dollar);
				if ( _Key == Qt::Key_5 )
					keyEvent = getNewKeyEvent(Qt::Key_Percent);
				if ( _Key == Qt::Key_6 )
					keyEvent = getNewKeyEvent(Qt::Key_Ampersand);
				if ( _Key == Qt::Key_7 )
					keyEvent = getNewKeyEvent(Qt::Key_Slash);
				if ( _Key == Qt::Key_8 )
					keyEvent = getNewKeyEvent(Qt::Key_ParenLeft);
				if ( _Key == Qt::Key_9 )
					keyEvent = getNewKeyEvent(Qt::Key_ParenRight);
				if ( _Key == Qt::Key_0 )
					keyEvent = getNewKeyEvent(Qt::Key_Equal);
				if ( _Key == Qt::Key_Apostrophe )
					keyEvent = getNewKeyEvent(Qt::Key_Question);
				if ( _Key == Qt::Key_exclamdown )
					keyEvent = getNewKeyEvent(Qt::Key_questiondown);
				if ( _Key == Qt::Key_Comma )
					keyEvent = getNewKeyEvent ( 0, ";");
				if ( _Key == Qt::Key_Period )
					keyEvent = getNewKeyEvent ( 0, ":");
				if ( _Key == Qt::Key_Minus )
					keyEvent = getNewKeyEvent ( 0, "_");
				if ( _Key == Qt::Key_Greater )
					keyEvent = getNewKeyEvent ( 0, "<");
			}
			
			//No ens han pintat un keyevent.
			if ( !keyEvent )
			{
				//No podem pintar.
				if (!( _Key == Qt::Key_acute || _Key == Qt::Key_Alt || _Key == Qt::Key_Shift || _Key == Qt::Key_CapsLock || _Key == Qt::Key_AltGr || (_Key == Qt::Key_Agrave && LastPressedKey != Qt::Key_Alt) ))
					keyEvent = new QKeyEvent ( QEvent::KeyPress, _Key, Qt::NoModifier, keyStr(_Key).toAscii().data() );
			}
			
			BeforeLastPressedKey = LastPressedKey;
			LastPressedKey = _Key;
			
			//Alguns casos no els hem de notificar perque son tecles que per si sol no tenen cap sentit.
			if ( keyEvent )
				qApp->notify(ReceiberObj, keyEvent);
		}
	}
}

void GKeyboard2::showHideAlphaNumericKeyboard()
{
	if ( KbUi.FrameAlphaKeys->isVisible())
		KbUi.FrameAlphaKeys->hide();
	else
		KbUi.FrameAlphaKeys->show();
}

void GKeyboard2::showHideNumericKeyboard()
{
	if ( KbUi.FrameKeyNum->isVisible())
		KbUi.FrameKeyNum->hide();
	else
		KbUi.FrameKeyNum->show();
}

void GKeyboard2::setAlphaNumericKeyboardVisible(bool _Visible)
{
	KbUi.FrameAlphaKeys->setVisible(_Visible);
}

void GKeyboard2::setNumericKeyboardVisible(bool _Visible)
{
	KbUi.FrameKeyNum->setVisible(_Visible);
}
