//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it 
//  and/or modify it under the terms of the GNU General 
//  Public License as published by the Free Software 
//  Foundation; either version 2 of the License, or  
//  (at your option) any later version.         
//==============================================

#ifndef GUI_EDITING_SCALEDPREVIEWINTERFACE_H
#define GUI_EDITING_SCALEDPREVIEWINTERFACE_H

#include "splitViewInterface.h"

//=====================================
/*! \brief */
//=====================================
class ScaledPreviewInterface : public SplitViewInterface
{
Q_OBJECT

public:
  ///Creates layout
  ScaledPreviewInterface(QString imageFilename, QWidget *parent=0, const char* name=0);

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;
//----------------------
protected:
  void resizeEvent( QResizeEvent * );
  //----------------------
private:
  ///Full screen version of image
  QImage fullScreenImage;
  
  ///original image dimensions
  QSize origImageSize;  
//----------------------
signals:
  //emited when the widget is resized indicating
  //adjusted image needs to be reset
  void resized();
  //----------------------
};
//======================

#endif //GUI_EDITING_SCALEDPREVIEWINTERFACE_H
