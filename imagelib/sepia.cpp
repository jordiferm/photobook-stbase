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
#include <qstring.h>
#include <qapplication.h>

//Projectwide includes
#include "sepia.h"
#include "manipulationOptions.h"
#include "sprocessstatuswidget.h"

//----------------------------------------------
// Inputs:
// -------
// QString filename - location of original image on disk
// StatusWidget* status - widget for making progress visible to user
//
// Outputs:
// --------
// QImage* returned - constructed image
//
// Description:
// ------------
// This method constructs a sepia version of
// the image by computing a sepia value for each pixel. 
// The sepia effect is intended to mimick sepia coloring 
// obtained during traditional film development.
//
// First, a sepia color is chosen with a nice yellowy-orange tone
// (r,g,b = 162,128,101). This color is next converted to HSL 
// (hue, saturation, luminance) space and used during sepia coloring.
// 
// For each pixel, we construct a modified sepia color where the original
// luminance (brightness) is replaced with that pixels luminance using
// a weighted sum of the color channels. (For more information on image
// luminance see the blackWhite effect). This modified sepia color is
// converted back to RGB color space and used for the pixel color
// in the modified image. This way image brightness is maintained
// while color information is replaced with the sepia color. The
// sepia color can be adjusted to produce a differnt overall color
// feeling by changing the original sepia color.
//----------------------------------------------

QImage* sepiaEffect ( QString fileName, ManipulationOptions* options )
{
	//load image
  QImage* editedImage = new QImage( fileName );
  return sepiaEffect( editedImage, options );
}

//==============================================
QImage* sepiaEffect( QImage* editedImage, ManipulationOptions* options )
{
  //convert to 32-bit depth if necessary
  if( editedImage->depth() < 32 )
  {
    QImage* tmp = editedImage;
    editedImage = new QImage( tmp->convertDepth( 32, Qt::AutoColor ) );
    delete tmp; tmp=NULL;
  }

  //determine if busy indicators will be used
  bool useBusyIndicators = false;
  SProcessStatusWidget* status = NULL;
  if( options != NULL && options->getStatus() != NULL )
  {
    useBusyIndicators = true;
    status = options->getStatus(); 
  }
  
  //setup progress bar
  if(useBusyIndicators)
  {
    QString statusMessage = qApp->translate( "sepiaEffect", "Applying Sepia Effect:" );
    status->showProgressBar( statusMessage, 100 );
    qApp->processEvents();  
  }
  
  //update progress bar for every 1% of completion
  const int updateIncrement = (int) ( 0.01 * editedImage->width() * editedImage->height() );
  int newProgress = 0; 

  //compute the hsl/hsv coordinates of sepia color
  int sepiaH, sepiaS, sepiaL;
  QColor(162,128,101).getHsv( &sepiaH, &sepiaS, &sepiaL );
  
  //iterate over each selected scanline 
  int x, y, pixelLuminance;
  QRgb* rgb;
  QColor sepiaColor;
  uchar* scanLine;
  
  for( y=0; y<editedImage->height(); y++)
  {   
    //iterate over each selected pixel in scanline
    scanLine = editedImage->scanLine(y);
    for( x=0; x<editedImage->width(); x++)
    {
      //compute gray value based on the display luminance of color coordinates
      rgb = ((QRgb*)scanLine+x);
      pixelLuminance = (int) (0.2125*qRed(*rgb) + 0.7154*qGreen(*rgb) + 0.0721*qBlue(*rgb));
      
      //compute and set sepia color
      sepiaColor.setHsv( sepiaH, sepiaS, pixelLuminance );      
      *rgb = sepiaColor.rgb();
      
      //update status bar if significant progress has been made since last update
      if(useBusyIndicators)
      {
        newProgress++;
        if(newProgress >= updateIncrement)
        {
          newProgress = 0;
          status->incrementProgress();
          qApp->processEvents();  
        }
      }
      
    }
  }
  
  //return pointer to edited image
  return editedImage;  
}
//==============================================
