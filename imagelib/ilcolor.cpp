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
#include "ilcolor.h"
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
// This method simply stretchs the color values to use up the entire dynamic range, in our
// case red/green/blue values ranging from [0,255].
//
// The algorithm can be broken up into two steps:
// 1.) finding the endpoints of the curretn color values (e.g. [6,120])
// 2.) stretching the current image colors to fit the full [0,255] range
//
// One could simply iterate over the entire image and find the minimum and
// maximum red, gree, and blue color values. However; often there is residual
// noise at the endpoints since camera sensors always provide some noise,
// while the majority of pixels fall somewhere in the middle. If we constructed
// a histogram for a particular color channel it might look like this:
//  5 |                      .
//  . |                     . .
//  . |                    .   .
//  1 | .    .          .        .              .
//  0 |. .... ........             ............. .....
//    |-X------------*--------------*-----------X-----
//     0            100             175            255
//
//
// Here color values are plotted across the x axis, while the y axis indicates
// how offten a particular color value was seen. Note we're plotting one of the
// three color channels here (red, green, blue).
//
// In this example we notice most pixels that were observed fall in the 100-175
// range (that is the red/green/blue color value was most often observed with a
// value between 100-175). A few pixels with values of 5 or 10 were seen, and a
// few with around 230 were seen. We want to stretch the common case, 100-175,
// out to the full 0-255 range. That will make the pixels that were 175 now 255,
// those that wer 100 now 0, thus darks will become darker, brights will become
// brigther. We'll lineraly scale all pixels inbetween. Any pixels beyond the
// 100-175 range will get clamped at 0 and 255 respectively.
//
// Now, if we simply find the smallest and largest values we'll get soemthinglike
// 5 and 235, and stretching will have very little effect on the image. Instead,
// we first actually populate a histogram array for each color channel (an int
// array of 256 elements). We then determine the 1% and 99% boundaries but walking
// from the end points of hte histogram array and adding up the elements. Once 1%
// of the pixels have been accounted for we set the the boundary endpoint to the index,
// thus giving us 100,175 in this case.
//
// Actually scaling the colors is relatively trivial. A second pass over the
// image takes place during which a pixels color is fetched, modified, then written
// back out according to the following equations. It should be noted that as a
// sanity check we clamp scaled color values to the [0,255] range.
//
// r' = [255*(r-rLow)] / [rHigh - rLow]
// g' = [255*(g-gLow)] / [gHigh - gLow]
// b' = [255*(b-bLow)] / [bHigh - bLow]
//
// where [r/g/b][Low/High] refer to the 1% and 99% endpoints for the red,
// green, and blue color changes respectively.
//
//----------------------------------------------

QImage* improveColorBalance(QImage* editedImage, SProcessStatusWidget* status)
{
	//convert to 32-bit depth if necessary
	if (editedImage->depth() < 32)
	{
		QImage* tmp = editedImage;
		editedImage = new QImage(tmp->convertDepth(32, Qt::AutoColor));
		delete tmp;
		tmp = NULL;
	}

	//setup progress bar
	QString statusMessage = qApp->translate("improveColorBalance", "Enhancing Color Balance:");
	if (status)
	{
		status->showProgressBar(statusMessage, 100);
		qApp->processEvents();
	}

	//update progress bar for every 1% of completion
	const int updateIncrement = (int)(0.01 * editedImage->width() * editedImage->height());
	int newProgress = 0;

	//construct intensity histographs for each color channel
	int redVals[256];
	int greenVals[256];
	int blueVals[256];
	int i = 0;
	for (i = 0; i < 256; i++)
	{
		redVals[i] = 0;
		greenVals[i] = 0;
		blueVals[i] = 0;
	}

	//populate histogram by iterating over all image pixels
	int numPixels = editedImage->width() * editedImage->height();
	QRgb* rgb;
	uchar* scanLine;
	int x, y;
	for (y = 0; y < editedImage->height(); y++)
	{
		//iterate over each selected pixel in scanline
		scanLine = editedImage->scanLine(y);
		for (x = 0; x < editedImage->width(); x++)
		{
			rgb = ((QRgb*)scanLine + x);
			redVals[qRed(*rgb)]++;
			greenVals[qGreen(*rgb)]++;
			blueVals[qBlue(*rgb)]++;
		} //for x
	} //for y

	//find 1% and 99% precenticles
	//we'll stretch these values so we avoid outliers from affecting the stretch
	int sumR = 0;
	int sumG = 0;
	int sumB = 0;
	int indexLowR, indexHighR;
	int indexLowG, indexHighG;
	int indexLowB, indexHighB;
	indexLowR = -1;
	indexHighR = -1;
	indexLowG = -1;
	indexHighG = -1;
	indexLowB = -1;
	indexHighB = -1;
	for (i = 0; i < 256; i++)
	{
		sumR += redVals[i];
		sumG += greenVals[i];
		sumB += blueVals[i];

		//if 1% not found yet and criteria met set index
		if (indexLowR < 0 && sumR >= 0.01*numPixels)
		{
			indexLowR = i;
		}
		if (indexLowG < 0 && sumG >= 0.01*numPixels)
		{
			indexLowG = i;
		}
		if (indexLowB < 0 && sumB >= 0.01*numPixels)
		{
			indexLowB = i;
		}

		//if 99% not found yet and criteria met set index
		if (indexHighR < 0 && sumR >= 0.99*numPixels)
		{
			indexHighR = i;
		}
		if (indexHighG < 0 && sumG >= 0.99*numPixels)
		{
			indexHighG = i;
		}
		if (indexHighB < 0 && sumB >= 0.99*numPixels)
		{
			indexHighB = i;
		}
	}

	//run through all image pixels a second time, this time scaling coordinates as necessary
	for (y = 0; y < editedImage->height(); y++)
	{
		//iterate over each selected pixel in scanline
		scanLine = editedImage->scanLine(y);
		for (x = 0; x < editedImage->width(); x++)
		{
			//get color coordinates and convert to 0-1 scale
			rgb = ((QRgb*)scanLine + x);
			double r = ((double)qRed(*rgb));
			double g = ((double)qGreen(*rgb));
			double b = ((double)qBlue(*rgb));

			if (indexHighR != indexLowR)
			{
				r = (255 * (r - indexLowR)) / (indexHighR - indexLowR);
			}
			if (indexHighG != indexLowG)
			{
				g = (255 * (g - indexLowG)) / (indexHighG - indexLowG);
			}
			if (indexHighB != indexLowB)
			{
				b = (255 * (b - indexLowB)) / (indexHighB - indexLowB);
			}

			int rp = (int) QMIN(QMAX((int)r, 0), 255);
			int gp = (int) QMIN(QMAX((int)g, 0), 255);
			int bp = (int) QMIN(QMAX((int)b, 0), 255);

			//set adjusted color value
			*rgb = qRgb(rp, gp, bp);

			//update status bar if significant progress has been made since last update
			newProgress++;
			if (newProgress >= updateIncrement)
			{
				newProgress = 0;
				if (status)
				{
					status->incrementProgress();
					qApp->processEvents();
				}
			}

		} //for x
	} //for y

	//remove status bar
	if (status)
	{
		status->hideProgressBar();
		qApp->processEvents();
	}	
	//return pointer to edited image
	return editedImage;
}

//==============================================
QImage* improveColorBalance(QString filename, SProcessStatusWidget* status)
{
	//load original image
	QImage* editedImage = new QImage(filename);
	return improveColorBalance(editedImage, status);
}
//==============================================
