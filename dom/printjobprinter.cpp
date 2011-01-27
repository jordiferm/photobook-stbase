/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "printjobprinter.h"
#include <QProgressBar>
#include <QApplication>
#include <QPrinter>
#include <QPainter>
#include <QRect>
#include <QFileInfoList>
#include <QDebug>
#include "sprintersettings.h"
#include "printjob.h"
#include "stimage.h"
#include "ddoc.h"
#include "printkeeper.h"
#include "sapplication.h"

using namespace STDom;

/*!
  Stores _Job images into _Dir and changes _Job paths. If image size is bigger than the size needed to print biggest
  format in current dpis it reduces it at optimal size.
  \note With printJob we have to pass the images already cropped. If we have a printJob with 2 different formats of the same image and perhaps different crops of it we need to
  copy this image using PrintJob copy method. This is because PrintJob assumes tha images are the part of the image that user wants to print.
  */
STDom::PrintJob PrintJobPrinter::storeImages(const STDom::PrintJob& _Job, const QDir& _DestDir, QProgressBar* _ProgBar)
{
	STDom::PrintJob Res;

	QFileInfoList Files = _Job.files();
	QFileInfoList::iterator it;
	STImage CImage;
	STImage StoreImage;

	if (!_DestDir.exists())
		StackAssert(_DestDir.mkpath(_DestDir.absolutePath()), Error(QObject::tr("Could not create order path")), ErrorStack);

	if (_ProgBar)
	{
		_ProgBar->setRange(0, Files.size());
		_ProgBar->setValue(0);
		QApplication::processEvents();
	}

	for (it = Files.begin(); it != Files.end(); ++it)
	{
		QFileInfo CurrentFile = *it;

		if (CImage.load(CurrentFile.absoluteFilePath()))
		{
			DDocFormat BiggestFormat = _Job.formats(CurrentFile).biggestFormat();
			QSize FormatSize =  BiggestFormat.pixelSize(Dpis);

			QSize ImageSize (CImage.size());
			if ((ImageSize.width() > ImageSize.height() && FormatSize.width() <= FormatSize.height()) ||
				(ImageSize.height() > ImageSize.width() && FormatSize.height() <= FormatSize.width()))
				FormatSize.transpose();
			//Only if image size is bigger we resize it to minimize image weight.
			if (ImageSize.width() > FormatSize.width() && ImageSize.height() > FormatSize.height())
				StoreImage = CImage.scaled(FormatSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
			else
				StoreImage = CImage;

			QString ImageFileName = StoreImage.hashString() + "." + it->completeSuffix();
			//To preserve printSize
			StoreImage.setDotsPerMeterX(CImage.dotsPerMeterX());
			StoreImage.setDotsPerMeterY(CImage.dotsPerMeterY());

			QString DestFilePath = _DestDir.absoluteFilePath(ImageFileName);
			if (!StoreImage.save( DestFilePath, 0, 100))
				ErrorStack.push(Error(QString(QObject::tr("Error storing image %1")).arg(DestFilePath)));

			DDocPrintList Prints = _Job.prints(CurrentFile);
			DDocPrintList::iterator it;
			for (it = Prints.begin(); it != Prints.end(); ++it)
			{
				STDom::DDocPrint Print(QFileInfo(DestFilePath), it->product(), it->numCopies());
				Print.setBackPrintText(it->backPrintText());
				Res.addPrint(Print);
				//Res.addCopies(it->product(), QFileInfo(DestFilePath), it->numCopies());
			}
		}
		else
			ErrorStack.push(Error(QString("Error loading File %1").arg(CurrentFile.absoluteFilePath())));
		if (_ProgBar)
		{
			_ProgBar->setValue(_ProgBar->value() + 1);
			QApplication::processEvents();
		}
	}
	return Res;
}

PrintJobPrinter::PrintJobPrinter(int _Dpis) : Dpis(_Dpis)
{
}

QString PrintJobPrinter::productSpool(const DDocProduct& _Product) const
{
	QString Res = DefaultSpool;
	if (ProductSpoolMap.contains(_Product))
		Res = ProductSpoolMap[_Product];
	return Res;
}

void PrintJobPrinter::setProductSpool(const DDocProduct& _Product, const QString& _Printer)
{
	ProductSpoolMap.insert(_Product, _Printer);
}

void PrintJobPrinter::configure(SPrinterSettings& _Settings)
{
	clearConfig();
	if (_Settings.printerByProductEnabled())
	{
		for (int Vfor = 0; Vfor < _Settings.numProdPrinters(); Vfor++)
		{
			setProductSpool(_Settings.prodId(Vfor), _Settings.printerName(Vfor));
		}
	}
	setDefaultSpool(_Settings.systemPrinterName());
}

void PrintJobPrinter::storeProduct(const DDocProduct& _Product)
{
	ProductToStoreList.push_back(_Product);
}

bool PrintJobPrinter::hasToStore(const DDocProduct& _Product) const
{
	return ProductToStoreList.contains(_Product);
}

void PrintJobPrinter::clearConfig()
{
	ProductToStoreList.clear();
	ProductSpoolMap.clear();
}


PrintJob PrintJobPrinter::print(const PrintJob& _Job, const QString& _JobName, QProgressBar* _ProgBar)
{
	PrintJob JobToStore;

	if (_ProgBar)
	{
		_ProgBar->setRange(0, _Job.totalCopies() - 1);
		_ProgBar->setValue(0);
		QApplication::processEvents();
	}

	STDom::DDocProductList ProdList = _Job.products();
	STDom::DDocProductList::iterator it;
	it = ProdList.begin();
	while (it != ProdList.end()) //For each product
	{
		if (hasToStore(it->ref()))
		{
			STDom::DDocPrintList ProdPrints = _Job.prints(*it);
			STDom::DDocPrintList::iterator pit;
			pit = ProdPrints.begin();
			while ( pit != ProdPrints.end()) //For each print
			{
				JobToStore.addPrint(*pit);
			}
		}
		else
		{
			QPrinter Printer;
			Printer.setPrinterName(productSpool(it->ref()));
			bool PrintAccess = true;

	#ifdef USE_PRINTKEEPER
			if (!PrintKeeper::printAcces(Printer.printerName()))
			{
				PrintKeeper::accesDeniedWarning(Printer.printerName());
				PrintAccess = false;
			}
	#endif
			if (PrintAccess)
			{
	#if QT_VERSION >= 0x040400
				Printer.setPaperSize(it->format().size(), QPrinter::Millimeter);
		#endif
				Printer.setDocName(_JobName);
				Printer.setPrintProgram(SApplication::fullApplicationName());
				Printer.setResolution(Dpis);
				Printer.setFullPage(true);
				Printer.setNumCopies(1);
				QPainter Painter(&Printer);

				STDom::DDocPrintList ProdPrints = _Job.prints(*it);
				STDom::DDocPrintList::iterator pit;
				pit = ProdPrints.begin();
				while ( pit != ProdPrints.end()) //For each print
				{
					STImage CImage;
					//Printer.setNumCopies(pit.value());
					if (CImage.load(pit->fileInfo().absoluteFilePath()))
					{
						int NumCopies = pit->numCopies();
						while (NumCopies > 0)
						{
							CImage.print(Painter, FitMode == FitModeWhiteMarginCenter, FitMode == FitModeCrop);
							NumCopies--;
							if (NumCopies > 0 )
								Printer.newPage();
						}
						++pit;
						if (pit != ProdPrints.end())
							Printer.newPage();
						if (_ProgBar)
						{
							_ProgBar->setValue(_ProgBar->value() + 1);
							QApplication::processEvents();
						}
					}
					else
					{
						ErrorStack.push(Error(QObject::tr("Error loading image %1").arg(pit->fileInfo().absoluteFilePath())));
						++pit;
					}
				}
			}//PrintAccess
		}//hasToStore
		++it;
	}

	return JobToStore;
}

void PrintJobPrinter::store(const STDom::PrintJob& _Job, STDom::XmlOrder& _Order, bool _FitImagesToFormat, QProgressBar* _ProgBar)
{
	if (_FitImagesToFormat)
	{
		STDom::PrintJob StoredJob = storeImages(_Job, _Order.orderInfo().orderPath(), _ProgBar);
		StoredJob.addOrderPrints(_Order);
		_Order.clearPrintPath(); //To make paths relative to order dir
		_Order.saveXml();
		ErrorStack += _Order.errorStack();
	}
	else
	{
		_Job.addOrderPrints(_Order);
		_Order.saveFiles(_ProgBar);
		_Order.clearPrintPath(); //To make paths relative to order dir
		_Order.saveXml();
		ErrorStack += _Order.errorStack();
	}
}

void PrintJobPrinter::clearErrorStack()
{
	ErrorStack.clear();
}
