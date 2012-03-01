/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef PRINTJOBPRINTER_H
#define PRINTJOBPRINTER_H

#include <QMap>
#include "stdomexport.h"
#include "ddoc.h"
#include "sterrorstack.h"
#include "sterror.h"
#include "printjob.h"
#include "xmlorder.h"

class SPrinterSettings;
class QProgressBar;
namespace STDom
{

class ST_DOM_EXPORT PrintJobPrinter
{
	ST_DECLARE_ERRORCLASS();
public:
	enum EnFitMode {
		FitModeCrop, //! Uses crop rect or crop by center of image if there is no crop rect.
		FitModeWhiteMarginRight,
		FitModeWhiteMarginCenter
	};

private:

	typedef QMap<DDocProduct, QString> TProductSpoolMap;
	typedef QMap<QString, QString> TFormatSpoolMap;
	typedef QList<DDocProduct> TProductToStoreList;
	int Dpis;
	bool AtomicPrint;
	STErrorStack ErrorStack;
	EnFitMode FitMode;
	TProductSpoolMap ProductSpoolMap;
	TFormatSpoolMap FormatSpoolMap;
	TProductToStoreList ProductToStoreList;
	QString DefaultSpool;
	static QString EncodeKey;

	void printProductPrints(QPrinter& _Printer, STDom::DDocPrintList& _ProdPrints, const STDom::DDocProduct& _Product,
							const QString& _JobName, QProgressBar* _ProgBar);
	void printAtomic(QPrinter& _Printer, STDom::DDocPrintList& _ProductPrints, QProgressBar* _ProgBar);
	void printNonAtomic(QPrinter& _Printer, STDom::DDocPrintList& _ProductPrints, QProgressBar* _ProgBar);
	STDom::PrintJob storeImages(const STDom::PrintJob& _Job, const QDir& _DestDir, QProgressBar* _ProgBar = 0, bool _Encode = false);

public:
	PrintJobPrinter(int _Dpis = 120);

	//Printer config
	void setDpis(int _Dpis) { Dpis = _Dpis; }
	int dpis() const { return Dpis; }
	void setFitMode(EnFitMode _Mode) { FitMode = _Mode; }
	EnFitMode fitMode() const { return FitMode; }

	//Print Operations
	QString productSpool(const DDocProduct& _Product) const;
	void setProductSpool(const DDocProduct& _Product, const QString& _Printer);
	void setFormatSpool(const QString& _FormatKey, const QString& _Printer);
	void setDefaultSpool(const QString& _SpoolName) { DefaultSpool = _SpoolName; }
	QString defaultSpool() const { return DefaultSpool; }
	void configure(SPrinterSettings& _Settings);
	void storeProduct(const DDocProduct& _Product);
	bool hasToStore(const DDocProduct& _Product) const;
	void clearConfig();
	void setAtomicPrint(bool _Atomic) { AtomicPrint = _Atomic; }
	bool atomicPrint() { return AtomicPrint; }
	void printToPrinter(QPrinter& _Printer, const PrintJob& _Job, const QString& _JobName = "noname", QProgressBar* _ProgBar = 0);
	PrintJob print(const PrintJob& _Job, const QString& _JobName = "noname", QProgressBar* _ProgBar = 0);
	//! If _FitImagestoFormat is false PrintJobPrinter don't modify images, else it uses FitMode, dpis and format to generate new images.
	void store(const PrintJob& _Job, STDom::XmlOrder& _Order, bool _FitImagesToFormat = true, const QString& _DatabaseFilePath = "", QProgressBar* _ProgBar = 0);
	static void setEncodeKey(const QString& _Value) { EncodeKey = _Value; }
	static QString encodeKey() { return EncodeKey; }

	void storeEncoded(const STDom::PrintJob& _Job, STDom::XmlOrder& _Order, const QDir& _DestinationDir, QProgressBar* _ProgBar = 0);

	//Error handling
	STErrorStack errorStack() const { return ErrorStack; }
	void clearErrorStack();

};

} //STDom namespace

#endif // PRINTJOBPRINTER_H
