//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_ENHANCEMENTS_REDEYE_INTERNAL_H
#define BACKEND_ENHANCEMENTS_REDEYE_INTERNAL_H

#include <qimage.h>
#include <qpoint.h>
#include <Q3ValueStack>

//--------------------
//forward declarations
//--------------------
class StatusWidget;

//------  
//handle on status widget
StatusWidget* status;

//update increment variable used to determine when progress bar should be updated
int updateIncrement;

//progress tracker, used to determine when to update the progress bar
int newProgress;

//original image
QImage rawImage;

//modified image
QImage* editedImage;
//------  
//find region within image that blob pixels are within
void findRegionOfInterest(QPoint topLeftExtreme, QPoint bottomRightExtreme);

QPoint topLeft, bottomRight;
//------- 
//find all blobs, including their size (pixel count) and width/height aspect ratio
void findBlobs();

int regionWidth, regionHeight;  

int blobPixelCount;
QPoint blobTopLeft, blobBottomRight;

int* regionOfInterest;
void pushPixel(int x, int y, int id);
Q3ValueStack<QPoint> spreadablePixels;

Q3ValueStack<int> blobIDs;
Q3ValueStack<int> blobSizes;
Q3ValueStack<double> blobAspectRatios;  
//-------  
//sort blob list by decreasing size (pixel count)
void sortBlobsByDecreasingSize();

int blobCount;
int* ids;
int* sizes;
double* ratios;
//------- 
//find biggest two consecutive blobs with similar aspect ratios
void findBestTwoBlobs();

int id1, id2;
//-------   
//desaturate selected blobs
void desaturateBlobs();

//desaturate entire image - only used if no two good blobs found
void desaturateEntireImage(QPoint topLeftExtreme, QPoint bottomRightExtreme);

bool IDedPixel( int x, int y);
double desaturateAlpha(int x, int y);
//------

#endif //BACKEND_ENHANCEMENTS_REDEYE_INTERNAL_H
