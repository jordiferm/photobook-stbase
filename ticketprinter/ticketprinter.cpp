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

#include "ticketprinter.h"
#include <QIODevice>
//#include "error.h"

TicketPrinter::TicketPrinter(QIODevice* _Device) : Device(_Device)
{
}

void TicketPrinter::cmdBold ()
{}

void TicketPrinter::cmdItalic ()
{}

void TicketPrinter::cmdUnderline ()
{}

void TicketPrinter::cmdNormal ()
{
	/* Turn of bold, italic, underline */
}

void TicketPrinter::cmdOpenCashDraw(unsigned char /*_Connector*/)
{}

void TicketPrinter::cmdFontSize (int , int )
{}

void TicketPrinter::cmdInvertColors ()
{}


void TicketPrinter::cmdLineFeed ()
{
}

void TicketPrinter::cmdLineFeedn (int _Rep)
{
	for (int Vfor = 0; Vfor < _Rep; Vfor++)
		cmdLineFeed();
}

void TicketPrinter::cmdCut(bool /*_Partial*/)
{}

void TicketPrinter::cmdReset()
{}

void TicketPrinter::string(const QString& _Str)
{
	Device->write(_Str.toAscii().data(), _Str.length());
}

int TicketPrinter::putch(int _Char)
{
	bool Res;
    //!!!!!!!!!!!!!El putchar no furrula, s-ha de mirar
	//Res = this->putChar(_Char);
	//Res = this->putChar(_Char);

	Res = Device->putChar(_Char);
/*    #ifndef Q_OS_WIN32
	Res = putChar(_Char);
    #else
        QextSerialPort::putch(_Char);
    #endif*/
    //Xo tpoc> Res = writeData(reinterpret_cast<const char*>(&_Char), 2) == 2;
	return Res;
}

