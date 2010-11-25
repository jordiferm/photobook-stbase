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
#include <qapplication.h>
#include <QDesktopWidget>

//Projectwide includes
#include "scaledPreviewInterface.h"
#include "imageTools.h"


//==============================================
ScaledPreviewInterface::ScaledPreviewInterface( QString imageFilename, 
                                                QWidget *parent, const char* name ) : 
                                                SplitViewInterface (parent, name )
{                  
  //store original image dimensions
  getImageSize( imageFilename, origImageSize );
  
  //resize image to current screen size for faster
  //scaling during resize events
  QRect screenSize = qApp->desktop()->availableGeometry();
  scaleImage( imageFilename, fullScreenImage, screenSize.width()/2, screenSize.height()/2 );
  //-----
}
//==============================================
void ScaledPreviewInterface::resizeEvent( QResizeEvent * )
{
  //if image has yet to be set return
  if( fullScreenImage.isNull() ) return;

  //generate orig image
  //set adjusted image to null so repain won't occur until it is reset
  setImages( fullScreenImage.scaled( width(), height(), Qt::KeepAspectRatio ), QImage() );
  
  //emit resized signal
  emit resized();
}
//==============================================
QSize ScaledPreviewInterface::sizeHint() const
{
  //a 500x375 is 4:3 aspect ratio which is fairly typical
  //we'll clamp the hight to 375 in the situation where an image is mucher taller than wide
  //(and hence computed height would be so big window won't fit on screen)  
  int w = 500;
  int h = QMIN( 375, (w*origImageSize.height())/origImageSize.width() );

  return QSize( w,h );
}
//==============================================
QSize ScaledPreviewInterface::minimumSizeHint() const
{
  QSize baseMinSize = SplitViewInterface::minimumSizeHint();
  int w = baseMinSize.width();
  int h = QMAX( baseMinSize.height(), (w*origImageSize.height())/origImageSize.width() );
  return QSize( w, h );
}
//==============================================                              
