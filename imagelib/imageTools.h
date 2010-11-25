//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_TOOLS_IMAGETOOLS_H
#define BACKEND_TOOLS_IMAGETOOLS_H
#include "stimagelibexport.h"

//--------------------
//forward declarations
//--------------------
class QString;
class QImage;
class QPoint;
class QSize;
#include <qcolor.h>

//Transform Codes
typedef enum 
{
  ROTATE_90,    //rotate clockwise 90 degrees
  ROTATE_270,  //rotate counter-clockwise 90 degrees
  FLIP_H,              //flip left-to-right
  FLIP_V,              //flip top-to-bottom
} TRANSFORM_CODE;

///Checks to see if an image is a valid jpg by seeing if the image dimensions can be read
bool ST_IMAGELIB_EXPORT isJpeg(const char* filename);

///Computes scale of image dimensions while respecting aspect ratio,
///equivalent to a QImage::scaleMin without actually scaling any image
void ST_IMAGELIB_EXPORT calcScaledImageDimensions(int origWidth, int origHeight,
                               int idealWidth, int idealHeight,
                               int& width, int& height);

///Constructs slideshow and thumbnail images for a full sized image.
void ST_IMAGELIB_EXPORT constructImages(QString imageName, 
                     QImage& slideshowImage, QImage& thumbnailImage);
                  
///Apply image transformation on image                  
bool ST_IMAGELIB_EXPORT transformImage( QString fileIn, QString fileOut, TRANSFORM_CODE transformation );
bool ST_IMAGELIB_EXPORT transformImage( QImage& origImage, QImage& transformedImage, TRANSFORM_CODE transformation );

///Scale image and save copy to disk
bool ST_IMAGELIB_EXPORT scaleImage( QString fileIn, QString fileOut, 
                 int newWidth, int newHeight, int _Quality = 95);

///Loaded scaled version of image                 
bool ST_IMAGELIB_EXPORT scaleImage(QString fileIn, QImage& scaledImage, int targetWidth, int targetHeight);

///Get image dimensions
bool ST_IMAGELIB_EXPORT getImageSize( const char* filename,QSize& size );

///Get image dimensions
bool ST_IMAGELIB_EXPORT getImageSize( const char* filename, int& width, int& height );

///find luminance of a rgb color triplet
double ST_IMAGELIB_EXPORT RGBtoL(QRgb* rgb);

///Convert a RGB color triplet to HSV
void ST_IMAGELIB_EXPORT RGBtoHSV( double r, double g, double b, 
               double *h, double *s, double *v );

///Convert a HSV color triplet to RGB
void ST_IMAGELIB_EXPORT HSVtoRGB( double *r, double *g, double *b, 
               double h, double s, double v );

//! Converts mm to Inches.
double ST_IMAGELIB_EXPORT mmToInch(double _Mm);

double ST_IMAGELIB_EXPORT inchTomm(double _Inch);

//! \return the number of pixels for a _Dpis resolution amb _Mm milimetres.
int ST_IMAGELIB_EXPORT mmToPixels(double _Mm, int _Dpis);

double ST_IMAGELIB_EXPORT pixelsToInch(int _Pixels, int _Dpis);

double ST_IMAGELIB_EXPORT pixelsTomm(int _Pixels, int _Dpis);


#endif //BACKEND_TOOLS_IMAGETOOLS_H
