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
#include <qcursor.h>
#include <qpixmap.h>
#include <qmap.h>

//Projectwide includes
#include "cursors.h"

//Include cursor xpm's
#include "crossCursor.xpm"

#include "moveSelectionCursor.xpm"
#include "moveHorCursor.xpm"
#include "moveVertCursor.xpm"
#include "moveTLCursor.xpm"
#include "moveTRCursor.xpm"
#include "scaleSelectionCursor.xpm"
#include "rotateCursor.xpm"
#include "targetCursor.xpm"
//==============================================
typedef QMap<int, QCursor*> TCursorMap;
static TCursorMap customCursors;
//==============================================
void loadCursors()
{
  customCursors.insert(CROSS_CURSOR, new QCursor( QPixmap( (const char**)crossCursor_xpm)));
  customCursors.insert(MOVE_SELECTION_CURSOR, new QCursor( QPixmap( (const char**)moveSelectionCursor_xpm)));
  customCursors.insert(MOVE_HOR_CURSOR, new QCursor( QPixmap( (const char**)moveHorCursor_xpm)));                              
  customCursors.insert(MOVE_VERT_CURSOR, new QCursor( QPixmap( (const char**)moveVertCursor_xpm)));  
  customCursors.insert(MOVE_TL_CURSOR, new QCursor( QPixmap( (const char**)moveTLCursor_xpm)));                              
  customCursors.insert(MOVE_TR_CURSOR,new QCursor( QPixmap( (const char**)moveTRCursor_xpm)));  
  customCursors.insert(SCALE_SELECTION_CURSOR, new QCursor( QPixmap( (const char**)scaleSelectionCursor_xpm)));              
  customCursors.insert(ROTATE_CURSOR, new QCursor( QPixmap( (const char**)rotateCursor_xpm))); 
  customCursors.insert(TARGET_CURSOR, new QCursor( QPixmap( (const char**)targetCursor_xpm))); 
  customCursors.insert(DEFAULT_CURSOR, new QCursor( Qt::ArrowCursor ));
}
//===========================================================
const QCursor& getCursor( CUSTOM_CURSOR_TYPE type )
{
	Q_ASSERT(customCursors[DEFAULT_CURSOR]);

  if( type < 0 || type >= CUSTOM_CURSOR_COUNT )
    return *(customCursors[DEFAULT_CURSOR]);
  else
    return *(customCursors[type]);
}
//===========================================================
