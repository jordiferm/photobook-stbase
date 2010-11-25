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

//Projectwide includes
#include "crop.h"

//----------------------------------------------
// Inputs:
// -------
// QString filename   - location of original image on disk
// QPoint topLeft     - top left of selected region
// QPoint bottomRight - bottom right of selected region
//
// Outputs:
// --------
// QImage* returned - constructed image
//
// Description:
// ------------
// This method constructs a cropped form of the original image
// within the selected region by simply copying over pixel values.
//----------------------------------------------

QImage* cropImage( QImage& origImage, QPoint topLeft, QPoint bottomRight )
{
	//convert to 32-bit depth if necessary
  if( origImage.depth() < 32 ) { origImage = origImage.convertDepth( 32, Qt::AutoColor ); }
  
  //construct cropped image
  QImage* croppedImage = new QImage(bottomRight.x() - topLeft.x() + 1, 
                                    bottomRight.y() - topLeft.y() + 1,
                                    origImage.depth());  
  
  //iterate over each selected scanline 
  int xOrig, yOrig;
  int xCropped, yCropped;
  uchar *origScanLine, *croppedScanLine;
  
  for( yOrig=topLeft.y(),yCropped=0;    yOrig<=bottomRight.y();     yOrig++, yCropped++)
  {   
    //iterate over each selected pixel in scanline
    origScanLine    = origImage.scanLine(yOrig);
    croppedScanLine = croppedImage->scanLine(yCropped);
    
    for( xOrig=topLeft.x(),xCropped=0; xOrig<=bottomRight.x(); xOrig++,xCropped++)
    {
      //copy pixel color from original image to cropped image
      *((QRgb*)croppedScanLine+xCropped) = *((QRgb*)origScanLine+xOrig);      
    }
  }
  
  //return pointer to cropped image
  return croppedImage;  
}


//==============================================
QImage* cropImage( QString filename, QPoint topLeft, QPoint bottomRight )
{
  //load original image
  QImage origImage( filename );
  return cropImage( origImage, topLeft, bottomRight );
}
//==============================================
