/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-22
 * Description : a generic widget to display metadata
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "metadatawidget.h"

// Qt includes.

#include <QTextDocument>
#include <QPaintDevice>
#include <QButtonGroup>
#include <QClipboard>
#include <QColorGroup>
#include <QDataStream>
#include <QFile>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QMimeData>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QApplication> 
#include <QFileDialog> 
#include <QDesktopServices> 
#if QT_VERSION < 0x040400
#include <QDir> 
#endif 



// Local includes.

#include "metadatalistview.h"
#include "metadatalistviewitem.h"
#include "mdkeylistviewitem.h"
#include "searchtextbar.h"

namespace Digikam
{

	class MetadataWidgetPriv
	{

	public:

		MetadataWidgetPriv()
		{
			toolButtons  = 0;
			levelButtons = 0;
			view         = 0;
			mainLayout   = 0;
			toolsGBox    = 0;
			levelGBox    = 0;
			searchBar    = 0;
		}

		QWidget                *levelGBox;
		QWidget                *toolsGBox;

		QGridLayout            *mainLayout;

		QButtonGroup           *toolButtons;
		QButtonGroup           *levelButtons;

		QString                 fileName;

		MetadataListView       *view;

		SearchTextBar          *searchBar;

		DMetadata               metadata;
		DMetadata::MetaDataMap  metaDataMap;
	};

	MetadataWidget::MetadataWidget(QWidget* parent, const char* name)
			: QWidget(parent), d(new MetadataWidgetPriv)
	{
		setObjectName(name);

		d->mainLayout           = new QGridLayout(this);
		//KIconLoader* iconLoader = KIconLoader::global();

		// -----------------------------------------------------------------

		d->levelGBox       = new QWidget(this);
		d->levelButtons    = new QButtonGroup(d->levelGBox);
		QHBoxLayout *hlay1 = new QHBoxLayout(d->levelGBox);
		d->levelButtons->setExclusive(true);

		QToolButton *simpleLevel = new QToolButton(d->levelGBox);
		//TODO: Set this icon.
		//simpleLevel->setIcon(iconLoader->loadIcon("user-identity", (KIconLoader::Group)KIconLoader::Toolbar));
		simpleLevel->setIcon(QIcon(":/digikam/user.png"));
		simpleLevel->setCheckable(true);
		simpleLevel->setWhatsThis(tr("Switch the tags view to a simple human-readable list"));
		simpleLevel->setToolTip(tr("Simple list"));
		d->levelButtons->addButton(simpleLevel, SIMPLE);

		QToolButton *fullLevel = new QToolButton(d->levelGBox);
		//TODO: Set this icon.
		//fullLevel->setIcon(iconLoader->loadIcon("view-media-playlist", (KIconLoader::Group)KIconLoader::Toolbar));
		fullLevel->setIcon(QIcon(":/digikam/view_list_details.png")); 
		fullLevel->setCheckable(true);
		fullLevel->setWhatsThis(tr("Switch the tags view to a full list"));
		fullLevel->setToolTip(tr("Full list"));
		d->levelButtons->addButton(fullLevel, FULL);

		hlay1->addWidget(simpleLevel);
		hlay1->addWidget(fullLevel);
		hlay1->setSpacing(1);
		hlay1->setMargin(0);

		// -----------------------------------------------------------------

		d->toolsGBox       = new QWidget(this);
		d->toolButtons     = new QButtonGroup(d->toolsGBox);
		QHBoxLayout *hlay2 = new QHBoxLayout(d->toolsGBox);

		QToolButton *saveMetadata = new QToolButton(d->toolsGBox);
		//TODO: Set this icon.
		//saveMetadata->setIcon(iconLoader->loadIcon("document-save", (KIconLoader::Group)KIconLoader::Toolbar));
		saveMetadata->setIcon(QIcon(":/digikam/saveas.png")); 
		saveMetadata->setWhatsThis(tr("Save metadata to a binary file"));
		saveMetadata->setToolTip(tr("Save metadata"));
		d->toolButtons->addButton(saveMetadata);

		QToolButton *printMetadata = new QToolButton(d->toolsGBox);
		//TODO: Set this icon.
		//printMetadata->setIcon(iconLoader->loadIcon("document-print", (KIconLoader::Group)KIconLoader::Toolbar));
		printMetadata->setIcon(QIcon(":/digikam/fileprint.png")); 
		printMetadata->setWhatsThis(tr("Print metadata to printer"));
		printMetadata->setToolTip(tr("Print metadata"));
		d->toolButtons->addButton(printMetadata);

		QToolButton *copy2ClipBoard = new QToolButton(d->toolsGBox);
		//TODO: Set this icon.
		//copy2ClipBoard->setIcon(iconLoader->loadIcon("edit-copy", (KIconLoader::Group)KIconLoader::Toolbar));
		copy2ClipBoard->setIcon(QIcon(":/digikam/editcopy.png")); 
		copy2ClipBoard->setWhatsThis(tr("Copy metadata to clipboard"));
		copy2ClipBoard->setToolTip(tr("Copy metadata to clipboard"));
		d->toolButtons->addButton(copy2ClipBoard);

		hlay2->addWidget(saveMetadata);
		hlay2->addWidget(printMetadata);
		hlay2->addWidget(copy2ClipBoard);
		hlay2->setSpacing(1);
		hlay1->setMargin(0);

		d->view         = new MetadataListView(this);
		QString barName = QString(name) + "SearchBar";
		d->searchBar    = new SearchTextBar(this, barName.toAscii());

		// -----------------------------------------------------------------

		d->mainLayout->addWidget(d->levelGBox, 0, 0, 1, 2);
		d->mainLayout->addWidget(d->toolsGBox, 0, 4, 1, 1);
		d->mainLayout->addWidget(d->view,      1, 0, 1, 5);
		d->mainLayout->addWidget(d->searchBar, 2, 0, 1, 5);
		d->mainLayout->setColumnStretch(3, 10);
		d->mainLayout->setRowStretch(1, 10);
		d->mainLayout->setSpacing(0);
		d->mainLayout->setMargin(1);

		// -----------------------------------------------------------------

		connect(d->levelButtons, SIGNAL(buttonReleased(int)),
		        this, SLOT(slotModeChanged(int)));

		connect(copy2ClipBoard, SIGNAL(clicked()),
		        this, SLOT(slotCopy2Clipboard()));

		connect(printMetadata, SIGNAL(clicked()),
		        this, SLOT(slotPrintMetadata()));

		connect(saveMetadata, SIGNAL(clicked()),
		        this, SLOT(slotSaveMetadataToFile()));

		connect(d->searchBar, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
		        d->view, SLOT(slotSearchTextChanged(const SearchTextSettings&)));

		connect(d->view, SIGNAL(signalTextFilterMatch(bool)),
		        d->searchBar, SLOT(slotSearchResult(bool)));
	}

	MetadataWidget::~MetadataWidget()
	{
		delete d;
	}

	MetadataListView* MetadataWidget::view()
	{
		return d->view;
	}

	void MetadataWidget::enabledToolButtons(bool b)
	{
		d->toolsGBox->setEnabled(b);
	}

	bool MetadataWidget::setMetadata(const DMetadata& data)
	{
		d->metadata = DMetadata(data);

		// Cleanup all metadata contents.
		setMetadataMap();

		if (d->metadata.isEmpty())
		{
			setMetadataEmpty();
			return false;
		}

		// Try to decode current metadata.
		if (decodeMetadata())
			enabledToolButtons(true);
		else
			enabledToolButtons(false);

		// Refresh view using decoded metadata.
		buildView();

		return true;
	}

	void MetadataWidget::setMetadataEmpty()
	{
		d->view->clear();
		enabledToolButtons(false);
	}

	const DMetadata& MetadataWidget::getMetadata()
	{
		return d->metadata;
	}

	bool MetadataWidget::storeMetadataToFile(const QUrl& url, const QByteArray& metaData)
	{
		if (url.isEmpty())
			return false;

		QFile file(url.path());
		if (!file.open(QIODevice::WriteOnly))
			return false;

		QDataStream stream(&file);
		stream.writeRawData(metaData.data(), metaData.size());
		file.close();
		return true;
	}

	void MetadataWidget::setMetadataMap(const DMetadata::MetaDataMap& data)
	{
		d->metaDataMap = data;
	}

	const DMetadata::MetaDataMap& MetadataWidget::getMetadataMap()
	{
		return d->metaDataMap;
	}

	void MetadataWidget::setIfdList(const DMetadata::MetaDataMap &ifds, const QStringList& tagsFilter)
	{
		d->view->setIfdList(ifds, tagsFilter);
	}

	void MetadataWidget::setIfdList(const DMetadata::MetaDataMap &ifds, const QStringList& keysFilter,
	                                const QStringList& tagsFilter)
	{
		d->view->setIfdList(ifds, keysFilter, tagsFilter);
	}

	void MetadataWidget::slotModeChanged(int)
	{
		buildView();
	}

	void MetadataWidget::slotCopy2Clipboard()
	{
		QString textmetadata  = tr("File name: %1 (%2)").arg(d->fileName, getMetadataTitle());
		int i                 = 0;
		QTreeWidgetItem *item = 0;
		do
		{
			item                      = d->view->topLevelItem(i);
			MdKeyListViewItem* lvItem = dynamic_cast<MdKeyListViewItem*>(item);
			if (lvItem)
			{
				textmetadata.append("\n\n>>> ");
				textmetadata.append(lvItem->getMdKey());
				textmetadata.append(" <<<\n\n");

				int j                  = 0;
				QTreeWidgetItem *item2 = 0;
				do
				{
					item2                         = dynamic_cast<QTreeWidgetItem*>(lvItem)->child(j);
					MetadataListViewItem* lvItem2 = dynamic_cast<MetadataListViewItem*>(item2);
					if (lvItem2)
					{
						textmetadata.append(lvItem2->text(0));
						textmetadata.append(" : ");
						textmetadata.append(lvItem2->text(1));
						textmetadata.append("\n");
					}
					j++;
				}
				while (item2);
			}
			i++;
		}
		while (item);

		QMimeData *mimeData = new QMimeData();
		mimeData->setText(textmetadata);
		QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
	}

	void MetadataWidget::slotPrintMetadata()
	{
		QString textmetadata = tr("<p><big><big><b>File name: %1 (%2)</b></big></big>").arg(
		                            d->fileName, getMetadataTitle());

		int i                 = 0;
		QTreeWidgetItem *item = 0;
		do
		{
			item                      = d->view->topLevelItem(i);
			MdKeyListViewItem* lvItem = dynamic_cast<MdKeyListViewItem*>(item);
			if (lvItem)
			{
				textmetadata.append("<br/><br/><b>");
				textmetadata.append(lvItem->getMdKey());
				textmetadata.append("</b><br/><br/>");

				int j                  = 0;
				QTreeWidgetItem *item2 = 0;
				do
				{
					item2                         = dynamic_cast<QTreeWidgetItem*>(lvItem)->child(j);
					MetadataListViewItem* lvItem2 = dynamic_cast<MetadataListViewItem*>(item2);
					if (lvItem2)
					{
						textmetadata.append(lvItem2->text(0));
						textmetadata.append(" : <i>");
						textmetadata.append(lvItem2->text(1));
						textmetadata.append("</i><br/>");
					}
					j++;
				}
				while (item2);
			}
			i++;
		}
		while (item);

		textmetadata.append("</p>");

		QPrinter printer;
		printer.setFullPage(true);

		QPrintDialog dialog(&printer, qApp->activeWindow());
		if (dialog.exec())
		{
			QTextDocument doc;
			doc.setHtml(textmetadata);
			QFont font(QApplication::font());
			font.setPointSize(10);                // we define 10pt to be a nice base size for printing.
			doc.setDefaultFont(font);
			doc.print(&printer);
		}
	}

	QUrl MetadataWidget::saveMetadataToFile(const QString& caption, const QString& fileFilter)
	{
	#if QT_VERSION >= 0x040400
		QFileDialog fileSaveDialog(this, caption, QString(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)), fileFilter);
	#else 
		QFileDialog fileSaveDialog(this, caption, QString(QDir::homePath()), fileFilter);
	#endif 
		fileSaveDialog.setAcceptMode(QFileDialog::AcceptSave); 
		//fileSaveDialog.setOperationMode(KFileDialog::Saving);
		fileSaveDialog.setFileMode(QFileDialog::AnyFile); 
		//fileSaveDialog.setMode(KFile::File);
		fileSaveDialog.selectFile(d->fileName); 
		//fileSaveDialog.setSelection(d->fileName);
		//fileSaveDialog.setCaption(caption);
		//fileSaveDialog.setFilter(fileFilter);

		// Check for cancel.
		if (fileSaveDialog.exec() == QFileDialog::Accepted)
			return QUrl(fileSaveDialog.selectedFiles().first());

		return QUrl();
	}

	void MetadataWidget::setMode(int mode)
	{
		if (d->levelButtons->checkedId() == mode)
			return;

		d->levelButtons->button(mode)->setChecked(true);
		buildView();
	}

	int MetadataWidget::getMode()
	{
		int level = d->levelButtons->checkedId();
		return level;
	}

	QString MetadataWidget::getCurrentItemKey() const
	{
		return d->view->getCurrentItemKey();
	}

	void MetadataWidget::setCurrentItemByKey(const QString& itemKey)
	{
		d->view->setCurrentItemByKey(itemKey);
	}

	bool MetadataWidget::loadFromData(const QString& fileName, const DMetadata& data)
	{
		setFileName(fileName);
		return(setMetadata(data));
	}

	QString MetadataWidget::getTagTitle(const QString&)
	{
		return QString();
	}

	QString MetadataWidget::getTagDescription(const QString&)
	{
		return QString();
	}

	void MetadataWidget::setFileName(const QString& fileName)
	{
		d->fileName = fileName;
	}

	void MetadataWidget::setUserAreaWidget(QWidget *w)
	{
		QVBoxLayout *vLayout = new QVBoxLayout();
		vLayout->setSpacing(1);
		vLayout->addWidget(w);
		vLayout->addStretch();
		d->mainLayout->addLayout(vLayout, 3, 0, 1, 5);
	}

	void MetadataWidget::buildView()
	{
		d->view->slotSearchTextChanged(d->searchBar->searchTextSettings());
	}

}  // namespace Digikam
