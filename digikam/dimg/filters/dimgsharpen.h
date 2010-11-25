/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-17-07
 * Description : A Sharpen threaded image filter.
 *
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

#ifndef DIMGSHARPEN_H
#define DIMGSHARPEN_H

// Local includes.

#include "stdigikamexport.h"
#include "dimgthreadedfilter.h"

namespace Digikam
{

class ST_DIGIKAM_EXPORT DImgSharpen : public DImgThreadedFilter
{

public:

    DImgSharpen(DImg *orgImage, QObject *parent=0, double radius=0.0, double sigma=1.0);

    // Constructor for slave mode: execute immediately in current thread with specified master filter
    DImgSharpen(DImgThreadedFilter *parentFilter, const DImg &orgImage, const DImg &destImage,
                int progressBegin=0, int progressEnd=100, double radius=0.0, double sigma=1.0);

    ~DImgSharpen(){};

private:  // DImgSharpen filter data.

    double m_radius;
    double m_sigma;

private:  // DImgSharpen filter methods.

    virtual void filterImage();

    void sharpenImage(double radius, double sigma);

    bool convolveImage(const unsigned int order, const double *kernel);

    int  getOptimalKernelWidth(double radius, double sigma);
};

}  // namespace Digikam

#endif /* DIMGSHARPEN_H */
