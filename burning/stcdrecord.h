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

#ifndef STCDRECORD_H
#define STCDRECORD_H

#include <QProcess>
#include <QVector>
#include "sterror.h"
#include "stburningexport.h"

/**
Process per executar i parsejar la sortida de cdrecord.

@author shadow
*/
class STBurningSettings; 
class ST_BURNING_EXPORT STCdRecord : public QProcess
{
Q_OBJECT 

public:

   class Error : public STError 
	{ 
		int ErrorId;

	public: 
		Error(const QString& _Description, const QString& _AdvancedDesc, int _Id = -1) : STError("KPS Error: " + _Description, _AdvancedDesc)
		, ErrorId(_Id) {} 
		Error(const QString& _Description, int _Id = -1) : STError("KPS Error: " + _Description), ErrorId(_Id) {} 
		int errorId() const {return ErrorId; }
	};

	typedef enum EnMessageType 
	{ 
		INFO,
		ERROR, 
		WARNING,
		SUCCESS
	};

private:
	QString BinFile; 
	QString TmpIsoFile;
	QString WriteMode;
	QString Device; 
	QString Speed;
	QString Version;
	bool m_totalTracksParsed;
	bool WriteDebug;
	int m_currentTrack;
	int m_totalTracks;
	int m_totalSize;
	int m_alreadyWritten;
	int m_lastFifoValue;

	QVector<int> trackSizes;

	void setWriteDebug(bool _Value) { WriteDebug = _Value; }
	void checkForBinFile();

	void processLine(const QString& _Line);
	void splitOutput(const char* _Data, int _Len, bool _StdOut);
	void processOuptut(const QByteArray& _Arr);
 
public:
	STCdRecord(QObject* _Parent = 0);
	void start();
	void configure(const QString& _CDRecordBinary, const QString& _TmpIsoFile, const QString& _Device, int _Speed, bool _SimulationMode);
	void configure(const STBurningSettings& _Settings);
	//! Get muntisession info
	QString getMsInfo();

private slots:
	void slotProcessOutput();
	void slotProcessError();
	void showDebugMessage(QString, EnMessageType );

signals: 
	void statusChanged(const QString& _StatusStr, int _PercentComplete);	
	void buffer( int _fifo );
	void deviceBuffer( int _devBuffer );
	void processedSize( int, int );
	void processedSubSize( int, int );
	void subPercent( int );
	void infoMessage(QString _info, EnMessageType _type);
};

#endif
