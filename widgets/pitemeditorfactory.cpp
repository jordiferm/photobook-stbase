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

#include "pitemeditorfactory.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <limits.h> //INT_MAX, INT_MIN, ...

QWidget *PItemEditorFactory::createEditor(QVariant::Type type, QWidget *parent) const
{
	switch (type)
	{
		case QVariant::Bool:
		{
/*			QComboBox *cb = new QComboBox(parent);
			//cb->setFrame(false);
			cb->addItem("False");
			cb->addItem("True");
			return cb;*/
			QCheckBox* ChkB = new QCheckBox(parent);
			return ChkB;
		}
		case QVariant::UInt:
		{
			QSpinBox *sb = new QSpinBox(parent);
			//sb->setFrame(false);
			sb->setMaximum(INT_MAX);
			return sb;
		}
		case QVariant::Int:
		{
			QSpinBox *sb = new QSpinBox(parent);
			//sb->setFrame(false);
			sb->setMinimum(INT_MIN);
			sb->setMaximum(INT_MAX);
			return sb;
		}
		case QVariant::Date:
		{
			QDateTimeEdit *ed = new QDateEdit(parent);
			ed->setDisplayFormat("dd/MM/yyyy");
			//ed->setFrame(false);
			return ed;
		}
		case QVariant::Time:
		{
			QDateTimeEdit *ed = new QTimeEdit(parent);
			//ed->setFrame(false);
			return ed;
		}
		case QVariant::DateTime:
		{
			QDateTimeEdit *ed = new QDateTimeEdit(parent);
			//ed->setFrame(false);
			return ed;
		}
		case QVariant::Pixmap:
			return new QLabel(parent);
		case QVariant::Double:
		{
			QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
			sb->setMinimum(LONG_MIN);
			sb->setMaximum(LONG_MAX);

			//sb->setFrame(false);
			return sb;
		}
		case QVariant::StringList:
		{
			PComboBox *cb = new PComboBox(parent);
			//cb->setFrame(false);
			return cb;
		}
		case QVariant::String:
		default:
		{
			// the default editor is a lineedit
			QLineEdit *le = new QLineEdit(parent);
			//le->setFrame(false);
			return le;
		}
	}
}

QByteArray PItemEditorFactory::valuePropertyName(QVariant::Type type) const
{
	switch (type)
	{
		case QVariant::Bool:
		return "checked";
		case QVariant::UInt:
		case QVariant::Int:
		case QVariant::Double:
		return "value";
		case QVariant::Date:
		return "date";
		case QVariant::Time:
		return "time";
		case QVariant::DateTime:
		return "dateTime";
		case QVariant::StringList:
		return "contents";
		case QVariant::Pixmap:
		return "pixmap";
		case QVariant::String:
		default:
		// the default editor is a lineedit
		return "text";
	}
}

