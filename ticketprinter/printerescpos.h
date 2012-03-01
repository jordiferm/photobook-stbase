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

#ifndef PRINTERESCPOS_H
#define PRINTERESCPOS_H
#include "ticketprinter.h"
#include "stticketprinterexport.h"

class ST_TICKETPRINTER_EXPORT PrinterESCPOS : public TicketPrinter
{
public:
	enum EnBarCodeType
	{
		BCTypeEan13,
		BCTypeCode39, 
		BCTypeCode93
	};
	
private:	
	unsigned char CurrentMode; 
	unsigned char CurrColor; 

	void updateMode(); 
	
public:
	PrinterESCPOS (QIODevice* _Device);

	virtual void cmdBold ();
	virtual void cmdItalic ();
	virtual void cmdUnderline ();
	virtual void cmdNormal ();
	virtual void cmdOpenCashDraw(unsigned char _Connector);

	virtual void cmdFontSize (int width, int height);
		
	// Barcode 
	QString dataForBarCodeType(EnBarCodeType _Type, const QString& _Data);
	int barCodeTypeId(EnBarCodeType _Type);
	bool fixedDataSizeBarCode(EnBarCodeType _Type);
	void cmdBarCodePrint(EnBarCodeType _Type, const QString& _Data, unsigned char _Height = 80, unsigned char _Width = 3);
	
	virtual void cmdLineFeed ();
	virtual void cmdInvertColors ();

	virtual void cmdCut(bool _Partial = false);
	virtual void cmdReset(); 
};

#endif
