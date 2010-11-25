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
#include <QImage>
#include <QString>
#include <QApplication>
#include <QProgressBar>
#include <math.h>

//Projectwide includes
#include "tilt.h"
#include "tilt_internal.h"

//----------------------------------------------
// Inputs:
// -------
// QString filename - location of original image on disk
// QPoint p1 and p2 - points along what should have been a vertical
//                    or horizontal edge
// StatusWidget* status - widget for making progress visible to user
//
// Outputs:
// --------
// QImage* returned - constructed image
//
// Description:
// ------------
// This method returned an image rotated as necessary to make points p1
// and p2 lie on what should have been a vertical or horizontal edge.
// Implicitly this task can be broken up into three steps:
//
// 1.) Determining the angle by which to rotate the image about it's
// center. We do this by computing angle the line supplied makes with the
// vertical and horizontal axis. We hypothesize that the smaller angle
// indicates the correct axis the user is indicating and hence the
// correct angle by which to rotate the image.
//
// 2.) Now that we have sidestepped having the user input the number
// of degrees to rotate the image by hand, we must use the computed angle
// to rotate the image. An intermediate image is constructed that is the
// same resolution as the original image. Each pixel is set by computed the
// un-rotated float coordinates and bilinearly interpolating the original
// pixel value. Regions that when un-rotated no-longer fall on the original
// image plane remain black and will be clipped during the next step.
//
// 3.) A side-effect of the rotation by a non-90 degree angle is that
// the corners remain undefined (pixels in the corners were not previously
// on the image plane and hence we have no data for them). A simplistic
// approach would be to paint these regions black and require the user to
// handle crop the image to cut these corners out.
//
// Frankly, I find taking such an approach unacceptable.
//
// Since understood behavior is for the image to rotate about it's
// center, and most likely for the aspect ratio to remain unchanged,
// is it necessary to compute the maximal bounding region centered
// about the intermediate image center that does not contain any
// undefined corner image pixels.
//
// This seemingly difficult problem can be reduced to 4 rotations
// and a number of line intersections! First, one must compute
// the rotated locations of the corners of the image. Next,
// lines made up by the rotated corners and the the new image
// corners are used to find the 8 intersection points (marked with *'s)
// as shown in the figure below:
//
//           b
//  A _____*/_\*___ B
//   |    /     \  |
//   |  /         \*
//   */            |\ d
// a/|             |/
//  \*             *
//   |*_________*/_|
//  C  \       /    D
//       \   /
//        \/
//        c
//
// By drawing lines from the center to each new image corner (A,B,C,D) and
// intersecting these lines with the lopped corners described by the newly
// found points (*'s), we can find four new points. It is a rather straight
// forward process determing the maximal height and weight of a bounding
// area centered about the image that will fall within these four points.
//
// At this point a true edited image is constructed and returned
// by copying pixel values withn the computed maximal bounding
// rectangle from the intermediate image previously constructed.
//----------------------------------------------

//==============================================
QImage* correctImageTilt(QString filename, QPoint p1, QPoint p2,
                         QProgressBar* _ProgressBar)
{
	QImage originalImage(filename);
	return correctImageTilt(originalImage, p1, p2, _ProgressBar);

}

QImage* correctImageTilt(QImage originalImage, QPoint p1, QPoint p2,
                         QProgressBar* _ProgressBar)
{
	//first compute distance between two points or "radius"
	int dx = p2.x() - p1.x();
	int dy = p2.y() - p1.y();

	//determine tilt angle
	int delta = 0;

	//compute recirpocal of distance between points
	double recip_r = 1.0 / sqrt((double)(dx * dx + dy * dy));

	//compute angle with horizontal axis
	if (QABS(dx) > QABS(dy))
	{
		delta = dy;
		if (dx > 0) delta = -delta;
	}
	//compute angle with vertical axis
	else
	{
		delta = dx;
		if (dy < 0) delta = -delta;
	}

	double sinTheta = (delta * recip_r);
	double theta = asin(sinTheta);
	double cosTheta = cos(theta);

	//if angle is 0 (improbable but possible) then quit now
	if (theta == 0)
		return NULL;


	//convert to 32-bit depth if necessary
	if (originalImage.depth() < 32)
	{
		originalImage = originalImage.convertDepth(32, Qt::AutoColor);
	}

	QImage rotatedImage(originalImage.width(), originalImage.height(), originalImage.depth());

	//setup progress bar
	//QString statusMessage = qApp->translate("correctImageTilt", "Correcting Tilt:");
	if (_ProgressBar)
	{
		_ProgressBar->setValue(0);
		qApp->processEvents();
	}
	//during the first phase update the status bar for every 1% of image pixels that are processed
	int updateIncrement = (int)(0.01 * originalImage.width() * originalImage.height());
	int newProgress = 0;

	//set each pixel to the rotated value
	double xp, yp;

	double w2 = 0.5 * rotatedImage.width();
	double h2 = 0.5 * rotatedImage.height();

	int x, y;
	uchar* scanLine;
	QRgb* rgb;
	for (y = 0; y < rotatedImage.height(); y++)
	{
		//iterate over each selected pixel in scanline
		scanLine = rotatedImage.scanLine(y);
		for (x = 0; x < rotatedImage.width(); x++)
		{
			//compute unrotated coordinates
			xp = cosTheta * (x - w2) + sinTheta * (y - h2) + w2;
			yp = -sinTheta * (x - w2) + cosTheta * (y - h2) + h2;

			//set unrotated value
			rgb = ((QRgb*)scanLine + x);
			*rgb = interpolatedPixelValue(xp, yp, &originalImage);

			//update status bar if significant progress has been made since last update
			newProgress++;
			if (newProgress >= updateIncrement && _ProgressBar)
			{
				newProgress = 0;
				_ProgressBar->setValue(_ProgressBar->value() + 1);
				qApp->processEvents();
			}

		}
	}

	//find rotated corners
	double nTheta = -theta;
	double sinNTheta = sin(nTheta);
	double cosNTheta = cos(nTheta);

	DPoint topLeft = DPoint(cosNTheta * (-w2) + sinNTheta * (-h2) + w2,
	                        -sinNTheta * (-w2) + cosNTheta * (-h2) + h2);

	DPoint topRight = DPoint(cosNTheta * (w2) + sinNTheta * (-h2) + w2,
	                         -sinNTheta * (w2) + cosNTheta * (-h2) + h2);

	DPoint bottomLeft = DPoint(cosNTheta * (-w2) + sinNTheta * (h2) + w2,
	                           -sinNTheta * (-w2) + cosNTheta * (h2) + h2);

	DPoint bottomRight = DPoint(cosNTheta * (w2) + sinNTheta * (h2) + w2,
	                            -sinNTheta * (w2) + cosNTheta * (h2) + h2);

	//determine which of these points are which in their rotated form
	DPoint top, bottom, left, right;
	if (theta < 0)
	{
		top = topRight;
		bottom = bottomLeft;
		left = topLeft;
		right = bottomRight;
	}
	else
	{
		top = topLeft;
		bottom = bottomRight;
		left = bottomLeft;
		right = topRight;
	}

	//construct true corners
	DPoint trueTopLeft(0, 0);
	DPoint trueTopRight(rotatedImage.width() - 1, 0);
	DPoint trueBottomLeft(0, rotatedImage.height() - 1);
	DPoint trueBottomRight(rotatedImage.width() - 1, rotatedImage.height() - 1);

	//find intersections with image boundary
	DPoint topEdgeL = findTwoLineIntersection(left, top, trueTopLeft, trueTopRight);
	DPoint topEdgeR = findTwoLineIntersection(top, right, trueTopLeft, trueTopRight);

	DPoint bottomEdgeL = findTwoLineIntersection(left, bottom, trueBottomLeft, trueBottomRight);
	DPoint bottomEdgeR = findTwoLineIntersection(bottom, right, trueBottomLeft, trueBottomRight);

	DPoint leftEdgeT = findTwoLineIntersection(left, top, trueTopLeft, trueBottomLeft);
	DPoint leftEdgeB = findTwoLineIntersection(left, bottom, trueTopLeft, trueBottomLeft);

	DPoint rightEdgeT = findTwoLineIntersection(right, top, trueTopRight, trueBottomRight);
	DPoint rightEdgeB = findTwoLineIntersection(right, bottom, trueTopRight, trueBottomRight);

	//shot rays out from image center to each true corner and find intersections with clipped corners
	DPoint center((int)w2, (int)h2);
	DPoint safeTopLeft     = findTwoLineIntersection(center, trueTopLeft, leftEdgeT, topEdgeL);
	DPoint safeTopRight    = findTwoLineIntersection(center, trueTopRight, rightEdgeT, topEdgeR);
	DPoint safeBottomLeft  = findTwoLineIntersection(center, trueBottomLeft, leftEdgeB, bottomEdgeL);
	DPoint safeBottomRight = findTwoLineIntersection(center, trueBottomRight, rightEdgeB, bottomEdgeR);

	//find constrained area
	double minY = QMAX(safeTopLeft.y(), safeTopRight.y());
	double maxY = QMIN(safeBottomLeft.y(), safeBottomRight.y());

	double minX = QMAX(safeTopLeft.x(), safeBottomLeft.x());
	double maxX = QMIN(safeTopRight.x(), safeBottomRight.x());

	//find contrained area in integer coordinates. this is semi-tricky.
	//if the minimum values decimal porition is nonzero then increment by one
	// (eg 5.37 -> 6)
	int xMin = (int) minX;
	int xMax = (int) maxX;

	int yMin = (int) minY;
	int yMax = (int) maxY;

	if (xMin < minX) xMin++;
	if (yMin < minY) yMin++;

	//construct cropped rotated image
	QImage* editedImage = new QImage(xMax - xMin + 1,
	                                 yMax - yMin + 1,
	                                 rotatedImage.depth());

	//during the second phase update the status bar for every 1% of cropped pixels that are procesed
	updateIncrement = (int)(0.01 * editedImage->width() * editedImage->height());
	newProgress = 0;

	int x2, y2;
	uchar* scanLine2;
	QRgb* rgb2;

	y2 = 0;
	for (y = yMin; y <= yMax; y++, y2++)
	{
		//iterate over each selected pixel in scanline
		scanLine = rotatedImage.scanLine(y);
		scanLine2 = editedImage->scanLine(y2);

		x2 = 0;
		for (x = xMin; x <= xMax; x++, x2++)
		{
			rgb  = ((QRgb*)scanLine + x);
			rgb2 = ((QRgb*)scanLine2 + x2);
			*rgb2 = *rgb;

			//update status bar if significant progress has been made since last update
			newProgress++;
			if (newProgress >= updateIncrement && _ProgressBar)
			{
				newProgress = 0;
				_ProgressBar->setValue(_ProgressBar->value() + 1);
				qApp->processEvents();
			}
		}
	}

	//return pointer to edited image
	return editedImage;
}
//==============================================
QRgb interpolatedPixelValue(double xp, double yp,
                            QImage* image)
{
	//do boundary checking to
	//ensure we don't read beyond image boundaries
	if (xp < 0 || xp >= image->width() ||
	      yp < 0 || yp >= image->height())
		return qRgb(0, 0, 0);

	//get four pixel colors,
	int x = (int)xp;
	int y = (int)yp;

	uchar* scanLine1 = image->scanLine(y);

	uchar* scanLine2;
	if (y < image->height() - 1)
		scanLine2 = image->scanLine(y + 1);
	else
		scanLine2 = scanLine1;

	QRgb p1, p2, p3, p4;

	p1 = *((QRgb*)scanLine1 + x);
	p3 = *((QRgb*)scanLine2 + x);

	if (x < image->width() - 1)
	{
		p2 = *((QRgb*)scanLine1 + x + 1);
		p4 = *((QRgb*)scanLine2 + x + 1);
	}
	else
	{
		p2 = p1;
		p4 = p3;
	}

	//blend four colors
	double alphaY = yp - y;
	double alphaX = xp - x;

	p1 = blendColors(p1, p2, alphaX);
	p3 = blendColors(p3, p4, alphaX);
	p1 = blendColors(p1, p3, alphaY);
	return p1;
}
//==============================================
QRgb blendColors(QRgb color1, QRgb color2, double alpha)
{
	double alpha2 = 1.0 - alpha;
	return qRgb((int) QMAX(QMIN(255, static_cast<int>(alpha2*qRed(color1) + alpha*qRed(color2))), 0),
	            (int) QMAX(QMIN(255, static_cast<int>(alpha2*qGreen(color1) + alpha*qGreen(color2))), 0),
	            (int) QMAX(QMIN(255, static_cast<int>(alpha2*qBlue(color1) + alpha*qBlue(color2))), 0));
}
//==============================================
DPoint findTwoLineIntersection(DPoint p1, DPoint p2,
                               DPoint p3, DPoint p4)
{
	//----------------------------------------------
	//=== Case 1: neither line has a change in X ===
	//----------------------------------------------
	//If there is no change in x for both lines,
	//either lines will NEVER or ALWAYS intersect.
	if (p1.x() == p2.x() &&
	      p4.x() == p3.x())
	{
		//Ok, if their x values are equal, return
		//intersection point as line A's point A.
		//Yes, this is a little arbitratry. But
		//theoreticaly this section of code will almost
		//never be executed.
		if (p1.x() == p3.x())
		{
			return DPoint(p1.x(), p1.y());
		}
		//Else lines will never intersect,
		//return pair (-32000,-32000)
		else
		{
			return DPoint(-32000, -32000);
		}
	}
	//----------------------------------------------
	//Else, we know at least one of the lines
	//does NOT have a slope of infinity!!!
	//----------------------------------------------

	//----------------------------------------------
	//=== Case 2: line A has no change in X      ===
	//----------------------------------------------
	//If line A has an infinite slope (no change in x)
	//we know line B does not have an infinite slope...
	else if (p1.x() == p2.x())
	{
		double slopeB = ((double)(p4.y() - p3.y())) / (p4.x() - p3.x());

		double yInterceptB = p3.y() - slopeB * p3.x();

		//y = mx+b
		return DPoint(p2.x(), slopeB*p2.x() + yInterceptB);
	}
	//----------------------------------------------
	//=== Case 3: line B has no change in X      ===
	//----------------------------------------------
	//If line B has an infinite slope (no change in x)
	//we know line A does not have an infinite slope...
	else if (p4.x() == p3.x())
	{
		double slopeA = ((double)(p2.y() - p1.y())) / (p2.x() - p1.x());

		double yInterceptA = p1.y() - slopeA * p1.x();

		//y = mx+b
		return DPoint(p4.x(), slopeA*p4.x() + yInterceptA);
	}
	//----------------------------------------------
	//=== Case 4: both lines have non infinite slopes ===
	//----------------------------------------------
	else
	{
		double slopeA = ((double)(p2.y() - p1.y())) / (p2.x() - p1.x());
		double slopeB = ((double)(p4.y() - p3.y())) / (p4.x() - p3.x());
		double yInterceptA = p1.y() - slopeA * p1.x();
		double yInterceptB = p3.y() - slopeB * p3.x();

		//y1 = mx1+b
		//y2 = nx2+c
		//at intersection y1=y2 and x1 = x2 so...
		//mx +b = nx + c
		//x(m-n) = c-b
		//x = (c-b)/(m-n)
		//where m and n are slope and
		//b and c are y-intercepts.
		//x = (c-b)/(m-n)
		double x = (yInterceptB - yInterceptA) / (slopeA - slopeB);
		return DPoint(x, (slopeA * x) + yInterceptA);
	}
}
//==============================================
DPoint::DPoint()
{
	xpos = 0; ypos = 0;
}
//==============================================
DPoint::DPoint(double x, double y)
{
	this->xpos = x;
	this->ypos = y;
}
//==============================================
double DPoint::x() const
{
	return xpos;
}
double DPoint::y() const
{
	return ypos;
}
//==============================================


