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

#include "mtdversion.h"
#include <math.h>


/*!
	Retorna una llista amb 4 floats: 
	Major, Minor, SubMajor i SubMinor (0..3)
	

*/

MtdVersion::TVersList MtdVersion::versList(const QString& _Version) const
{
	MtdVersion::TVersList Res;
	Res.clear();
	for (int Vfor=0; Vfor < 4; Vfor++)
		Res.push_back(0);
	
	QString CurrVers = _Version;
	int CurrIndex = 0;
	int CntVal = 0;
	while (CurrIndex != -1 && CntVal < 4)
	{
		CurrIndex = CurrVers.indexOf(".", CurrIndex);
		int PosCut = CurrIndex;
		if (PosCut == -1)
			PosCut = CurrVers.length();
		Res[CntVal] = CurrVers.left(PosCut).toInt();
		CurrVers = CurrVers.right(CurrVers.length() - ( CurrIndex + 1 ) );
		CntVal++;
	}
	
	return Res;
}

/*!
	Retorna un valor m�s gran a una versi� considerada superior.
*/

float MtdVersion::versValue(const QString& _Version) const
{
	return versValue(versList(_Version));	
}

/*!
	Retorna un valor m�s gran a una versi� considerada superior.
	_Version cont� floats amb el numero de versi� de Major a SubMinor comen�ant a l'index 0.
*/

float MtdVersion::versValue(const MtdVersion::TVersList& _Version) const 
{
	float Res = 0;
	for (int Vfor=0; Vfor < 4; Vfor++)
	{
		float CurrNVer = _Version[Vfor];
		Res += CurrNVer * pow(100, 2-Vfor);
	}
	return Res;
}


MtdVersion::MtdVersion(const QString& _Value )
{
	setVersion(_Value);
}

MtdVersion& MtdVersion::operator=(const QString& _Version)
{
	setVersion(_Version);
	return *this;
}

MtdVersion& MtdVersion::operator=(const MtdVersion& _Other)
{
	CurrVer = _Other.CurrVer;
	return *this;
}

bool MtdVersion::operator>(const MtdVersion& _Other)
{
	return versValue(CurrVer) > versValue(_Other.CurrVer);
}

bool MtdVersion::operator<(const MtdVersion& _Other)
{
	return versValue(CurrVer) < versValue(_Other.CurrVer);
}

bool MtdVersion::operator<=(const MtdVersion& _Other)
{
	return versValue(CurrVer) <= versValue(_Other.CurrVer);
}

bool MtdVersion::operator>=(const MtdVersion& _Other)
{
	return versValue(CurrVer) >= versValue(_Other.CurrVer);
}

bool MtdVersion::operator==(const MtdVersion& _Other)
{
	return versValue(CurrVer) == versValue(_Other.CurrVer);
}

bool MtdVersion::operator!=(const MtdVersion& _Other)
{
	return versValue(CurrVer) != versValue(_Other.CurrVer);
}

void MtdVersion::setVersion(const QString& _Value)
{
	CurrVer = versList(_Value);
}

QString MtdVersion::version() const
{
	QString Res;
	for (int Vfor=0; Vfor < 4; Vfor++)
	{
		Res += QString::number(static_cast<int>(CurrVer[Vfor]));
		if (Vfor < 3)
			Res += ".";
	}
	return Res; 
}

QString MtdVersion::toString() const
{
	return version();
}

int MtdVersion::major() const
{
	return CurrVer[0];
}

void MtdVersion::setMajor(int _Value)
{
	CurrVer[0] = _Value;
}

int MtdVersion::minor() const
{
	return CurrVer[1];
}

void MtdVersion::setMinor(int _Value)
{
	CurrVer[1] = _Value;
}

int MtdVersion::subMajor() const
{
	return CurrVer[2];
}

void MtdVersion::setSubMajor(int _Value)
{
	CurrVer[2] = _Value;
}

int MtdVersion::subMinor() const
{
	return CurrVer[3];
}

void MtdVersion::setSubMinor(int _Value)
{
	CurrVer[3] = _Value;
}

MtdVersion MtdVersion::smallestVersion()
{
	return MtdVersion();
}


