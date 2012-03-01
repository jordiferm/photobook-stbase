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

#include "fsqlmodelviewutils.h"
#include <QSqlTableModel>
#include <QStringList>
#include <QSqlField>
#include <QSqlRecord>
#include <QTableView>
#include <QSqlRelation>
#include <QHeaderView>

#include "fsqlrelationaltablemodel.h"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QWidget>


/*!
	Borra totes les columnes del model menys les indicades. S'indiquen en un 
	String amb els noms dels camps separats per ','
*/
void FSqlModelViewUtils::setColumns(QSqlTableModel* _Model, const QString& _Cols)
{
	QString Cols = _Cols;
	if (!Cols.isEmpty())
		setColumns(_Model, Cols.replace(" ","").split(","));
}


/*!
	Borra totes les columnes del model menys les indicades. S'indica un StringList amb els noms 
	dels camps.
*/
void FSqlModelViewUtils::setColumns(QSqlTableModel* _Model, const QStringList& _Cols)
{
	if (_Cols.size() > 0)
	{
		QSqlRecord MRecord = _Model->record();
		int Removed = 0;
		int NumCols = _Model->columnCount();
		for (int Vfor = 0; Vfor < NumCols; Vfor++)
		{
			//static_cast<QTableView*>(view())->setColumnHidden(Vfor, !_Cols.contains(MRecord.field(Vfor).name()));
			//Hi ha problemes amb els tableviews i la seleccio quan hi ha columnes amagades.
			if (!_Cols.contains(MRecord.field(Vfor).name()))
			{
				_Model->removeColumn(Vfor - Removed);
				Removed++;
			}
		}
	}
}

void FSqlModelViewUtils::setColumns(QTableView* _View, const QString& _Cols)
{
	QString Cols = _Cols;
	if (!Cols.isEmpty())
		setColumns(_View, Cols.replace(" ","").split(","));
}

/*!
	En un model QSqlRelationalTableModel i Qt <=4.0.1 hem de assegurar-nos de que la �ltima columna sigui visible, 
	sin� fa coses extranyes amb la selecci�.
*/

void FSqlModelViewUtils::setColumns(QTableView* _View, const QStringList& _Cols)
{
	if (_Cols.size() > 0)
	{
		if (_View->model()->inherits("QSqlQueryModel"))
		{
			QSqlQueryModel* Model = static_cast<QSqlQueryModel*>(_View->model());
			QSqlRecord MRecord;
			if (_View->model()->inherits("QSqlRelationalTableModel"))
			{
				QSqlRelationalTableModel* RModel = static_cast<QSqlRelationalTableModel*>(Model);
				MRecord = RModel->database().record(RModel->tableName());
			}
			else 
				MRecord = Model->record();
			if (_View->model()->inherits("FSqlRelationalTableModel"))
			{
				FSqlRelationalTableModel* RModel = static_cast<FSqlRelationalTableModel*>(Model);
				for (int Vfor = 0; Vfor < RModel->columnCount(); Vfor++)
				{
					_View->setColumnHidden(Vfor, !_Cols.contains(MRecord.field(RModel->relativeColumn(Vfor)).name()));
				}
			}
			else 
			{
				for (int Vfor = 0; Vfor < Model->columnCount(); Vfor++)
				{
					_View->setColumnHidden(Vfor, !_Cols.contains(MRecord.field(Vfor).name()));
					//_View->horizontalHeader()->setSectionHidden(Vfor, !_Cols.contains(MRecord.field(Vfor).name()));
				}
			}
		}
	}
}

void FSqlModelViewUtils::sortColumns(QTableView* _View, const QStringList& _Cols)
{
	if (_View->model()->inherits("QSqlQueryModel"))
	{
		QStringList::const_iterator it; 
		QSqlQueryModel* Model = static_cast<QSqlQueryModel*>(_View->model());
		it = _Cols.end();
		it--;
		bool Beginning = false;
		while(!Beginning)
		{
			int Index;
			if (_View->model()->inherits("QSqlRelationalTableModel"))
				Index = indexOf(static_cast<QSqlRelationalTableModel*>(Model), *it);
			else 
				Index = Model->record().indexOf(*it);
				
			_View->horizontalHeader()->moveSection(_View->horizontalHeader()->visualIndex(Index), 0);
			Beginning = (it == _Cols.begin());
			--it;
			
		}
	}
}

void FSqlModelViewUtils::sortColumns(QTableView* _View, const QString& _Cols)
{
	QString Cols = _Cols;
	if (!Cols.isEmpty())
		sortColumns(_View, Cols.replace(" ","").split(","));
}

int FSqlModelViewUtils::indexOf(const QSqlTableModel* _Model, const QString& _FieldName)
{
	int Res = -1;
	QSqlRecord BaseRec = _Model->database().record(_Model->tableName());
	Res = BaseRec.indexOf(_FieldName);
	if (Res == -1)
		Res = _Model->record().indexOf(_FieldName);
	return Res; 
}

/*! 
	Retorna l'index de _FieldName dins el model _Model. Busca primer per els noms dels camps de la taula del model 
	o sigui que si hi ha una relaci� comparar� amb indexColumn de les relations. En cas de no trovar-lo a les relacions 
	retornar� indexOf(_FieldName) del record() de _Model.
*/

int FSqlModelViewUtils::indexOf(const QSqlRelationalTableModel* _Model, const QString& _FieldName)
{
	int Res = -1; 
	QSqlRecord BaseRec = _Model->database().record(_Model->tableName());
	Res = BaseRec.indexOf(_FieldName);
	if (Res == -1)
		Res = _Model->record().indexOf(_FieldName);
	//relativePos �s deprecated !!!
/*	if (_Model->inherits("FSqlRelationalTableModel"))
		Res = static_cast<const FSqlRelationalTableModel*>(_Model)->relativePos(Res);*/
	
	return Res; 
}

/*!
	Donat a que en els models relacionals els QSqlRecord poden tenir v�ries taules amb columnes amb el 
	mateix nom i els noms retornat per QSqlRecord no contemplen la taula, aquesta funcio recorra tots 
	els camps del model per a retornar l'equivalent a _FieldName de la taula _TableName. 
	\return Retorna l'index (O Column) del camp en el model _Model  o -1 si no l'ha trovat.
*/
int FSqlModelViewUtils::indexOf(const QSqlRelationalTableModel* _Model, const QString& _TableName, const QString& _FieldName)
{
	int Res = -1;
	QSqlRecord Rec = _Model->record();
	int Cnt = 0; 
	bool Trobat = false;
	while (! Trobat && Cnt < Rec.count())
	{
		if (Rec.fieldName(Cnt) == _FieldName)
		{
			if (_Model->tableName() == _TableName)
				Trobat = true;  
			else 
			{
				QSqlRelation Rel = _Model->relation(Cnt);
				if (Rel.isValid())
					Trobat = Rel.tableName() == _TableName;
			}
		}
		if (Trobat) 
		{
			Res = Cnt; 
	//relativePos �s deprecated !!!
/*			if (_Model->inherits("FSqlRelationalTableModel"))
				Res = static_cast<const FSqlRelationalTableModel*>(_Model)->relativePos(Res);*/
		}
		Cnt++;
	}
	return Res; 
}

