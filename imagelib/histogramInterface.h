//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it 
//  and/or modify it under the terms of the GNU General 
//  Public License as published by the Free Software 
//  Foundation; either version 2 of the License, or  
//  (at your option) any later version.         
//==============================================

#ifndef GUI_EDITING_HISTOGRAMINTERFACE_H
#define GUI_EDITING_HISTOGRAMINTERFACE_H

#include <qwidget.h>
#include <qcolor.h>

///chanel histogram displays
typedef enum
{  
  LUMINOSITY,
  RED,
  GREEN,
  BLUE,
} DISPLAYED_CHANNEL;

///current drag mode, effect of mouse movement on selected range
typedef enum
{  
  DRAG,
  NO_EFFECT
} DRAG_MODE;

//=====================================
/*! \brief
 */
//=====================================
class HistogramInterface : public QWidget
{
Q_OBJECT

public:
  ///Creates layout
  HistogramInterface( QString imageFilename,
                      QWidget *parent=0);
                 
  ///Deletes objects
  ~HistogramInterface();
  
  ///Sets currently displayed channel
  void setDisplayChannel( DISPLAYED_CHANNEL channel );

  virtual QSize minimumSizeHint() const;
  
  ///returns histogram boundaries
  void getHistBoundaries(int &lumLeft, int &lumRight,
                         int &redLeft, int &redRight,
                         int &greenLeft, int &greenRight,
                         int &blueLeft, int &blueRight);                         
  
  ///resets all boundaries
  void resetBoundaries();
  //----------------------
protected:
  void paintEvent( QPaintEvent *e);
  void resizeEvent( QResizeEvent * );
  void mousePressEvent( QMouseEvent *e);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *e);
  //----------------------
signals:
  void selectedRangeChanged();
  //----------------------    
private slots:
  //selects entire range of current channel
  void selectAll();
  //----------------------
private:    
  ///this utility function finds currently selected range
  void getSelectedRange( int &left, int &right );  

  ///convert screen coordinate to index in 0-255 range
  double displayToIndex(int val);
  
  ///converts index in 0-255 ranges to screen coordinate
  int indexToDisplay(int val);
  
  ///determines if mouse is near boundary
  bool nearBoundary( QPoint p );  
  
    ///Currently displayed channel
  DISPLAYED_CHANNEL displayedChannel;  

  ///color and luminosity histograms
  int redVals[256];
  int greenVals[256];
  int blueVals[256];
  int grayVals[256];
  
  ///max r,g,b, and gray counts
  int maxRcount, maxGcount, maxBcount, maxGRAYcount;
  
  ///left and right bounds for each channel
  int lumClick, lumDrag;
  int redClick, redDrag;
  int greenClick, greenDrag;
  int blueClick, blueDrag;
  
  ///original image dimensions, needed for painting
  QSize origImageSize;
  
  ///effect of mouse drags
  DRAG_MODE dragMode;  
  
  ///current mouse shape. by caching this value we avoid 
  ///resetting the mouse cursor every time it moves etc.
  DRAG_MODE currentMouseShape;
  
  ///carefully picked red, green, and blue pastel colors used to display historgram
  QColor red, green, blue;
//----------------------
};
//======================

#endif //GUI_EDITING_HISTOGRAMINTERFACE_H
