/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-07-28
 * Description : a color gradient widget
 *
 * Copyright (C) 2004-2008 by Gilles Caulier<caulier dot gilles at gmail dot com>
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

#ifndef COLORGRADIENTWIDGET_H
#define COLORGRADIENTWIDGET_H

// KDE includes.

#include <QtGui/QWidget>
#include <QtGui/QColor>

// Local includes.

#include "stdigikamexport.h"

namespace Digikam
{

class ColorGradientWidgetPriv;

class ST_DIGIKAM_EXPORT ColorGradientWidget : public QWidget
{
    Q_OBJECT

public:

    ColorGradientWidget(Qt::Orientation orientation, int size, QWidget *parent=0);
    ~ColorGradientWidget();

    void setColors(const QColor &col1, const QColor &col2);

protected:

    void paintEvent(QPaintEvent*);

private:

    ColorGradientWidgetPriv* const d;
};

} // namespace Digikam

#endif /* COLORGRADIENTWIDGET_H */
