//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_ENHANCEMENTS_CONTRAST_H
#define BACKEND_ENHANCEMENTS_CONTRAST_H
#include "stimagelibexport.h"

//--------------------
//forward declarations
//--------------------
class QImage;
class QString;
class SProcessStatusWidget;

//Enhance image contrast using contrast normalization technique
ST_IMAGELIB_EXPORT void enhanceImageContrast( QString filename, SProcessStatusWidget* status = NULL );

//Behaves in same fashion as above function, but takes in an already loaded image
ST_IMAGELIB_EXPORT void enhanceImageContrast( QImage* image, SProcessStatusWidget* status=NULL );

#endif //BACKEND_ENHANCEMENTS_CONTRAST_H
