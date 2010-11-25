/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-07-03
 * Description : a color gradient slider
 *
 * Copyright (C) 2008 by Gilles Caulier<caulier dot gilles at gmail dot com>
 * Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
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

#include "dgradientslider.h"

// Qt includes.

#include <QPainter>
#include <QPoint>
#include <QPen>
#include <QMouseEvent>

namespace Digikam
{

class DGradientSliderPriv
{

public:

  enum Cursor
  {
    NoCursor = 0,
    LeftCursor,
    RightCursor,
    MiddleCursor
  };

public:

    DGradientSliderPriv()
    {
        activeCursor     = NoCursor;
        parent           = 0;
        leftCursor       = 0.0;
        middleCursor     = 0.5;
        rightCursor      = 1.0;
        showMiddleCursor = false;
        leftColor        = Qt::black;
        rightColor       = Qt::white;

        middleColor.setRgb((leftColor.red()   + rightColor.red())   / 2,
                           (leftColor.green() + rightColor.green()) / 2,
                           (leftColor.blue()  + rightColor.blue())  / 2);
    }

    bool             showMiddleCursor;

    double           leftCursor;
    double           middleCursor;
    double           rightCursor;

    QColor           leftColor;
    QColor           rightColor;
    QColor           middleColor;

    DGradientSlider *parent;

    Cursor           activeCursor;

public:

    int gradientHeight() const
    {
        return parent->height() / 3;
    }
    int cursorWidth() const
    {
        return gradientHeight();
    }
    int gradientWidth() const
    {
        return parent->width() - cursorWidth();
    }
    int gradientOffset() const
    {
        return cursorWidth() / 2;
    }
};

DGradientSlider::DGradientSlider(QWidget *parent)
               : QWidget(parent), d(new DGradientSliderPriv)
{
    d->parent = this;

    setMinimumWidth(256);
    setFixedHeight(20);
}

DGradientSlider::~DGradientSlider()
{
    delete d;
}

inline void drawCursorAt(QPainter& painter, double pos, const QColor& brushColor, 
                         int width, int height, int gradientWidth)
{
    painter.setBrush( brushColor );
    int pos2 = (int)(gradientWidth * pos);
    QPoint points[3];
    points[0] = QPoint(pos2, 3 * height - 1);
    points[1] = QPoint(pos2 + width / 2, 2 * height);
    points[2] = QPoint(pos2 + width, 3 * height - 1);
    painter.drawPolygon(points, 3);
}

void DGradientSlider::paintEvent(QPaintEvent*)
{
    int gradientHeight = d->gradientHeight();
    int cursorWidth    = d->cursorWidth();
    int gradientWidth  = d->gradientWidth();
    int gradientOffset = d->gradientOffset();

    QPainter painter(this);
    // Draw first gradient
    QLinearGradient lrGradient(QPointF(0, 0), QPointF(gradientWidth, 0));
    lrGradient.setColorAt(0.0, d->leftColor );
    lrGradient.setColorAt(1.0, d->rightColor );
    painter.setPen( Qt::NoPen );
    painter.setBrush( lrGradient );
    painter.drawRect(gradientOffset, 0, gradientWidth, gradientHeight );

    // Draw second gradient
    QLinearGradient lrcGradient(QPointF(0, 0), QPointF(gradientWidth, 0));
    lrcGradient.setColorAt( d->leftCursor, d->leftColor );
    if( d->showMiddleCursor )
    {
        lrcGradient.setColorAt( d->middleCursor, d->middleColor );
    }
    lrcGradient.setColorAt( d->rightCursor, d->rightColor );
    painter.setBrush( lrcGradient );
    painter.drawRect(gradientOffset, gradientHeight, gradientWidth, gradientHeight );

    // Draw cursors
    painter.setPen( palette().color(QPalette::Text) );
    drawCursorAt( painter, d->leftCursor, d->leftColor, cursorWidth, gradientHeight, gradientWidth );
    if(d->showMiddleCursor)
    {
        drawCursorAt( painter, d->middleCursor, d->middleColor, cursorWidth, gradientHeight, gradientWidth );
    }
    drawCursorAt( painter, d->rightCursor, d->rightColor, cursorWidth, gradientHeight, gradientWidth );
}

inline bool isCursorClicked(const QPoint& pos, double cursorPos, 
                            int width, int height, int gradientWidth)
{
    int pos2 = (int)(gradientWidth * cursorPos);
    return ((pos.y() >= 2 * height)    &&
            (pos.y() < 3 * height)     &&
            (pos.x() >= pos2)          &&
            (pos.x() <= (pos2 + width)));
}

void DGradientSlider::mousePressEvent(QMouseEvent *e)
{
    if( e->button() == Qt::LeftButton )
    {
        int gradientHeight = d->gradientHeight();
        int cursorWidth    = d->cursorWidth();
        int gradientWidth  = d->gradientWidth();

        // Select cursor
        if( isCursorClicked( e->pos(), d->leftCursor , cursorWidth, gradientHeight, gradientWidth ) )
        {
            d->activeCursor = DGradientSliderPriv::LeftCursor;
        }
        else if( d->showMiddleCursor && isCursorClicked( e->pos(), d->middleCursor , cursorWidth, gradientHeight, gradientWidth ) )
        {
            d->activeCursor = DGradientSliderPriv::MiddleCursor;
        }
        else if( isCursorClicked( e->pos(), d->rightCursor , cursorWidth, gradientHeight, gradientWidth ) )
        {
            d->activeCursor = DGradientSliderPriv::RightCursor;
        }
    }
}

void DGradientSlider::mouseReleaseEvent(QMouseEvent*)
{
    d->activeCursor = DGradientSliderPriv::NoCursor;
}

void DGradientSlider::mouseMoveEvent(QMouseEvent *e)
{
    double v = ( e->pos().x() - d->gradientOffset() ) / (double) d->gradientWidth();
    switch(d->activeCursor)
    {
        case DGradientSliderPriv::LeftCursor:
            setLeftValue( v );
            break;
        case DGradientSliderPriv::MiddleCursor:
            setMiddleValue( v );
            break;
        case DGradientSliderPriv::RightCursor:
            setRightValue( v );
            break;
        default:
            break;
    }
}

void DGradientSlider::leaveEvent(QEvent*)
{
    d->activeCursor = DGradientSliderPriv::NoCursor;
}

void DGradientSlider::showMiddleCursor(bool b)
{
    d->showMiddleCursor = b;
}

double DGradientSlider::leftValue() const
{
    return d->leftCursor;
}

double DGradientSlider::rightValue() const
{
    return d->rightCursor;
}

double DGradientSlider::middleValue() const
{
    return d->middleCursor;
}

void DGradientSlider::adjustMiddleValue( double newLeftValue, double newRightValue )
{
    double newDist  = newRightValue  - newLeftValue;
    double oldDist  = d->rightCursor  - d->leftCursor;
    double oldPos   = d->middleCursor - d->leftCursor;
    d->middleCursor = oldPos * newDist / oldDist + newLeftValue;
}

void DGradientSlider::setRightValue(double v)
{
    if( v <= 1.0 && v > d->leftCursor && v != d->rightCursor )
    {
        adjustMiddleValue( d->leftCursor, v );
        d->rightCursor = v;
        update();
        emit rightValueChanged(v);
        emit middleValueChanged(d->middleCursor);
    }
}

void DGradientSlider::setLeftValue(double v)
{
    if( v >= 0.0 && v != d->leftCursor && v < d->rightCursor )
    {
        adjustMiddleValue( v, d->rightCursor );
        d->leftCursor = v;
        update();
        emit leftValueChanged(v);
        emit middleValueChanged(d->middleCursor);
    }
}

void DGradientSlider::setMiddleValue(double v)
{
    if( v > d->leftCursor && v != d->middleCursor && v < d->rightCursor )
    {
        d->middleCursor = v;
        update();
        emit middleValueChanged(v);
    }
}

void DGradientSlider::setColors(const QColor& lcolor, const QColor& rcolor)
{
    d->leftColor  = lcolor;
    d->rightColor = rcolor;
    update();
}

} // namespace Digikam
