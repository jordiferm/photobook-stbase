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

/*
 *     This is a collection of various functions used to parse the
 * different xml structures used in the Report definitions.  Most
 * of these are structures that are common to several or the complex
 * structures.
 */

#ifndef __PARSEXMLUTILS_H__
#define __PARSEXMLUTILS_H__

// forward declarations
class QDomElement;

#include "querysource.h"
#include "reportpageoptions.h"

#include <qstring.h>
#include <qrect.h>
#include <qfont.h>
#include <qcolor.h>
#include <q3ptrlist.h>
#include <q3valuelist.h>
#include <qmap.h>

#include <QList>
#include <QPair>
#include "stopenrptcommonexport.h"

//
// Data Structures
//
class ST_OPENRPT_COMMON_EXPORT ORDataData
{
  public:
    ORDataData()
    {
      query = QString::null;
      column = QString::null;
    }
    ORDataData(const QString & q, const QString & c)
    {
      query = q;
      column = c;
    }
    ORDataData(const ORDataData & d)
    {
      query = d.query;
      column = d.column;
    }

    ORDataData & operator=(const ORDataData & d)
    {
      query = d.query;
      column = d.column;
      return *this;
    }

    bool operator==(const ORDataData & d) const
    {
      return ((query == d.query) && (column == d.column));
    }

    bool operator< (const ORDataData & d) const
    {
      if((query < d.query) || (query == d.query && column < d.column))
        return true;
      return false;
    }

    QString query;
    QString column;
};
class ST_OPENRPT_COMMON_EXPORT ORKeyData
{
  public:
    QString query;
    QString column;
};

class ST_OPENRPT_COMMON_EXPORT ORColorDefData
{
  public:
    QString name;
    int red, green, blue;
};
class ST_OPENRPT_COMMON_EXPORT ORTitleData
{
  public:
    QString string;
    QFont font;
    bool font_defined;
};
class ST_OPENRPT_COMMON_EXPORT ORStyleData
{
  public:
    bool bar;
    bool line;
    bool point;
};
class ST_OPENRPT_COMMON_EXPORT ORDataAxisData
{
  public:
    ORTitleData title;
    QString column;
    QFont font;
    bool font_defined;
};
class ST_OPENRPT_COMMON_EXPORT ORValueAxisData
{
  public:
    ORTitleData title;
    double min;
    double max;
    bool autominmax;
    QFont font;
    bool font_defined;
};
class ST_OPENRPT_COMMON_EXPORT ORSeriesData
{
  public:
    QString name;
    QString color; 
    QString column;
    ORStyleData style;
};


class ORLineData;
class ORRectData;
class ORLabelData;
class ORFieldData;
class ORTextData;
class ORBarcodeData;
class ORImageData;
class ORGraphData;

class ST_OPENRPT_COMMON_EXPORT ORObject
{
  public:
    ORObject();
    virtual ~ORObject();

    virtual bool isLine();
    virtual ORLineData * toLine();
    virtual bool isRect();
    virtual ORRectData * toRect();
    virtual bool isLabel();
    virtual ORLabelData * toLabel();
    virtual bool isField();
    virtual ORFieldData * toField();
    virtual bool isText();
    virtual ORTextData * toText();
    virtual bool isBarcode();
    virtual ORBarcodeData * toBarcode();
    virtual bool isImage();
    virtual ORImageData * toImage();
    virtual bool isGraph();
    virtual ORGraphData * toGraph();
};

class ST_OPENRPT_COMMON_EXPORT ORLineData : public ORObject
{
  public:
    int xStart, yStart;
    int xEnd,   yEnd;
    int weight;

    virtual bool isLine();
    virtual ORLineData * toLine();
};

class ST_OPENRPT_COMMON_EXPORT ORRectData : public ORObject
{
public:
    int x, y;
    int width,  height;
    int weight;

    virtual bool isRect();
    virtual ORRectData * toRect();
};

class ST_OPENRPT_COMMON_EXPORT ORLabelData : public ORObject
{
  public:
    QRect rect;
    QFont font;
    int align;
    QString string;

    virtual bool isLabel();
    virtual ORLabelData * toLabel();
};

class ST_OPENRPT_COMMON_EXPORT ORFieldData : public ORObject
{
  public:
    QRect rect;
    QFont font;
    int align;
    ORDataData data;

    bool trackTotal;
    bool sub_total;
    bool builtinFormat;
    QString format;

    virtual bool isField();
    virtual ORFieldData * toField();
};

class ST_OPENRPT_COMMON_EXPORT ORTextData : public ORObject
{
  public:
    QRect rect;
    QFont font;
    int align;
    ORDataData data;
    int bottompad;

    virtual bool isText();
    virtual ORTextData * toText();
};

class ST_OPENRPT_COMMON_EXPORT ORBarcodeData : public ORObject
{
  public:
    QRect rect;
    QString format;
    int maxlength;
    ORDataData data;
    int align; // 0 = left, 1 = center, 2 = right

    virtual bool isBarcode();
    virtual ORBarcodeData * toBarcode();
};

class ST_OPENRPT_COMMON_EXPORT ORImageData : public ORObject
{
  public:
    QRect rect;

    QString mode;

    QString format;      // } 
    QString inline_data; // } INLINE
                         //     OR
    ORDataData data;     // } FROM DB

    virtual bool isImage();
    virtual ORImageData * toImage();
};

class ST_OPENRPT_COMMON_EXPORT ORGraphData : public ORObject
{
  public:
    ORDataData data;

    QFont font;
    QRect rect;

    ORTitleData title;
    ORDataAxisData dataaxis;
    ORValueAxisData valueaxis;

    Q3PtrList<ORSeriesData> series;

    virtual bool isGraph();
    virtual ORGraphData * toGraph();
};


//
// ORWatermarkData
//
class ST_OPENRPT_COMMON_EXPORT ORWatermarkData
{
  public:
    ORWatermarkData();

    int opacity;
    bool useDefaultFont;
    QFont font;
    bool staticText;
    QString text;
    ORDataData data;

    bool valid;
};

//
// ORBackgroundData
//
class ST_OPENRPT_COMMON_EXPORT ORBackgroundData
{
  public:
    ORBackgroundData();

    bool enabled;
    bool staticImage;
    QString image;
    ORDataData data;
    int opacity;
    QString mode;
    int align;
    QRect rect;
};

//
// ORSectionData is used to store the information about a specific
// section. A section has a name and optionally extra data. `name'
// rpthead, rptfoot, pghead, pgfoot, grphead, grpfoot or detail.
// In the case of pghead and pgfoot extra would contain the page
// designation (firstpage, odd, even or lastpage).
//
class ST_OPENRPT_COMMON_EXPORT ORSectionData
{
  public:
    QString name;
    QString extra; // extra info about the section
    int height;

    Q3PtrList<ORObject> objects;
    Q3ValueList<ORDataData> trackTotal;
};

class ST_OPENRPT_COMMON_EXPORT ORDetailGroupSectionData
{
  public:
    ORDetailGroupSectionData();

    enum PageBreak {
      BreakNone = 0,
      BreakAfterGroupFoot = 1
    };

    QString name;
    QString column;
    int pagebreak;

    QMap<ORDataData,double> _subtotCheckPoints;

    ORSectionData * head;
    ORSectionData * foot;
};

class ST_OPENRPT_COMMON_EXPORT ORDetailSectionData
{
  public:
    ORDetailSectionData();

    enum PageBreak {
      BreakNone = 0,
      BreakAtEnd = 1
    };

    QString name;
    int pagebreak;

    ORKeyData key;

    ORSectionData * detail;

    Q3ValueList<ORDetailGroupSectionData*> groupList;
    Q3ValueList<ORDataData> trackTotal;
};

class ST_OPENRPT_COMMON_EXPORT ORParameter
{
  public:
    ORParameter() : active(false) {};
    ORParameter(const QString & n) : name(n), active(false) {}
    ORParameter(const ORParameter & d)
    {
      name = d.name;
      type = d.type;
      defaultValue = d.defaultValue;
      description = d.description;
      listtype = d.listtype;
      query = d.query;
      values = d.values;
      active = d.active;
    }

    ORParameter & operator=(const ORParameter & d)
    {
      name = d.name;
      type = d.type;
      defaultValue = d.defaultValue;
      description = d.description;
      listtype = d.listtype;
      query = d.query;
      values = d.values;
      active = d.active;
      return *this;
    }

    QString name;
    QString type;
    QString defaultValue;
    QString description;
    QString listtype;
    QString query;
    QList<QPair<QString,QString> > values;
    bool active;
};

class ST_OPENRPT_COMMON_EXPORT ORReportData
{
  public:
    ORReportData();

    QString title;
    QString name;
    QString description;

    QMap<QString,ORParameter> definedParams;

    ORWatermarkData wmData;
    ORBackgroundData bgData;

    ReportPageOptions page;
    QuerySourceList queries;

    ORSectionData * pghead_first;
    ORSectionData * pghead_odd;
    ORSectionData * pghead_even;
    ORSectionData * pghead_last;
    ORSectionData * pghead_any;

    ORSectionData * rpthead;
    ORSectionData * rptfoot;

    ORSectionData * pgfoot_first;
    ORSectionData * pgfoot_odd;
    ORSectionData * pgfoot_even;
    ORSectionData * pgfoot_last;
    ORSectionData * pgfoot_any;

    Q3PtrList<ORDetailSectionData> sections;
    QMap<QString, QColor> color_map;
    Q3ValueList<ORDataData> trackTotal;
};


bool ST_OPENRPT_COMMON_EXPORT parseReportRect(const QDomElement &, QRect &);
bool ST_OPENRPT_COMMON_EXPORT parseReportFont(const QDomElement &, QFont &);

bool ST_OPENRPT_COMMON_EXPORT parseReportData(const QDomElement &, ORDataData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportKey(const QDomElement &, ORKeyData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportLine(const QDomElement &, ORLineData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportLabel(const QDomElement &, ORLabelData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportField(const QDomElement &, ORFieldData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportText(const QDomElement &, ORTextData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportBarcode(const QDomElement &, ORBarcodeData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportImage(const QDomElement &, ORImageData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportWatermark(const QDomElement &, ORWatermarkData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportBackground(const QDomElement &, ORBackgroundData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportColorDefData(const QDomElement &, ORColorDefData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportGraphData(const QDomElement &, ORGraphData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportTitleData(const QDomElement &, ORTitleData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportStyleData(const QDomElement &, ORStyleData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportDataAxisData(const QDomElement &, ORDataAxisData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportValueAxisData(const QDomElement &, ORValueAxisData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportSeriesData(const QDomElement &, ORSeriesData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportSection(const QDomElement &, ORSectionData &);
bool ST_OPENRPT_COMMON_EXPORT parseReportDetailSection(const QDomElement &, ORDetailSectionData &);
bool ST_OPENRPT_COMMON_EXPORT parseReport(const QDomElement &, ORReportData &);

bool ST_OPENRPT_COMMON_EXPORT parseReportParameter(const QDomElement &, ORReportData &);

#endif
