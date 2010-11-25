/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-02-06
 * Description : undo actions manager for image editor.
 *
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005 by Joern Ahrens <joern.ahrens@kdemail.net>
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

#ifndef UNDOACTION_H
#define UNDOACTION_H

// Qt includes 
#include <QString> 
#include <QObject> 

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class DImgInterface;

class ST_DIGIKAM_EXPORT UndoAction
{

public:

    UndoAction(DImgInterface* iface);
    virtual ~UndoAction();

    virtual void rollBack() = 0;
    virtual void execute()  = 0;

    QString getTitle() const;

protected:

    DImgInterface *m_iface;
    QString        m_title;
};

class ST_DIGIKAM_EXPORT UndoActionRotate : public UndoAction
{

public:

    enum Angle
    {
        R90,
        R180,
        R270
    };

    UndoActionRotate(DImgInterface* iface, Angle angle);
    ~UndoActionRotate();

    void rollBack();
    void execute();

private:

    int m_angle;
};

class ST_DIGIKAM_EXPORT UndoActionFlip : public UndoAction
{

public:

    enum Direction
    {
        Horizontal,
        Vertical
    };

    UndoActionFlip(DImgInterface* iface, Direction dir);
    ~UndoActionFlip();

    void rollBack();
    void execute();

private:

    int m_dir;
};

class ST_DIGIKAM_EXPORT UndoActionBCG : public UndoAction
{

public:

    UndoActionBCG(DImgInterface* iface,
                  double oldGamma, double oldBrightness,
                  double oldContrast, double newGamma,
                  double newBrightness, double newContrast);
    ~UndoActionBCG();

    void rollBack();
    void execute();    

private:

    double m_oldGamma;
    double m_oldBrightness;
    double m_oldContrast;
    double m_newGamma;
    double m_newBrightness;
    double m_newContrast;
};

class ST_DIGIKAM_EXPORT UndoActionIrreversible : public UndoAction
{

public:

    UndoActionIrreversible(DImgInterface* iface,
                           const QString &caller=QObject::tr("Unknown"));
    ~UndoActionIrreversible();

    void rollBack();
    void execute();
};

}  // namespace Digikam

#endif /* UNDOACTION_H */
