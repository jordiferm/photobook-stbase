/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2008 by OpenMFG, LLC
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

#ifndef __REPORTPAGEOPTIONS_H__
#define __REPORTPAGEOPTIONS_H__

#include <qobject.h>
#include <qstring.h>
#include "stopenrptcommonexport.h"

class ST_OPENRPT_COMMON_EXPORT ReportPageOptions : public QObject
{
  Q_OBJECT
  public:
    ReportPageOptions();
    ReportPageOptions(const ReportPageOptions &);

    ReportPageOptions & operator=(const ReportPageOptions &);

    enum PageOrientation {
      Landscape = 0, // essentially false
      Portrait = 1   // and true
    };

    double getMarginTop();
    void setMarginTop(double v);
    double getMarginBottom();
    void setMarginBottom(double v);
    double getMarginLeft();
    void setMarginLeft(double v);
    double getMarginRight();
    void setMarginRight(double v);

    const QString & getPageSize();
    void setPageSize(const QString & s);
    double getCustomWidth();
    void setCustomWidth(double v);
    double getCustomHeight();
    void setCustomHeight(double v);

    PageOrientation getOrientation();
    bool isPortrait();
    void setOrientation(PageOrientation o);
    void setPortrait(bool yes);

    void setLabelType(const QString &);
    const QString & getLabelType();

  signals:
    void pageOptionsChanged();

  private:
    double _marginTop;
    double _marginBottom;
    double _marginLeft;
    double _marginRight;

    QString _pagesize;
    double _customWidth;
    double _customHeight;

    PageOrientation _orientation;

    QString _labelType;
};

#endif
