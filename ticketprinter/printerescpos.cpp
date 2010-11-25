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

#include "printerescpos.h"


void PrinterESCPOS::updateMode()
{
	putch(27);
	putch('!');
	putch(CurrentMode);
}

PrinterESCPOS::PrinterESCPOS (QIODevice* _Device) 
:TicketPrinter(_Device), CurrentMode(1)
{
	cmdReset();
}



void PrinterESCPOS::cmdBold ()
{
}

void PrinterESCPOS::cmdItalic ()
{
	//Emphasized.
	CurrentMode |= 8;
	updateMode();	

}

void PrinterESCPOS::cmdUnderline ()
{
	CurrentMode |= 128;
	updateMode();	
}

void PrinterESCPOS::cmdNormal ()
{
	CurrentMode &= ~(8 + 128);
	updateMode();	
	/* Turn of bold, italic, underline */
}

/*!
	_Conector '0' per al pin 2 
	_Connector '1' per al pin 5
	
*/

void PrinterESCPOS::cmdOpenCashDraw(unsigned char _Connector)
{
	putch(27); 
	putch(112);  
	putch(_Connector);
	putch(100);  //Pulse on TIME
	putch(100);  //Pulse off TIME 
}



void PrinterESCPOS::cmdFontSize (int _Width, int _Height)
{
	if (_Width) 
		CurrentMode |= 32;
	else 
		CurrentMode &= ~32;
	
		
	if (_Height) 
		CurrentMode |= 16;
	else 
		CurrentMode &= ~16;
	updateMode();	
}

void PrinterESCPOS::cmdInvertColors ()
{
	CurrColor ^= 1;
	CurrColor &= 1;
	putch(27); 
	putch(114);
	qDebug("Printing in color %d", CurrColor);
	putch(CurrColor);
/*	if (CurrColor == 0)
		cmdReset();*/
}

/*!
	Formats Data According to _Type.
*/

QString PrinterESCPOS::dataForBarCodeType(EnBarCodeType _Type, const QString& _Data)
{
	QString Res = _Data;
	if (_Type == BCTypeEan13)
		Res = Res.rightJustified(12,'0',true);
	else 
		Res = Res.left(255);
	return Res;
}

int PrinterESCPOS::barCodeTypeId(EnBarCodeType _Type)
{	
	int Res;
	switch (_Type)
	{
		case BCTypeCode39 :
			Res = 69;
		break;
		case BCTypeCode93 : 
			Res = 72;
		break; 
		case BCTypeEan13 : 
			Res = 2;
		break;
	};

	return Res;
}

bool PrinterESCPOS::fixedDataSizeBarCode(EnBarCodeType _Type)
{
	return (_Type == BCTypeEan13);
}

/*!
	\param _Width  2<= _Width <= 6 where 2 i smallest size.

*/
void PrinterESCPOS::cmdBarCodePrint(EnBarCodeType _Type, const QString& _Data, unsigned char _Height, unsigned char _Width)
{
	QString Data = dataForBarCodeType(_Type, _Data);
	
	putch(0x1D);
	putch('h');
	putch(_Height);
	
	putch(0x1D);
	putch('w');
	putch(_Width);
	
	putch(0x1D);
	putch('k');
	putch(barCodeTypeId(_Type));
	if (!fixedDataSizeBarCode(_Type))
		putch(Data.size());
				
	string(Data);
	
	if (fixedDataSizeBarCode(_Type))
		putch(0);
	
	string(QString("(%1)").arg(Data));
}

void PrinterESCPOS::cmdLineFeed ()
{
	putch(0x0A);
}

void PrinterESCPOS::cmdCut(bool _Partial)
{
	putch(29);
	putch(86);
	if (_Partial)
		putch(49);
	else
		putch(48);
}

void PrinterESCPOS::cmdReset()
{
	CurrColor = 0;
	CurrentMode = 1;
	putch(27); 
	putch(64);
}


