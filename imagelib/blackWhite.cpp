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
#include "blackWhite.h"
#include "manipulationOptions.h"
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
// This method constructs a grayscale version of
// the image by computing a gray value for each pixel. A naive
// approach would use the average the red, green, and blue components
// at each pixel for the new pixel component values, that is:
//
// gray = (r+g+b) / 3;
// r' = gray; g' = gray; b'= gray
//
// The problem with this approach is that image luminance
// (observed brightness) is not maintained. Instead, we conpute
// the brightness at each pixel using a weighted sum of the color
// components. Weights vary depending on display device.
// Old school NTSC TV's which came out in the 50's would
// tell us to use these r,b,g weights (which Gimp uses):
// (see http://www.gimp.org/tutorials/Color2BW)
// (0.3, 0.59, 0.11)
//
// But these days we're using modern monitors and the green phosphur
// is brigther or something so it's best to use these instead:
// http://lists.trolltech.com/qt-interest/1999-10/thread00337-0.html
// (0.2125, 0.7154, 0.0721)
//
// After weighting, the resulting gray value is clamped to fall within the
// [0-255] range and assigned to the pixel in the new image.
//----------------------------------------------

//====================================================

QImage* blackWhiteEffect ( QString filename, ManipulationOptions* options )
{
	QImage* editedImage = new QImage ( filename );
	blackWhiteEffect ( *editedImage, options );
	return editedImage;
}
//====================================================
void blackWhiteEffect ( QImage &editedImage, ManipulationOptions* options )
{
	//convert to 32-bit depth if necessary
	if ( editedImage.depth() < 32 )
		{ editedImage = editedImage.convertDepth ( 32, Qt::AutoColor ); }

	//determine if busy indicators will be used
	bool useBusyIndicators = false;
	SProcessStatusWidget* status = NULL;
	if ( options != NULL && options->getStatus() != NULL )
	{
		useBusyIndicators = true;
		status = options->getStatus();
	}

	//setup progress bar
	if ( useBusyIndicators )
	{
		QString statusMessage = qApp->translate ( "blackWhiteEffect", "Applying Black + White Effect:" );
		if ( status )
		{
			status->showProgressBar ( statusMessage, 100 );
			qApp->processEvents();
		}
	}

	//update progress bar for every 1% of completion
	const int updateIncrement = ( int ) ( 0.01 * editedImage.width() * editedImage.height() );
	int newProgress = 0;

	//iterate over each selected scanline
	int x, y, grayValue;
	QRgb* rgb;
	uchar* scanLine;
	for ( y=0; y<editedImage.height(); y++ )
	{
		//iterate over each selected pixel in scanline
		scanLine = editedImage.scanLine ( y );
		for ( x=0; x<editedImage.width(); x++ )
		{
			//compute gray value based on the display luminance of color coordinates
			rgb = ( ( QRgb* ) scanLine+x );
			grayValue = ( int ) ( 0.2125*qRed ( *rgb ) + 0.7154*qGreen ( *rgb ) + 0.0721*qBlue ( *rgb ) );

			//clamp to ensure it falls in the 0-255 range
			grayValue = QMIN ( QMAX ( grayValue, 0 ), 255 );

			//set pixel channel values using computed gray value
			*rgb = qRgb ( grayValue, grayValue, grayValue );

			//update status bar if significant progress has been made since last update
			if ( useBusyIndicators )
			{
				newProgress++;
				if ( newProgress >= updateIncrement )
				{
					newProgress = 0;
					if ( status )
					{
						status->incrementProgress();
						qApp->processEvents();
					}
				}
			}

		}
	}
}
//==============================================
