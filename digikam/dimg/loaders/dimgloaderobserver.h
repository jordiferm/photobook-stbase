/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-03
 * Description : DImgLoader observer interface
 *
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#ifndef DIMGLOADEROBSERVER_H
#define DIMGLOADEROBSERVER_H

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class DImg;

class ST_DIGIKAM_EXPORT DImgLoaderObserver
{

public:
    // posts progress information about image IO
    virtual void progressInfo(const DImg *, float /*progress*/)
            {};

    // queries whether the image IO operation shall be continued
    virtual bool continueQuery(const DImg *)
            { return true; };

    // Return a relative value which determines the granularity, the frequency
    // with which the DImgLoaderObserver is checked and progress is posted.
    // Standard is 1.0. Values < 1 mean less granularity (fewer checks),
    // values > 1 mean higher granularity (more checks).
    virtual float granularity()
            { return 1.0; };

    // This is a hack needed to prevent hanging when a K3Process-based loader (raw loader)
    // is waiting for the process to finish, but the main thread is waiting
    // for the thread to finish and no K3Process events are delivered.
    // Remove when porting to Qt4.
    virtual bool isShuttingDown()
            { return false; }

    virtual ~DImgLoaderObserver(){};
};

}      // namespace Digikam

#endif // DIMGLOADEROBSERVER_H
