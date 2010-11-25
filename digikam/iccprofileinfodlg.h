/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-16
 * Description : a dialog to display ICC profile information.
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

#ifndef ICCPROFILEINFODLG_H
#define ICCPROFILEINFODLG_H

// Qt includes.

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QDialog> 


// Local includes.

#include "stdigikamexport.h"

class QWidget;

namespace Digikam
{


class ST_DIGIKAM_EXPORT ICCProfileInfoDlg : public QDialog
{
	Q_OBJECT

public:

    ICCProfileInfoDlg(QWidget *parent, const QString& profilePath, const QByteArray& profileData=QByteArray());
    ~ICCProfileInfoDlg();
};

}  // namespace Digikam

#endif /* ICCPROFILEINFODLG_H */
