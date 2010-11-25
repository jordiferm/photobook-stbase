/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-01
 * Description : a widget to display an image preview with some 
 *               modes to compare effect results.
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

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

// Qt includes.

#include <QtGui/QWidget>
#include <QtCore/QPoint>
#include <QtGui/QColor>
#include <QtCore/QString>

// Local includes.

#include "dcolor.h"
#include "imageguidewidget.h"
#include "stdigikamexport.h"

namespace Digikam
{

class ImageIface;
class ImageWidgetPriv;

class ST_DIGIKAM_EXPORT ImageWidget : public QWidget
{
Q_OBJECT

public:

    enum ExposureIndicator 
    {
        UnderExposure=0,
        OverExposure
    };

public:

    ImageWidget(const QString& settingsSection, QWidget *parent=0, 
                const QString& previewWhatsThis=QString(), bool prevModeOptions=true, 
                int guideMode=ImageGuideWidget::PickColorMode, 
                bool guideVisible=true, bool useImageSelection=false);
    ~ImageWidget();

    ImageIface* imageIface();

    QPoint getSpotPosition();
    DColor getSpotColor(int getColorFrom);
    void   setSpotVisible(bool spotVisible, bool blink=false);
    int    getRenderingPreviewMode();
    void   resetSpotPosition();
    void   updatePreview();
    void   writeSettings();

    void   setRenderingPreviewMode(int mode);

public Q_SLOTS:

    void slotChangeGuideColor(const QColor &color);
    void slotChangeGuideSize(int size);

Q_SIGNALS:

    void spotPositionChangedFromOriginal( const Digikam::DColor &color, const QPoint &position );
    void spotPositionChangedFromTarget( const Digikam::DColor &color, const QPoint &position );
    void signalResized();

private Q_SLOTS:

    void slotUpdateSpotInfo(const Digikam::DColor &col, const QPoint &point);

private:

    void readSettings();

private:

    ImageWidgetPriv* const d;
};

}  // namespace Digikam

#endif /* IMAGEWIDGET_H */
