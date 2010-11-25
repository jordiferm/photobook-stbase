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
#include "usbprinterdevice.h"

extern "C"
{
	#include <usb.h>
}


usb_dev_handle* USBPrinterDevice::locate_xsv(int _IdVendor, int _IdProduct) 
{
	unsigned char located = 0;
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *device_handle = 0;
 		
	usb_find_busses();
	usb_find_devices();
 
 	for (bus = usb_busses; bus; bus = bus->next)
	{
		for (dev = bus->devices; dev; dev = dev->next)	
		{
			if (dev->descriptor.idVendor == _IdVendor && dev->descriptor.idProduct == _IdProduct) 
			{	
				located++;
				device_handle = usb_open(dev);
			}
		}	
	}

	return (device_handle);  	
}


USBPrinterDevice::USBPrinterDevice(int _IdVendor, int _IdProduct, QObject* parent): QIODevice(parent), 
											  IdVendor(_IdVendor), IdProduct(_IdProduct), WriteTimeOut(500)
{
	setTextModeEnabled(true); 
	setOpenMode(QIODevice::WriteOnly); 
}


USBPrinterDevice::~USBPrinterDevice()
{
	close(); 
}


bool USBPrinterDevice::open(OpenMode _Mode)
{
	int OpenStatus;

	usb_init();
	usb_set_debug(0);
 	if ((DeviceHandle = locate_xsv(IdVendor, IdProduct))==0) 
	{
		qWarning("USBPrinterDevice:Could not open the USB device.");
		return false;
	}  
	else
	{
		OpenStatus = usb_set_configuration(DeviceHandle,1);
		if (OpenStatus == 0)
			OpenStatus = usb_claim_interface(DeviceHandle,0);
		
		if (OpenStatus == 0)
			OpenStatus = usb_set_altinterface(DeviceHandle,0);
	}
	return OpenStatus == 0;
}

bool USBPrinterDevice::reset()
{
	return QIODevice::reset();
}

qint64 USBPrinterDevice::readData(char* data, qint64 maxlen)
{
	return 0; 
}

qint64 USBPrinterDevice::writeData(const char* data, qint64 len)
{
	usb_bulk_write(DeviceHandle, 1, const_cast<char*>(data), len, WriteTimeOut);	
}

void USBPrinterDevice::close()
{
	usb_release_interface(DeviceHandle,0);
}

