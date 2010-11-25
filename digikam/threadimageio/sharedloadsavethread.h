/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-16
 * Description : image file IO threaded interface.
 *
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#ifndef SHARED_LOAD_SAVE_THREAD_H
#define SHARED_LOAD_SAVE_THREAD_H

// Local includes.

#include "managedloadsavethread.h"
#include "stdigikamexport.h"

namespace Digikam
{

class ST_DIGIKAM_EXPORT SharedLoadSaveThread : public ManagedLoadSaveThread
{
public:

    void load(LoadingDescription description, AccessMode mode, LoadingPolicy policy = LoadingPolicyAppend);
    DImg cacheLookup(const QString& filePath, AccessMode mode);
};

}   // namespace Digikam

#endif // SHARED_LOAD_SAVE_THREAD_H
