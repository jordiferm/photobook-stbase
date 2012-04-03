#ifndef SPIDER_H
#define SPIDER_H

#include <QFtp>
#include <QStringList>

class QFile;
class SProcessStatusWidget;
class FtpSpider : public QObject
{
    Q_OBJECT

	QFtp ftp;
	QList<QFile *> openedFiles;
	QString currentDir;
	QString currentLocalDir;
	QStringList pendingDirs;
	QString LocalDir, UserName, Password;
	QString LastError;
	SProcessStatusWidget* ProcessWidget;

	void initStatus(const QString& _Message);
	void updateStatus(const QString& _Message, int _Percent);
	void logError(const QString& _Error);
	void processNextDirectory();

public:
	FtpSpider(QObject *parent = 0);

    bool getDirectory(const QUrl &url);
	void setLocalDir(const QString& _LocalDir) { LocalDir = _LocalDir; }
	void setUserName(const QString& _UserName) { UserName = _UserName; }
	void setPassword(const QString& _Password) { Password = _Password; }
	void setProcessStatusWidget(SProcessStatusWidget* _ProcessWidget) { ProcessWidget = _ProcessWidget; }

signals:
    void done();

private slots:
    void ftpDone(bool error);
    void ftpListInfo(const QUrlInfo &urlInfo);
	void slotDataTransferProgress(qint64 _Done, qint64 _Total);

};

#endif
