//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_ENHANCEMENTS_ILCOLOR_H
#define BACKEND_ENHANCEMENTS_ILCOLOR_H
#include "stimagelibexport.h"


//--------------------
//forward declarations
//--------------------
class QImage;
class QString;
class SProcessStatusWidget;

//Correct color balanace by applying normalization independently to each channel
ST_IMAGELIB_EXPORT QImage* improveColorBalance ( QImage* editedImage, SProcessStatusWidget* status );
ST_IMAGELIB_EXPORT QImage* improveColorBalance ( QString filename, SProcessStatusWidget* status );

#endif //BACKEND_ENHANCEMENTS_COLOR_H
