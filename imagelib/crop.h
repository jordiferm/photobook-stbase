//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_MANIPULATIONS_CROP_H
#define BACKEND_MANIPULATIONS_CROP_H
#include "stimagelibexport.h"


//--------------------
//forward declarations
//--------------------
class QImage;
class QString;
class QPoint;

//Crop image
ST_IMAGELIB_EXPORT QImage* cropImage( QString filename, QPoint topLeft, QPoint bottomRight );
ST_IMAGELIB_EXPORT QImage* cropImage( QImage& origImage, QPoint topLeft, QPoint bottomRight );

#endif //BACKEND_MANIPULATIONS_CROP_H
