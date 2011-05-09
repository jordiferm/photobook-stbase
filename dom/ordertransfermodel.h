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
#ifndef ORDERTRANSFERMODEL_H
#define ORDERTRANSFERMODEL_H

#include <QStandardItemModel>

/**
Model to handle starblitz photocollection orders status.

	@author
*/
class OrderTransferModel : public QStandardItemModel
{
	Q_OBJECT

public:
	enum EnModelCols
	{
		ColName = 0,
		ColDateTime,
		ColSize,
		ColState,
		ColProgress,
		ColUpSpeed,
		ColPriority, 
		ColKey, 
		ColEta
	};
	enum EnTransferStatus
	{
		StatusStopped, 
		StatusTransfering, 
		StatusError
	};

	QString idToKey(const QString& _OrderId);

public:
	OrderTransferModel(QObject* parent = 0);
	static bool isTherePendingOrders();
	int getRowFromOrder(const QString& _OrderId);
	QString getOrderFromRow(int _Row);
	//! Reads pendant orders from disk.
	void updateOrders(); 
	QString getNextOrderId(); 
	void removeOrder(const QString& _OrderId); 
	static void removeAllOrders();

	void setOrderData(const QString& _OrderId, EnModelCols _Column, const QVariant& _Data, int _Role = Qt::DisplayRole);
	QVariant orderData(const QString& _OrderId, OrderTransferModel::EnModelCols _Column, int _Role = Qt::DisplayRole);
	void setTransferStatus(const QString& _OrderId, EnTransferStatus _Status);
	void moveBottom(const QString& _OrderId);
	void setOrderErrorDesc(const QString& _OrderId, const QString& _ErrorDesc);
	void removeOrderByRow(int _Row); 
};

#endif
