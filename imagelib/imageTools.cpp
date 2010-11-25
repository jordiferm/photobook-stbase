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
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <cmath>

#include <qstring.h>
#include <qimage.h>
#include <qdir.h>
#include <qfile.h>
#include <qcolor.h>
#include <fstream>
#include <qpoint.h>
#include <qpainter.h>

#include "sttemporaryimagefile.h"

//Projectwide includes
#include "imageTools.h"
#include "jpeg/jpegTools.h"
#include "jpeg/jpegSize.h"
#define THUMBNAIL_WIDTH 300
#define THUMBNAIL_HEIGHT 300
#define SLIDESHOW_WIDTH 300
#define SLIDESHOW_HEIGHT 300

//==============================================
bool isJpeg(const char* filename)
{
	int w, h;
	return getJPEGSize(QFile::encodeName(filename), w, h);
}
//==============================================
void calcScaledImageDimensions(int origWidth, int origHeight,
                               int idealWidth, int idealHeight,
                               int& width, int& height)
{
	//if original dimensions are within ideal new size then use
	//original dimensions
	if (origWidth <= idealWidth &&
	        origHeight <= idealHeight)
	{
		width = origWidth;
		height = origHeight;
		return;
	}

	//else find dimension which is way over bounds
	float widthRatio = ((float)idealWidth) / ((float)origWidth);
	float heightRatio = ((float)idealHeight) / ((float)origHeight);

	if (widthRatio < heightRatio)
	{
		width = idealWidth;
		height = (int)((((float)idealWidth) / ((float)origWidth)) * ((float)origHeight));
	}
	else
	{
		height = idealHeight;
		width = (int)((((float)idealHeight) / ((float)origHeight)) * ((float)origWidth));
	}
}
//==============================================
void constructImages(QString imageName,
                     QImage& slideshowImage, QImage& thumbnailImage)
{
	//---------------------------------------------------------
	//obtain original image width and height
	int origWidth, origHeight;
	getImageSize(imageName, origWidth, origHeight);

	//compute dimensions of unhapped scaled thumbnail and slideshow images
	int thumbWidth, thumbHeight;
	calcScaledImageDimensions(origWidth, origHeight,
	                          THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT,
	                          thumbWidth, thumbHeight);

	int slideWidth, slideHeight;
	calcScaledImageDimensions(origWidth, origHeight,
	                          SLIDESHOW_WIDTH, SLIDESHOW_HEIGHT,
	                          slideWidth, slideHeight);
	//---------------------------------------------------------
	//create slide show image

	//first scale full image to unpadded dimensions
	QImage temp;
	scaleImage(imageName, temp, slideWidth, slideHeight);
	slideWidth = temp.width();
	slideHeight = temp.height();

	//create slideshow image and fill with white
	slideshowImage.create(SLIDESHOW_WIDTH, SLIDESHOW_HEIGHT, 32);
	slideshowImage.fill(QColor("white").rgb());

	//paint unpadded scaled image
	int xDiff = SLIDESHOW_WIDTH - slideWidth;
	int yDiff = SLIDESHOW_HEIGHT - slideHeight;
	bitBlt(&slideshowImage, xDiff / 2, yDiff / 2,
	       &temp, 0, 0, slideWidth, slideHeight);

	//---------------------------------------------------------
	//create thumnail image
	scaleImage(imageName, thumbnailImage, thumbWidth, thumbHeight);
	//---------------------------------------------------------
}
//==============================================
bool transformImage(QString fileIn, QString fileOut, TRANSFORM_CODE transformation)
{
	//if file is jpeg use faster method
	if (isJpeg(fileIn))
	{
		if (fileIn == fileOut)
		{
			bool Res;
			STTemporaryImageFile TmpImageFile(fileOut, "st_imagetools_temp");
			QString TmpFileOut = TmpImageFile.fileName();
			qDebug(TmpFileOut.toLatin1());
			Res = transformJPEG(fileIn, TmpFileOut, transformation);
			QImage transformedImage(TmpFileOut);
			Res = Res && transformedImage.save(fileOut); 
			return Res; 
		}
		else
			return transformJPEG(fileIn, fileOut, transformation);
	}
	//load image
	QImage origImage(fileIn);
	QImage transformedImage;

	//transform image
	if (transformation == ROTATE_90)
	{
		if (!transformedImage.create(origImage.height(), origImage.width(), origImage.depth()))
			return false;

		int x, y;
		for (x = 0; x < origImage.height(); x++)
		{
			for (y = 0; y < origImage.width(); y++)
				transformedImage.setPixel(origImage.height() - 1 - x, y, origImage.pixel(y, x));
		}
	}
	else if (transformation == ROTATE_270)
	{
		if (!transformedImage.create(origImage.height(), origImage.width(), origImage.depth()))
			return false;

		int x, y;
		for (x = 0; x < origImage.height(); x++)
		{
			for (y = 0; y < origImage.width(); y++)
				transformedImage.setPixel(x, origImage.width() - 1 - y, origImage.pixel(y, x));
		}
	}
	else if (transformation == FLIP_H)
		{  transformedImage = origImage.mirrored(false, true); }
	else
		{  transformedImage = origImage.mirrored(true, false); }

	//save out transformed image
	//transformedImage.save(fileOut, "JPEG", 95);
	transformedImage.save(fileOut); 
	return true;
}

bool transformImage(QImage& origImage, QImage& transformedImage, TRANSFORM_CODE transformation)
{
	//transform image
	if (transformation == ROTATE_90)
	{
		if (!transformedImage.create(origImage.height(), origImage.width(), origImage.depth()))
			return false;

		int x, y;
		for (x = 0; x < origImage.height(); x++)
		{
			for (y = 0; y < origImage.width(); y++)
				transformedImage.setPixel(origImage.height() - 1 - x, y, origImage.pixel(y, x));
		}
	}
	else if (transformation == ROTATE_270)
	{
		if (!transformedImage.create(origImage.height(), origImage.width(), origImage.depth()))
			return false;

		int x, y;
		for (x = 0; x < origImage.height(); x++)
		{
			for (y = 0; y < origImage.width(); y++)
				transformedImage.setPixel(x, origImage.width() - 1 - y, origImage.pixel(y, x));
		}
	}
	else if (transformation == FLIP_H)
		{  transformedImage = origImage.mirrored(false, true); }
	else
		{  transformedImage = origImage.mirrored(true, false); }

	return true;
}


//==============================================
bool scaleImage(QString fileIn, QString fileOut,
                int newWidth, int newHeight, int _Quality)
{
	//scale image
	QImage scaledImage;
	if (scaleImage(fileIn, scaledImage, newWidth, newHeight))
	{
		return scaledImage.save(fileOut, "JPEG", _Quality);
	}
	else
		return false;
}
//==============================================
bool scaleImage(QString fileIn, QImage& scaledImage, int targetWidth, int targetHeight)
{
	//if file is jpeg use faster method
	QString extension = QFileInfo(fileIn).extension(false).lower();
	if (extension.compare("jpeg") == 0 || extension.compare("jpg") == 0)
	{
		if (scaleJPEG(QFile::encodeName(fileIn), scaledImage, targetWidth, targetHeight))
			return true;
	}
	//use slow smooth-scale method for scaling image.
	//clamp scaling to <= 2x
	QImage orig(fileIn);
	if (QMIN(((float)targetWidth) / orig.width(), ((float)targetHeight) / orig.height()) > 2)
	{
		targetWidth = 2 * orig.width();
		targetHeight = 2 * orig.height();
	}

	scaledImage = orig.scaled(QSize(targetWidth, targetHeight), Qt::KeepAspectRatio);
	return true;
}
//==============================================
bool getImageSize(const char* filename, QSize& size)
{
	int w, h;
	bool result = getImageSize(filename, w, h);
	size.setWidth(w);
	size.setHeight(h);
	return result;
}
//==============================================
bool getImageSize(const char* filename, int& width, int& height)
{
	//if file is jpeg use faster method
	QString extension = QFileInfo(filename).extension(false).lower();
	if (extension.compare("jpeg") == 0 ||
	        extension.compare("jpg") == 0)
		return getJPEGSize(QFile::encodeName(filename),
		                   width, height);

	//load entire image to qimage object in order to determine size
	QImage image(filename);
	width = image.width();
	height = image.height();
	return true;
}
//==============================================
double RGBtoL(QRgb* rgb)
{
	double r = ((double)qRed(*rgb)) / 255.0;
	double g = ((double)qGreen(*rgb)) / 255.0;
	double b = ((double)qBlue(*rgb)) / 255.0;

	double h, s, v;
	RGBtoHSV(r, g, b, &h, &s, &v);
	return 255.0*v;
}
//==============================================
//RGB to HSV and HSB to RGB algorithms based
//on by A. R. Smith in 1978, found at
//http://www.cs.rit.edu/~ncs/color/t_convert.html
void RGBtoHSV(double r, double g, double b,
              double *h, double *s, double *v)
{
	double min, max, delta;

	min = QMIN(QMIN(r, g), b);
	max = QMAX(QMAX(r, g), b);
	*v = max;				// v

	delta = max - min;

	if (max != 0)
		*s = delta / max;		// s
	else
	{
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if (r == max)
		*h = (g - b) / delta;		// between yellow & magenta
	else if (g == max)
		*h = 2 + (b - r) / delta;	// between cyan & yellow
	else
		*h = 4 + (r - g) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if (*h < 0)
		*h += 360;

}
//==============================================
void HSVtoRGB(double *r, double *g, double *b,
              double h, double s, double v)
{
	int i;
	double f, p, q, t;

	if (s == 0)
	{
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = (int)floor(h);
	f = h - i;			// factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));

	switch (i)
	{
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

double mmToInch(double _Mm)
{
	return _Mm * 0.03937;
}

double inchTomm(double _Inch)
{
	return _Inch / 0.03937;
}

int mmToPixels(double _Mm, int _Dpis)
{
	return mmToInch(_Mm) * _Dpis;
}

double pixelsToInch(int _Pixels, int _Dpis)
{
	return _Pixels / _Dpis; 
}

double pixelsTomm(int _Pixels, int _Dpis)
{
	return inchTomm(pixelsToInch(_Pixels, _Dpis)); 
}

//==============================================
