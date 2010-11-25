/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-22
 * Description : a generic widget to display a panel to choose
 *               a rectangular image area.
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "paniconwidget.h"

// C++ includes.

#include <cmath>

// Qt includes.

#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QTimer>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QHideEvent>


namespace Digikam
{

class PanIconWidgetPriv
{

public:

    PanIconWidgetPriv()
    {
        moveSelection = false;
    }

    bool    moveSelection;

    int     xpos;
    int     ypos;

    QRect   regionSelection;         // Original size image selection.

    QTimer *timer;
};

PanIconWidget::PanIconWidget(QWidget *parent, Qt::WidgetAttribute attribute)
             : QWidget(parent), d(new PanIconWidgetPriv)
{
    m_flicker    = false;
    m_zoomFactor = 1.0;

    d->timer = new QTimer(this);
    d->timer->setInterval(800);

    setMouseTracking(true);
    setAttribute(attribute);

    connect(d->timer, SIGNAL(timeout()),
            this, SLOT(slotFlickerTimer()));
}

PanIconWidget::~PanIconWidget()
{
    delete d;
}

void PanIconWidget::setImage(int previewWidth, int previewHeight, const QImage& image)
{
    QSize sz(image.width(), image.height());
    sz.scale(previewWidth, previewHeight, Qt::KeepAspectRatio);
    m_width           = sz.width();
    m_height          = sz.height();
    m_orgWidth        = image.width();
    m_orgHeight       = image.height();
    m_zoomedOrgWidth  = image.width();
    m_zoomedOrgHeight = image.height();

    QImage scaledImg  = image.scaled(sz.width(), sz.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_pixmap          = QPixmap(previewWidth, previewHeight);
    m_pixmap.fill(palette().color(QPalette::Background));
    QPainter p(&m_pixmap);
    p.drawImage(0, 0, scaledImg);

    setFixedSize(m_width, m_height);

    m_rect = QRect(width()/2-m_width/2, height()/2-m_height/2, m_width, m_height);
    update();
}

void PanIconWidget::setImage(int previewWidth, int previewHeight, const DImg& image)
{
    DImg img(image);
    setImage(previewWidth, previewHeight, img.copyQImage());
}

void PanIconWidget::slotZoomFactorChanged(double factor)
{
    if (m_zoomFactor == factor) return;
    m_zoomFactor      = factor;
    m_zoomedOrgWidth  = (int)(m_orgWidth  * factor);
    m_zoomedOrgHeight = (int)(m_orgHeight * factor);
    update();
}

void PanIconWidget::setRegionSelection(const QRect& regionSelection)
{
    d->regionSelection = regionSelection;
    m_localRegionSelection.setX( m_rect.x() + (int)((float)d->regionSelection.x() *
                                 ((float)m_width / (float)m_zoomedOrgWidth)) );

    m_localRegionSelection.setY( m_rect.y() + (int)((float)d->regionSelection.y() *
                                 ((float)m_height / (float)m_zoomedOrgHeight)) );

    m_localRegionSelection.setWidth( (int)((float)d->regionSelection.width() *
                                     ((float)m_width / (float)m_zoomedOrgWidth)) );

    m_localRegionSelection.setHeight( (int)((float)d->regionSelection.height() *
                                      ((float)m_height / (float)m_zoomedOrgHeight)) );

    update();
}

QRect PanIconWidget::getRegionSelection()
{
    return (d->regionSelection);
}

void PanIconWidget::setCursorToLocalRegionSelectionCenter()
{
    QCursor::setPos(mapToGlobal(m_localRegionSelection.center()));
}

void PanIconWidget::setCenterSelection()
{
    setRegionSelection(QRect(
             (int)(((float)m_zoomedOrgWidth  / 2.0) - ((float)d->regionSelection.width()  / 2.0)),
             (int)(((float)m_zoomedOrgHeight / 2.0) - ((float)d->regionSelection.height() / 2.0)),
             d->regionSelection.width(),
             d->regionSelection.height()));
}

void PanIconWidget::regionSelectionMoved(bool targetDone)
{
    if (targetDone)
        update();

    int x = (int)lround( ((float)m_localRegionSelection.x() - (float)m_rect.x() ) *
                         ((float)m_zoomedOrgWidth / (float)m_width) );

    int y = (int)lround( ((float)m_localRegionSelection.y() - (float)m_rect.y() ) *
                         ((float)m_zoomedOrgHeight / (float)m_height) );

    int w = (int)lround( (float)m_localRegionSelection.width() *
                         ((float)m_zoomedOrgWidth / (float)m_width) );

    int h = (int)lround( (float)m_localRegionSelection.height() *
                         ((float)m_zoomedOrgHeight / (float)m_height) );

    d->regionSelection.setX(x);
    d->regionSelection.setY(y);
    d->regionSelection.setWidth(w);
    d->regionSelection.setHeight(h);

    emit signalSelectionMoved( d->regionSelection, targetDone );
}

void PanIconWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.drawPixmap(m_rect.x(), m_rect.y(), m_pixmap);

    // Drawing selection border

    if (m_flicker)
        p.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    else
        p.setPen(QPen(Qt::red, 1, Qt::SolidLine));

    p.drawRect(m_localRegionSelection.x(),
               m_localRegionSelection.y(),
               m_localRegionSelection.width(),
               m_localRegionSelection.height());

    if (m_flicker)
        p.setPen(QPen(Qt::red, 1, Qt::DotLine));
    else
        p.setPen(QPen(Qt::white, 1, Qt::DotLine));

    p.drawRect(m_localRegionSelection.x(),
               m_localRegionSelection.y(),
               m_localRegionSelection.width(),
               m_localRegionSelection.height());
}

void PanIconWidget::setMouseFocus()
{
    raise();
    d->xpos          = m_localRegionSelection.center().x();
    d->ypos          = m_localRegionSelection.center().y();
    d->moveSelection = true;
    setCursor( Qt::SizeAllCursor );
    emit signalSelectionTakeFocus();
}

void PanIconWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    d->timer->start();
}

void PanIconWidget::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    d->timer->stop();

    if ( d->moveSelection )
    {
        d->moveSelection = false;
        setCursor( Qt::ArrowCursor );
        emit signalHidden();
    }
}

void PanIconWidget::mousePressEvent ( QMouseEvent * e )
{
    if ( (e->button() == Qt::LeftButton || e->button() == Qt::MidButton) &&
         m_localRegionSelection.contains( e->x(), e->y() ) )
    {
        d->xpos          = e->x();
        d->ypos          = e->y();
        d->moveSelection = true;
        setCursor( Qt::SizeAllCursor );
        emit signalSelectionTakeFocus();
    }
}

void PanIconWidget::mouseMoveEvent ( QMouseEvent * e )
{
    if ( d->moveSelection &&
         (e->buttons() == Qt::LeftButton || e->buttons() == Qt::MidButton) )
    {
        int newxpos = e->x();
        int newypos = e->y();

        m_localRegionSelection.translate(newxpos - d->xpos, newypos - d->ypos);

        d->xpos = newxpos;
        d->ypos = newypos;

        // Perform normalization of selection area.

        if (m_localRegionSelection.left() < m_rect.left())
            m_localRegionSelection.moveLeft(m_rect.left());

        if (m_localRegionSelection.top() < m_rect.top())
            m_localRegionSelection.moveTop(m_rect.top());

        if (m_localRegionSelection.right() > m_rect.right())
            m_localRegionSelection.moveRight(m_rect.right());

        if (m_localRegionSelection.bottom() > m_rect.bottom())
            m_localRegionSelection.moveBottom(m_rect.bottom());

        update();
        regionSelectionMoved(false);
        return;
    }
    else
    {
        if ( m_localRegionSelection.contains( e->x(), e->y() ) )
            setCursor( Qt::PointingHandCursor );
        else
            setCursor( Qt::ArrowCursor );
    }
}

void PanIconWidget::mouseReleaseEvent ( QMouseEvent * )
{
    if ( d->moveSelection )
    {
        d->moveSelection = false;
        setCursor( Qt::ArrowCursor );
        regionSelectionMoved(true);
    }
}

void PanIconWidget::slotFlickerTimer()
{
    m_flicker = !m_flicker;
    update();
}

}  // namespace Digikam
