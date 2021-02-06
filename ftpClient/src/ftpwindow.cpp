#include <QtWidgets>
#include <QtNetwork>
#include "ftpwindow.h"

FtpWindow::FtpWindow(QWidget* parent)
    : QDialog(parent), ftp(0), networkSession(0), downloadBytes(0),
    downloadTotalBytes(0), downloadTotalFiles(0), downloadPath("c:/temp/ftpdown"),
    downFinished(true), enterSubDir(false), currentDownPath("")
{
    ftpServerLabel = new QLabel(tr("Ftp &server:"));
    ftpServerLineEdit = new QLineEdit("ftp://test:a1b2c3@127.0.0.1:21");
    ftpServerLabel->setBuddy(ftpServerLineEdit);

    statusLabel = new QLabel(tr("Please enter the name of an FTP server."));

    fileList = new QTreeWidget;
    fileList->setEnabled(false);
    fileList->setRootIsDecorated(false);
    fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileList->setHeaderLabels(QStringList() << tr("Name") << tr("Size") << tr("IsDir") << tr("Owner") << tr("Group") << tr("Time"));
    fileList->setColumnWidth(0, 200);
    fileList->header()->setStretchLastSection(false);

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);

    cdToParentButton = new QPushButton;
    cdToParentButton->setIcon(QPixmap(":/images/cdtoparent.png"));
    cdToParentButton->setEnabled(false);

    downloadButton = new QPushButton(tr("Download"));
    downloadButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(downloadButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

	progressDialog = new QProgressDialog(this);
	//progressDialog = new QProgressDialog("download...", nullptr, 0, 100, this);
    //progressDialog->setWindowModality(Qt::WindowModal);
    //auto winFlags = windowFlags() & ~Qt::WindowMinMaxButtonsHint;
    //progressDialog->setWindowFlags(winFlags &~ Qt::WindowCloseButtonHint);
    progressDialog->reset();
    //progressDialog->setAutoClose(false);
    //progressDialog->setAutoReset(false);

    connect(fileList, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(processItem(QTreeWidgetItem*,int)));
    connect(fileList, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(enableDownloadButton()));
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectOrDisconnect()));
    connect(cdToParentButton, SIGNAL(clicked()), this, SLOT(cdToParent()));
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(ftpServerLabel);
    topLayout->addWidget(ftpServerLineEdit);
    topLayout->addWidget(cdToParentButton);
    topLayout->addWidget(connectButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(fileList);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("FTP"));
}

QSize FtpWindow::sizeHint() const
{
    return QSize(500, 300);
}

//![0]
void FtpWindow::connectOrDisconnect()
{
    if (ftp) {
        ftp->abort();
        ftp->deleteLater();
        ftp = 0;
//![0]
        fileList->setEnabled(false);
        cdToParentButton->setEnabled(false);
        downloadButton->setEnabled(false);
        connectButton->setEnabled(true);
        connectButton->setText(tr("Connect"));
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif
        statusLabel->setText(tr("Please enter the name of an FTP server."));
        return;
    }

#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif

    if (!networkSession || !networkSession->isOpen()) {
        if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
            if (!networkSession) {
                // Get saved network configuration
                QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
                settings.beginGroup(QLatin1String("QtNetwork"));
                const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
                settings.endGroup();

                // If the saved network configuration is not currently discovered use the system default
                QNetworkConfiguration config = manager.configurationFromIdentifier(id);
                if ((config.state() & QNetworkConfiguration::Discovered) !=
                    QNetworkConfiguration::Discovered) {
                    config = manager.defaultConfiguration();
                }

                networkSession = new QNetworkSession(config, this);
                connect(networkSession, SIGNAL(opened()), this, SLOT(connectToFtp()));
                connect(networkSession, SIGNAL(error(QNetworkSession::SessionError)), this, SLOT(enableConnectButton()));
            }
            connectButton->setEnabled(false);
            statusLabel->setText(tr("Opening network session."));
            networkSession->open();
            return;
        }
    }
    connectToFtp();
}

void FtpWindow::connectToFtp()
{
//![1]
    ftp = new QFtp(this);
    connect(ftp, SIGNAL(commandFinished(int,bool)),
            this, SLOT(ftpCommandFinished(int,bool)));
    connect(ftp, SIGNAL(listInfos(QVector<QUrlInfo>)),
            this, SLOT(addToList(QVector<QUrlInfo>)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
            this, SLOT(updateDataTransferProgress(qint64,qint64)));

    fileList->clear();
    currentPath.clear();
    isDirectory.clear();
//![1]

//![2]
    QUrl url(ftpServerLineEdit->text());
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
        ftp->connectToHost(ftpServerLineEdit->text(), 21);
        ftp->login();
    } else {
        ftp->connectToHost(url.host(), url.port(21));

        if (!url.userName().isEmpty())
            ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        else
            ftp->login();
        if (!url.path().isEmpty())
            ftp->cd(url.path());
    }
//![2]

    fileList->setEnabled(true);
    connectButton->setEnabled(false);
    connectButton->setText(tr("Disconnect"));
    statusLabel->setText(tr("Connecting to FTP server %1...")
                         .arg(ftpServerLineEdit->text()));
}

//![3]
void FtpWindow::downloadFile()
{
	downFinished = false;
	//QTimer::singleShot(0, this, SLOT(showProgressDialog()));
	downAllFile(currentPath);
	showProgressDialog();
}

void FtpWindow::downAllFile(QString rootDir) {
    QString thisRoot(rootDir + "/");
	QList<QTreeWidgetItem*> selectedItemList = fileList->selectedItems();
	for (int i = 0; i < selectedItemList.size(); i++)
	{
		QString fileName = selectedItemList[i]->text(0);
        if (isDirectory.value(fileName)) {
            if(fileName != "..")
                downDirs.push(thisRoot + fileName);
        }
        else {
			downloadTotalBytes += selectedItemList[i]->text(1).toLongLong();
            QString dirTmp(downloadPath);
            dirTmp.append(rootDir);
            QDir downDir(dirTmp);
            if (!downDir.exists(dirTmp)) {
                downDir.mkpath(dirTmp);
			}
            QFile* file = new QFile(dirTmp.append("/").append(fileName));
			if (!file->open(QIODevice::WriteOnly)) {
				QMessageBox::information(this, tr("FTP"),
					tr("Unable to save the file %1: %2.")
					.arg(fileName).arg(file->errorString()));
				delete file;
				return;
			}
			int id = ftp->get(QString::fromLatin1((selectedItemList[i]->text(0)).toStdString().c_str()), file);
			files.insert(id, file);
        }
	}
    if (downDirs.size() > 0) {
        enterSubDir = true;
        QString nextDir(downDirs.pop());
        ftp->cd(nextDir);
        currentDownPath = nextDir;
        ftp->list();
    }
    else {
        enterSubDir = false;
		ftp->cd(currentPath == "" ? "/" : currentPath);
		ftp->list();
		downloadButton->setEnabled(false);
		downloadTotalFiles = files.size();
    }
}

void FtpWindow::showProgressDialog() {
    if (!downFinished) {    // && downloadTotalFiles > 0
        progressDialog->setLabelText(tr("Downloading selected files ..."));
        progressDialog->exec();
    }
}

void FtpWindow::cancelDownload()
{
	ftp->abort();
	downloadTotalBytes = 0;
	downloadBytes = 0; 
    if (enterSubDir) {
        QString curTmp = currentPath;
        enterSubDir = false;
		downDirs.clear();
		connectButton->click();
		connectButton->click();
        ftp->cd(curTmp == "" ? "/" : curTmp);
        files.clear();
        //QTimer::singleShot(100, this, SLOT(clearDownFilesWhenCancleDownDir()));
    }
}

void FtpWindow::clearDownFilesWhenCancleDownDir() {
	QMapIterator<int, QFile*> i(files);
	while (i.hasNext()) {
		i.next();
		QFile* file = i.value();
		file->close();
		file->remove();
		delete file;
	}
	files.clear();
}

void FtpWindow::ftpCommandFinished(int id, bool error)
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::ArrowCursor);
#endif

    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct.")
                                     .arg(ftpServerLineEdit->text()));
            connectOrDisconnect();
            return;
        }
        statusLabel->setText(tr("Logged onto %1.")
                             .arg(ftpServerLineEdit->text()));
        fileList->setFocus();
        downloadButton->setDefault(true);
        connectButton->setEnabled(true);
        return;
    }

    if (ftp->currentCommand() == QFtp::Login)
        ftp->list();

    if (ftp->currentCommand() == QFtp::Get) {
        QFile* file = files.take(id);
        if (error) {
            statusLabel->setText(tr("Canceled download of %1.")
                                 .arg(file->fileName()));
            file->close();
            file->remove();
			QString dd = currentPath == "" ? "/" : currentPath;
			bool f = ftp->cd(dd);
			ftp->list();
			qDebug() << "Cancle the file success: " << file->fileName();
		}
		else {
            QStringList fileInfo = file->fileName().split("/");
            statusLabel->setText(tr("Download success, filename : %1")
                                 .arg(fileInfo[fileInfo.size() - 1]));
            file->close();
			qDebug() << "Download the file success: " << file->fileName();
		}
        delete file;
        file = nullptr;
		if (files.size() == 0 && !enterSubDir) {
			downFinished = true;
			enableDownloadButton();
			progressDialog->reset();
			progressDialog->hide();
			downloadBytes = 0;
			downloadTotalBytes = 0;
			downloadTotalFiles = 0;
		}
    } else if (ftp->currentCommand() == QFtp::List) {
        if (isDirectory.isEmpty()) {
            fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            fileList->setEnabled(false);
        }
    }
//![9]
}

//![10]
void FtpWindow::addToList(const QVector<QUrlInfo>& urlInfos)
{
    fileList->clear();
	for (int i = 0; i < urlInfos.size(); i++)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem;
		QUrlInfo urlInfo = urlInfos[i];
		if (urlInfo.name().compare(".") != 0) {
			item->setText(0, urlInfo.name().toLatin1());
			item->setText(1, QString::number(urlInfo.size()));
			item->setText(2, QString::number(urlInfo.isDir()));
			item->setText(3, urlInfo.owner());
			item->setText(4, urlInfo.group());
			item->setText(5, urlInfo.lastModified().toString("MMM dd yyyy"));

			QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
			item->setIcon(0, pixmap);

			isDirectory[urlInfo.name()] = urlInfo.isDir();
			fileList->addTopLevelItem(item);
		}
	}

	if (!enterSubDir) {
		if (!fileList->currentItem()) {
			fileList->setCurrentItem(fileList->topLevelItem(0));
			fileList->setEnabled(true);
		}
	}
	else {
		fileList->selectAll();
		downAllFile(currentDownPath);
		//cdToParent();
        //if (currentPath == currentDownPath) {
        //    enterSubDir = false;
        //}
	}
}
//![10]

//![11]
void FtpWindow::processItem(QTreeWidgetItem *item, int /*column*/)
{
    QString name = item->text(0);
    if (isDirectory.value(name)) {
        fileList->clear();
        isDirectory.clear();
        currentPath += '/';
        currentPath += name;
        ftp->cd(name);
        ftp->list();
        cdToParentButton->setEnabled(true);
#ifndef QT_NO_CURSOR
        setCursor(Qt::WaitCursor);
#endif
        return;
    }
}
//![11]

//![12]
void FtpWindow::cdToParent()
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif
    fileList->clear();
    isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    if (currentPath.isEmpty()) {
        cdToParentButton->setEnabled(false);
        ftp->cd("/");
    } else {
        ftp->cd(currentPath);
    }
    ftp->list();
}
//![12]

//![13]
void FtpWindow::updateDataTransferProgress(qint64 readBytes,
                                           qint64 totalBytes)
{
    downloadBytes += readBytes;
    if (!progressDialog->isHidden()) {
		progressDialog->setMaximum(downloadTotalBytes);
		progressDialog->setValue(downloadBytes);
        /*if (oldDownTotalBytes != downloadTotalBytes) {
			qDebug() << "----------1------------------" << downloadBytes << " / " << downloadTotalBytes;
            oldDownTotalBytes = downloadTotalBytes;
        }*/
    }
}
//![13]

//![14]
void FtpWindow::enableDownloadButton()
{
    downloadButton->setEnabled(true);
   /* QTreeWidgetItem *current = fileList->currentItem();
    if (current) {
        QString currentFile = current->text(0);
        downloadButton->setEnabled(!isDirectory.value(currentFile));
    } else {
        downloadButton->setEnabled(false);
    }*/
}
//![14]

void FtpWindow::enableConnectButton()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    connectButton->setEnabled(true);
    statusLabel->setText(tr("Please enter the name of an FTP server."));
}

