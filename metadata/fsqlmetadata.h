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

#ifndef FSQLMETADATA_H
#define FSQLMETADATA_H

#include <QMap> 
#include <QSqlRelation> 
#include <QSqlIndex> 
#include <QSqlField>
#include <QVariant>
#include <QRegExp>
#include <QStringList>
#include "sterror.h"
#include "mtdversion.h" 
#include "stmetadataexport.h"


class QDomElement;
class ST_METADATA_EXPORT FRelationMetaData 
{

public: 
	QString indexColumn; 
	QString displayColumn; 
	QString tableName; 
	bool delCascade; 
	bool updateCascade;
	
public: 
	FRelationMetaData();
	void load(const QDomElement& _Element); 
	//! Crea una QSqlRelation a partir de l'Objecte actual.
	QSqlRelation qSqlRelation() const; 
	//! Retorna cert si la relació no conté cap informació.
	bool isNull() const; 
};

class ST_METADATA_EXPORT FLIndexMetaData : public QSqlIndex 
{
};

class ST_METADATA_EXPORT FFieldMetaData : public QSqlField
{
	
	FRelationMetaData Relation;

public: 
	QString alias;
	bool isPrimaryKey;
	bool hasIndex;
	bool isSerial;
	bool isVisible;
	QRegExp regExp;

private: 
	QVariant::Type strToVariantType(const QString& _Str);

public: 
	FFieldMetaData();
	void load(const QDomElement& _Element); 
	FRelationMetaData relation() const {return Relation;}
	void setRelation(const FRelationMetaData& _Relation) { Relation = _Relation; }
};

class QDomDocument;
class ST_METADATA_EXPORT FTableMetaData 
{
public: 
	ST_DECLARE_ERRORCLASS();

public: 
	typedef QMap<QString, FFieldMetaData> TFieldList;

typedef struct SSqlInstr
{
	QString Sql;
	QString Driver;
 	bool Exclusive;
}
TSqlInstr;

typedef struct SSqlUpdateInstr : public SSqlInstr
{
	QString FromRev;
}
TSqlUpdateInstr;

typedef QList<TSqlUpdateInstr> TUpdateStrInstList;
	
public: 	
	TFieldList fields; 
	FLIndexMetaData index;
	QString alias; 
	QString name; 
	MtdVersion revision;
	QString fileName;
	QString baseClass;

	TSqlInstr SqlAfterCreate; 
	TSqlInstr SqlBeforeCreate; 
	TSqlInstr SqlAfterDelete;
	TSqlInstr SqlBeforeDelete;
	TUpdateStrInstList SqlAfterUpdateList;
	TUpdateStrInstList SqlBeforeUpdateList;

private:	
	bool IsNull;
	bool CreateRelations;
	void loadSqlInst(TSqlInstr& _Instr, const QDomElement& _Element);
	
public: 
	FTableMetaData(); 
	bool createRelations() const {return CreateRelations;}
	void load(const QDomDocument& _Doc);
	void loadFromStr(const QString& _Xml);
	void load(const QString& _FileName); 
	QString xml() const;
	bool isNull() const {return IsNull;} //No està carregada;
	void configure(QSqlTableModel& _Model) const;
	void configureHeaderData(QSqlTableModel& _Model) const;
	void configure(QSqlRelationalTableModel& _Model) const;
	FFieldMetaData relatedField(const QString& _RelatedTableName) const;
	TSqlInstr sqlAfterCreate() const {return SqlAfterCreate;} 
	TSqlInstr sqlBeforeCreate() const {return SqlBeforeCreate;} 
	TSqlInstr sqlAfterDelete() const {return SqlAfterDelete;} 
	TSqlInstr sqlBeforeDelete() const {return SqlBeforeDelete;} 
	TUpdateStrInstList sqlAfterUpdateList() const {return SqlAfterUpdateList;}
	TUpdateStrInstList sqlBeforeUpdateList() const {return SqlBeforeUpdateList;}
};


/**
Classe de MetaInformació sobre la base de dades.

Estructura de les metadades:
Cada taula es definirà amb un fitxer XML amb extensió .mtd 
L'estructura del fitxer XML de les taules és la següent: 

<!DOCTYPE TMD>
<TMD revision="$Revision: 1.15 $" createrelations="false"> <!-- Revisió controlada per CVS o manualment: $Revision: 1.15 $ -->
<!-- Parametres: (Opcional) createrelations per defecte = true, indica si es crearan les relacions -->
    <name>tablename</name> <!-- Nom de la taula -->
    <alias lang="ca">Nom de la taula</alias> <!-- (Opcional) Alias amb el llenguange lang (Opcional) -->
    <inherits>Nom de la taula base</inherits> <!-- (Opcional) Taula base (Opcional) -->
    <field> <!-- Definició d'un camp de la taula -->
		<!-- Requerits -->
        <name>idpeca</name> <!-- Nom del camp -->
		  <type>uint</type> <!-- Tipus: \sa FFieldMetaData::strToVariantType -->
		<!-- Opcionals -->
        <alias lang="ca">Peces</alias> <!-- Alias amb el llenguange lang (Opcional) -->
        <required>true</required>
        <pk>true</pk> <!-- Si és clau primària. Per defecte false (Si hi ha varis camps llavors la clau és composta. -->
        <serial>true</serial> <!-- Si conté una seqüència. Per defecte false. -->
        <index>true</index> <!-- Crea un index tablename_fieldname_idx. (Per crear indexos complexos utilitzar el tag sqlcreate) -->
        <length>nnnn</length> <!-- Longitud del camp en cas de que sigui String -->
        <precision>nnnn</precision> <!-- Nombre de decimals -->
        <regexp>nnnn</regexp> <!-- Expressió regular per a validar-lo -->
        <default>nnnn</default> <!-- Valor per defecte -->
        <editable>nnnn</editable> <!-- Si es pot editar -->
        <visible>nnnn</visible> <!-- Si es veurà per defecte en els viewers -->
		<!-- Definició de relació. -->
        <relation>
            <table>relatedtable</table> <!-- Nom de la taula relacionada -->
            <field>relatedtablefield</field> <!-- Nom del camp relacionat amb l'actual de la taula relacionada -->
				<display>displayfieldname</display> <!-- Camp de la taula relacionada per a els models tipus SqlRelationalTableModel -->
        </relation>
    </field>
	<!-- Instruccions Sql explicites. -->
	<!-- S'executen a cada operació. -->
	<!-- Si s'indica el drivername només s'executen en cas de que la base de dades utilitzi aquell driver. -->
	<!-- exclusive => Només s'executarà aquest codi SQL, FLAM no farà res. -->
	<!-- S'executarà cada vegada que es crei de nou la taula després de la creació d'aquesta. -->
	 <sqlaftercreate driver="drivername" > 
CREATE INDEX code_idx ON tablename(field1, field2); 
INSERT INTO TABLE control (desc ) VALUES ('table tablename created !.');
	 </sqlaftercreate>
	<!-- S'executarà cada vegada que es crei de nou la taula abans de la creació d'aquesta. -->
	 <sqlbeforecreate exclusive="true" driver="drivername" ></sqlbeforecreate>
	<!-- S'executarà cada vegada que s'actualitzi la taula. Després de l'actualització -->
	<!-- fromrevision => Només s'executarà en cas de que actualitzem desde les revisions indicades. -->
	 <sqlafterupdate fromrevision="[&lt;,=,&lt;=,&gt;=,&gt;]nn.nn" driver="drivername"> </sqlafterupdate>
	<!-- S'executarà cada vegada que s'actualitzi la taula. Abans de l'actualització -->
	 <sqlbeforeupdate exclusive="true" fromrevision="[<,=,<=,>=,>]nn.nn" driver="drivername"> </sqlbeforeupdate>
	<!-- S'executarà cada vegada que es borri la taula. Despres del borrat de la taula. -->
	 <sqlafterdelete driver="drivername"> </sqlafterdelete>
	<!-- S'executarà cada vegada que es borri la taula. Abans del borrat de la taula. -->
	 <sqlbeforedelete exclusive="true" driver="drivername"> </sqlbeforedelete>
</TMD>

<!-- Al fitxer (sharedroot)/database.mtd hi tenim la informació genèrica per a tota la base de dades. -->
<!-- Aquest fitxer és opcional. -->
<!DOCTYPE DBMD>
<DBMD >
<!-- Indica l'ordre en que es crearan o s'actualitzaran les taules. Si una taula no apareix s'enten que està 
al final de la llista i que no depen de cap altre taula.. -->
	<creationorder>table1,table2,table3</creationorder>
</DBMD>

 
@author Shadow

*/
class ST_METADATA_EXPORT FSqlMetaData
{
public:
	ST_DECLARE_ERRORCLASS();

public:
	enum EnSharedDataType
	{
		RootPath, 
		TablePath
	};
	


public: 	
	typedef QMap<QString, FTableMetaData> TTablesList;
	
	TTablesList tables; 
	//! Llista dels noms de les taules ordenades per preferencies de creació segons les dependencies;
	QStringList orderedTables; 

private:
	QStringList CreationorderList;
	
private:	
	void loadDbInfo(const QDomDocument& _Doc);

public:
	FSqlMetaData();
	//! Carrega les metadades de disk.
	void load(const QString& _TablesMetadataDir = ""); 
	void save(const QString& _TablesMetadataDir = ""); 
	~FSqlMetaData();
	//void configureView();
	
	
	//Nom del subdirectori segons el tipus de dades _DType 
	static QString dataSubDir(FSqlMetaData::EnSharedDataType _DType = RootPath); 
};




#endif
