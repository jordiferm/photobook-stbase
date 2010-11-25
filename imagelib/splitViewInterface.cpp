//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it 
//  and/or modify it under the terms of the GNU General 
//  Public License as published by the Free Software 
//  Foundation; either version 2 of the License, or  
//  (at your option) any later version.         
//==============================================

//Systemwide includes
#include <qpainter.h>
#include <qcursor.h>

//Projectwide includes
#include "splitViewInterface.h"
#include "cursors.h"
#include "imageTools.h"
#include <QPaintEvent>

#define TEXT_BACKGROUND_MARGIN 2
#define TEXT_INSET 4

//a mouse press within DRAG_THRESHOLD will move split point
#define DRAG_THRESHOLD 8

//==============================================
using namespace Qt;

SplitViewInterface::SplitViewInterface( QWidget *parent, const char* name ) : 
                                        QWidget (parent, name )
{                  
  //setup split point                                          
  dragOffset = 0.5;                                          
  mouseMode = NO_EFFECT_ON_SPLIT;  
  currentMouseShape = NO_EFFECT_ON_SPLIT;
  
  //set default mode to adjusted image
  previewMode = SPLIT_VIEW;
  
  //don't force draw labels by default. this is useful when
  //the user pressed the Ctrl button or other key to toggle between opposite views
  //and drop down menu for choosing display type disagrees with actuall image being shown.
  forceDrawLabel = false;
  
  //setup strings and fonts
  originalString = QString( tr("Original") );
  adjustedString = QString( tr("Adjusted") );          
  
  textFont = this->font();
  textFont.setPointSize( textFont.pointSize() + 7 );  
  
  //watch mouse movements in order to drag selection
  //watch mouse movements in order to move split point between adjusted and original image
  setMouseTracking(true);
  
  //accept focus when clicked on
  setFocusPolicy( ClickFocus );
}
//==============================================
void SplitViewInterface::setAdjustedString(QString val)
{
  adjustedString = val;
  dragOffset = 0.5;
  update();
}
//==============================================
void SplitViewInterface::paintEvent(QPaintEvent *e)
{ 
  //if orig image not setup yet then return immediately
  if(origImage.isNull()) { return; }

  //if viewing adjusted or split view and adjusted image is null bail  
  if( 
      (previewMode == ADJUSTED_IMAGE || previewMode == SPLIT_VIEW ) &&
      adjustedImage.isNull() 
    )
  { return; }
  
  //create buffer to draw in
  QPixmap buffer( size() );
  
  //create a painter pointing to the buffer
  QPainter bufferPainter( &buffer );
  
  //turn off clipping to make painting operations faster
  bufferPainter.setClipping(false);

  //initialize buffer with background brush
  bufferPainter.fillRect( buffer.rect(), backgroundBrush() );

  //setup pen color
  QPen pen;
  pen.setStyle( Qt::SolidLine );
  pen.setColor( white );
  pen.setWidth( 2 );
  bufferPainter.setPen( pen);  

  int xOffset = (width() - origImage.width()) / 2;
  int yOffset = (height() - origImage.height()) / 2;

  //setup font metrics
  bufferPainter.setFont( textFont );
  QFontMetrics fm( textFont );

  //paint the adjusted image
  if(previewMode == ADJUSTED_IMAGE)
  {
    bufferPainter.drawImage( QPoint(xOffset, yOffset), adjustedImage );

    //"Adjusted" label
    if(forceDrawLabel)
    {
      int x = xOffset + (origImage.width()-fm.width(adjustedString))/2;
      int y = yOffset + fm.ascent() + TEXT_INSET;
    
      bufferPainter.fillRect( QRect(x - TEXT_BACKGROUND_MARGIN, 
                                    y - TEXT_BACKGROUND_MARGIN - fm.ascent(), 
                                    fm.width(adjustedString) + 2*TEXT_BACKGROUND_MARGIN, 
                                    fm.height() + 2*TEXT_BACKGROUND_MARGIN),
                              QBrush(darkGray) );    
      bufferPainter.drawText( x, y,
                              adjustedString );    
    }
  }
  //paint the original image
  else if(previewMode == ORIGINAL_IMAGE)
  {
    bufferPainter.drawImage( QPoint(xOffset, yOffset), origImage );

    //"Original" label
    if(forceDrawLabel)
    {
      int x = xOffset + (origImage.width()-fm.width(originalString))/2;
      int y = yOffset + fm.ascent() + TEXT_INSET;
    
      bufferPainter.fillRect( QRect(x - TEXT_BACKGROUND_MARGIN, 
                                    y - TEXT_BACKGROUND_MARGIN - fm.ascent(), 
                                    fm.width(originalString) + 2*TEXT_BACKGROUND_MARGIN, 
                                    fm.height() + 2*TEXT_BACKGROUND_MARGIN),
                              QBrush(darkGray) );    
      bufferPainter.drawText( x, y,
                              originalString );    
    }
  }
  //if using split view also draw line down center and original image on left
  else if(previewMode == SPLIT_VIEW ||
          previewMode == INV_SPLIT_VIEW )
  {
    //determine what left/right or top/bottom strings are
    QString label1, label2;
    if(previewMode == SPLIT_VIEW)
    {
      label1 = originalString;
      label2 = adjustedString;
    }
    else
    {
      label2 = originalString;
      label1 = adjustedString;
    }
    
    //find split point in screen coordinates
    int halfWay = worldToDisplay( dragOffset );
    
    //paint the original image
    bufferPainter.drawImage( QPoint(xOffset, yOffset), origImage );

    //-------
    if(origImage.width() > origImage.height() )
    {    
      //paint the adjusted image
      if(previewMode == SPLIT_VIEW)
      {
        bufferPainter.drawImage( xOffset + halfWay, 
                                 yOffset,
                                 adjustedImage,
                                 halfWay,0,
                                 origImage.width() - halfWay,
                                 origImage.height() );  
      }
      else
      {
        bufferPainter.drawImage( xOffset, 
                                 yOffset,
                                 adjustedImage,
                                 0,0,
                                 halfWay,
                                 origImage.height() );  
      }

      
      //paint white line
      bufferPainter.drawLine( xOffset + halfWay,
                              yOffset,
                              xOffset + halfWay,
                              yOffset + origImage.height() );

      //Left label
      int x = xOffset + (halfWay-fm.width(label1))/2;
      int y = yOffset + fm.ascent() + TEXT_INSET;
      
      bufferPainter.fillRect( QRect(x - TEXT_BACKGROUND_MARGIN, 
                                    y - TEXT_BACKGROUND_MARGIN - fm.ascent(), 
                                    fm.width(label1) + 2*TEXT_BACKGROUND_MARGIN, 
                                    fm.height() + 2*TEXT_BACKGROUND_MARGIN),
                              QBrush(darkGray) );    
      bufferPainter.drawText( x, y,
                              label1 );    

      //Right label
      x = xOffset + halfWay + (origImage.width() - halfWay - fm.width(label2))/2;
      
      bufferPainter.fillRect( QRect(x - TEXT_BACKGROUND_MARGIN, 
                                    y - TEXT_BACKGROUND_MARGIN - fm.ascent(), 
                                    fm.width(label2) + 2*TEXT_BACKGROUND_MARGIN, 
                                    fm.height() + 2*TEXT_BACKGROUND_MARGIN),
                              QBrush(darkGray) );    
      bufferPainter.drawText( x, y,
                              label2 );
    }
    //-------
    else
    {
      //paint the adjusted image
      if(previewMode == SPLIT_VIEW)
      {
        bufferPainter.drawImage( xOffset, 
                                 yOffset + halfWay,
                                 adjustedImage,
                                 0,halfWay,
                                 origImage.width(),
                                 origImage.height()-halfWay );  
      }
      else
      {
        bufferPainter.drawImage( xOffset, 
                                 yOffset,
                                 adjustedImage,
                                 0,0,
                                 origImage.width(),
                                 halfWay );  
      }
      
      //paint white line
      bufferPainter.drawLine( xOffset,
                              yOffset + halfWay,
                              xOffset + origImage.width(),
                              yOffset + halfWay );

      //Top label
      int x = xOffset + (origImage.width()-fm.width(label1))/2;
      int y = yOffset + fm.ascent() + TEXT_INSET;
      
      bufferPainter.fillRect( QRect(x - TEXT_BACKGROUND_MARGIN, 
                                    y - TEXT_BACKGROUND_MARGIN - fm.ascent(), 
                                    fm.width(label1) + 2*TEXT_BACKGROUND_MARGIN, 
                                    fm.height() + 2*TEXT_BACKGROUND_MARGIN),
                              QBrush(darkGray) );    
      bufferPainter.drawText( x, y,
                              label1 );    
      
      //Bottom label
      x = xOffset + (origImage.width()-fm.width(label2))/2;
      y = yOffset + halfWay + fm.height();
      
      bufferPainter.fillRect( QRect(x - TEXT_BACKGROUND_MARGIN, 
                                    y - TEXT_BACKGROUND_MARGIN - fm.ascent(), 
                                    fm.width(label2) + 2*TEXT_BACKGROUND_MARGIN, 
                                    fm.height() + 2*TEXT_BACKGROUND_MARGIN),
                              QBrush(darkGray) );    
      bufferPainter.drawText( x, y,
                              label2 );
    }
    //-------    
  }

  //end painter  
  bufferPainter.end();
  
  //blit buffer to screen
  bitBlt( this,
          e->rect().x(), e->rect().y(),
          &buffer, 
          e->rect().x(), e->rect().y(),
          e->rect().width(), e->rect().height() );
}
//==============================================
void SplitViewInterface::setPreviewMode( PREVIEW_MODE mode, bool forceDrawLabel )
{
  //set mode and repaint
  previewMode = mode;
  this->forceDrawLabel = forceDrawLabel;
  repaint(false);
}
//==============================================
bool SplitViewInterface::nearSplitPoint( QPoint p )
{
  //always false if not in split view mode
  if( previewMode != SPLIT_VIEW )
    return false;
  
  //compute painting offset and get important mouse coordinate
  int paintingOffset;
  int mousePos;
  if(origImage.width() > origImage.height())
  {
    paintingOffset = (width() - origImage.width()) / 2;
    mousePos = p.x();
  }
  else
  {
    paintingOffset = (height() - origImage.height()) / 2;
    mousePos = p.y();
  }
  
  //convert drag offset to display coordinates
  int displayCoor = worldToDisplay( dragOffset) + paintingOffset;
  
  //check if within threshold of split point
  return ( mousePos > displayCoor - DRAG_THRESHOLD &&
           mousePos < displayCoor + DRAG_THRESHOLD);
}
//==============================================
void SplitViewInterface::mousePressEvent( QMouseEvent *e)
{ 
  //if within threshold of split point enter drag split mode
  if( nearSplitPoint(e->pos()) )  
    mouseMode = DRAG_SPLIT;
}
//==============================================
void SplitViewInterface::mouseMoveEvent( QMouseEvent *e)
{
  //if not dragging split point update mosue cursor
  if(mouseMode == NO_EFFECT_ON_SPLIT)
  {
    if( !nearSplitPoint(e->pos()) && currentMouseShape == DRAG_SPLIT )
    { 
      currentMouseShape = NO_EFFECT_ON_SPLIT;
      setCursor( Qt::ArrowCursor ); 
    }
    else if( nearSplitPoint(e->pos()) && currentMouseShape == NO_EFFECT_ON_SPLIT )
    {
      currentMouseShape = DRAG_SPLIT;
      if( origImage.width() > origImage.height() )
      {
        setCursor( getCursor(MOVE_HOR_CURSOR) );
      }
      else
      {
        setCursor( getCursor(MOVE_VERT_CURSOR) );
      }
    }

    return;
  }
  
  //compute painting offset, get important mouse
  //coordinate and clamp to valid range
  QFontMetrics fm( textFont );
  int paintingOffset;
  int mousePos;
  if(origImage.width() > origImage.height())
  {
    paintingOffset = (width() - origImage.width()) / 2;
    mousePos = e->pos().x();
    mousePos = QMAX( mousePos, paintingOffset + 4*TEXT_BACKGROUND_MARGIN + fm.width(originalString) );
    mousePos = QMIN( mousePos, paintingOffset + origImage.width() - 
                     fm.width(adjustedString) - 2*TEXT_BACKGROUND_MARGIN - 2*TEXT_INSET);
  }
  else
  {
    paintingOffset = (height() - origImage.height()) / 2;
    mousePos = e->pos().y();
    mousePos = QMAX( mousePos, paintingOffset + 4*TEXT_BACKGROUND_MARGIN + fm.height() );
    mousePos = QMIN( mousePos, paintingOffset + origImage.height() - 
                     fm.height() - 2*TEXT_BACKGROUND_MARGIN - 2*TEXT_INSET);
  }
  
  //update location of split point and repaint
  dragOffset = displayToWorld(mousePos - paintingOffset);   
  repaint(false);  
}
//==============================================
void SplitViewInterface::mouseReleaseEvent( QMouseEvent *e)
{
  //disable dragging
  mouseMode = NO_EFFECT_ON_SPLIT;  

  //update mouse cursor if necessary
  if( !nearSplitPoint(e->pos()) && currentMouseShape == DRAG_SPLIT )
  { 
    currentMouseShape = NO_EFFECT_ON_SPLIT;
    setCursor( Qt::ArrowCursor ); 
  }  
}
//==============================================
double SplitViewInterface::displayToWorld( int coordinate )
{
  if( origImage.width() > origImage.height() )
  { return ((double) (coordinate+1))/origImage.width(); }
  else
  { return ((double) (coordinate+1))/origImage.height(); }
}
//==============================================
int SplitViewInterface::worldToDisplay( double coordinate )
{
  if( origImage.width() > origImage.height() )
  { return (int) (coordinate*(origImage.width() -1) ); }
  else
  { return (int) (coordinate*(origImage.height()-1) ); }
}
//==============================================
QSize SplitViewInterface::minimumSizeHint() const
{
  QFontMetrics fm( textFont );
  int w = 5*TEXT_BACKGROUND_MARGIN + fm.width(originalString) + fm.width(adjustedString);
  int h = 2*TEXT_BACKGROUND_MARGIN + fm.height(); 
  return QSize( w, h );
}
//==============================================
QImage& SplitViewInterface::getOrigImage()
{ return origImage; }
//==============================================
void SplitViewInterface::setImages( QImage origImage, 
                                    QImage adjustedImage )
{
  this->origImage = origImage;
  this->adjustedImage = adjustedImage;
  repaint(false);
}
//==============================================
void SplitViewInterface::setAdjustedImage( QImage adjustedImage )
{
  this->adjustedImage = adjustedImage;
  repaint(false);
}
//==============================================

