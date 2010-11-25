//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_ENHANCEMENTS_REDEYE_H
#define BACKEND_ENHANCEMENTS_REDEYE_H

#include <QStack>
#include "stimagelibexport.h"

//--------------------
//forward declarations
//--------------------
class QImage;
class QString;
class QPoint;
class SProcessStatusWidget;

class ST_IMAGELIB_EXPORT RedEye
{
	//------  
	//handle on status widget
	SProcessStatusWidget* status;
	
	//update increment variable used to determine when progress bar should be updated
	int updateIncrement;
	
	//progress tracker, used to determine when to update the progress bar
	int newProgress;
	
	//original image
	QImage rawImage;
	
	//modified image
	QImage* editedImage;
	
	QPoint topLeft, bottomRight;
	
	int regionWidth, regionHeight;  
	
	int blobPixelCount;
	QPoint blobTopLeft, blobBottomRight;
	
	int* regionOfInterest;
	void pushPixel(int x, int y, int id);
	QStack<QPoint> spreadablePixels;
	
	QStack<int> blobIDs;
	QStack<int> blobSizes;
	QStack<double> blobAspectRatios;  
	
	int blobCount;
	int* ids;
	int* sizes;
	double* ratios;
	
	int id1, id2;
	
private:
	//find region within image that blob pixels are within
	void findRegionOfInterest(QPoint topLeftExtreme, QPoint bottomRightExtreme);
	
	//find all blobs, including their size (pixel count) and width/height aspect ratio
	void findBlobs();
	
	//sort blob list by decreasing size (pixel count)
	void sortBlobsByDecreasingSize();
	
	//find biggest two consecutive blobs with similar aspect ratios
	void findBestTwoBlobs();
	
	//desaturate selected blobs
	void desaturateBlobs();
	
	//desaturate entire image - only used if no two good blobs found
	void desaturateEntireImage(QPoint topLeftExtreme, QPoint bottomRightExtreme);
	
	bool IDedPixel( int x, int y);
	double desaturateAlpha(int x, int y);
	//------

public:	
	//Remove redeye in image
	QImage* removeRedeyeRegions( QString filename, 
															QPoint topLeftExtreme, QPoint bottomRightExtreme, 
															SProcessStatusWidget* status );
															
	QImage* removeRedeyeRegions( QImage* img, 
                             QPoint topLeftExtreme, QPoint bottomRightExtreme,
                             SProcessStatusWidget* SProcessStatusWidget );
};

#endif //BACKEND_ENHANCEMENTS_REDEYE_H
