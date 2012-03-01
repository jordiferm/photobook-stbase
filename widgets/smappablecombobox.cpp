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
#include "smappablecombobox.h"


SMappableComboBox::SMappableComboBox(QWidget *parent)
 : QComboBox(parent), UseTextAsData(false), ValueModelColumn(0), ValsFromModel(false)
{
	//setFocusPolicy(Qt::NoFocus);
}

QVariant SMappableComboBox::value() const
{
	QVariant Res; 
	if (ValsFromModel)
	{
		if (model())
			Res = model()->data(model()->index(currentIndex(), ValueModelColumn));
	}
	else 
	{
		if (UseTextAsData)
			Res = currentText();
		else 
			Res = itemData(currentIndex());	
	}
	return Res; 
}

void SMappableComboBox::setValue(const QVariant& _Value)
{
	if (UseTextAsData)
		setCurrentIndex(findText(_Value.toString())); 
	else 
		setCurrentIndex(findData(_Value)); 
}
