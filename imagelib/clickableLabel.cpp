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
#include <qimage.h>
#include <qpainter.h>
#include <QPaintEvent>

//Projectwide includes
#include "clickableLabel.h"

#include <iostream>
using namespace std;

//==============================================
ClickableLabel::ClickableLabel( QWidget* parent ) : QLabel( parent ) 
{
  //setWFlags(WRepaintNoErase);
  enabled = true;
  invisible = false;
  pressed = false;
  setMouseTracking(true);
}
//==============================================
void ClickableLabel::paintEvent(QPaintEvent *e)
{
  if( movie() != NULL ) QLabel::paintEvent(e);
  if( pixmap() == NULL ) return;
  
  QPixmap buffer( size() );
  QPainter bufferPainter( &buffer );
  bufferPainter.setClipping(false);
  bufferPainter.fillRect( buffer.rect(), backgroundBrush() );

  //only paint label if not invisible
  if( !invisible ) 
  { bufferPainter.drawImage( QPoint(0,0), pixmap()->convertToImage() ); }
  
  bufferPainter.end();
  bitBlt( this, e->rect().x(), e->rect().y(),
          &buffer, 
          e->rect().x(), e->rect().y(),
          e->rect().width(), e->rect().height() );
}
//==============================================
void ClickableLabel::setPixmap( const QPixmap &p )
{
  defaultPixmap = p;
  defaultPixmap.detach();
  QLabel::setPixmap( p );  
}
//==============================================
void ClickableLabel::setEnabled( bool val )
{
  if( val )
    setPixmap( defaultPixmap );
  else if( ! defaultPixmap.isNull() )
  {
    //create over exposed gray version
    QImage newImage = defaultPixmap.convertToImage();
    QImage alpha = newImage.createAlphaMask();
    int x, y;
    for(y=0; y<newImage.height(); y++)
    {
      for(x=0; x<newImage.width(); x++)
      {
        if(alpha.isNull() || qRed(alpha.pixel(x,y)) == 0)
        {
          QRgb rgb = newImage.pixel(x,y);
          int gray = qGray(rgb);
          double alpha = 0.5;
          gray = (int) (alpha*gray+(1-alpha)*255);
          rgb = qRgb( gray, gray, gray );
          newImage.setPixel( x, y, rgb );
        }
      }
    }
    QPixmap newPixmap;
    newPixmap.convertFromImage( newImage );
    QLabel::setPixmap( newPixmap );
  }
  
  enabled = val;
}
//==============================================
void ClickableLabel::mouseMoveEvent( QMouseEvent* e )
{
  if( !enabled ) return;

  QPoint p = e->pos();
  if( p.x() < 0 || p.y() < 0 ||
      p.x() >= width() || p.y() >= height() )
  {
    if(!defaultPixmap.isNull())
    { this->setPixmap( defaultPixmap ); }      
  }
  else
  {  
    if( pressed )
      setActive();
    else
      setHighlighted();
  }  
}
//==============================================
void ClickableLabel::leaveEvent ( QEvent * )
{
  if( !enabled ) return;

  if(!defaultPixmap.isNull())
  { this->setPixmap( defaultPixmap ); }      
}
//==============================================
void ClickableLabel::enterEvent ( QEvent * )
{
  if( !enabled ) return;
  
  if( pressed )
    setActive();
  else
    setHighlighted();
}
//==============================================
void ClickableLabel::setActive()
{
  if( ! defaultPixmap.isNull() )
  {
    //create darkened out version
    QImage newImage = defaultPixmap.convertToImage();
    QImage alpha = newImage.createAlphaMask();
    int x, y;
    for(y=0; y<newImage.height(); y++)
    {
      for(x=0; x<newImage.width(); x++)
      {
        if(alpha.isNull() || qRed(alpha.pixel(x,y)) == 0)
        {
          QRgb rgb = newImage.pixel(x,y);
          int r = (int) (0.5*qRed(rgb));
          int g = (int) (0.5*qGreen(rgb));
          int b = (int) (0.5*qBlue(rgb));
          rgb = qRgb( r, g, b );
          newImage.setPixel( x, y, rgb );
        }
      }
    }
    QPixmap newPixmap;
    newPixmap.convertFromImage( newImage );
    QLabel::setPixmap( newPixmap );
  }  
}
//==============================================
void ClickableLabel::setHighlighted()
{
  if( ! defaultPixmap.isNull() )
  {
    //create slightly darker version
    QImage newImage = defaultPixmap.convertToImage();
    QImage alpha = newImage.createAlphaMask();
    int x, y;
    for(y=0; y<newImage.height(); y++)
    {
      for(x=0; x<newImage.width(); x++)
      {
        if(alpha.isNull() || qRed(alpha.pixel(x,y)) == 0)
        {
          QRgb rgb = newImage.pixel(x,y);
          int r = (int) (0.8*qRed(rgb));
          int g = (int) (0.8*qGreen(rgb));
          int b = (int) (0.8*qBlue(rgb));
          rgb = qRgba( r, g, b, qAlpha(rgb) );
          newImage.setPixel( x, y, rgb );
        }
      }
    }
    QPixmap newPixmap;
    newPixmap.convertFromImage( newImage );
    QLabel::setPixmap( newPixmap );
  }
}
//==============================================
void ClickableLabel::mousePressEvent ( QMouseEvent * )
{
  if( !enabled ) return;
  
  pressed = true;
  setActive();  
}
//==============================================
void ClickableLabel::mouseReleaseEvent ( QMouseEvent *e )
{  
  if( !enabled || invisible ) return;
  
  pressed = false;

  //restore pixmap if exists
  if(!defaultPixmap.isNull())
  { this->setPixmap( defaultPixmap ); }
  
  //if mouse still over button emit clicked signal
  if(e->pos().x() >= 0 &&
     e->pos().y() >= 0 &&
     e->pos().x() < width() &&
     e->pos().y() < height() )
  {    
    emit clicked();
  }
}
//==============================================
void ClickableLabel::setInvisible( bool val )
{
  //avoid repainting when not necessary
  if( invisible != val)
  {
    invisible = val;
    repaint();
  }
}
//==============================================

