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
#include "contrast.h"
#include "imageTools.h"

#include "sprocessstatuswidget.h"

//----------------------------------------------
// Inputs:
// -------
// QString filename - location of original image on disk
// SProcessStatusWidget* status - widget for making progress visible to user
//
// Outputs:
// --------
// QImage* returned - constructed image
//
// Description:
// ------------
// This method simply stretchs the luminance rnage to use up the entire dynamic range, in our
// case from [0,255]. This algorithm is very similar to the color enhancement algorithm, except
// in this case we convert color values to HSL space and work with the L values (luminance).
//
// The algorithm can be broken up into two steps:
// 1.) finding the current luminance endpoints (e.g. [6,120])
// 2.) stretching the luminance range to fit the full [0,255] range
//
// One could simply iterate over the entire image and find the minimum and 
// maximum luminance values. However; often there is residual 
// noise at the endpoints since camera sensors always provide some noise, 
// while the majority of pixels fall somewhere in the middle. If we constructed 
// a histogram for of pixel luminance values it might look like this:
//  5 |                      .
//  . |                     . .
//  . |                    .   .
//  1 | .    .          .        .              .
//  0 |. .... ........             ............. .....
//    |-X------------*--------------*-----------X-----
//     0            100             175            255            
//
// Here luminance values are plotted across the x axis, while the y axis indicates 
// how offten a particular luminance value was seen.
//
// In this example we notice most pixel luminance values were observed fall in 
// the 100-175 range. A few pixels with values of 5 or 10 were seen, and a 
// few with around 230 were seen. We want to stretch the common case, 100-175, 
// out to the full 0-255 range. That will make the pixels that were 175 now 255, 
// those that wer 100 now 0, thus darks will become darker, brights will become
// brigther. We'll lineraly scale all pixels inbetween. Any pixels beyond the 
// 100-175 range will get clamped at 0 and 255 respectively.
//
// Now, if we simply find the smallest and largest values we'll get soemthinglike 
// 5 and 235, and stretching will have very little effect on the image. Instead, 
// we first actually populate a histogram array, then determine the 1% and 
// 99% boundaries but walking from the end points of hte histogram array and 
// adding up the elements. Once 1% of the pixels have been accounted for we set 
// the boundary endpoint to the current index, thus giving us 100,175 in this case.
//
// Scaling the pixels is only marginally more complicated when in 
// the color enhancement algorithm. A second pass over the 
// image takes place during which a pixels color is fetched, converted to HSL,
// the luminance is stretched according to the following equation, the color is
// converted back to RGB, and the value is written back to the image.
//
// l' = [255*(l-lLow)] / [lHigh - lLow]
//
// where l[Low/High] refer to the 1% and 99% endpoints for the luminance histogram.
//
//----------------------------------------------

//==============================================
void enhanceImageContrast( QString filename, SProcessStatusWidget* status )
{
  //load original image
  QImage* editedImage = new QImage( filename );

  //enhance contrast
  enhanceImageContrast( editedImage, status );
}
//==============================================
void enhanceImageContrast( QImage* editedImage, SProcessStatusWidget* status )
{  
  //convert to 32-bit depth if necessary
  if( editedImage->depth() < 32 )
  {
    QImage* tmp = editedImage;
    editedImage = new QImage( tmp->convertDepth( 32, Qt::AutoColor ) );
    delete tmp; tmp=NULL;
  }
	
	//setup progress bar
  if(status)
  {
  QString statusMessage = qApp->translate( "enhanceImageContrast", "Enhancing Contrast:" );
  status->showProgressBar( statusMessage, 100 );
  qApp->processEvents();  
  }
  
  //update progress bar for every 1% of completion
  const int updateIncrement = (int) ( 0.01 * editedImage->width() * editedImage->height() );
  int newProgress = 0; 

  ///------
  /// Contrast stretching - http://www.ph.tn.tudelft.nl/Courses/FIP/frames/fip-istogram.html
  ///-------  
  
  //construct intensity histograph
  int grayVals[256];
  int i=0;
  for(i=0; i<256; i++) { grayVals[i] = 0; }  
  
  //populate histogram by iterating over all image pixels
  int numPixels = editedImage->width()*editedImage->height();  
  QRgb* rgb;
  double grayValue; 
  uchar* scanLine;
  int x, y;
  for( y=0; y<editedImage->height(); y++)
  {   
    //iterate over each selected pixel in scanline
    scanLine = editedImage->scanLine(y);
    for( x=0; x<editedImage->width(); x++)
    {
      rgb = ((QRgb*)scanLine+x);
      grayValue = RGBtoL(rgb);
      grayVals[(int)grayValue]++;              
    } //for x
  } //for y
  
  //find 1% and 99% precenticles
  //we'll stretch these values so we avoid outliers from affecting the stretch
  int sum=0;
  double indexLow, indexHigh;
  indexLow = -1.0; indexHigh = -1.0;
  for(i=0; i<256; i++)
  {
    sum+=grayVals[i];
    
    //if 1% not found yet and criteria met set index
    if(indexLow < 0 &&
       sum >= 0.01*numPixels)
    {
      indexLow = ((double)i)/255.0;
    }
    
    //if 99% not found yet and criteria met set index
    if(indexHigh < 0 &&
       sum >= 0.99*numPixels)
    {
      indexHigh = ((double)i)/255.0;
    }    
  }
  
  //only apply scaling if indexHigh > indexLow
  if(indexHigh > indexLow)
  {   
    //run through all image pixels a second time, this time scaling coordinates as necessary
    for( y=0; y<editedImage->height(); y++)
    {   
      //iterate over each selected pixel in scanline
      scanLine = editedImage->scanLine(y);
      for( x=0; x<editedImage->width(); x++)
      {
        //get color coordinates and convert to 0-1 scale
        rgb = ((QRgb*)scanLine+x);
        double r = ((double)qRed(*rgb)   )/255.0;
        double g = ((double)qGreen(*rgb) )/255.0;
        double b = ((double)qBlue(*rgb)  )/255.0;
        
        //convert to hsv
        double h,s,v;
        RGBtoHSV(r,g,b,&h,&s,&v);
        
        //scale and clamp v
        v = (v-indexLow)/(indexHigh-indexLow);
        
        //convert adjusted color back to rgb colorspace and clamp
        HSVtoRGB( &r,&g,&b, h,s,v);         
        int rp = (int) QMIN( QMAX((int)(r*255), 0), 255 );
        int gp = (int) QMIN( QMAX((int)(g*255), 0), 255 );
        int bp = (int) QMIN( QMAX((int)(b*255), 0), 255 );
        
        //set adjusted color value
        *rgb = qRgb(rp,gp,bp);

        //update status bar if significant progress has been made since last update
        if(status)
        {
          newProgress++;
          if(newProgress >= updateIncrement)
          {
            newProgress = 0;
            status->incrementProgress();
            qApp->processEvents();  
          }
        }
      
      } //for x
    } //for y
  } //if scaling should be preforemd
  
  //remove status bar
  if(status)
  {
    status->hideProgressBar();
    qApp->processEvents();
  }
}
//==============================================
