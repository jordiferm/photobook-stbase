/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#ifndef __BARCODES_H__
#define __BARCODES_H__

#include <QRect>
#include <QString>
#include "stopenrptrendererexport.h"

class OROPage;

//
// 3of9
//
void ST_OPENRPT_RENDERER_EXPORT render3of9(const QRect &, const QString &, int align, QPainter *);
void ST_OPENRPT_RENDERER_EXPORT render3of9(OROPage *, const QRectF &, const QString &, int align);

//
// 3of9+
//
void ST_OPENRPT_RENDERER_EXPORT renderExtended3of9(const QRect &, const QString &, int align, QPainter *);
void ST_OPENRPT_RENDERER_EXPORT renderExtended3of9(OROPage *, const QRectF &, const QString &, int align);

//
// Code 128
//
void ST_OPENRPT_RENDERER_EXPORT renderCode128(const QRect &, const QString &, int align, QPainter *);
void ST_OPENRPT_RENDERER_EXPORT renderCode128(OROPage *, const QRectF &, const QString &, int align);

//
// Code EAN/UPC
//
void ST_OPENRPT_RENDERER_EXPORT renderCodeEAN13(const QRect &, const QString &, int align, QPainter *);
void ST_OPENRPT_RENDERER_EXPORT renderCodeEAN8(const QRect &, const QString &, int align, QPainter *);
void ST_OPENRPT_RENDERER_EXPORT renderCodeUPCA(const QRect &, const QString &, int align, QPainter *);
void ST_OPENRPT_RENDERER_EXPORT renderCodeUPCE(const QRect &, const QString &, int align, QPainter *);

void ST_OPENRPT_RENDERER_EXPORT renderCodeEAN13(OROPage *, const QRectF &, const QString &, int align);
void ST_OPENRPT_RENDERER_EXPORT renderCodeEAN8(OROPage *, const QRectF &, const QString &, int align);
void ST_OPENRPT_RENDERER_EXPORT renderCodeUPCA(OROPage *, const QRectF &, const QString &, int align);
void ST_OPENRPT_RENDERER_EXPORT renderCodeUPCE(OROPage *, const QRectF &, const QString &, int align);

#endif

