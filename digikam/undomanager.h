/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-02-06
 * Description : an image editor actions undo/redo manager
 *
 * Copyright (C) 2005-2006 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005-2006 by Joern Ahrens <joern.ahrens@kdemail.net>
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

#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H

// Qt includes.

#include <QStringList>

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class DImgInterface;
class UndoManagerPriv;
class UndoAction;

class ST_DIGIKAM_EXPORT UndoManager
{

public:

    UndoManager(DImgInterface* iface);
    ~UndoManager();

    void undo();
    void redo();

    void clear(bool clearCache=true);
    bool anyMoreUndo();
    bool anyMoreRedo();
    void getUndoHistory(QStringList &titles);
    void getRedoHistory(QStringList &titles);
    bool isAtOrigin();
    void setOrigin();

    void addAction(UndoAction* action);

private:

    void clearUndoActions();
    void clearRedoActions();

private:

    UndoManagerPriv* const d;
};

}  // namespace Digikam

#endif /* UNDOMANAGER_H */
