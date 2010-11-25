/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-09-30
 * Description : a widget to display an image histogram and its control widgets
 *
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef HISTOGRAMBOX_H
#define HISTOGRAMBOX_H

// Qt includes.

#include <QtGui/QWidget>

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class HistogramWidget;
class HistogramBoxPriv;

class ST_DIGIKAM_EXPORT HistogramBox : public QWidget
{
Q_OBJECT

public:

    enum HistogramCode
    {
        RGB=0,
        RGBA,
        LRGB,
        LRGBA,
        LRGBC,
        LRGBAC
    };

    enum HistogramScale
    {
        Linear=0,
        Logarithmic
    };

    enum ColorChannel
    {
        LuminosityChannel=0,
        RedChannel,
        GreenChannel,
        BlueChannel,
        AlphaChannel,
        ColorChannels
    };

public:

    explicit HistogramBox(QWidget* parent = 0, int histogramType = LRGB, bool selectMode = false);
    ~HistogramBox();

    HistogramWidget* histogram() const;

    void setGradientVisible(bool visible);
    void setGradientColors(const QColor &from, const QColor &to);

    int  channel() const;
    void setChannel(int channel);
    void setChannelEnabled(bool enabled);

    int  colorsChannel() const;
    void setColorsChannel(int color);
    void setColorsEnabled(bool enabled);

    int  scale() const;
    void setScale(int scale);

Q_SIGNALS:

    void signalChannelChanged();
    void signalColorsChanged();
    void signalScaleChanged();

public Q_SLOTS:

    void slotChannelChanged();
    void slotScaleChanged();
    void slotColorsChanged();

private:

    HistogramBoxPriv* const d;
};

} // namespace Digikam

#endif /* HISTOGRAMBOX_H */
