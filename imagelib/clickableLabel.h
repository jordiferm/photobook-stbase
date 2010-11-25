//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef GUI_CLICKABLELABEL_H
#define GUI_CLICKABLELABEL_H

#include <qlabel.h>
#include <qpixmap.h>
#include "stimagelibexport.h"

//=====================================
/*! \brief A clickable label
 */
//=====================================

//======================
class ST_IMAGELIB_EXPORT ClickableLabel : public QLabel
{
Q_OBJECT
//----------------------
public:
  ClickableLabel( QWidget *parent=0);  
  void setInvisible( bool val );
//----------------------
protected:
  ///double buffer to avoid flicker
  void paintEvent( QPaintEvent *e );

  void enterEvent ( QEvent * );
  void leaveEvent ( QEvent * );
  
  void mouseMoveEvent( QMouseEvent* e );
  void mousePressEvent ( QMouseEvent * );
  void mouseReleaseEvent ( QMouseEvent * e );
  //----------------------
public slots:
  void setEnabled( bool val );
  void setPixmap( const QPixmap &p );
//----------------------
signals:
  void clicked();
//----------------------
private:
  //redraws label using an "active" pixmap
  void setActive();

  //redraws label using a "hightlighted" pixmap
  void setHighlighted();
  
  QPixmap defaultPixmap;
  bool enabled;
  bool invisible;
  bool pressed;
//----------------------
};
//======================

#endif //GUI_CLICKABLELABEL_H
