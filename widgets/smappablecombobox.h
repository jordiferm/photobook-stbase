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
#ifndef SMAPPABLECOMBOBOX_H
#define SMAPPABLECOMBOBOX_H

#include <QComboBox>
#include "stwidgetsexport.h"

/**
ComboBox with an USER property to map user values.

	@author 
*/
class ST_WIDGETS_EXPORT SMappableComboBox : public QComboBox
{
Q_OBJECT 
Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
	
	bool UseTextAsData; 
	int ValueModelColumn; 
	bool ValsFromModel; 
				
public:	
	SMappableComboBox(QWidget* _Parent = 0);
	QVariant value() const;
	void setValue(const QVariant& _Value);
	//! Text values of the combo are used as data.(By default this value is set to false)
	void setUseTextAsData(bool _Value) { UseTextAsData = _Value; }
	bool useTextAsData() const { return UseTextAsData; }
	void setGetValuesFromModel(bool _Values) { ValsFromModel = _Values; } 
	void setValueModelColumn(int _Column) { ValueModelColumn = _Column; }
};

#endif
