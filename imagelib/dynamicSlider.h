//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef GUI_DYNAMICSLIDER_H
#define GUI_DYNAMICSLIDER_H
#include "stimagelibexport.h"

#include <qslider.h>
#include <qlabel.h>
#include <qpoint.h>

class SliderToolTip;
//==========================================
///A more dynamic slider that provides moving tooltips that show the slider value
class ST_IMAGELIB_EXPORT DynamicSlider : public QSlider
{
Q_OBJECT
public:
  DynamicSlider( Qt::Orientation orientation, QWidget* parent, const char* name=0 );
  ~DynamicSlider();

  ///when set, a zero string is shown instead of the current value/prefix/suffix when the slider value is 0
  void setZeroString( QString val );
  
  ///set the prefix that is displayed before the current slider value
  void setPrefix( QString val );

  ///set two prefix values, one for when the value is positive and one for when the value is negative.
  void setPrefixes( QString prefix1, QString prefix2 );

  ///set the suffix that is displayed after the current slider value
  void setSuffix( QString val );
  
  ///set two suffix values, one for when the value is positive and one for when the value is negative.
  void setSuffixes( QString suffix1, QString suffix2 );
 
  //used by the tooltip class to determine the current mouse position
  QPoint getMousePos();

protected:
  //cache and announce to the tooltip class that the mouse has moved
  void mouseMoveEvent(QMouseEvent* e);
  
  ///subclass DynamicSlider and reimplement this method
  ///to change the behavior used to display slider values
  virtual QString mapValToString();

private:
  QString zeroString;
    
  QString prefix1, prefix2;
  QString suffix1, suffix2;
  
  SliderToolTip* tooltip;
  QPoint cachedMousePos;

signals:
  void mouseHasMoved();

private slots:
  void updateTooltipLabel();
};
//==========================================
///private class used by the DynamicSlider to show tooltips. do not use!
class SliderToolTip : public QLabel
{
Q_OBJECT

public:
  SliderToolTip( QWidget* parent, DynamicSlider* slider);

private:
  DynamicSlider* slider;  
  
private slots:
  void showTooltip();
  void update();
  void hideTooltip();
};
//==========================================

#endif //GUI_DYNAMICSLIDER_H
