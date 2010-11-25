/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-01
 * Description : a widget to draw histogram curves
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef CURVESWIDGET_H
#define CURVESWIDGET_H

// Qt includes.

#include <QtGui/QWidget>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>

// Local includes.

#include "dcolor.h"
#include "stdigikamexport.h"

namespace Digikam
{

class ImageHistogram;
class ImageCurves;
class CurvesWidgetPriv;

class ST_DIGIKAM_EXPORT CurvesWidget : public QWidget
{
Q_OBJECT

public:

    enum HistogramType
    {
        ValueHistogram = 0,       // Luminosity.
        RedChannelHistogram,      // Red channel.
        GreenChannelHistogram,    // Green channel.
        BlueChannelHistogram,     // Blue channel.
        AlphaChannelHistogram     // Alpha channel.
    };

    enum HistogramScale
    {
        LinScaleHistogram=0,      // Linear scale.
        LogScaleHistogram         // Logarithmic scale.
    };

public:

    CurvesWidget(int w, int h, QWidget *parent, bool readOnly=false);

    CurvesWidget(int w, int h,                         // Widget size.
                 uchar *i_data, uint i_w, uint i_h,    // Full image info.
                 bool i_sixteenBits,                   // 8 or 16 bits image.
                 QWidget *parent=0,                    // Parent widget instance.
                 bool readOnly=false);                 // If true : widget with full edition mode capabilities.
                                                       // If false : display curve data only without edition.

    ~CurvesWidget();

    void setup(int w, int h, bool readOnly);
    void updateData(uchar *i_data, uint i_w, uint i_h, bool i_sixteenBits);

    // Stop current histogram computations.
    void stopHistogramComputation();

    void setDataLoading();
    void setLoadingFailed();

    void reset();
    void curveTypeChanged();
    void setCurveGuide(const DColor& color);

    ImageCurves* curves() const;

public:

    int             m_channelType;     // Channel type to draw.
    int             m_scaleType;       // Scale to use for drawing.

    ImageHistogram *m_imageHistogram;

Q_SIGNALS:

    void signalMouseMoved(int x, int y);
    void signalCurvesChanged();
    void signalHistogramComputationDone();
    void signalHistogramComputationFailed();

protected Q_SLOTS:

    void slotBlinkTimerDone();
    void slotCalculationStarted(const ImageHistogram *histogram);
    void slotCalculationFinished(const ImageHistogram *histogram, bool success);

protected:

    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void leaveEvent(QEvent*);

private:

    CurvesWidgetPriv* const d;
};

}  // namespace Digikam

#endif /* CURVESWIDGET_H */
