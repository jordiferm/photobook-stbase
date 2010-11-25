/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-07-29
 * Description : image levels manipulation methods.
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGELEVELS_H
#define IMAGELEVELS_H

#define CLAMP(x,l,u) ((x)<(l)?(l):((x)>(u)?(u):(x)))

/*  Map RGB to intensity  */

#define LEVELS_RGB_INTENSITY_RED    0.30
#define LEVELS_RGB_INTENSITY_GREEN  0.59
#define LEVELS_RGB_INTENSITY_BLUE   0.11
#define LEVELS_RGB_INTENSITY(r,g,b) ((r) * LEVELS_RGB_INTENSITY_RED   + \
                                     (g) * LEVELS_RGB_INTENSITY_GREEN + \
                                     (b) * LEVELS_RGB_INTENSITY_BLUE)

// Qt includes 
#include <QUrl> 

// Local includes.

#include "dcolor.h"
#include "stdigikamexport.h"

namespace Digikam
{

class ImageLevelsPriv;
class ImageHistogram;

class ST_DIGIKAM_EXPORT ImageLevels
{

public:

    ImageLevels(bool sixteenBit);
    ~ImageLevels();

    bool   isDirty();
    bool   isSixteenBits();
    void   reset();

    // Methods for to manipulate the levels data.

    void   levelsChannelReset(int channel);
    void   levelsAuto(ImageHistogram *hist);
    void   levelsChannelAuto(ImageHistogram *hist, int channel);
    int    levelsInputFromColor(int channel, const DColor& color);
    void   levelsBlackToneAdjustByColors(int channel, const DColor& color);
    void   levelsGrayToneAdjustByColors(int channel, const DColor& color);
    void   levelsWhiteToneAdjustByColors(int channel, const DColor& color);
    void   levelsCalculateTransfers();
    float  levelsLutFunc(int n_channels, int channel, float value);
    void   levelsLutSetup(int nchannels);
    void   levelsLutProcess(uchar *srcPR, uchar *destPR, int w, int h);

    // Methods for to set manually the levels values.

    void   setLevelGammaValue(int Channel, double val);
    void   setLevelLowInputValue(int Channel, int val);
    void   setLevelHighInputValue(int Channel, int val);
    void   setLevelLowOutputValue(int Channel, int val);
    void   setLevelHighOutputValue(int Channel, int val);

    double getLevelGammaValue(int Channel);
    int    getLevelLowInputValue(int Channel);
    int    getLevelHighInputValue(int Channel);
    int    getLevelLowOutputValue(int Channel);
    int    getLevelHighOutputValue(int Channel);

    // Methods for to save/load the levels values to/from a Gimp levels text file.

    bool   saveLevelsToGimpLevelsFile(const QUrl& fileUrl);
    bool   loadLevelsFromGimpLevelsFile(const QUrl& fileUrl);

private:

    ImageLevelsPriv* const d;
};

}  // namespace Digikam

#endif /* IMAGELEVELS_H */
