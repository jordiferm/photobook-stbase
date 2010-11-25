//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_ENHANCEMENTS_TILT_H
#define BACKEND_ENHANCEMENTS_TILT_H
#include "stimagelibexport.h"

//--------------------
//forward declarations
//--------------------
class QImage;
class QString;
class QProgressBar;

//Correct for image tilt using idenfitifed true horizonal or vertical edge in image
ST_IMAGELIB_EXPORT QImage* correctImageTilt(QString filename,
      QPoint p1, QPoint p2,
      QProgressBar* _Progress = 0);
ST_IMAGELIB_EXPORT QImage* correctImageTilt(QImage originalImage,
      QPoint p1, QPoint p2,
      QProgressBar* _Progress = 0);

#endif //BACKEND_ENHANCEMENTS_TILT_H
