/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-18
 * Description : color modifier methods for DImg framework
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

#ifndef COLORMODIFIER_H
#define COLORMODIFIER_H

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class DImg;
class ColorModifierPriv;

class ST_DIGIKAM_EXPORT ColorModifier
{
public:

    ColorModifier();
    ~ColorModifier();

    void reset();
    bool modified() const;
    void applyColorModifier(DImg& image, double r, double g, double b, double a);

private:

    void setTables(int *redMap, int *greenMap, int *blueMap, int *alphaMap, bool sixteenBit);
    void getTables(int *redMap, int *greenMap, int *blueMap, int *alphaMap, bool sixteenBit);
    void setGamma(double val);
    void adjustRGB(double r, double g, double b, double a, bool sixteenBit);

private:

    ColorModifierPriv* const d;
};

}  // namespace Digikam

#endif /* COLORMODIFIER_H */
