#include <QtCore>
#include <QtNetwork>
#include <iostream>
#include <QDebug>

#include "ftpspider.h"
#include "sprocessstatuswidget.h"

using namespace std;

void FtpSpider::logError(const QString& _Error)
{
	LastError += _Error + "\n";
	qDebug() << _Error;
}

void FtpSpider::initStatus(const QString& _Message)
{
	if (ProcessWidget)
		ProcessWidget->showProgressBar(_Message, 100);
}

void FtpSpider::updateStatus(const QString& _Message, int _Percent = -1)
{
	emit statusUpdated(_Message, _Percent);
	if (ProcessWidget)
	{
		if (_Percent < 0)
			ProcessWidget->updateProgress(ProcessWidget->currentProgress(), _Message);
		else
			ProcessWidget->updateProgress(_Percent, _Message);

	}
}

FtpSpider::FtpSpider(QObject *parent)
    : QObject(parent)
{
    connect(&ftp, SIGNAL(done(bool)), this, SLOT(ftpDone(bool)));
    connect(&ftp, SIGNAL(listInfo(const QUrlInfo &)),
            this, SLOT(ftpListInfo(const QUrlInfo &)));
	connect(&ftp, SIGNAL(dataTransferProgress( qint64, qint64 )), this, SLOT(slotDataTransferProgress( qint64, qint64 )));
	LocalDir = "downloads";
	LastError = "";
	UserName = "anonymous";
	Password = "guest";
	ProcessWidget = 0;
}

void FtpSpider::processNextDirectory()
{
	if (!pendingDirs.isEmpty()) {
		currentDir = pendingDirs.takeFirst();
		currentLocalDir = LocalDir + "/" + QDir(currentDir).dirName();
		QDir(".").mkpath(currentLocalDir);

		ftp.cd(currentDir);
		ftp.list();
	} else {
		emit done();
	}
}

int FtpSpider::inactiveSeconds()
{
	return LastActivityTime.secsTo(QTime::currentTime());
}

bool FtpSpider::getDirectory(const QUrl &url)
{
	LastError = "";
	FilesToTransfer = 0;
	TransferedFiles = 0;
	LastActivityTime = QTime::currentTime();
    if (!url.isValid()) {
		logError("Error: Invalid URL");
        return false;
    }

    if (url.scheme() != "ftp") {
		logError("Error: URL must start with 'ftp:'");
        return false;
    }

    ftp.connectToHost(url.host(), url.port(21));
	ftp.login(UserName, Password);

	initStatus(QString(tr("Downloading from: %1").arg(url.toString())));

    QString path = url.path();
    if (path.isEmpty())
        path = "/";

    pendingDirs.append(path);
    processNextDirectory();

    return true;
}

void FtpSpider::ftpDone(bool error)
{
    if (error) {
		logError("Error: " + ftp.errorString());
    } else {
		qDebug() << "Downloaded " << qPrintable(currentDir) << " to "
             << qPrintable(QDir::convertSeparators(
                               QDir(currentLocalDir).canonicalPath()));
    }

    qDeleteAll(openedFiles);
    openedFiles.clear();

    processNextDirectory();
}

void FtpSpider::ftpListInfo(const QUrlInfo &urlInfo)
{
	qDebug() << urlInfo.name();
	if (urlInfo.isFile()) {
		//if (urlInfo.isReadable()) {
            QFile *file = new QFile(currentLocalDir + "/"
                                    + urlInfo.name());

            if (!file->open(QIODevice::WriteOnly)) {
				qDebug() << "Warning: Cannot open file "
                     << qPrintable(
                            QDir::convertSeparators(file->fileName()))
                     << endl;
                return;
            }
            ftp.get(urlInfo.name(), file);
            openedFiles.append(file);
			FilesToTransfer++;
		//}
    } else if (urlInfo.isDir() && !urlInfo.isSymLink()) {
        pendingDirs.append(currentDir + "/" + urlInfo.name());
    }
}

void FtpSpider::slotDataTransferProgress(qint64 _Done, qint64 _Total)
{
	LastActivityTime = QTime::currentTime();
	if (_Done == _Total)
		TransferedFiles++;
	updateStatus(tr("Transfering file... %1 \%").arg(((double)_Done / _Total) * 100 ), ((double)TransferedFiles / qMax(FilesToTransfer, 1)) * 100);
}
