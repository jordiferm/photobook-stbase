/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-21
 * Description : a generic list view widget to
 *               display metadata
 *
 * Copyright (c) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "metadatalistview.h"

// Qt includes.

#include <QHeaderView>
#include <QTimer>
#include <QPalette>

// Local includes.

#include "mdkeylistviewitem.h"
#include "metadatalistviewitem.h"

namespace Digikam
{

MetadataListView::MetadataListView(QWidget* parent)
                : QTreeWidget(parent)
{
    setRootIsDecorated(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    setColumnCount(2);
    header()->setResizeMode(QHeaderView::Stretch);
    header()->hide();

    QStringList labels;
    labels.append( "Name" );        // no i18n here: hidden header
    labels.append( "Value" );       // no i18n here: hidden header
    setHeaderLabels(labels);

    m_parent = dynamic_cast<MetadataWidget *>(parent);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotSelectionChanged(QTreeWidgetItem*, int)));
}

MetadataListView::~MetadataListView()
{
}

QString MetadataListView::getCurrentItemKey()
{
    if (currentItem() && currentItem()->flags() & Qt::ItemIsSelectable)
    {
        MetadataListViewItem *item = static_cast<MetadataListViewItem *>(currentItem());
        return item->getKey();
    }

    return QString();
}

void MetadataListView::setCurrentItemByKey(const QString& itemKey)
{
    if (itemKey.isNull())
        return;

    int i                 = 0;
    QTreeWidgetItem *item = 0;
    do
    {
        item = topLevelItem(i);
        if (item && item->flags() & Qt::ItemIsSelectable)
        {
            MetadataListViewItem* lvItem = dynamic_cast<MetadataListViewItem*>(item);
            if (lvItem)
            {
                if (lvItem->getKey() == itemKey)
                {
                    setCurrentItem(item);
                    scrollToItem(item);
                    m_selectedItemKey = itemKey;
                    return;
                }
            }
        }
        i++;
    }
    while (item);
}

void MetadataListView::slotSelectionChanged(QTreeWidgetItem *item, int)
{
    if (!item)
        return;

    MetadataListViewItem* viewItem = static_cast<MetadataListViewItem *>(item);
    m_selectedItemKey = viewItem->getKey();
    QString tagValue  = viewItem->getValue().simplified();
    QString tagTitle  = m_parent->getTagTitle(m_selectedItemKey);
    QString tagDesc   = m_parent->getTagDescription(m_selectedItemKey);
    if (tagValue.length() > 128)
    {
        tagValue.truncate(128);
        tagValue.append("...");
    }

    this->setWhatsThis( tr("<b>Title: </b><p>%1</p>"
                             "<b>Value: </b><p>%2</p>"
                             "<b>Description: </b><p>%3</p>").arg(
                             tagTitle).arg(tagValue).arg(tagDesc));
}

void MetadataListView::setIfdList(const DMetadata::MetaDataMap& ifds, const QStringList& tagsfilter)
{
    clear();

    uint               subItems = 0;
    QString            ifDItemName;
    MdKeyListViewItem *parentifDItem = 0;

    for (DMetadata::MetaDataMap::const_iterator it = ifds.begin(); it != ifds.end(); ++it)
    {
        // We checking if we have changed of ifDName
        QString currentIfDName = it.key().section('.', 1, 1);

        if ( currentIfDName != ifDItemName )
        {
            ifDItemName = currentIfDName;

            // Check if the current IfD have any items. If no remove it before to toggle to the next IfD.
            if ( subItems == 0 && parentifDItem)
                delete parentifDItem;

            parentifDItem = new MdKeyListViewItem(this, currentIfDName);
            subItems = 0;
        }

        // We ignore all unknown tags if necessary.
        if (!it.key().section('.', 2, 2).startsWith("0x"))
        {
            if (!tagsfilter.isEmpty())
            {
                // We using the filter to make a more user friendly output (Simple Mode)

                if (tagsfilter.contains(it.key().section('.', 2, 2), Qt::CaseInsensitive))
                {
                    QString tagTitle = m_parent->getTagTitle(it.key());
                    new MetadataListViewItem(parentifDItem, it.key(), tagTitle, it.value());
                    subItems++;
                }
            }
            else
            {
                // We don't filter the output (Complete Mode)

                QString tagTitle = m_parent->getTagTitle(it.key());
                new MetadataListViewItem(parentifDItem, it.key(), tagTitle, it.value());
                subItems++;
            }
        }
    }

    // To check if the last IfD have any items...
    if ( subItems == 0 && parentifDItem)
        delete parentifDItem;

    setCurrentItemByKey(m_selectedItemKey);
    update();
}

void MetadataListView::setIfdList(const DMetadata::MetaDataMap& ifds, const QStringList& keysFilter,
                                  const QStringList& tagsFilter)
{
    clear();

    uint               subItems = 0;
    MdKeyListViewItem *parentifDItem = 0;
    if (ifds.count() == 0) return;

    for (QStringList::const_iterator itKeysFilter = keysFilter.begin();
         itKeysFilter != keysFilter.end();
         ++itKeysFilter)
    {
        subItems = 0;

        parentifDItem = new MdKeyListViewItem(this, *itKeysFilter);

        DMetadata::MetaDataMap::const_iterator it = ifds.constEnd();
        while (it != ifds.constBegin())
        {
            --it;

            if ( *itKeysFilter == it.key().section('.', 1, 1) )
            {
                // We ignore all unknown tags if necessary.
                if (!it.key().section('.', 2, 2).startsWith("0x"))
                {
                    if (!tagsFilter.isEmpty())
                    {
                        // We using the filter to make a more user friendly output (Simple Mode)

                        if (tagsFilter.contains(it.key().section('.', 2, 2)))
                        {
                            QString tagTitle = m_parent->getTagTitle(it.key());
                            new MetadataListViewItem(parentifDItem, it.key(), tagTitle, it.value());
                            subItems++;
                        }
                    }
                    else
                    {
                        // We don't filter the output (Complete Mode)

                        QString tagTitle = m_parent->getTagTitle(it.key());
                        new MetadataListViewItem(parentifDItem, it.key(), tagTitle, it.value());
                        subItems++;
                    }
                }
            }
        }

        // We checking if the last IfD have any items. If no, we remove it.
        if ( subItems == 0 && parentifDItem)
            delete parentifDItem;
    }

    setCurrentItemByKey(m_selectedItemKey);
    update();
}

void MetadataListView::slotSearchTextChanged(const SearchTextSettings& settings)
{
    bool query     = false;
    QString search = settings.text;

    QTreeWidgetItemIterator it(this);
    while (*it)
    {
        MetadataListViewItem *item = dynamic_cast<MetadataListViewItem*>(*it);
        if (item)
        {
            if (item->text(0).contains(search, settings.caseSensitive) ||
                item->text(1).contains(search, settings.caseSensitive))
            {
                query = true;
                item->setHidden(false);
            }
            else
            {
                item->setHidden(true);
            }
        }
        ++it;
    }

    emit signalTextFilterMatch(query);
}

}  // namespace Digikam
