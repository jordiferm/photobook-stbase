/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-12-26
 * Description : Multithreaded loader for previews
 *
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#ifndef PREVIEW_TASK_H
#define PREVIEW_TASK_H

// Qt includes.

#include <QImage>

// Local includes.

#include "loadsavetask.h"

namespace Digikam
{

class PreviewLoadingTask : public SharedLoadingTask
{
public:

    PreviewLoadingTask(LoadSaveThread* thread, LoadingDescription description)
        : SharedLoadingTask(thread, description, LoadSaveThread::AccessModeRead, LoadingTaskStatusLoading)
        {}

    virtual void execute();

private:

    bool needToScale(const QSize &imageSize, int previewSize);
    bool loadImagePreview(QImage& image, const QString& path);
};

} // namespace Digikam

#endif // PREVIEW_TASK_H
