/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-23
 * Description : a tab widget to display ICC profile infos
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

#ifndef ICCPROFILEWIDGET_H
#define ICCPROFILEWIDGET_H

// Qt includes.

#include <QtGui/QWidget>
#include <QtCore/QString>
#include <QUrl> 

// Local includes.

#include "metadatawidget.h"
#include "stdigikamexport.h"

namespace Digikam
{

class ICCProfileWidgetPriv;

class ST_DIGIKAM_EXPORT ICCProfileWidget : public MetadataWidget
{
    Q_OBJECT

public:

    ICCProfileWidget(QWidget* parent, int w=256, int h=256);
    ~ICCProfileWidget();

    bool    loadFromURL(const QUrl& url);
    bool    loadFromProfileData(const QString& fileName, const QByteArray& data=QByteArray());

    QString getTagDescription(const QString& key);
    QString getTagTitle(const QString& key);

    QString getMetadataTitle();

    void    setLoadingFailed();
    void    setDataLoading();

    bool  setProfileData(const QByteArray& data=QByteArray());
    const QByteArray& getProfileData();

protected Q_SLOTS:

    virtual void slotSaveMetadataToFile();

private:

    bool decodeMetadata();
    void buildView();

private:

    ICCProfileWidgetPriv* const d;
};

}  // namespace Digikam

#endif /* ICCPROFILEWIDGET_H */
