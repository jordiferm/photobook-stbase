//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#include <qapplication.h>
#include <qtooltip.h>
#include <QDesktopWidget>
#include <QMouseEvent>

#include "dynamicSlider.h"


//==========================================
DynamicSlider::DynamicSlider( Qt::Orientation orientation, QWidget* parent,
                              const char* name) : QSlider(orientation, parent, name)
{
  //determine the parent screen the tooltip will be displayed in and create tooltip
  int scr = QApplication::desktop()->screenNumber( this );                                
  tooltip = new SliderToolTip( QApplication::desktop()->screen( scr ), this);
  updateTooltipLabel();

  //make sure tooltip label is updated when the slider value changes
  connect( this, SIGNAL( valueChanged(int) ),
           this, SLOT( updateTooltipLabel() ) );
}
//==========================================
DynamicSlider::~DynamicSlider()
{
  delete tooltip;
  tooltip = NULL;
}
//==========================================
void DynamicSlider::setZeroString( QString val )
{
  zeroString = val;  
  updateTooltipLabel();
}
//==========================================
void DynamicSlider::setPrefix( QString val )
{
  prefix1 = val;
  prefix2 = QString("");
  updateTooltipLabel();
}
//==========================================
void DynamicSlider::setPrefixes( QString v1, QString v2 )
{
  prefix1 = v1;
  prefix2 = v2;
  updateTooltipLabel();
}
//==========================================
void DynamicSlider::setSuffix( QString val )
{
  suffix1 = val;
  suffix2 = QString("");
  updateTooltipLabel();
}
//==========================================
void DynamicSlider::setSuffixes( QString v1, QString v2 )
{
  suffix1 = v1;
  suffix2 = v2;
  updateTooltipLabel();
}
//==========================================
QString DynamicSlider::mapValToString()
{
  //the default behavior is to simply use the slider value directly
  if( orientation() == Qt::Vertical )  
    return QString("%1").arg( -value() );
  else
    return QString("%1").arg(value());
}
//==========================================
void DynamicSlider::updateTooltipLabel()
{
  //determine string that will be used for tooltip
  QString tipString;
  
  //if the value is zero and a zero string has been provided used that
  if( value() == 0 && !zeroString.isNull() )
  {
    tipString = zeroString;
  }
  //otherwise construct a tip string using provided prefixes, suffixes, and the current slider value
  else
  { 
    //determine prefix and suffix that will be used to construct tooltip string
    QString p, s;
    if( value() > 0 || prefix2.isNull() ) p = prefix1;
    else                                  p = prefix2;

    if( value() > 0 || suffix2.isNull() ) s = suffix1;
    else                                  s = suffix2;
    
    //construct tipstring
    tipString = QString("%1%2%3").arg(p).arg(mapValToString()).arg(s);
    
  }

  //update tooltip
  tooltip->setText( tipString );
  tooltip->adjustSize();
  if( tooltip->isShown() ) qApp->processEvents();
}
//==========================================
void DynamicSlider::mouseMoveEvent(QMouseEvent* e)
{
  //cache the mouse position since the tooltip will need this information when updating itself
  cachedMousePos = e->pos();
  QSlider::mouseMoveEvent(e);
  emit mouseHasMoved();
}
//==========================================
QPoint DynamicSlider::getMousePos() { return cachedMousePos; }
//==========================================
SliderToolTip::SliderToolTip( QWidget* parent, DynamicSlider* slider) 
: QLabel( parent )
// "toolTipTip",
//           WStyle_StaysOnTop | WStyle_Customize | 
//           WStyle_NoBorder | WStyle_Tool | WX11BypassWM )
{
  //store slider handle                             
  this->slider = slider;
  
  //setup lable to use standard black writing on a light yellow background so it
  //looks like a normal tooltip
  setPaletteForegroundColor( QColor("Black") );
  setPaletteBackgroundColor( QColor("LightYellow") );

  //use default system tooltip font
  //setFont( QToolTip::font() );
  
  //setup the otherparmslike a frame etc so it looks like a normal tooltip
  setMargin(1);
//  setAutoMask( FALSE );
  setFrameStyle( QFrame::Plain | QFrame::Box );
  setLineWidth( 1 );
  setAlignment( Qt::AlignTop );
  setIndent(0);
  polish();
  adjustSize();
  
  //show the tooltip when the user presses the slider
  connect( slider, SIGNAL( sliderPressed() ),  this, SLOT( showTooltip() ) );
  
  //move tooltip to follow the slider handle
  setMouseTracking(true);
  connect( slider, SIGNAL( mouseHasMoved() ),  this, SLOT( update() ) );
  
  //hide tooltip when users releases the slider
  connect( slider, SIGNAL( sliderReleased() ), this, SLOT( hideTooltip() ) );
}
//==========================================
void SliderToolTip::showTooltip()
{
  //make sure label is up-to-date
  update();
  show();
}
//==========================================
void SliderToolTip::hideTooltip() { hide(); }
//==========================================
void SliderToolTip::update()
{
  //margin well provide betweent the slider and the tooltip
  const int TOOLTIP_MARGIN = 4;
  
  //fetch slider handle rect
	QRect sliderRect = slider->rect(); //style()->querySubControlMetrics();
  
  //determine location tooltip will be shown
  QPoint tooltipTopLeft;
  if( slider->orientation() == Qt::Horizontal )
  {
    tooltipTopLeft = QPoint( sliderRect.right() + TOOLTIP_MARGIN,
                             slider->getMousePos().y() >= sliderRect.top() ?
                             sliderRect.top() - TOOLTIP_MARGIN - height() :
                             sliderRect.bottom() + TOOLTIP_MARGIN );
  }
  else
  {
    tooltipTopLeft = QPoint( slider->getMousePos().x() >= sliderRect.right() ?
                             sliderRect.left() - TOOLTIP_MARGIN - width() :
                             sliderRect.right() + TOOLTIP_MARGIN,
                             (sliderRect.top() + sliderRect.bottom())/2 - height()/2 );
  }
  
  //map tooltip position from slider widget to screen coordinates
  tooltipTopLeft = slider->mapToGlobal( tooltipTopLeft );
  
  //position tooltip
  move( tooltipTopLeft );
  if( isShown() ) qApp->processEvents();
}
//==========================================
