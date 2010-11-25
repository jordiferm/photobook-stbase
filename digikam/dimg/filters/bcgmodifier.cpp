/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-06
 * Description : a Brightness/Contrast/Gamma image filter.
 *
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define CLAMP_0_255(x)   qMax(qMin(x, 255), 0)
#define CLAMP_0_65535(x) qMax(qMin(x, 65535), 0)

#include "bcgmodifier.h"

// C++ includes.

#include <cstdio>
#include <cmath>

// Local includes.

#include "dimg.h"

namespace Digikam
{

class BCGModifierPriv
{
public:

    BCGModifierPriv()
    {
        channel  = BCGModifier::CHANNEL_ALL;
        modified = false;
    }

    bool modified;

    int  channel;
    int  map16[65536];
    int  map[256];
};

BCGModifier::BCGModifier() 
           : d(new BCGModifierPriv)
{
    reset();
}

BCGModifier::~BCGModifier()
{
    delete d;
}

bool BCGModifier::modified() const
{
    return d->modified;
}

void BCGModifier::reset()
{
    // initialize to linear mapping

    for (int i=0; i<65536; i++)
        d->map16[i] = i;

    for (int i=0; i<256; i++)
        d->map[i] = i;

    d->modified = false;
}

void BCGModifier::applyBCG(DImg& image)
{
    if (!d->modified || image.isNull())
        return;

    applyBCG(image.bits(), image.width(), image.height(), image.sixteenBit());
}

void BCGModifier::applyBCG(uchar *bits, uint width, uint height, bool sixteenBits)
{
    if (!d->modified || !bits)
        return;

    uint size = width*height;

    if (!sixteenBits)                    // 8 bits image.
    {
        uchar* data = bits;

        for (uint i=0; i<size; i++)
        {
            switch (d->channel)
            {
                case CHANNEL_BLUE:
                    data[0] = CLAMP_0_255(d->map[data[0]]);
                    break;

                case CHANNEL_GREEN:
                    data[1] = CLAMP_0_255(d->map[data[1]]);
                    break;

                case CHANNEL_RED:
                    data[2] = CLAMP_0_255(d->map[data[2]]);
                    break;

                default:      // CHANNEL_ALL
                    data[0] = CLAMP_0_255(d->map[data[0]]);
                    data[1] = CLAMP_0_255(d->map[data[1]]);
                    data[2] = CLAMP_0_255(d->map[data[2]]);
                    break;
            }

            data += 4;
        }
    }
    else                                        // 16 bits image.
    {
        ushort* data = (ushort*)bits;

        for (uint i=0; i<size; i++)
        {
            switch (d->channel)
            {
                case CHANNEL_BLUE:
                    data[0] = CLAMP_0_65535(d->map16[data[0]]);
                    break;

                case CHANNEL_GREEN:
                    data[1] = CLAMP_0_65535(d->map16[data[1]]);
                    break;

                case CHANNEL_RED:
                    data[2] = CLAMP_0_65535(d->map16[data[2]]);
                    break;

                default:      // CHANNEL_ALL
                    data[0] = CLAMP_0_65535(d->map16[data[0]]);
                    data[1] = CLAMP_0_65535(d->map16[data[1]]);
                    data[2] = CLAMP_0_65535(d->map16[data[2]]);
                    break;
            }

            data += 4;
        }
    }
}

void BCGModifier::setChannel(int channel)
{
    d->channel = channel;
}

void BCGModifier::setGamma(double val)
{
    val = (val < 0.01) ? 0.01 : val;

    for (int i=0; i<65536; i++)
        d->map16[i] = lround(pow(((double)d->map16[i] / 65535.0), (1.0 / val)) * 65535.0);

    for (int i=0; i<256; i++)
        d->map[i] = lround(pow(((double)d->map[i] / 255.0), (1.0 / val)) * 255.0);

    d->modified = true;
}

void BCGModifier::setBrightness(double val)
{
    int val1 = lround(val * 65535);

    for (int i = 0; i < 65536; i++)
        d->map16[i] = d->map16[i] + val1;

    val1 = lround(val * 255);

    for (int i = 0; i < 256; i++)
        d->map[i] = d->map[i] + val1;

    d->modified = true;
}

void BCGModifier::setContrast(double val)
{
    for (int i = 0; i < 65536; i++)
        d->map16[i] = lround((d->map16[i] - 32767) * val) + 32767;

    for (int i = 0; i < 256; i++)
        d->map[i] = lround((d->map[i] - 127) * val) + 127;

    d->modified = true;
}

}  // namespace Digikam
