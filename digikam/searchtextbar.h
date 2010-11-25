/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-11-25
 * Description : a bar used to search a string.
 *
 * Copyright (C) 2007-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SEARCH_TEXT_BAR_H
#define SEARCH_TEXT_BAR_H

// Qt includes 
#include <QLineEdit> 

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class SearchTextBarPriv;

class ST_DIGIKAM_EXPORT SearchTextSettings
{

public:

    SearchTextSettings()
    {
        caseSensitive = Qt::CaseInsensitive;
    }

    Qt::CaseSensitivity caseSensitive;

    QString             text;
};

class ST_DIGIKAM_EXPORT SearchTextBar : public QLineEdit
{
    Q_OBJECT

public:

    SearchTextBar(QWidget *parent, const char* name, const QString& msg=tr("Search..."));
    ~SearchTextBar();

    void setTextQueryCompletion(bool b);
    bool hasTextQueryCompletion() const;

    void setCaseSensitive(bool b);
    bool hasCaseSensitive() const;

    void setSearchTextSettings(const SearchTextSettings& settings);
    SearchTextSettings searchTextSettings() const;

Q_SIGNALS:

    void signalSearchTextSettings(const SearchTextSettings& settings);

public Q_SLOTS:

    void slotSearchResult(bool);

private Q_SLOTS:

    void slotTextChanged(const QString&);

private:

    void contextMenuEvent(QContextMenuEvent* e);

private:

    SearchTextBarPriv* const d;
};

}  // namespace Digikam

#endif /* SEARCH_TEXT_BAR_H */
