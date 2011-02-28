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


#include "fsqlmetadata.h"
#include <QApplication>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QTextStream>
#include <QRegExp>
#include <QCoreApplication>
#include "fsqlmodelviewutils.h"

// _________________________________________________________________________*/
//
//  class FRelationMetaData
// _________________________________________________________________________*/


FRelationMetaData::FRelationMetaData() : indexColumn(""), displayColumn(""), tableName(""), 
	delCascade(false), updateCascade(false)
{
}


void FRelationMetaData::load(const QDomElement& _Element)
{
	QDomNode CurrNode = _Element.firstChild();
	while (!CurrNode.isNull())
	{
		QDomElement CurrEl = CurrNode.toElement();
		if (!CurrEl.isNull())
		{
			QString TagName = CurrEl.tagName().toLower();
			QString Text = CurrEl.text().toLower();
			if (TagName == "table")
				tableName = CurrEl.text();
			else if (TagName == "display")
				displayColumn = CurrEl.text();
			else if (TagName == "field")
				indexColumn = CurrEl.text();
			else if (TagName == "delc")
				delCascade = Text == "true";
			else if (TagName == "updc")
				updateCascade = Text == "true";
		}
		CurrNode = CurrNode.nextSibling();
	}
	//Comprovem namica la sintaxi.
	if (isNull())
	{
		if (tableName.isEmpty())
			qWarning("No tablename in relatoin!");		
		if (indexColumn.isEmpty())
			qWarning(QString("No indexcolumn in relation of table : %s !" + tableName).toLatin1());		
		if (displayColumn.isEmpty())
			qWarning("No displaycolumn in relation");		
	}
}

QSqlRelation FRelationMetaData::qSqlRelation() const
{
	return QSqlRelation(tableName, indexColumn, displayColumn);
}

bool FRelationMetaData::isNull() const
{
	return indexColumn.isEmpty() || tableName.isEmpty() || displayColumn.isEmpty();
}


// _________________________________________________________________________*/
//
//  class FFieldMetaData
// _________________________________________________________________________*/

QVariant::Type FFieldMetaData::strToVariantType(const QString& _Str)
{
	QVariant::Type Res = QVariant::Invalid;

	if (_Str == "int")
		Res = QVariant::Int;
	else if (_Str == "uint")
		Res = QVariant::UInt;
	else if (_Str == "bool")
		Res = QVariant::Bool;
	else if (_Str == "double")
		Res = QVariant::Double;
	else if (_Str == "time")
		Res = QVariant::Time;
	else if (_Str == "date")
		Res = QVariant::Date;
	else if (_Str == "datetime")
		Res = QVariant::DateTime;
	else if (_Str == "pixmap")
		Res = QVariant::Pixmap;
	else if (_Str == "string")
		Res = QVariant::String;
	else if (_Str == "stringlist")
		Res = QVariant::StringList;
	else if (_Str == "image")
		Res = QVariant::Image;

	return Res;
}


FFieldMetaData::FFieldMetaData() : 	isPrimaryKey(false), hasIndex(false), isSerial(false), isVisible(false)
{}


void FFieldMetaData::load(const QDomElement& _Element)
{
	QDomNode CurrNode = _Element.firstChild();
	while (!CurrNode.isNull())
	{
		QDomElement CurrEl = CurrNode.toElement();
		if (!CurrEl.isNull())
		{
			QString TagName = CurrEl.tagName().toLower();
			QString Text = CurrEl.text().toLower();
			if (TagName == "name")
				setName(CurrEl.text());
			else if (CurrEl.tagName() == "alias")
			{
				//alias = CurrEl.text().remove("QT_TR_NOOP(\"").remove("\")");
				alias = CurrEl.text().remove(QRegExp("QT_TRANSLATE_NOOP\\(.*\\,\"")).remove("\")");
			}
			else if (TagName == "required")
			{
				if (Text == "true")
					setRequiredStatus(QSqlField::Required);
				else 
					setRequiredStatus(QSqlField::Optional);
			}
			else if (TagName == "pk")
				isPrimaryKey = Text == "true";
			else if (TagName == "index")
				hasIndex = Text == "true";
			else if (TagName == "serial")
				isSerial = Text == "true";
			else if (TagName == "type")
				setType(strToVariantType(Text));
			else if (TagName == "length")
				setLength(Text.toInt());
			else if (TagName == "precision")
				setPrecision(Text.toInt());
			else if (TagName == "regexp")
				regExp.setPattern(Text);
			else if (TagName == "default")
				setDefaultValue(Text);
			else if (TagName == "editable")
				setReadOnly(Text != "true");
/*			else if (TagName == "optionlist")
				????????????????????????";*/
			else if (TagName == "visible")
				isVisible = Text == "true";
			else if (TagName == "relation")
				Relation.load(CurrEl);
			
		}
		CurrNode = CurrNode.nextSibling();
	}
}


// _________________________________________________________________________*/
//
//  class FTableMetaData
// _________________________________________________________________________*/

void FTableMetaData::loadSqlInst(TSqlInstr& _Instr, const QDomElement& _Element)
{
	_Instr.Sql = _Element.text();	
	_Instr.Driver = _Element.attribute("driver", "");
	_Instr.Exclusive = _Element.attribute("exclusive", "false").toLower() == "true";
}

FTableMetaData::FTableMetaData() : alias(""), name(""), fileName(""), IsNull(true), CreateRelations(true)
{
	SqlBeforeCreate.Sql = "";
	SqlBeforeCreate.Driver = "";
	SqlBeforeCreate.Exclusive = false;
	SqlBeforeDelete.Sql = "";
	SqlBeforeDelete.Driver = "";
	SqlBeforeDelete.Exclusive = false;
}

void FTableMetaData::load(const QDomDocument& _Doc)
{
	IsNull = false;
	QDomElement RootElem = _Doc.documentElement();
	revision = RootElem.attribute("revision", "$Revision: 1.16 $").remove("$").remove("Revision:").remove(" "); 
	CreateRelations = RootElem.attribute("createrelations", "true").toLower() == "true";
	QDomNode CurrNode = RootElem.firstChild();
	while (!CurrNode.isNull())
	{
		QDomElement CurrEl = CurrNode.toElement();
		if (!CurrEl.isNull())
		{
			QString TagName = CurrEl.tagName().toLower();
			if (TagName == "name")
			{
				name = CurrEl.text();
			}
			else
				if (TagName == "alias")
				{
					alias = CurrEl.text().remove(QRegExp("QT_TRANSLATE_NOOP\\(.*\\,\"")).remove("\")");
				}
				else
					if (TagName == "field")
					{
						FFieldMetaData NField;
						NField.load(CurrEl);
						fields.insert(NField.QSqlField::name(), NField);
					}
				else 
					if (TagName == "inherits")
						baseClass = CurrEl.text();
				else 
					if (TagName == "sqlaftercreate")
						loadSqlInst(SqlAfterCreate, CurrEl); 
				else 
					if (TagName == "sqlbeforecreate")
						loadSqlInst(SqlBeforeCreate, CurrEl); 
				else 
					if (TagName == "sqlafterdelete")
						loadSqlInst(SqlAfterDelete, CurrEl); 
				else 
					if (TagName == "sqlbeforedelete")
						loadSqlInst(SqlBeforeDelete, CurrEl); 
				else 
					if (TagName == "sqlafterupdate")
					{
						TSqlUpdateInstr NInstr; 
						loadSqlInst(NInstr, CurrEl); 
						NInstr.FromRev = CurrEl.attribute("fromrev", "");
						SqlAfterUpdateList.push_back(NInstr);
					}
				else 
					if (TagName == "sqlbeforeupdate")
					{
						TSqlUpdateInstr NInstr; 
						loadSqlInst(NInstr, CurrEl); 
						NInstr.FromRev = CurrEl.attribute("fromrev", "");
						SqlBeforeUpdateList.push_back(NInstr);
					}
		}
		CurrNode = CurrNode.nextSibling();
	}
}


void FTableMetaData::loadFromStr(const QString& _Xml)
{
	QDomDocument Doc;
	Assert(Doc.setContent(_Xml), Error(QString("Unable to set xml document content:" + _Xml).toLatin1()));
	load(Doc);
}


void FTableMetaData::load(const QString& _FileName)
{
	QFile OpFile(_FileName);
	Assert(OpFile.open(QIODevice::ReadOnly | QIODevice::Text), Error("Unable to open file " + _FileName));
	fileName = _FileName;
	QDomDocument Doc;
	QString ErrMsg;
	int ErrLine, ErrCol;
	Assert(Doc.setContent(&OpFile, &ErrMsg, &ErrLine, &ErrCol), 
		Error(QString("Unable to set xml document content for file:" + _FileName + "->" + 
		ErrMsg + " at line %1, col %2").arg(ErrLine).arg(ErrCol).toLatin1()));
	load(Doc);
}

/*! Retorna tot el fitxer xml en forma de QString.
	Perque funcioni aquest mètode els settings han d'estar carregats amb el mètode load().
	Aquesta funció utilitzarà el fitxer de la última crida amb el fitxer correcte a load()
*/
QString FTableMetaData::xml() const
{
	QFile OpFile(fileName);
	Assert(OpFile.open(QIODevice::ReadOnly | QIODevice::Text), Error("Unable to open file " + fileName));
	QTextStream Strm(&OpFile); 
	return Strm.readAll();
}

void FTableMetaData::configure(QSqlTableModel* _Model) const
{
	_Model->setTable(name);
	TFieldList::const_iterator it;
	for (it = fields.begin(); it != fields.end(); ++it)
	{
		_Model->setHeaderData(_Model->record().indexOf(it->name()), Qt::Horizontal, QApplication::translate("MetaData", it->alias.toLatin1()));
	}
}

void FTableMetaData::configure(QSqlTableModel& _Model) const
{
	configure(&_Model);
}

void FTableMetaData::configureHeaderData(QSqlTableModel& _Model) const
{
	TFieldList::const_iterator it;
	for (it = fields.begin(); it != fields.end(); ++it)
	{
		_Model.setHeaderData(_Model.record().indexOf(it->name()), Qt::Horizontal, QApplication::translate("MetaData", it->alias.toLatin1()));
	}
	
}

/*! 
	Retorna el camp que te una relació amb la taula _RelatedTableName
	Si no en te cap retornarà un FFieldMetaData null.
	Si la FTableMetaData és null farà un throw.
*/

FFieldMetaData FTableMetaData::relatedField(const QString& _RelatedTableName) const
{
	FFieldMetaData Result; 
	//Busquem una relacio que correspongui a la taula _RelatedTableName
	Assert(!isNull(), Error(QApplication::tr("FTableMetaData::relatedValueQuery TableMetadata is Null !")));
	FRelationMetaData CRelation;
	TFieldList::const_iterator it; 
	it = fields.begin();
	bool Found = false; 
	while (it!= fields.end() && !Found)
	{
		CRelation = it->relation();
		if (!CRelation.isNull())
			Found = (CRelation.tableName == _RelatedTableName);
		if (Found) 
			Result = *it;	
		++it;
	}
	return Result;
}

void FTableMetaData::configure(QSqlRelationalTableModel& _Model) const
{
	configure(&_Model);
}

void FTableMetaData::configure(QSqlRelationalTableModel* _Model) const
{
	configure(static_cast<QSqlTableModel*>(_Model));
	TFieldList::const_iterator it;
	for (it = fields.begin(); it != fields.end(); ++it)
	{
		if (!it->relation().isNull())
		{
//			qDebug(QString("Configuring table: %1, index of %2 is %3 ").arg(_Model.tableName()).arg(it->name()).arg(FSqlModelViewUtils::indexOf(&_Model, it->name())).toLatin1() );
			_Model->setRelation( FSqlModelViewUtils::indexOf(_Model, it->name()), it->relation().qSqlRelation());
			#ifndef FLAM_NO_WARNING_OUTPUT
			if (!_Model->database().tables().contains(it->relation().qSqlRelation().tableName()))
			{
				qWarning(QString("FTableMetaData::configure-> Table: " + 
					it->relation().qSqlRelation().tableName() +  " not found !. Configuring table: " 
					+ _Model->tableName() ).toLatin1());
			}
			else 
			{
				QSqlRelation Relation = it->relation().qSqlRelation();
				if (!_Model->database().record(Relation.tableName()).contains(Relation.displayColumn()))
					qWarning(QString("FTableMetaData::configure-> Field: " + 
						Relation.displayColumn() +  " not found in table " + Relation.tableName() + 
						" !. Configuring table: " + _Model->tableName() ).toLatin1());
				else 
				if (!_Model->database().record(Relation.tableName()).contains(Relation.indexColumn()))
					qWarning(QString("FTableMetaData::configure-> Field: " + 
						Relation.indexColumn() +  " not found in table " + Relation.tableName() + 
						" !. Configuring table: " + _Model->tableName() ).toLatin1());
				
			}		
			
			#endif 
		}
	}
}



// _________________________________________________________________________*/
//
//  class FSqlMetaData
// _________________________________________________________________________*/


void FSqlMetaData::loadDbInfo(const QDomDocument& _Doc)
{
	QDomElement RootElem = _Doc.documentElement();
	//revision = RootElem.attribute("revision", "$Revision: 1.16 $").remove("$").remove("Revision:").remove(" "); 
	QDomNode CurrNode = RootElem.firstChild();
	while (!CurrNode.isNull())
	{
		QDomElement CurrEl = CurrNode.toElement();
		if (!CurrEl.isNull())
		{
			QString TagName = CurrEl.tagName().toLower();
			if (TagName == "creationorder")
			{
				CreationorderList = CurrEl.text().split(",");
			}
		}
		CurrNode = CurrNode.nextSibling();
	}
}

FSqlMetaData::FSqlMetaData()
{}


FSqlMetaData::~FSqlMetaData()
{}


void FSqlMetaData::loadFromResources(const QString& _MetadataPath)
{
	//Carreguem info global de la bd.
	orderedTables.clear();

	QString ResourceRoot = _MetadataPath;

	QFile OpFile(ResourceRoot + "database.mtd");
	//Assert(OpFile.open(QIODevice::ReadOnly | QIODevice::Text), Error("Unable to open file " + OpFile.fileName()));
	if (OpFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QDomDocument Doc;
		Assert(Doc.setContent(&OpFile), Error(QString("Unable to set xml document content for file:" +
			OpFile.fileName()).toLatin1()));
		loadDbInfo(Doc);
		orderedTables << CreationorderList;
	}

	QStringList::iterator it;
	for (it = orderedTables.begin(); it != orderedTables.end(); ++it)
	{
		FTableMetaData NTable;
		//qDebug(QString("Inserting table: " + TablesDir.absolutePath() + "/" + (*it) ).toLatin1());
		NTable.load(_MetadataPath + *it + ".mtd");
		tables.insert(NTable.name, NTable);
		if (!orderedTables.contains(NTable.name))
			orderedTables << NTable.name ;
	}
}

/*!
	En cas de no trobar _TablesMetadataDir el buscarà al $(path del binari executable)/../share/tables/
*/

void FSqlMetaData::load(const QString& _TablesMetadataDir)
{
	//Carreguem info global de la bd.
	orderedTables.clear(); 
	
	QString OpDirName = _TablesMetadataDir;
	if (OpDirName == "")
		OpDirName = QApplication::applicationDirPath() + "/../share/" + FSqlMetaData::dataSubDir(FSqlMetaData::TablePath);
	
	QDir TablesDir(OpDirName);
	QFile OpFile(OpDirName + "/../database.mtd");

	Assert(TablesDir.exists(), Error("Could not find directory: " + OpDirName));
	QStringList TableFiles = TablesDir.entryList(QStringList("*.mtd"));
	QStringList::iterator it;
	
	//Assert(OpFile.open(QIODevice::ReadOnly | QIODevice::Text), Error("Unable to open file " + OpFile.fileName()));
	if (OpFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QDomDocument Doc;
		Assert(Doc.setContent(&OpFile), Error(QString("Unable to set xml document content for file:" +
			OpFile.fileName()).toLatin1()));
		loadDbInfo(Doc);

		//Mirem que a creationOrderList totes les taules existeixin: 
		for (it = CreationorderList.begin(); it != CreationorderList.end(); ++it)
		{
			if (TableFiles.contains(QString(*it + ".mtd")))
			{
				orderedTables << *it;
			}
			else 
				qWarning(QString("FSqlMetaData: " + *it + " in database.mtd(ordered list) does not exist in metadata. Ignoring it .").toLatin1());
		}
	}

	for (it = TableFiles.begin(); it != TableFiles.end(); ++it)
	{
		FTableMetaData NTable;
		//qDebug(QString("Inserting table: " + TablesDir.absolutePath() + "/" + (*it) ).toLatin1());
		NTable.load(TablesDir.absolutePath() + "/" + (*it) );
		tables.insert(NTable.name, NTable);
		if (!orderedTables.contains(NTable.name))
			orderedTables << NTable.name ;
	}
}

void FSqlMetaData::save(const QString& /*_TablesMetadataDir*/)
{}


QString FSqlMetaData::dataSubDir(FSqlMetaData::EnSharedDataType _DType)
{
	QString Res; 
	switch (_DType)
	{
		case RootPath: 
			Res = ""; 
		break;
		case TablePath: 
			Res = "tables"; 
		break;
	}
	return Res;
}
