/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-21
 * Description : a generic list view item widget to
 *               display metadata key like a title
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

#include "mdkeylistviewitem.h"

// Qt includes.

#include <QPalette>
#include <QFont>
#include <QPainter>


// Local includes.

#include "themeengine.h"

namespace Digikam
{

MdKeyListViewItem::MdKeyListViewItem(QTreeWidget *parent, const QString& key)
                 : QObject(parent), QTreeWidgetItem(parent)
{
    m_decryptedKey = key;

    // Standard Exif key descriptions.
    if      (key == "Iop")            m_decryptedKey = tr("Interoperability");
    else if (key == "Image")          m_decryptedKey = tr("Image Information");
    else if (key == "Photo")          m_decryptedKey = tr("Photograph Information");
    else if (key == "GPSInfo")        m_decryptedKey = tr("Global Positioning System");
    else if (key == "Thumbnail")      m_decryptedKey = tr("Embedded Thumbnail");

    // Standard IPTC key descriptions.
    else if (key == "Envelope")       m_decryptedKey = tr("IIM Envelope");
    else if (key == "Application2")   m_decryptedKey = tr("IIM Application 2");

    // Standard XMP key descriptions.
    else if (key == "dc")             m_decryptedKey = tr("Dublin Core");
    else if (key == "xmp")            m_decryptedKey = tr("Basic Schema");
    else if (key == "xmpRights")      m_decryptedKey = tr("Rights Management");
    else if (key == "xmpMM")          m_decryptedKey = tr("Media Management ");
    else if (key == "xmpBJ")          m_decryptedKey = tr("Basic Job Ticket");
    else if (key == "xmpTPg")         m_decryptedKey = tr("Paged-Text");
    else if (key == "xmpDM")          m_decryptedKey = tr("Dynamic Media");
    else if (key == "MicrosoftPhoto") m_decryptedKey = tr("Microsoft Photo");
    else if (key == "pdf")            m_decryptedKey = tr("Adobe PDF");
    else if (key == "photoshop")      m_decryptedKey = tr("Adobe Photoshop");
    else if (key == "crs")            m_decryptedKey = tr("Camera Raw");
    else if (key == "tiff")           m_decryptedKey = tr("TIFF Properties");
    else if (key == "exif")           m_decryptedKey = tr("Exif-specific Properties");
    else if (key == "aux")            m_decryptedKey = tr("Additional Exif Properties");
    else if (key == "iptc")           m_decryptedKey = tr("IPTC Core");

    // Reset all item flags: item is not selectable.
    setFlags(Qt::ItemIsEnabled);

    setDisabled(false);
    setExpanded(true);

    setFirstColumnSpanned(true);
    setTextAlignment(0, Qt::AlignCenter);
    QFont fn0(font(0));
    fn0.setBold(true);
    fn0.setItalic(false);
    setFont(0, fn0);
    QFont fn1(font(1));
    fn1.setBold(true);
    fn1.setItalic(false);
    setFont(1, fn1);
    setText(0, m_decryptedKey);
    slotThemeChanged();

    connect(ThemeEngine::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));
}

MdKeyListViewItem::~MdKeyListViewItem()
{
}

QString MdKeyListViewItem::getMdKey()
{
    return m_decryptedKey;
}

void MdKeyListViewItem::slotThemeChanged()
{
    setBackground(0, QBrush(ThemeEngine::instance()->thumbSelColor()));
    setBackground(1, QBrush(ThemeEngine::instance()->thumbSelColor()));
    setForeground(0, QBrush(ThemeEngine::instance()->textSelColor()));
    setForeground(1, QBrush(ThemeEngine::instance()->textSelColor()));
}

}  // namespace Digikam
