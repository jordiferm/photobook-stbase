/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : image file IO threaded interface.
 *
 * Copyright (C) 2005-2008 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#include "previewloadthread.h"

// Local includes.

#include "previewtask.h"

namespace Digikam
{

PreviewLoadThread::PreviewLoadThread()
{
}

void PreviewLoadThread::load(LoadingDescription description)
{
#ifdef ST_RAWSUPPORT
    description.rawDecodingSettings.sixteenBitsImage = false;
#endif
    ManagedLoadSaveThread::loadPreview(description);
}

void PreviewLoadThread::loadHighQuality(LoadingDescription description)
{
#ifdef ST_RAWSUPPORT
    description.rawDecodingSettings.optimizeTimeLoading();
    description.rawDecodingSettings.sixteenBitsImage = false;
#endif
    ManagedLoadSaveThread::load(description, LoadingModeShared, LoadingPolicyFirstRemovePrevious);
}

}   // namespace Digikam
