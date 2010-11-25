/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : save JPEG image options.
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef JPEGSETTINGS_H
#define JPEGSETTINGS_H

// KDE includes.

#include <QWidget>

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class JPEGSettingsPriv;

class ST_DIGIKAM_EXPORT JPEGSettings : public QWidget
{
    Q_OBJECT

public:

    JPEGSettings(QWidget *parent=0);
    ~JPEGSettings();

    void setCompressionValue(int val);
    int  getCompressionValue();

    void setSubSamplingValue(int val);
    int  getSubSamplingValue();

signals:

    void signalSettingsChanged();

private:

    JPEGSettingsPriv* const d;
};

}  // namespace Digikam

#endif /* JPEGSETTINGS_H */
