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
#ifndef STNCOPIESPROXYMODEL_H
#define STNCOPIESPROXYMODEL_H

#include "stcheckedproxymodel.h"
#include "stphototemplatesexport.h"

#include <QMap> 

/**
Proxy model with index counts and checks.

	@author
*/
class ST_PHOTOTEMPLATES_EXPORT STNCopiesProxyModel : public STCheckedProxyModel
{
Q_OBJECT
	typedef QMap<QModelIndex, int> TCopiesMap; 
	TCopiesMap CopiesMap; 

public:
	STNCopiesProxyModel(QObject* _Parent, const QImage& _CheckedImage);
	~STNCopiesProxyModel();
	QVariant data(const QModelIndex& _Index, int _Role) const;
	void setNCopies(const QModelIndex& _Index, int _NCopies);
	int nCopies(const QModelIndex& _Index) const;
	virtual void setChecked(const QModelIndex& _Index, bool _Checked);

public slots:
	void incCheckedCopies(); 
	void incNCopies(const QModelIndex& _Index);
	void decNCopies(const QModelIndex& _Index);

};

#endif
