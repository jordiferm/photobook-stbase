//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef GUI_CURSORS_H
#define GUI_CURSORS_H
#include "stimagelibexport.h"


///custom cursor types
typedef enum
{  
  CROSS_CURSOR = 0,
  
  MOVE_SELECTION_CURSOR,
  MOVE_HOR_CURSOR,
  MOVE_VERT_CURSOR,
  MOVE_TL_CURSOR,
  MOVE_TR_CURSOR,

  SCALE_SELECTION_CURSOR,
  ROTATE_CURSOR,
  
  TARGET_CURSOR,
  
  DEFAULT_CURSOR,
  //-----------------
  CUSTOM_CURSOR_COUNT
} CUSTOM_CURSOR_TYPE;

ST_IMAGELIB_EXPORT void loadCursors();
ST_IMAGELIB_EXPORT const QCursor& getCursor( CUSTOM_CURSOR_TYPE type );

#endif //GUI_CURSORS_H


