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

#ifndef TICKETPRINTER_H
#define TICKETPRINTER_H


#include <QString>
#include "stticketprinterexport.h"

/**
	Ticket printer �s un I/O device de tipus s�rie.
*/
class QIODevice;
class ST_TICKETPRINTER_EXPORT TicketPrinter
{

protected:
	QIODevice* Device;

public:
	TicketPrinter(QIODevice* _Device);

	virtual void cmdBold ();
	virtual void cmdItalic ();
	virtual void cmdUnderline ();
	virtual void cmdNormal ();
	virtual void cmdOpenCashDraw(unsigned char _Connector);
	
	virtual void cmdFontSize (int width, int height);

	virtual void cmdLineFeed ();
	//! cmdLineFeed, _Rep Nombre de cops.
	void cmdLineFeedn (int _Rep);
	
	virtual void cmdInvertColors ();

	virtual void cmdCut(bool _Partial = false);
	virtual void cmdReset();
	
	void string(const QString& );
	int putch(int _Char);
	QIODevice* device() { return Device; }
};

#endif
