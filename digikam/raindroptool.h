/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-09-30
 * Description : a plugin to add rain drop over an image
 *
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef RAINDROPTOOL_H
#define RAINDROPTOOL_H

// Local includes.

#include "editortool.h"
#include "stdigikamexport.h"

namespace Digikam
{
class ImageWidget;
class EditorToolSettings;
}

namespace KDcrawIface
{
class RIntNumInput;
}

namespace DigikamRainDropImagesPlugin
{

class ST_DIGIKAM_EXPORT RainDropTool : public Digikam::EditorToolThreaded
{
    Q_OBJECT

public:

    RainDropTool(QObject *parent);
    ~RainDropTool();

private slots:

    void slotResetSettings();

private:

    void readSettings();
    void writeSettings();
    void prepareEffect();
    void prepareFinal();
    void putPreviewData();
    void putFinalData();
    void renderingFinished();

private:

    KDcrawIface::RIntNumInput   *m_dropInput;
    KDcrawIface::RIntNumInput   *m_amountInput;
    KDcrawIface::RIntNumInput   *m_coeffInput;

    Digikam::ImageWidget        *m_previewWidget;

    Digikam::EditorToolSettings *m_gboxSettings;
};

}  // namespace DigikamRainDropImagesPlugin

#endif /* RAINDROPTOOL_H */
