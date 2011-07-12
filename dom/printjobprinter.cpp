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
#ifdef USE_PRINTKEEPER
	#include "printkeeper.h"
#endif
#include "sapplication.h"

using namespace STDom;


void PrintJobPrinter::printProductPrints(QPrinter& _Printer, STDom::DDocPrintList& _ProdPrints, const STDom::DDocProduct& _Product,
										 const QString& _JobName, QProgressBar* _ProgBar)
{
	bool PrintAccess = true;

#ifdef USE_PRINTKEEPER
	if (!PrintKeeper::printAcces(_Printer.printerName(), _ProdPrints.numCopies()))
	{
		PrintKeeper::accesDeniedWarning(_Printer.printerName());
		PrintAccess = false;
	}
#endif
	if (PrintAccess)
	{
#if QT_VERSION >= 0x040400
		_Printer.setPaperSize(_Product.format().size(), QPrinter::Millimeter);
	#endif
		_Printer.setDocName(_JobName);
		_Printer.setPrintProgram(SApplication::fullApplicationName());
		_Printer.setResolution(Dpis);
		_Printer.setFullPage(true);
		_Printer.setNumCopies(1);

		if (AtomicPrint)
			printAtomic(_Printer, _ProdPrints, _ProgBar);
		else
			printNonAtomic(_Printer, _ProdPrints, _ProgBar);

	}//PrintAccess
}


void PrintJobPrinter::printAtomic(QPrinter& _Printer, STDom::DDocPrintList& _ProductPrints, QProgressBar* _ProgBar)
{
	STDom::DDocPrintList::iterator pit;
	bool RemainsCopies = true;
	QPainter* Painter = 0;

	while (RemainsCopies)
	{
		if (Painter)
		{
			delete Painter;
			Painter = 0;
		}
		RemainsCopies = false;
		pit = _ProductPrints.begin();
		while ( pit != _ProductPrints.end()) //For each print
		{
			STImage CImage;
			//Printer.setNumCopies(pit.value());
			if (CImage.load(pit->fileInfo().absoluteFilePath()))
			{
				int NumCopies = pit->numCopies();
				if (NumCopies > 0)
				{
					if (!Painter)
						Painter = new QPainter(&_Printer); //We need to start a new job
					RemainsCopies = true;
					CImage.print(*Painter, FitMode == FitModeWhiteMarginCenter, FitMode == FitModeCrop);
					pit->setNumCopies(NumCopies -1);
					//if (pit->numCopies() > 0)
					//	_Printer.newPage();
				}
				if (_ProgBar && pit->numCopies() == 1)
				{
					_ProgBar->setValue(_ProgBar->value() + 1);
					QApplication::processEvents();
				}
				++pit;
				if (pit != _ProductPrints.end() && NumCopies > 0)
					_Printer.newPage();
			}
			else
			{
				ErrorStack.push(Error(QObject::tr("Error loading image %1").arg(pit->fileInfo().absoluteFilePath())));
				++pit;
			}
		}
	}
}

void PrintJobPrinter::printNonAtomic(QPrinter& _Printer, STDom::DDocPrintList& _ProductPrints, QProgressBar* _ProgBar)
{
	QPainter Painter(&_Printer);
	STDom::DDocPrintList::iterator pit;
	pit = _ProductPrints.begin();
	while ( pit != _ProductPrints.end()) //For each print
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
					_Printer.newPage();
			}
			if (_ProgBar)
			{
				_ProgBar->setValue(_ProgBar->value() + pit->numCopies());
				QApplication::processEvents();
			}

			++pit;
			if (pit != _ProductPrints.end())
				_Printer.newPage();
		}
		else
		{
			ErrorStack.push(Error(QObject::tr("Error loading image %1").arg(pit->fileInfo().absoluteFilePath())));
			++pit;
		}
	}
}

/*!
  Stores _Job images into _Dir and changes _Job paths. If image size is bigger than the size needed to print biggest
  format in current dpis it reduces it at optimal size.
  \note With printJob we have to pass the images already cropped. If we have a printJob with 2 different formats of the same image and perhaps different crops of it we need to
  copy this image using PrintJob copy method. This is because PrintJob assumes tha images are the part of the image that user wants to print.
  */
STDom::PrintJob PrintJobPrinter::storeImages(const STDom::PrintJob& _Job, const QDir& _DestDir, QProgressBar* _ProgBar, bool _Encode)
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

			//To preserve printSize
			StoreImage.setDotsPerMeterX(CImage.dotsPerMeterX());
			StoreImage.setDotsPerMeterY(CImage.dotsPerMeterY());

			QString ImageFileName;
			if (_Encode)
			{
				StoreImage.blowFishEncode(PUBLISHER_KEY);
				ImageFileName = StoreImage.hashString() + ".PNG";
			}
			else
				ImageFileName = StoreImage.hashString() + "." + it->completeSuffix();


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

	if(!_Product.format().key().isEmpty())
	{
		if (FormatSpoolMap.contains(_Product.format().key()))
			Res = FormatSpoolMap[_Product.format().key()];
	}

	if (ProductSpoolMap.contains(_Product))
		Res = ProductSpoolMap[_Product];
	return Res;
}

void PrintJobPrinter::setProductSpool(const DDocProduct& _Product, const QString& _Printer)
{
	ProductSpoolMap.insert(_Product, _Printer);
}

void PrintJobPrinter::setFormatSpool(const QString& _FormatKey, const QString& _Printer)
{
	FormatSpoolMap.insert(_FormatKey, _Printer);
}


void PrintJobPrinter::configure(SPrinterSettings& _Settings)
{
	clearConfig();

	if (_Settings.printerByFormatEnabled())
	{
		for (int Vfor = 0; Vfor < _Settings.numFormatPrinters(); Vfor++)
		{
			setFormatSpool(_Settings.formatId(Vfor), _Settings.formatPrinterName(Vfor));
		}
	}

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
	return productSpool(_Product) == SPrinterSettings::LabPrinterName || ProductToStoreList.contains(_Product);
}

void PrintJobPrinter::clearConfig()
{
	ProductToStoreList.clear();
	ProductSpoolMap.clear();
	FormatSpoolMap.clear();
}

void PrintJobPrinter::printToPrinter(QPrinter& _Printer, const PrintJob& _Job, const QString& _JobName, QProgressBar* _ProgBar)
{
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
		STDom::DDocPrintList ProdPrints = _Job.prints(*it);
		printProductPrints(_Printer, ProdPrints, *it, _JobName, _ProgBar);
		++it;
	}
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
		if (hasToStore(*it))
		{
			STDom::DDocPrintList ProdPrints = _Job.prints(*it);
			STDom::DDocPrintList::iterator pit;
			pit = ProdPrints.begin();
			while ( pit != ProdPrints.end()) //For each print
			{
				JobToStore.addPrint(*pit);
				if(_ProgBar)
					_ProgBar->setValue(_ProgBar->value() + pit->numCopies());
				++pit;
			}
		}
		else
		{
			QPrinter Printer;
			Printer.setPrinterName(productSpool(*it));
			STDom::DDocPrintList ProdPrints = _Job.prints(*it);

			printProductPrints(Printer, ProdPrints, *it, _JobName, _ProgBar);

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
	if (PublisherXmlFile.exists())
	{
		QString DestFileName = _Order.orderInfo().orderPath() + "/" + PublisherXmlFile.fileName();
		StackAssert(QFile::copy(PublisherXmlFile.absoluteFilePath(), DestFileName),
					Error(QObject::tr("Error copying file %1 -> %2").arg(PublisherXmlFile.absoluteFilePath()).arg(DestFileName)),
					ErrorStack);
	}

}

void PrintJobPrinter::storeEncoded(const STDom::PrintJob& _Job, STDom::XmlOrder& _Order, const QDir& _DestinationDir, QProgressBar* _ProgBar)
{
	STDom::PrintJob StoredJob = storeImages(_Job, _DestinationDir, _ProgBar, true);
	StoredJob.addOrderPrints(_Order);
	_Order.clearPrintPath(); //To make paths relative to order dir
	_Order.saveXml(_DestinationDir.absoluteFilePath(XmlOrderInfo::orderXmlFileName()));
	ErrorStack += _Order.errorStack();
}

void PrintJobPrinter::clearErrorStack()
{
	ErrorStack.clear();
}
