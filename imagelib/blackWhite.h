//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_MANIPULATIONS_BLACKWHITE_H
#define BACKEND_MANIPULATIONS_BLACKWHITE_H
#include "stimagelibexport.h"


//--------------------
//forward declarations
//--------------------
class QImage;
class QString;
class ManipulationOptions;

//Apply black and white effect
ST_IMAGELIB_EXPORT QImage* blackWhiteEffect( QString filename, ManipulationOptions* options ); 
ST_IMAGELIB_EXPORT void blackWhiteEffect( QImage &editedImage, ManipulationOptions* options );


#endif //BACKEND_MANIPULATIONS_BLACKWHITE_H
