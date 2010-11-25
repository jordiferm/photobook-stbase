/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-08-21
 * Description : Editor tool settings template box
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef EDITORTOOLSETTINGS_H
#define EDITORTOOLSETTINGS_H

// Qt includes.

#include <QScrollArea>

// Local includes.

#include "histogrambox.h"

class QPushButton;

namespace Digikam
{
class HistogramBox;
class ImagePanIconWidget;
class EditorToolSettingsPriv;

class ST_DIGIKAM_EXPORT EditorToolSettings : public QScrollArea
{
    Q_OBJECT

public:

    enum ButtonCode
    {
        Default = 0x00000001,
        Try     = 0x00000002,
        Ok      = 0x00000004,
        Cancel  = 0x00000008,
        SaveAs  = 0x00000010,
        Load    = 0x00000020
    };

    enum ToolCode
    {
        NoTool     = 0x00000001,
        ColorGuide = 0x00000002,
        PanIcon    = 0x00000004,
        Histogram  = 0x00000008
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

    EditorToolSettings(int buttonMask, int toolMask=NoTool, int histogramType=HistogramBox::LRGB, QWidget *parent=0);
    ~EditorToolSettings();

    virtual void setBusy(bool){};
    virtual void writeSettings(){};
    virtual void readSettings(){};
    virtual void resetSettings(){};

    int marginHint();
    int spacingHint();

    QWidget* plainPage() const;
    HistogramBox* histogramBox() const;
    ImagePanIconWidget* panIconView() const;

    QColor guideColor() const;
    void setGuideColor(const QColor& color);

    int guideSize() const;
    void setGuideSize(int size);

    QPushButton* button(int buttonCode) const;
    void enableButton(int buttonCode, bool state);

    virtual QSize minimumSizeHint() const;

signals:

    void signalOkClicked();
    void signalCancelClicked();
    void signalTryClicked();
    void signalDefaultClicked();
    void signalSaveAsClicked();
    void signalLoadClicked();
    void signalColorGuideChanged();
    void signalChannelChanged();
    void signalScaleChanged();
    void signalColorsChanged();

private:

    EditorToolSettingsPriv* const d;
};

} // namespace Digikam

#endif // EDITORTOOLSETTINGS_H
