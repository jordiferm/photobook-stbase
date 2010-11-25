/*
Gwenview: an image viewer
Copyright 2007 Aurélien Gâteau <aurelien.gateau@free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#ifndef PAINTUTILS_H
#define PAINTUTILS_H
#include <QtGlobal>
#include "stcoreexport.h"


class QColor;
class QPainterPath;
class QPixmap;
class QRectF;
class QSize;

/**
 * A collection of independent painting functions
 */
namespace PaintUtils {

/**
 * Returns a rounded-corner version of @rect. Corner radius is @p radius.
 * (Copied from KFileItemDelegate)
 */
ST_CORE_EXPORT QPainterPath roundedRectangle(const QRectF& rect, qreal radius);

/**
 * Generates a pixmap of size @p size, filled with @p color, whose borders have
 * been blurred by @p radius pixels.
 */
ST_CORE_EXPORT QPixmap generateFuzzyRect(const QSize& size, const QColor& color, int radius);

/**
 * Returns a modified version of @p color, where hue, saturation and value have
 * been adjusted according to @p deltaH, @p deltaS and @p deltaV.
 */
ST_CORE_EXPORT QColor adjustedHsv(const QColor& color, int deltaH, int deltaS, int deltaV);

/**
 * Returns a modified version of @p color, where alpha has been set to @p
 * alphaF.
 */
ST_CORE_EXPORT QColor alphaAdjustedF(const QColor& color, qreal alphaF);

} // namespace


#endif /* PAINTUTILS_H */
