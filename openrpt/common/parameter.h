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

#ifndef parameter_h
#define parameter_h

#include "stopenrptcommonexport.h"
#include <QList>
#include <QPair>
#include <QString>
#include <QVariant>

class ParameterList;

class ST_OPENRPT_COMMON_EXPORT Parameter
{
  friend class ParameterList;

  public:
    Parameter();
    Parameter(const QString &, const QVariant &);
    Parameter(const Parameter &);

    Parameter & operator= (const Parameter &);

inline
    QString  name() const  { return _name;   };
inline
    QVariant value() const { return _value;  };

  protected:
    QString  _name;
    QVariant _value;
};

class ST_OPENRPT_COMMON_EXPORT ParameterList : public QList<Parameter>
{
  public:
    void append(const char *);
    void append(const char *, const QVariant &);
    void append(const char *, const QString &);
    void append(const char *, const QDate &);
    void append(const char *, const char *);
    void append(const char *, unsigned int);
    void append(const char *, int);
    void append(const char *, double);
    void append(const Parameter &);

    QString  name(int) const;
    QVariant value(int, bool * = NULL) const;
    QVariant value(const char *, bool * = NULL) const;
    QVariant value(const QString &, bool * = NULL) const;
    bool     inList(const QString &) const;
};
  
#endif

