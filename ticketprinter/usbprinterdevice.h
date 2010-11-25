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
#ifndef USBPRINTERDEVICE_H
#define USBPRINTERDEVICE_H

#include <QIODevice>
#include "stticketprinterexport.h"

/**
	@author
*/
class usb_dev_handle; 
class ST_TICKETPRINTER_EXPORT USBPrinterDevice : public QIODevice
{
	usb_dev_handle* DeviceHandle;
	int IdVendor, IdProduct; 
	int WriteTimeOut; 
	
	usb_dev_handle* locate_xsv(int _IdVendor, int _IdProduct);
	
public:
	USBPrinterDevice(int _IdVendor, int _IdProduct, QObject* parent = 0);
	~USBPrinterDevice();

	virtual bool open(OpenMode mode);
	virtual bool reset();
	virtual qint64 readData(char* data, qint64 maxlen);
	virtual qint64 writeData(const char* data, qint64 len);
	virtual void close();

};

#endif
