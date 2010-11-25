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

#include "stcdrecord.h"
#include <QFile>
#include "stburningsettings.h" 


void STCdRecord::splitOutput(const char* data, int len, bool /*stdout*/)
{
	//
  // The stderr splitting is mainly used for parsing of messages
  // That's why we simplify the data before proceeding
  //
  QString buffer;
  for( int i = 0; i < len; i++ ) {
    if( data[i] == '\b' ) {
      while( data[i] == '\b' )  // we replace multible backspaces with a single line feed
			i++;
      buffer += '\n';
    }
    if( data[i] == '\r' )
      buffer += '\n';
    else if( data[i] == '\t' )  // replace tabs with a single space
      buffer += " ";
    else
      buffer += data[i];
  }
	QStringList lines = buffer.split('\n');
	
	QStringList::iterator it;
	
	for( it = lines.begin(); it != lines.end(); ++it ) {
		QString& str = *it;
	
		// just to be sure since something above does not do this right
		if( str.isEmpty() )
			continue;

		processLine(str);	
	}
}



void STCdRecord::processLine(const QString& line)
{
	//qDebug(QString("Processing line :%1").arg(line).toLatin1());

	static QRegExp s_burnfreeCounterRx( "^BURN\\-Free\\swas\\s(\\d+)\\stimes\\sused" );
	static QRegExp s_burnfreeCounterRxPredict( "^Total\\sof\\s(\\d+)\\s\\spossible\\sbuffer\\sunderruns\\spredicted" );

	// tracknumber: cap(1)
	// done: cap(2)
	// complete: cap(3)
	// fifo: cap(4)  (it seems as if some patched STCdRecord versions do not emit the fifo info but only the buf... :(
	// buffer: cap(5)
	static QRegExp s_progressRx( "Track\\s(\\d\\d)\\:\\s*(\\d*)\\sof\\s*(\\d*)\\sMB\\swritten\\s(?:\\(fifo\\s*(\\d*)\\%\\)\\s*)?(?:\\[buf\\s*(\\d*)\\%\\])?.*" );

	//
	// Progress and toc parsing
	//
	if( line.startsWith("Track"))
	{
		if( !m_totalTracksParsed )
		{
			// this is not the progress display but the list of tracks that will get written
			// we always extract the tracknumber to get the highest at last
			bool ok;
			int tt = line.mid( 6, 2 ).toInt(&ok);
	
			if( ok )
			{
// 				struct Private::Track track;
// 				track.audio  = ( line.mid( 10, 5 ) == "audio" );
				m_totalTracks = tt;
				int sizeStart = line.indexOf( QRegExp("\\d"), 10 );
				int sizeEnd = line.indexOf( "MB", sizeStart );
				int trackSize = line.mid( sizeStart, sizeEnd-sizeStart ).toInt(&ok);
			
				if( ok )
				{
					trackSizes.append( trackSize );
					m_totalSize += trackSize;
				}
				else
				{	
					qDebug(QString("(K3bSTCdRecordWriter) track number parse error: %0\n")
						.arg(line.mid(sizeStart, sizeEnd-sizeStart)).toAscii());
				}
			}
			else
				qDebug(QString("(K3bSTCdRecordWriter) track number parse error: %0").arg(line.mid(6, 2)).toAscii());
		}
	
		else if( s_progressRx.exactMatch( line ) )
		{
			//int num = s_progressRx.cap(1).toInt();
			int made = s_progressRx.cap(2).toInt();
			int size = s_progressRx.cap(3).toInt();
			int fifo = s_progressRx.cap(4).toInt();
	
			emit buffer( fifo );
			m_lastFifoValue = fifo;
	
			if( s_progressRx.numCaptures() > 4 )
				emit deviceBuffer( s_progressRx.cap(5).toInt() );
	
			//
			// STCdRecord's output sucks a bit.
			// we get track sizes that differ from the sizes in the progress
			// info since these are dependant on the writing mode.
			// so we just use the track sizes and do a bit of math...
			//
	
			if( trackSizes.count() > m_currentTrack-1 && size > 0 )
			{
				double convV = (double)trackSizes[m_currentTrack-1]/(double)size;
				made = (int)((double)made * convV);
				size = trackSizes[m_currentTrack-1];
			}
			else
			{
				qWarning ("(K3bSTCdRecordWriter) Did not parse all tracks sizes!");
			}
	
			if( size > 0 )
			{
				emit processedSubSize( made, size );
				emit subPercent( 100*made/size );
			}
	
			if( m_totalSize > 0 )
			{
				emit processedSize( m_alreadyWritten+made, m_totalSize );
				int Percent = 100*(m_alreadyWritten+made)/m_totalSize;
				QString StatMsg = QString("Completed %0 \%, Written %1 of %3 MB").arg(Percent).arg(
					made).arg(m_totalSize);
				qDebug(StatMsg.toLatin1());
				emit statusChanged(StatMsg, Percent);
			}
	
			//d->speedEst->dataWritten( (m_alreadyWritten+made)*1024 );
		}
	}

	//
	// STCdRecord starts all error and warning messages with it's path
	// With Debian's script it starts with STCdRecord
	//
	
	else if( line.startsWith( "STCdRecord" ) ||
				line.startsWith( BinFile ) ||
				line.startsWith( BinFile.left(BinFile.length()-5)))
	{
		// get rid of the path and the following colon and space
		QString errStr = line.mid( line.indexOf(':') + 2 );
	
		if( errStr.startsWith( "Drive does not support SAO" ) )
		{
			emit infoMessage( QObject::tr("DAO (Disk At Once) recording not supported with this writer"), ERROR );
			emit infoMessage( QObject::tr("Please choose TAO (Track At Once) and try again"), ERROR );
		}
		else if( errStr.startsWith( "Drive does not support RAW" ) )
		{
			emit infoMessage( QObject::tr("RAW recording not supported with this writer"), ERROR );
		}
		else if( errStr.startsWith("Input/output error.") )
		{
			emit infoMessage( QObject::tr("Input/output error. Not necessarily serious."), WARNING );
		}
		else if( errStr.startsWith("shmget failed") )
		{
			throw Error("shmget failed");
		}
		else if( errStr.startsWith("No disk") )
		{
			throw Error(tr("Any recordable disk is found."));
		}
		else if( errStr.startsWith("OPC failed") )
		{
			throw Error("OPC failed");
		}
		else if( errStr.startsWith( "Drive needs to reload the media" ) )
		{
			emit infoMessage( QObject::tr("Reload disk "), INFO );
		}
		else if( errStr.startsWith( "The current problem looks like a buffer underrun" ) )
		{
			throw Error("The current problem looks like a buffer underrun");
		}
		else if( errStr.startsWith("WARNING: Data may not fit") )
		{
			emit infoMessage( QObject::tr("Trying to write more than the official disk capacity"), WARNING );
			throw Error("WARNING: Data may not fit");
		}
		else if( errStr.startsWith("Bad Option") )
		{
			throw Error("Bad Option");
			
			// parse option
			int pos = line.indexOf( "Bad Option" ) + 13;
			int len = line.length() - pos - 1;
			emit infoMessage( QObject::tr("No valid %1 option: %2").arg("cdrecord").arg(line.mid(pos, len)), ERROR );
		}
		else if( errStr.startsWith("Cannot set speed/dummy") )
		{
			throw Error("Cannot set speed/dummy");
		}
		else if( errStr.startsWith("Cannot open new session") )
		{
			throw Error("Cannot open new session");
		}
		else if( errStr.startsWith("Cannot send CUE sheet") )
		{
			throw Error("Cannot send CUE sheet");
		}
		else if( errStr.startsWith( "Trying to use ultra high speed medium on improper writer" ) ||
					errStr.startsWith( "Trying to use high speed medium on low speed writer" ) )
		{
			throw Error("Trying to use ultra high speed medium on improper writer");
		}
		else if( errStr.startsWith( "Permission denied. Cannot open" ) )
		{
			throw Error("Permission denied. Cannot open");
		}
		else if( errStr.startsWith( "Can only copy session # 1") )
		{
			emit infoMessage( QObject::tr("Only session 1 will be cloned."), WARNING );
		}
		else if( errStr.startsWith( "Cannot load media"))
		{
			throw Error("Cannot load media");
		}
	}

	//
	// All other messages
	//

	else if( line.contains( "at speed" ) )
	{
		// parse the speed and inform the user if cdrdao switched it down
		int pos = line.indexOf( "at speed" );
		int pos2 = line.indexOf( "in", pos+9 );
		int speed = static_cast<int>( line.mid( pos+9, pos2-pos-10 ).toDouble() );  // cdrecord-dvd >= 2.01a25 uses 8.0 and stuff
		int CurrSpeed = Speed.toInt();
		if( speed != CurrSpeed )
		{
			emit infoMessage( QObject::tr("Medium or burner do not support writing at %1x speed").arg(CurrSpeed), WARNING );
			if( speed > CurrSpeed )
				emit infoMessage( QObject::tr("Switching up burn speed to %1x").arg(speed), WARNING );
			else
				emit infoMessage( QObject::tr("Switching down burn speed to %1x").arg(speed), WARNING );
		}
	}
	else if( line.startsWith( "Starting new" ) )
	{
		m_totalTracksParsed = true;
		if( m_currentTrack > 0 )
		{// nothing has been written at the start of track 1
			if( trackSizes.count() > m_currentTrack-1 )
				m_alreadyWritten += trackSizes[m_currentTrack-1];
			else
				qWarning ( "(K3bSTCdRecordWriter) Did not parse all tracks sizes!");
		}
		else
			emit infoMessage( QObject::tr("Starting burn process"), INFO );
	
		m_currentTrack++;
	
		if( m_currentTrack > trackSizes.count() )
		{
			qDebug("(K3bSTCdRecordWriter) need to add dummy track struct.");
			trackSizes.append(1);
		}
	
		qDebug(QString("(K3bSTCdRecordWriter) writing track %0 of %1 tracks.").arg(m_currentTrack).arg(m_totalTracks).toAscii());
		//emit nextTrack( m_currentTrack, m_totalTracks );
	}
	else if( line.startsWith( "Fixating" ) )
	{
		emit statusChanged( tr("Finishing the recording..."), 100 );
	}
	else if( line.contains("seconds.") )
	{
		int pos2 = line.indexOf("seconds.") - 2;
		int pos1 = line.lastIndexOf( QRegExp("\\D"), pos2 ) + 1;
		int secs = line.mid(pos1, pos2-pos1+1).toInt();
		if( secs > 0 )
			emit statusChanged( QString(tr("Start burn process in %0 seconds.").arg(secs)), (secs - 1) * 10);
	}
	else if( line.startsWith( "Writing lead-in" ) )
	{
		m_totalTracksParsed = true;
		//emit newSubTask( QObject::tr("Writing Leadin") );
	}
	else if( line.startsWith( "Writing Leadout") )
	{
		//emit newSubTask( QObject::tr("Writing Leadout") );
	}
	else if( line.startsWith( "Writing pregap" ) )
	{
		//emit newSubTask( QObject::tr("Writing pregap") );
	}
	else if( line.startsWith( "Performing OPC" ) )
	{
		emit infoMessage( QObject::tr("Performing Optimum Power Calibration"), INFO );
	}
	else if( line.startsWith( "Sending" ) )
	{
		emit infoMessage( QObject::tr("Sending CUE sheet"), INFO );
	}
	else if( line.startsWith( "Turning BURN-Free on" ) || line.startsWith( "BURN-Free is ON") )
	{
		emit infoMessage( QObject::tr("Enabled Burnfree"), INFO );
	}
	else if( line.startsWith( "Turning BURN-Free off" ) )
	{
		emit infoMessage( QObject::tr("Disabled Burnfree"), WARNING );
	}
	else if( line.startsWith( "Performing OPC" ) )
	{
		emit infoMessage( QObject::tr("Performing Optimum Power Calibration"), INFO );
	}
//		Això ara no té sentit el procés sortirà i informarà que no hi ha disc. ERROR ( NO_MEDIA ) 
// 	else if( line.startsWith( "Re-load disk and hit" ) )
// 	{
// 		// this happens on some notebooks where STCdRecord is not able to close the
// 		// tray itself, so we need to ask the user to do so
// 		QMessageBox::information( 0, QObject::tr("Please reload the medium and press 'ok'"),
// 											QObject::tr("Unable to close the tray") );
// 	
// 		// now send a <CR> to STCdRecord
// 		// hopefully this will do it since I have no possibility to test it!
// 		Process.write( "\n", 1 );
// 	}
	else if( s_burnfreeCounterRx.exactMatch( line ) )
	{
		bool ok;
		int num = s_burnfreeCounterRx.cap(1).toInt(&ok);
		if( ok )
			emit infoMessage( QObject::tr("Burnfree was used %1 times.").arg(num), INFO );
	}
	else if( s_burnfreeCounterRxPredict.exactMatch( line ) )
	{
		bool ok;
		int num = s_burnfreeCounterRxPredict.cap(1).toInt(&ok);
		if( ok )
			emit infoMessage( QObject::tr("Buffer was low %1 times.").arg(num), INFO );
	}
	else
	{
		// debugging
		qDebug(QString("(cdrecord) %0").arg(line).toAscii());
	}
}

void STCdRecord::processOuptut(const QByteArray& _Arr)
{
	try
	{
		splitOutput(_Arr.data(), _Arr.length(), false );
		if (WriteDebug)
			qDebug(QString(_Arr).toLatin1());
	}
	catch (STCdRecord::Error _Error)
	{
		//Segons l'Id Podem fer altres coses...
		emit statusChanged(_Error.description(), 0);
	}
	catch(STError& _Error)
	{
		emit statusChanged(_Error.description(), 0);
	}
}


STCdRecord::STCdRecord(QObject* _Parent)
 : QProcess(_Parent)
{
	//TODO: Default settings for windows and MAC-OSX.
	configure("/usr/bin/cdrecord", "/tmp/kpstmp.iso", "/dev/cdrom", 48, false);
	connect(this, SIGNAL(readyReadStandardError()), this, SLOT(slotProcessError()));
	connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotProcessOutput()));
	connect(this, SIGNAL(infoMessage(QString, EnMessageType )), this, SLOT(showDebugMessage(QString, EnMessageType )));
}


void STCdRecord::checkForBinFile()
{
  	Assert(QFile::exists(BinFile), Error(QString(tr("cdrecord can't be found at %0")).arg(BinFile)));

	QProcess MProcess;
	MProcess.start(BinFile, QStringList() << "-version");
	Assert(MProcess.waitForStarted(), Error(tr("cdrecord: the process can't be started")));
	Assert(MProcess.waitForFinished(),	Error(tr("cdrecord: the process can't be finished")));

	QByteArray Result = MProcess.readAllStandardOutput();
	QString StrResult(Result);

	int Pos = StrResult.indexOf("cdrecord-clone", 0, Qt::CaseInsensitive);
	qDebug(StrResult.toLatin1());
	if (Pos < 0)
		Pos = StrResult.indexOf("cdrecord-ProDVD-Clone", 0, Qt::CaseInsensitive);
	if (Pos < 0)
		Pos = StrResult.indexOf("like-version", 0, Qt::CaseInsensitive);
	
	if (Pos >=0)
	{
		Assert(Pos >= 0, Error(tr("cdrecord-clone: where the version can be found.")));		
	
		Pos = StrResult.indexOf(QRegExp("[0-9]"), Pos);
		Assert(Pos >= 0, Error(tr("cdrecord: the version can't be detected..")));		
	
		int EndPos = StrResult.indexOf(' ', Pos+1);
		Assert(EndPos >= 0, Error(tr("cdrecord: the version can't be detected..")));		
		Version = StrResult.mid(Pos, EndPos-Pos);
	}
	else 	
		qWarning("Warning: Could not get cdrecord version"); 
	//TODO: !!!Comprovar la versió !!
}


void STCdRecord::start()
{
	m_totalTracksParsed = 0;
	m_currentTrack = 0;
	m_totalTracks = 0;
	m_totalSize = 0;
	m_alreadyWritten = 0;
	m_lastFifoValue = 0;
	trackSizes.clear();

	checkForBinFile();

	QStringList Arguments;
	Arguments << "-v" << QString("dev=%0").arg(Device) << QString("speed=%1").arg(Speed) <<
						WriteMode << "driveropts=burnfree" << "-eject"  << "-multi" <<  TmpIsoFile;
	qDebug(QString(BinFile + " " + Arguments.join(" ")).toLatin1() );
	QProcess::start(BinFile, Arguments);
}

void STCdRecord::configure(const QString& _STCdRecordBinary, const QString& _TmpIsoFile, const QString& _Device, int _Speed, bool _SimulationMode)
{
	BinFile = _STCdRecordBinary;//Settings.STCdRecordSTCdRecordBinary();
	TmpIsoFile = _TmpIsoFile; //Settings.STCdRecordTmpIsoFile();
	
	Device = _Device; //Settings.STCdRecordDevice();
	Speed = _Speed; //QString::number(Settings.STCdRecordSpeed());
	if (_SimulationMode)
		WriteMode = "-dummy";
	else 
		WriteMode = "-tao";
}

void STCdRecord::configure(const STBurningSettings& _Settings)
{
	configure(_Settings.cdRecordBinary(), _Settings.tmpIsoFile(), _Settings.device(), _Settings.speed(), _Settings.simulationMode());
}

QString STCdRecord::getMsInfo()
{
	QProcess MSInfoProcess;
	checkForBinFile();
	
	QStringList Arguments;
	Arguments << QString("dev=%0").arg(Device) << "-msinfo";
	QString MSInfo = ""; // No multiSesion Info
	MSInfoProcess.start(BinFile, Arguments);
	
	QString Command = QString(BinFile + " " + Arguments.join(" "));
  	Assert(MSInfoProcess.waitForStarted(), Error(QString(tr("could not start command: %1")).arg(Command)));
  	Assert(MSInfoProcess.waitForFinished(), Error(QString(tr("could not start command: %1")).arg(Command)));
	
	QString ProcRes(MSInfoProcess.readAll());
	ProcRes = ProcRes.remove("\n");
	
	QRegExp MSInfoRegExp("[0-9]+[,][0-9]+");
	if (MSInfoRegExp.exactMatch(ProcRes))
		MSInfo = ProcRes;
	return MSInfo;
}

void STCdRecord::slotProcessError()
{
	QByteArray Arr = readAllStandardError();
	processOuptut(Arr);
}

void STCdRecord::slotProcessOutput()
{
	QByteArray Arr = readAllStandardOutput();
	processOuptut(Arr);
}

void STCdRecord::showDebugMessage(QString _Message, EnMessageType )
{
	if (WriteDebug)
		qDebug(QString("cdrecord::->" + _Message).toLatin1());
}
