#ifndef _POSIX_QEXTSERIALPORT_H_
#define _POSIX_QEXTSERIALPORT_H_

#include <stdio.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include "qextserialbase.h"

/*ensure handling of CRTSCTS constant*/
#ifdef CNEW_RTSCTS
#ifndef CRTSCTS
#define CRTSCTS CNEW_RTSCTS
#endif  //CRTSCTS
#else 
#ifndef CRTSCTS
#define CRTSCTS     0
#endif  //CRTSCTS
#endif  //CNEW_RTSCTS

//weird scoping issue with gcc 2.95 - this should fix it
#if QT_VERSION <= 0x040000
	#ifdef QT_LARGE_FILE_SUPPORT
		typedef off_t Offset;
	#else
		typedef Q_ULONG Offset;
	#endif
#endif

class Posix_QextSerialPort:public QextSerialBase {
public:
    Posix_QextSerialPort();           
    Posix_QextSerialPort(const Posix_QextSerialPort& s);
    Posix_QextSerialPort(const char* name);
    Posix_QextSerialPort(const PortSettings& settings);
    Posix_QextSerialPort(const char* name, const PortSettings& settings);
    Posix_QextSerialPort& operator=(const Posix_QextSerialPort& s);
    virtual ~Posix_QextSerialPort();
    virtual bool open(int mode=0);
    virtual void close();
    virtual void flush();
    virtual qlonglong size() const;
	 
	 #if QT_VERSION < 0x040000
	 virtual int getch();
    virtual int putch(int);
	 #endif
	 
    virtual int ungetch(int);
    virtual void setFlowControl(FlowType);
    virtual void setParity(ParityType);
    virtual void setDataBits(DataBitsType);
    virtual void setStopBits(StopBitsType);
    virtual void setBaudRate(BaudRateType);
    virtual void setDtr(bool set=true);
    virtual void setRts(bool set=true);
    virtual unsigned long lineStatus(void);
    virtual bool waitForReadyRead(int _MSecs);
	 virtual	qint64 bytesAvailable() const;
    virtual int bytesWaiting();
    virtual void translateError(unsigned long);
    virtual void setTimeout(unsigned long=0, unsigned long=0);

#if QT_VERSION >= 0x040000
virtual qint64 readData ( char * data, qint64 maxSize );
virtual qint64 writeData(const char *data, qint64 maxSize);
#else
	#ifdef QTVER_PRE_30
		virtual Q_LONG readBlock(char *data, uint maxlen);
		virtual Q_LONG writeBlock(const char *data, uint len);
	#else
		virtual Q_LONG readBlock(char *data, unsigned long maxlen);
		virtual Q_LONG writeBlock(const char *data, unsigned long len);
	#endif
#endif

protected:
    QFile* Posix_File;
    struct termios Posix_CommConfig;
    struct timeval Posix_Timeout;
    struct timeval Posix_Copy_Timeout;
    void construct(void);
};

#endif
