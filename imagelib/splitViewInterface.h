//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it 
//  and/or modify it under the terms of the GNU General 
//  Public License as published by the Free Software 
//  Foundation; either version 2 of the License, or  
//  (at your option) any later version.         
//==============================================

#ifndef GUI_EDITING_SPLITVIEWINTERFACE_H
#define GUI_EDITING_SPLITVIEWINTERFACE_H
#include "stimagelibexport.h"

//--------------------
//forward declarations
//--------------------

#include <qwidget.h>
#include <qimage.h>

///current preview mode
typedef enum
{  
  SPLIT_VIEW,
  ORIGINAL_IMAGE,
  ADJUSTED_IMAGE,
  INV_SPLIT_VIEW,
} PREVIEW_MODE;

///ceffect of mouse movement on split point
typedef enum
{  
  DRAG_SPLIT,
  NO_EFFECT_ON_SPLIT
} PREVIEW_MOUSE_MODE;

//=====================================
/*! \brief A split view interface provides a means to show before and after
versions of an image while adjustments are being made, in addition to dragging this split
point left/right or up/down. This abtract class must be subclassed
in order to properly handle resize events, in addition to actually setting the original and adjusted images. */
//=====================================
class ST_IMAGELIB_EXPORT SplitViewInterface : public QWidget
{
Q_OBJECT

public:
  ///Creates layout
  SplitViewInterface(QWidget *parent=0, 
                     const char* name=0);
  
  ///Sets preview mode
  void setPreviewMode( PREVIEW_MODE mode, bool forceDrawLabel=false );
  
  virtual QSize sizeHint() const = 0;
  virtual QSize minimumSizeHint() const;

  //sets both orig and adjusted images and repaints
  void setImages( QImage origImage, QImage adjustedImage );

  ///sets adjusted image and repaints
  void setAdjustedImage( QImage adjustedImage );

  ///returns orig image object
  QImage& getOrigImage();
  
  ///change the adjusted string  
  void setAdjustedString(QString val);
  //----------------------
protected:
  void paintEvent( QPaintEvent *e);
  void mousePressEvent( QMouseEvent *e);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *e);

  virtual void resizeEvent( QResizeEvent * ) = 0;
  //----------------------
private:
  ///convert display coordinates to world coordinates (double from 0.0 - 1.0)
  double displayToWorld( int coordinate );
  
  ///convert world coordinates to display coordinates (int from 0 to origImage width-1)
  int worldToDisplay( double coordinate );

  ///determines if mouse is near split point
  bool nearSplitPoint( QPoint p );
  
  ///Current display setting (adjusted or split screen)
  PREVIEW_MODE previewMode;  
  
  ///Draw original/adjusted lables outside of split view mode?
  bool forceDrawLabel;
  
  ///Original and adjusted strings
  QString originalString;
  QString adjustedString;
  
  ///Larger font used for drawing text
  QFont textFont;

  ///x (or y) coordinate of split between drawn adjusted and original images
  double dragOffset;

  ///current mouse move mode
  PREVIEW_MOUSE_MODE mouseMode;

  ///current mouse shape. by caching this value we avoid 
  ///resetting the mouse cursor every time it moves etc.
  PREVIEW_MOUSE_MODE currentMouseShape;
  
  ///Scaled original image
  QImage origImage;
  
  ///Scaled adjusted image
  QImage adjustedImage;
  //----------------------
};
//======================

#endif //GUI_EDITING_SPLITVIEWINTERFACE_H
