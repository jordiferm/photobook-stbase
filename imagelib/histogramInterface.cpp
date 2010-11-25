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
#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qcursor.h>
#include <Q3Accel>
#include <QDesktopWidget>
#include <QPaintEvent>

//Projectwide includes
#include "histogramInterface.h"
#include "histogramEditor.h"
#include "imageTools.h"
#include "cursors.h"

//a mouse press within DRAG_THRESHOLD will move boundaries of selection
//if mouse press is not within DRAG_THRESHOLD a new selection will be started
//and both click and drag points will be reset
#define DRAG_THRESHOLD 5

#define COLOR_BAR_MARGIN 2
#define COLOR_BAR_BORDER 2
#define COLOR_BAR_HEIGHT 6
#define HISTOGRAM_HEIGHT ( height() - COLOR_BAR_BORDER - 2*COLOR_BAR_MARGIN - COLOR_BAR_HEIGHT )

//==============================================
using namespace Qt;
HistogramInterface::HistogramInterface( QString imageFilename, 
                                        QWidget *parent ) : 
                                        QWidget (parent ) //WNoAutoErase
{                     
  //set default mode to adjusted image
  displayedChannel = LUMINOSITY;
  
  //record original image width and height
  getImageSize( imageFilename, origImageSize );
  
  //construct histogram for color and luminosity channels
  //resize image to current screen size for faster
  //scaling during resize events
  QRect screenSize = qApp->desktop()->availableGeometry();
  QImage image;
  scaleImage( imageFilename, image, screenSize.width()/4, screenSize.height()/4 );

  int i;
  for(i=0; i<256; i++)
  {    
    redVals[i] = 0;
    greenVals[i] = 0;
    blueVals[i] = 0;
    grayVals[i] = 0;
  }
  int x, y;
  QRgb* rgb;
  uchar* scanLine;
  for( y=0; y<image.height(); y++)
  {   
    scanLine = image.scanLine(y);
    for( x=0; x<image.width(); x++)
    {
      rgb = ((QRgb*)scanLine+x);
      redVals[ qRed(*rgb) ]++;
      greenVals[ qGreen(*rgb) ]++; 
      blueVals[ qBlue(*rgb) ]++;
      grayVals[ qGray(*rgb) ]++;
    } //x
  } //y
  
  //find max r,g,b, and gray counts
  maxRcount = 0;
  maxGcount = 0;
  maxBcount = 0;
  maxGRAYcount = 0;
  for(i=0; i<256; i++)
  {
    if(redVals[i]   > maxRcount)    maxRcount    = redVals[i]; 
    if(greenVals[i] > maxGcount)    maxGcount    = greenVals[i]; 
    if(blueVals[i]  > maxBcount)    maxBcount    = blueVals[i]; 
    if(grayVals[i]  > maxGRAYcount) maxGRAYcount = grayVals[i]; 
  }
  //----          
  //by default mouse drags have no effect
  dragMode = NO_EFFECT;
  currentMouseShape = NO_EFFECT;

  //watch mouse movements in order to drag selection
  setMouseTracking(true);
  
  //accept focus when clicked on
  //setFocusPolicy( QWidget::ClickFocus );  

  ///add keybinding for selecting entire range
  Q3Accel *keyAccel = new Q3Accel( this );
  keyAccel->connectItem( keyAccel->insertItem( Qt::CTRL + Qt::Key_A),
                         this, SLOT(selectAll()) );  
  
  //default cursor is cross hair indication regions can be selected
  setCursor( getCursor(CROSS_CURSOR) );
  
  //by default entire range is selected for all channels
  resetBoundaries();

  //setup pastel colors
  red = QColor(235,50,50);
  green = QColor(37,232,78);
  blue = QColor(31,97,249);
}
//==============================================
HistogramInterface::~HistogramInterface() { }
//==============================================
void HistogramInterface::resizeEvent( QResizeEvent * )
{  
  repaint(false);
}
//==============================================
void HistogramInterface::getSelectedRange( int &left, int &right )
{
  if(displayedChannel == LUMINOSITY) 
  {
    left = QMIN( lumClick, lumDrag );
    right = QMAX( lumClick, lumDrag );
  }
  else if(displayedChannel == RED)   
  { 
    left = QMIN( redClick, redDrag );
    right = QMAX( redClick, redDrag );
  }
  else if(displayedChannel == GREEN) 
  { 
    left = QMIN( greenClick, greenDrag );
    right = QMAX( greenClick, greenDrag );
  }
  else if(displayedChannel == BLUE)  
  { 
    left = QMIN( blueClick, blueDrag );
    right = QMAX( blueClick, blueDrag );
  }   
  else
  { left = 0; right = 0; }
}
//==============================================
double HistogramInterface::displayToIndex( int coordinate )
{
  return (255.0*coordinate) / ( width()-1 );
}
//==============================================
int HistogramInterface::indexToDisplay( int index )
{
  return (index* (width()-1) ) / 255;
}
//==============================================
void HistogramInterface::paintEvent(QPaintEvent *e)
{ 
  //create buffer to draw in
  QPixmap buffer( size() );
  buffer.fill( Qt::white );
  
  //create a painter pointing to the buffer
  QPainter bufferPainter( &buffer );
  
  //turn off clipping to make painting operations faster
  bufferPainter.setClipping(false);

  //initialize buffer with background brush
  bufferPainter.fillRect( buffer.rect(), backgroundBrush() );

  //get handle on histogram data, get max count, set default draw color, and find
  //left and right boundaries of current selection  
  int* data = NULL;
  QColor color;
  int maxCount;

  switch( displayedChannel )
  {
    case LUMINOSITY:
      data = grayVals;
      color = Qt::black;
      maxCount = maxGRAYcount;
      break;
    case RED:
      data = redVals;
      color = Qt::red;
      maxCount = maxRcount;
      break;
    case GREEN:
      data = greenVals;
      color = Qt::green;
      maxCount = maxGcount;
      break;
    case BLUE:
    default:
      data = blueVals;
      color = Qt::blue;
      maxCount = maxBcount;
      break;
  }
  
  int indexLeft, indexRight;
  getSelectedRange(indexLeft,indexRight);
  int displayLeft = indexToDisplay ( indexLeft );
  int displayRight = indexToDisplay ( indexRight );  

  int histogramHeight = HISTOGRAM_HEIGHT;
  
  //iterate over each pixel column
  int x;
  for(x=0; x<width(); x++)
  {    
    double index = displayToIndex( x );
    int indexL = (int)index;
    double scaleR = index - indexL;
    
    int h = 0;
    if(indexL < 255)
    { 
      h = (int) ((1-scaleR)*data[indexL] + scaleR*data[indexL+1]); 
    }
    else 
    { 
      h = data[255]; 
    }

    //scale count so that the maxCount maps to the maximum height
    double scaledH = (histogramHeight*h)/maxCount;
    h = (int) scaledH;    
    //round up values between 0 and 1 so show data is there
    if( h == 0 && scaledH > h) h++;
    
    if(h > 0)
    {
      //use a gray color outside selected range
      QColor usedColor = color;
      if(x < displayLeft || x > displayRight) { usedColor = Qt::gray; }
        
      bufferPainter.fillRect( QRect(x, histogramHeight - h, 
                                    1, h),
                              QBrush(usedColor) );    
    }
    
    //if this is left or right boundary of selection and entire range not selected then
    //draw a vertical black line to make it stand out more
    if( (x == displayLeft || x == displayLeft+1 ||
         x == displayRight || x == displayRight-1) )
    {
      bufferPainter.drawLine( x, 0, x, histogramHeight-1 );
    }
  }
  //----
  //paint color bar key below
  
  //first a black border
  bufferPainter.fillRect( QRect(0, histogramHeight + COLOR_BAR_MARGIN, 
                                width(), COLOR_BAR_HEIGHT+2*COLOR_BAR_BORDER),
                          QBrush(Qt::black) );    

  //next the color gradient
  QColor scaledColor;
  for(x=COLOR_BAR_BORDER; x < width()-COLOR_BAR_BORDER; x++)
  {    
    float index;
    if(x <= displayLeft )
      index = 0;
    else if(x >= displayRight)
      index = 1;
    else
      index = ((float)(x-displayLeft))/(displayRight - displayLeft);

    int r, g, b;
    //black color was used when adjusting luminance, scale to white instead (since black is 0)
    if( color == black )
    { r = g = b = (int) (255*index); }
    else
    {
      r = (int) (index * color.red()   );
      g = (int) (index * color.green() );
      b = (int) (index * color.blue()  );
    }

    scaledColor.setRgb( r,g,b );
    bufferPainter.fillRect( QRect(x, histogramHeight + COLOR_BAR_MARGIN + COLOR_BAR_BORDER, 
                                  1, COLOR_BAR_HEIGHT),
                            QBrush(scaledColor) );    
  }    
    
  //end painter  
  bufferPainter.end();
  
  //blit buffer to screen
  bitBlt( this,
          e->rect().x(), e->rect().y(),
          &buffer, 
          e->rect().x(), e->rect().y(),
          e->rect().width(), e->rect().height() );
}
//==============================================
void HistogramInterface::setDisplayChannel( DISPLAYED_CHANNEL channel )
{
  //set mode and repaint
  displayedChannel = channel;
  repaint(false);
}
//==============================================
QSize HistogramInterface::minimumSizeHint() const
{
  return QSize( 256,100 + COLOR_BAR_MARGIN + 2*COLOR_BAR_BORDER + COLOR_BAR_HEIGHT );
}
//==============================================
bool HistogramInterface::nearBoundary( QPoint p )
{
  //compute index from mouse position
  int index = (int) displayToIndex( p.x() );
  
  //get left and right to check for clicks near current boundaries
  int left, right;
  getSelectedRange( left, right );

  //check if within threshold of left or right boundaries
  return ( (index < left + 1 + DRAG_THRESHOLD &&
            index > left -     DRAG_THRESHOLD) ||
           (index < right + DRAG_THRESHOLD &&
            index > right - 1 - DRAG_THRESHOLD) );
}
//==============================================
void HistogramInterface::mousePressEvent( QMouseEvent *e)
{
  //begin drag mode!
  dragMode = DRAG;

  //compute index from mouse position
  int index = (int) displayToIndex( e->pos().x() );
      
  //get left and right to check for clicks near current boundaries
  int left, right;
  getSelectedRange( left, right );

  //get click and drag handles
  int *click, *drag;
  if(displayedChannel == LUMINOSITY) 
  { 
    click = &lumClick; drag = &lumDrag; 
  }
  else if(displayedChannel == RED)   
  { 
    click = &redClick; drag = &redDrag; 
  }
  else if(displayedChannel == GREEN) 
  { 
    click = &greenClick; drag = &greenDrag; 
  }
  else
  { 
    click = &blueClick; drag = &blueDrag; 
  }
    
  //if within threshold of left then start dragging that side
  if( index < left + DRAG_THRESHOLD &&
      index > left - DRAG_THRESHOLD )
  {
    *click = right;
    *drag = left;
    return;
  }
  //if within threshold of left then start dragging that side
  if( index < right + DRAG_THRESHOLD &&
      index > right - DRAG_THRESHOLD )
  {
    *click = left;
    *drag = right;
    return;
  }
  //else begin new drag
  else
  {
    *click = index;
    *drag = index;
    repaint(false);

    //emit selectection changed signal
    int left, right;
    getSelectedRange( left, right );
    emit selectedRangeChanged();    
  }
}
//==============================================
void HistogramInterface::mouseMoveEvent( QMouseEvent *e)
{
  //if not dragging a selection then update mouse cursor as appropriate
  if(dragMode == NO_EFFECT)
  {
    if( nearBoundary(e->pos()) && currentMouseShape == NO_EFFECT )
    {
      currentMouseShape = DRAG;
      setCursor( getCursor(MOVE_HOR_CURSOR) );
    }
    else if( !nearBoundary(e->pos()) && currentMouseShape == DRAG )
    { 
      currentMouseShape = NO_EFFECT;
      setCursor( getCursor(CROSS_CURSOR) );
    }

    return;
  }
  
  //compute index in 0-255 range from mouse coordinates
  int x = QMAX( QMIN( e->pos().x(), width()-1 ), 0 );
  int index = (int) displayToIndex( x );

  //reset boundary
  if(displayedChannel == LUMINOSITY) { lumDrag = index; }
  else if(displayedChannel == RED)   { redDrag = index; }
  else if(displayedChannel == GREEN) { greenDrag = index; }
  else if(displayedChannel == BLUE)  { blueDrag = index; }          
  
  //repaint
  repaint(false);  
  
  //emit selectection changed signal
  int left, right;
  getSelectedRange( left, right );
  emit selectedRangeChanged();    
}
//==============================================
void HistogramInterface::mouseReleaseEvent( QMouseEvent *e)
{
  //set mouse drags to no longer have any effect on boundary
  dragMode = NO_EFFECT;

  //update mouse cursor if necessary
  if( !nearBoundary(e->pos()) && currentMouseShape == DRAG )
  { 
    currentMouseShape = NO_EFFECT;
    setCursor( getCursor(CROSS_CURSOR) );
  }    
}
//==============================================
void HistogramInterface::selectAll()
{
  //reset boundary
  if(displayedChannel == LUMINOSITY) { lumClick = 0,   lumDrag = 255;   }
  else if(displayedChannel == RED)   { redClick = 0;   redDrag = 255;   }
  else if(displayedChannel == GREEN) { greenClick = 0; greenDrag = 255; }
  else if(displayedChannel == BLUE)  { blueClick = 0;  blueDrag = 255;  } 
  repaint(false);  

  //emit selectection changed signal
  int left, right;
  getSelectedRange( left, right );
  emit selectedRangeChanged();    
}
//==============================================
void HistogramInterface::getHistBoundaries(int &lumLeft, int &lumRight,
                                           int &redLeft, int &redRight,
                                           int &greenLeft, int &greenRight,
                                           int &blueLeft, int &blueRight)
{
  lumLeft = QMIN( lumClick, lumDrag );
  lumRight = QMAX( lumClick, lumDrag );

  redLeft = QMIN( redClick, redDrag );
  redRight = QMAX( redClick, redDrag );

  greenLeft = QMIN( greenClick, greenDrag );
  greenRight = QMAX( greenClick, greenDrag );

  blueLeft = QMIN( blueClick, blueDrag );
  blueRight = QMAX( blueClick, blueDrag );
}
//==============================================
void HistogramInterface::resetBoundaries()
{
  lumClick = redClick = greenClick = blueClick = 0; 
  lumDrag = redDrag = greenDrag = blueDrag = 255;  
  repaint(false);
  emit selectedRangeChanged();    
}
//==============================================




