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
#include "ordertransfermodel.h"
#include <QDir> 
#include <QFileInfoList>

#include "stutils.h" 
#include "stcollectionpublishermodel.h"
#include "xmlorder.h"

QString OrderTransferModel::idToKey(const QString& _OrderId)
{
	return QString("%1").arg(_OrderId, 10, QChar('0'));
}

OrderTransferModel::OrderTransferModel(QObject* parent): QStandardItemModel(parent)
{
	setColumnCount(ColEta +1); 
	setHeaderData(ColName, Qt::Horizontal, tr("Id", "i.e: file name"));
	setHeaderData(ColDateTime, Qt::Horizontal, tr("C.Date", "i.e: creation datetime"));
	setHeaderData(ColSize, Qt::Horizontal, tr("Size", "i.e: file size"));
	setHeaderData(ColState, Qt::Horizontal, tr("State", "i.e: Connecting"));
	setHeaderData(ColProgress, Qt::Horizontal, tr("Overall Progress", "i.e: % downloaded"));
	setHeaderData(ColUpSpeed, Qt::Horizontal, tr("UP Speed", "i.e: Upload speed"));
	setHeaderData(ColPriority, Qt::Horizontal, tr("Priority", "i.e: order number"));
	setHeaderData(ColKey, Qt::Horizontal, tr("Key", "i.e: 0000046"));
	setHeaderData(ColEta, Qt::Horizontal, tr("ETA", "i.e: Estimated Time of Arrival / Time left"));
}

int OrderTransferModel::getRowFromOrder(const QString& _OrderId)
{
	int Res = -1; 
	QModelIndexList MatchList = match(index(0,ColKey), Qt::DisplayRole, idToKey(_OrderId), Qt::MatchFixedString ); 
	if (!MatchList.isEmpty()) 
		Res = MatchList.first().row(); 

	return Res; 
}

QString OrderTransferModel::getOrderFromRow(int _Row)
{
	return data(index(_Row, ColName)).toString(); 
}

bool OrderTransferModel::isTherePendingOrders()
{
	bool Found = false;
	QDir Dir(STDom::XmlOrderInfo::defaultOrderFilesPath());
	QFileInfoList OrderDirs = Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
	//clear();
	//removeRows(0, rowCount());
	if (OrderDirs.size() > 0)
	{

		QFileInfoList::iterator it = OrderDirs.begin();
		while (it != OrderDirs.end() && !Found)
		{
			QDir CurrDir(it->absoluteFilePath());
			Found = (QFile::exists(CurrDir.absoluteFilePath(STDom::XmlOrderInfo::orderXmlFileName()))); //To be sure that order is entirelly saved at disk.
			++it;
		}
	}
	return Found;
}

void OrderTransferModel::updateOrders()
{
	QDir Dir(STDom::XmlOrderInfo::defaultOrderFilesPath());
	QFileInfoList OrderDirs = Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time); 
	//clear();
	//removeRows(0, rowCount()); 
	if (OrderDirs.size() > 0)
	{
		QFileInfoList::iterator it; 
		for (it = OrderDirs.begin(); it != OrderDirs.end(); ++it)
		{
			QDir CurrDir(it->absoluteFilePath());
			if (QFile::exists(CurrDir.absoluteFilePath(STDom::XmlOrderInfo::orderXmlFileName()))) //To be sure that order is entirelly saved at disk.
			{
				QString OrderId = it->fileName(); 
				QString OrderKey = idToKey(OrderId); 
				if (getRowFromOrder(OrderId) == -1) //If the order does not exist.
				{
					insertRow(0);
					setData(index(0, ColName), OrderId);
					setData(index(0, ColDateTime), it->created());
					setData(index(0, ColSize), STUtils::dirSize(it->absoluteFilePath()));
					setData(index(0, ColState), tr("Waiting...")); 
					setData(index(0, ColKey), OrderKey); 
					setData(index(0, ColPriority), OrderKey); 
				}
				else 
					qWarning(QString("Order already exists %1").arg(OrderId).toLatin1());
	
				//QString CurrOrderId = it->fileName();
				//STFtpOrderTransferDialog::execTransferOrder(CurrOrderId, this);
			}
		}
	}
	sort(ColPriority); 
}

QString OrderTransferModel::getNextOrderId()
{
	QString Res; 
	if (rowCount() > 0)
		Res = data(index(0, ColName), Qt::DisplayRole).toString();
	return Res; 
}

void OrderTransferModel::removeOrder(const QString& _OrderId)
{
	int Row = getRowFromOrder(_OrderId); 
	if ( Row != -1) 
		removeOrderByRow(Row); 
}

void OrderTransferModel::removeAllOrders()
{
	OrderTransferModel Model;
	Model.updateOrders();
	while (Model.rowCount() > 0)
	{
		Model.removeOrderByRow(0);
	}
}

void OrderTransferModel::setOrderData(const QString& _OrderId, OrderTransferModel::EnModelCols _Column, const QVariant& _Data, int _Role)
{
	int Row = getRowFromOrder(_OrderId); 
	if ( Row != -1) 
	{
		setData(index(Row, _Column), _Data, _Role); 	
	}
}

QVariant OrderTransferModel::orderData(const QString& _OrderId, OrderTransferModel::EnModelCols _Column, int _Role)
{
	QVariant Res;
	int Row = getRowFromOrder(_OrderId); 
	if ( Row != -1) 
		Res = data(index(Row, _Column), _Role); 	
	return Res; 
}

void OrderTransferModel::setTransferStatus(const QString& _OrderId, EnTransferStatus _Status)
{
	int Row = getRowFromOrder(_OrderId); 
	if ( Row != -1) 
	{
		QColor BgStatusColor = Qt::white;
		switch (_Status)
		{	
			case StatusStopped :
				BgStatusColor = Qt::white; 
			break; 
			case StatusTransfering :
				BgStatusColor = QColor("#B1FF07"); 
			break; 
			case StatusError :
				BgStatusColor = Qt::red; 
			break; 
		}
		for (int Vfor = 0; Vfor < columnCount(); Vfor++)
			setData(index(Row, Vfor), BgStatusColor, Qt::BackgroundColorRole); 	
	}
}

void OrderTransferModel::moveBottom(const QString& _OrderId)
{
	QString CurrPriority = orderData(_OrderId, ColPriority).toString(); 
	setOrderData(_OrderId, ColPriority, "1" + CurrPriority); 
	sort(ColPriority); 
}


void OrderTransferModel::setOrderErrorDesc(const QString& _OrderId, const QString& _ErrorDesc)
{
	setOrderData(_OrderId, ColState, _ErrorDesc, Qt::ToolTipRole); 
}

void OrderTransferModel::removeOrderByRow(int _Row)
{
	STUtils::rmDir(QDir(STDom::XmlOrderInfo::defaultOrderPath(getOrderFromRow(_Row))));
	removeRows(_Row, 1);
}
