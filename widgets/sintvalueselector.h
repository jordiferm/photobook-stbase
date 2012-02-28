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

#ifndef SPRINTVALUESELECTOR_H
#define SPRINTVALUESELECTOR_H

#include <QWidget>
#include <QVariant>
#include "stwidgetsexport.h"

/**
QVariant value selector.
QSqlRelationalDelegate uses combobox cast to get the value from Text. 
Its a probem if we want to map the combo value only. 
So, to solve this this class inherits from QWidget nor from QComboBox.
@author Shadow
*/
class QComboBox;
class ST_WIDGETS_EXPORT SIntValueSelector : public QWidget
{
Q_OBJECT 
Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
        QComboBox* Combo;
public: 
        SIntValueSelector(QWidget* _Parent = 0);
        QVariant value() const;
        void setValue(const QVariant& _Value);
        void addItem(const QString& _Text, const QVariant& _Value);     
signals: 
        void selectedValueChanged();    
};

#endif 
