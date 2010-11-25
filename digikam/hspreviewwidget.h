/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-01-08
 * Description : Hue/Saturation preview widget
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef HSPREVIEWWIDGET_H
#define HSPREVIEWWIDGET_H

// Qt includes.

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>

namespace DigikamImagesPluginCore
{

class HSPreviewWidgetPrivate;

class HSPreviewWidget : public QWidget
{
  Q_OBJECT

public:

    HSPreviewWidget(QWidget *parent=0, int xBorder=0);
    ~HSPreviewWidget();

    void setHS(double hue, double sat);

protected:

    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

private:

    void updatePixmap();

private:

    HSPreviewWidgetPrivate* const d;
};

}  // namespace DigikamImagesPluginCore

#endif /* HSPREVIEWWIDGET_H */
