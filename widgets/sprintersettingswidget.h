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
#ifndef SPRINTERSETTINGSWIDGET_H
#define SPRINTERSETTINGSWIDGET_H

#include <QModelIndex> 

#if QT_VERSION >= 0x040400
 #include <QPrinterInfo> 
#endif 

#include "smappedwidget.h"
#include "stwidgetsexport.h"
#include "sprintersettings.h"

/**
Widget class for product and printer selector.
 
@author Shadow
*/

class QComboBox; 
class QLabel; 
class ProdPrintSelector : public QWidget 
{
Q_OBJECT
	QComboBox* CBProduct; 
	QComboBox* CBPrinters; 
	QLabel* PrinterLabel;

public:
	ProdPrintSelector(QWidget* _Parent = 0); 
	void setProduct(const QModelIndex& _Index); 
	QModelIndex product() const; 
	void setProductModel(QAbstractItemModel* _Model); 
	void setProductModelColumn(int _VisibleColumn); 
	void setPrinterName(const QString& _PrinterName);
	QString printerName() const; 
#if QT_VERSION >= 0x040400
	void setPrinterInfo(const QList<QPrinterInfo>& _PrinterList);
#endif 

private slots: 
	void getPrinterName(); 
	void slotDeleteMe();

signals: 
	void removeMe(ProdPrintSelector* _Me); 	
};

/**
Widget for n productPrints.
 
@author Shadow
*/

class QVBoxLayout;
class QAbstractItemModel;
class ProdPrintSettingsWidget : public QWidget 
{
Q_OBJECT
	QVBoxLayout* MLayout;
	QAbstractItemModel* ProdModel; 
	int DisplayCol; 
	int KeyCol; 
	QList<ProdPrintSelector*> ProdPrSelList; 
#if QT_VERSION >= 0x040400
	QList<QPrinterInfo> PrinterList;
#endif 

public: 
	ProdPrintSettingsWidget(QWidget* _Parent = 0); 
#if QT_VERSION >= 0x040400
	void setPrinterList(QList<QPrinterInfo> _PrinterList);
#endif 
	void setProductModel(QAbstractItemModel* _Model, int _DisplayCol, int _KeyCol); 
	void addLine(const QString& _ProdId, const QString& _PrinterName); 
	void clear();
	int linesCount() const;
	QString prodId(int _Index) const;
	QString printerName(int _Index) const;

private slots: 
	ProdPrintSelector* slotAddLine();
	void slotDeleteLine(ProdPrintSelector* _LineWidget); 
};


/**
Widget class for printer settings.
 
@author Shadow
*/

class FileSelectLineEdit; 
class ST_WIDGETS_EXPORT SPrinterSettingsWidget : public SMappedWidget
{
	Q_OBJECT

	SAppSettingsModel* Model;
	QModelIndex PrnNameIndex;
	QModelIndex ReportFileIndex;
	FileSelectLineEdit* LERFile;	
	ProdPrintSettingsWidget* MProdPrSettingsW;
	ProdPrintSettingsWidget* MFormatPrSettingsW;

public:	
	SPrinterSettingsWidget(SPrinterSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent = 0, bool _LabEnabled = true);
	void setProductModel(QAbstractItemModel* _Model, int _DisplayCol, int _KeyCol); 
	void setFormatsModel(QAbstractItemModel* _Model, int _DisplayCol, int _KeyCol);
	void saveProductPrinters();
	void loadProductPrinters(); 
	void saveFormatPrinters();
	void loadFormatPrinters();
	virtual QSize sizeHint () const { return QSize(400, 800); }

public slots: 
	void getPrinterName(); 
};


#endif
