#include <QtCore>
#include <QtNetwork>
#include <iostream>

#include "ftpspider.h"
#include "sprocessstatuswidget.h"

using namespace std;

void FtpSpider::logError(const QString& _Error)
{
	LastError += _Error + "\n";
}

void FtpSpider::initStatus(const QString& _Message)
{
	if (ProcessWidget)
		ProcessWidget->showProgressBar(_Message, 100);
}

void FtpSpider::updateStatus(const QString& _Message, int _Percent)
{
	if (ProcessWidget)
		ProcessWidget->updateProgress(_Percent, _Message);
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
		currentLocalDir = LocalDir + "/" + currentDir;
		QDir(".").mkpath(currentLocalDir);

		ftp.cd(currentDir);
		ftp.list();
	} else {
		emit done();
	}
}

bool FtpSpider::getDirectory(const QUrl &url)
{
	LastError = "";
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
        cout << "Downloaded " << qPrintable(currentDir) << " to "
             << qPrintable(QDir::convertSeparators(
                               QDir(currentLocalDir).canonicalPath()));
    }

    qDeleteAll(openedFiles);
    openedFiles.clear();

    processNextDirectory();
}

void FtpSpider::ftpListInfo(const QUrlInfo &urlInfo)
{
    if (urlInfo.isFile()) {
        if (urlInfo.isReadable()) {
            QFile *file = new QFile(currentLocalDir + "/"
                                    + urlInfo.name());

            if (!file->open(QIODevice::WriteOnly)) {
                cerr << "Warning: Cannot open file "
                     << qPrintable(
                            QDir::convertSeparators(file->fileName()))
                     << endl;
                return;
            }

            ftp.get(urlInfo.name(), file);
            openedFiles.append(file);
        }
    } else if (urlInfo.isDir() && !urlInfo.isSymLink()) {
        pendingDirs.append(currentDir + "/" + urlInfo.name());
    }
}

void FtpSpider::slotDataTransferProgress(qint64 _Done, qint64 _Total)
{
	updateStatus("Transfering...", (int)(_Done / _Total));
}
