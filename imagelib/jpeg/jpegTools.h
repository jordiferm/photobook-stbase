//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_TOOLS_JPEG_JPEGTOOLS_H
#define BACKEND_TOOLS_JPEG_JPEGTOOLS_H

class QString;
class QImage;

//fast scale a Jpeg img using libjpg to intially scale image by power of 2
bool scaleJPEG(QString fileIn, QImage& scaledImage,  int targetWidth, int targetHeight);

//apply lossless transformation on image (rotation or flip)
bool transformJPEG ( QString fileIn, QString fileOut, TRANSFORM_CODE transformation );

#endif //BACKEND_TOOLS_JPEG_JPEGTOOLS_H

