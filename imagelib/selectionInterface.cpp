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
#include <qsize.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qevent.h>
#include <qpoint.h>
#include <qcursor.h>
#include <QDesktopWidget>
#include <QtGlobal>

//Projectwide includes
#include "selectionInterface.h"
#include "imageTools.h"
#include "cursors.h"
//#include "myapplication.h"
#include "histogramEditor.h"

#include "stimage.h"

//a mouse press within DRAG_THRESHOLD will move corners or sides
//if mouse press is not within DRAG_THRESHOLD of any corner or side of
//current selection a new selection will be started and both press and drag points will be reset
#define DRAG_THRESHOLD 8

//a mouse press within MOVE_THRESHOLD of the center along both x and y axis will
//move the entier selection
#define MOVE_THRESHOLD 150

//minimum size of a valid selection
#define MINIMUM_SELECTION_SIZE 3
#define SLIDER_RADIUS 40


//==============================================
SelectionInterface::SelectionInterface(QWidget *parent) : QLabel(parent)
{ 
	resetValues();
	DrawSelectionAllowed = false;
  //avoid flicker when repainting
  //setWFlags(WRepaintNoErase);
  
  //by default mouse movement does not effect the current selection
  currentDragMode = NO_EFFECT;
  currentMouseShape = NO_EFFECT;
    
//   QAccel *keyAccel = new QAccel( this );
//   keyAccel->connectItem( keyAccel->insertItem( CTRL + Key_A),
//                          this, SLOT(selectAll()) );
//   keyAccel->connectItem( keyAccel->insertItem( CTRL + SHIFT + Key_A ),
//                          this, SLOT(selectNone()) );
//   
  //watch mouse movements in order to change mouse cursor
  setMouseTracking(true);

  //accept focus when clicked on
  setFocusPolicy( Qt::ClickFocus );
  

  //no crop size by default
  cropMaxDimen = -1.0;
  MinDpiSettings = 300;
}

void SelectionInterface::resetValues()
{
  	Bright = 0;
	Contrast = 0;
  //set default selection off screen
	mousePressPoint = QPoint(-1,-1);
	mouseDragPoint = QPoint(-1,-1);
	//initialize cached mouse position to be offscreen by default
	cachedMousePosition = QPoint(-1,-1);  
}


//==============================================
SelectionInterface::~SelectionInterface() { }
//==============================================

void SelectionInterface::setPhoto(const QImage& img, bool resetSelection)
{
	origImageSize = img.size();
	
	QRect screenSize = qApp->desktop()->availableGeometry();
	fullScreenImage = img.scaled(screenSize.size(), Qt::KeepAspectRatio, Qt::FastTransformation ); 

	//construct display images  
	constructDisplayImages();
	
	//reset selection area to nothing
	if(resetSelection)
	{
		mousePressPoint = QPoint(-1,-1);
		mouseDragPoint  = QPoint(-1,-1);
		cropMaxDimen = -1.0;
		SHIFT_Pressed = false;
		CTRL_Pressed = false;
		emit selectionChanged();
	}

	//repaint widget
	//if we are resetting the selection then aspect ratio may have changed
	//so a full repaint (with erase) is necessary
	repaint(resetSelection);
}

void SelectionInterface::setPhoto(QString imageFilename, bool resetSelection)
{  
  //store original image filename
  QString origImageFilename;
	origImageFilename = imageFilename;

  //store original image dimensions
  getImageSize( origImageFilename, origImageSize );
    
  //resize image to current screen size for faster
  //scaling during resize events
	QRect screenSize = qApp->desktop()->availableGeometry();
  scaleImage( origImageFilename, fullScreenImage, screenSize.width(), screenSize.height() );
	//construct display images  
  constructDisplayImages();
  
  //reset selection area to nothing
  if(resetSelection)
  {
    mousePressPoint = QPoint(-1,-1);
    mouseDragPoint  = QPoint(-1,-1);
    cropMaxDimen = -1.0;
    SHIFT_Pressed = false;
    CTRL_Pressed = false;
    emit selectionChanged();
  }

  //repaint widget
  //if we are resetting the selection then aspect ratio may have changed
  //so a full repaint (with erase) is necessary
  repaint(resetSelection);
}
//==============================================
void SelectionInterface::resizeEvent( QResizeEvent * _Event)
{  
  //if image has yet to be set return
  if( fullScreenImage.isNull() ) return;
  
  //construct new images for painting
  constructDisplayImages();
  
  emit resized(_Event);
}
//==============================================
void SelectionInterface::constructDisplayImages()
{  
  //rescale image to fit on screen
  scaledImage = fullScreenImage.scaled( QSize(width(), height()), Qt::KeepAspectRatio );     
  LastScaledImage = scaledImage.copy();
	
  if ( Bright != 0 || Contrast != 0 )
  	adjustBrightAndContrast( &scaledImage, Bright, Contrast );
  
  //construct an unselected scaled image
  unselectedScaledImage = scaledImage.copy();  
  int x, y;
  QRgb* rgb;
  uchar* scanLine;
  for( y=0; y<unselectedScaledImage.height(); y++)
  {   
    //iterate over each selected pixel in scanline
    scanLine = unselectedScaledImage.scanLine(y);
    for( x=0; x<unselectedScaledImage.width(); x++)
    {
      //compress dynamic range to 25% of original
      rgb = ((QRgb*)scanLine+x);
      
      double r = ((double)qRed(*rgb)   )/255.0;
      double g = ((double)qGreen(*rgb) )/255.0;
      double b = ((double)qBlue(*rgb)  )/255.0;
      
      //convert to hsv
      double h,s,v;
      RGBtoHSV(r,g,b,&h,&s,&v);
      
      //scale and clamp v
      v*=0.25;
      
      //convert adjusted color back to rgb colorspace and clamp
      HSVtoRGB( &r,&g,&b, h,s,v);         
      int rp = (int) qMin( qMax((int)(r*255), 0), 255 );
      int gp = (int) qMin( qMax((int)(g*255), 0), 255 );
      int bp = (int) qMin( qMax((int)(b*255), 0), 255 );
      
      //set adjusted color value
      *rgb = qRgb(rp,gp,bp);          
    }
  }  
  
  
}
//==============================================
void SelectionInterface::paintEvent(QPaintEvent *e)
{  
    //if no scaled image just return
  if ( scaledImage.isNull() )
		return;
    
  //create buffer to draw in
  QRect rct = rect();
  rct.moveBy(-x(), -y());
  QPixmap buffer( size() );
  
  //create a painter pointing to the buffer
  QPainter bufferPainter( &buffer );

  //turn off clipping to make painting operations faster
  bufferPainter.setClipping(false);
  
  //initialize buffer with background brush
  bufferPainter.fillRect( buffer.rect(), backgroundBrush() );
  
  //paint the image
  int xOffset = (width() - scaledImage.width()) / 2;
  int yOffset = (height() - scaledImage.height()) / 2;

  //if in draw line mode paint the image normall and then the currently selected line
  //if the two points are set
  if(currentDragMode == DRAW_LINE)
  {
    //paint entire image normally
    bufferPainter.drawImage( QPoint(xOffset, yOffset), scaledImage );       

    //if the first and 2nd points have been set draw the line as well
    if( mousePressPoint.x() != -1 &&
        mouseDragPoint.x() != -1 )
    {
      //get points in display space
      QPoint p1, p2;
      p1 = ConvertImageToDisplayCoordinate( mousePressPoint );
      p2 = ConvertImageToDisplayCoordinate( mouseDragPoint  );

      //setup painter to use green color
      QPen pen;
      pen.setStyle( Qt::SolidLine );
      pen.setCapStyle( Qt::RoundCap );
      pen.setColor( QColor("green") );
      pen.setWidth( 2 );
      bufferPainter.setPen( pen);
      
      //draw line
      bufferPainter.drawLine( p1.x(), p1.y(), p2.x(), p2.y() );
    }
  }
  //else we're in normal selection mode
  else
  {  
    //if selected region is empty paint entire image in color
		//qDebug(QString("debugger_: %0, %1").arg((int)(currentDragMode == NO_EFFECT)).arg((int)(mousePressPoint.x() - mouseDragPoint.x() == 0 || mousePressPoint.y() - mouseDragPoint.y() == 0) ));
    if( mousePressPoint.x() == -1 ||
        (
         currentDragMode == NO_EFFECT &&
         (
          mousePressPoint.x() - mouseDragPoint.x() == 0 ||
          mousePressPoint.y() - mouseDragPoint.y() == 0 
          )
         ))
    {
      bufferPainter.drawImage( QPoint(xOffset, yOffset), scaledImage );       
    }
    //otherwise paint using a two-pass approach,
    //first in monochrome and then the selected region in color
    else
    {
      //first paint using unselected coloring
      bufferPainter.drawImage( QPoint(xOffset, yOffset), unselectedScaledImage );
      
      //construct topLeft and bottomRight points to make logic easier
      QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                      QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
      QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                          QMAX(mousePressPoint.y(), mouseDragPoint.y()) );  
      
      //convert coordinates from original image space to display space
      topLeft = ConvertImageToDisplayCoordinate( topLeft );
      bottomRight = ConvertImageToDisplayCoordinate( bottomRight );
      
      //now paint selected region in color
			
			#if 0
			qDebug(QString(" BtopLeft (%0,%1)").arg(topLeft.x()).arg(topLeft.y()));
			qDebug(QString(" topLeft (%0,%1), bottomRight (%2,%3)").arg(topLeft.x()-xOffset).arg(topLeft.y()-yOffset).arg(bottomRight.x() - topLeft.x()).arg(bottomRight.y() - topLeft.y()));
			//TODO: Això nosé perque va així, comprovar. 
			#endif
			
			if ( bottomRight.x() - topLeft.x() != 0 && bottomRight.y() - topLeft.y() != 0 )
			{
				bufferPainter.drawImage( topLeft.x(), 
																topLeft.y(),
																scaledImage,
																topLeft.x()-xOffset, topLeft.y()-yOffset,
																bottomRight.x() - topLeft.x(),
																bottomRight.y() - topLeft.y() );                                   
			}
      
      //determine color for painting selection rectangle and dimensions
      //use gray/white if cropping to a custom size (and thus real world dimensions
      //are not known). If the crop size is known (eg 5"x7") use green if above
      //resolution is great enough, otherwise use red to indicate that cropping to
      //the specified size will result in pixelation during printing
      QPen pen;
      QColor selectionColor("gray");
      QColor textColor ("white");
      int selectedWidth = QABS( mouseDragPoint.x() - mousePressPoint.x() ) + 1;
      int selectedHeight = QABS( mouseDragPoint.y() - mousePressPoint.y() ) + 1;
      if(selectedWidth == 1) selectedWidth = 0;
      if(selectedHeight == 1) selectedHeight = 0;
      
      int DPI = -1;
	  int minDPI = MinDpiSettings;
      if( cropMaxDimen != -1.0 )
      {
        //compute DPI that will be used in dominant direction
        int maxDimen = QMAX( selectedWidth, selectedHeight );
        DPI = (int) (maxDimen / cropMaxDimen);          
        
        //if DPI is below minimum use red color for selection rectangle to indicate pixalation may occur
        if( DPI < minDPI ) 
        { 
					selectionColor = QColor( 200, 0, 0 ); 
				}
        else 
				{ 
					selectionColor = QColor( 0, 200, 0 ); 
				}
				//selectionColor = QColor( 0, 200, 0 );
      } 
      pen.setStyle( Qt::SolidLine );
      pen.setWidth( 2 );
      bufferPainter.setPen( pen);
      
      //paint selection size in lower right corner of selected area
      QString selectionText;
      if( cropMaxDimen != -1.0 &&
          DPI < minDPI )
        selectionText = QString(tr("Low quality image %1 x %2 (DPI: %3!)")).arg(selectedWidth).arg(selectedHeight).arg(DPI);
      else
        selectionText = QString("%1 x %2").arg(selectedWidth).arg(selectedHeight);
      
      QFontMetrics fm( this->font() );
      int stringWidth = fm.width(selectionText);
      int stringHeight = fm.ascent();
      
      int textX = 0;
      int textY = 0;
      const int TEXT_MARGIN = 4;
      if( mouseDragPoint.x() > mousePressPoint.x() )
      { 
        textX = QMIN( xOffset + unselectedScaledImage.width() - TEXT_MARGIN - stringWidth,
                      bottomRight.x() + TEXT_MARGIN ); 
      }
      else
      { 
        textX = QMAX( xOffset + TEXT_MARGIN, 
                      topLeft.x() - TEXT_MARGIN - stringWidth ); 
      }
      
      if( mouseDragPoint.y() > mousePressPoint.y() )
      { 
        textY = QMIN( yOffset + unselectedScaledImage.height() - TEXT_MARGIN,
                      bottomRight.y() + TEXT_MARGIN + stringHeight); 
      }
      else
      { 
        textY = QMAX( yOffset + TEXT_MARGIN + stringHeight, 
                      topLeft.y() - TEXT_MARGIN ); 
      }
      
      //paint a thin outline around the selection
      pen.setColor( selectionColor );
      bufferPainter.setPen( pen);
      QRect selection( topLeft, bottomRight );
      bufferPainter.drawRect(selection);       
      
      //paint image under selected dimension using selected coloring to help make it more visible
      bufferPainter.drawImage( textX, textY-stringHeight,
                               unselectedScaledImage,
                               textX - xOffset, textY-stringHeight-yOffset,
                               stringWidth, fm.height() );
      
      
      //paint selected dimensions    
      pen.setColor( textColor );
      bufferPainter.setPen( pen);
      bufferPainter.drawText( textX, textY, selectionText );
    }        
  }
  
  bufferPainter.end();
  
  //blit buffer to screen
  bitBlt( this,
          e->rect().x(), e->rect().y(),
          &buffer, 
          e->rect().x(), e->rect().y(),
          e->rect().width(), e->rect().height() );
}
//==============================================
DRAG_MODE SelectionInterface::mouseActionByPosition(QPoint p)
{   
  //If SHIFT_Pressed is set then scaling a selection
  if(SHIFT_Pressed) { return SCALE_SELECTION; }
  
  //construct topLeft and bottomRight points to make logic easier
  QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                  QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
  QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                      QMAX(mousePressPoint.y(), mouseDragPoint.y()) );  
   
  //convert selelection bounary to display space since thresholds are all in display space
  topLeft = ConvertImageToDisplayCoordinate( topLeft );
  bottomRight = ConvertImageToDisplayCoordinate( bottomRight );
    
  //compute selection center
  QPoint center( (topLeft.x() + bottomRight.x()) / 2,
                 (topLeft.y() + bottomRight.y()) / 2);
  
  //if pointer is not over image no effect will occur when clicking
  int xOffset = (width() - scaledImage.width()   ) / 2;
  int yOffset = (height() - scaledImage.height() ) / 2;  
  if(p.x() < xOffset || p.x() >= scaledImage.width() + xOffset ||
     p.y() < yOffset || p.y() >= scaledImage.height() + yOffset )
  { return NO_EFFECT; }
    
  //if either there is no selection or the entire image is selected, new
  //mouse clicks are interpreted as being for selecting new regions
  if( mousePressPoint.x() == -1 ||
      ( bottomRight.x() - topLeft.x() == origImageSize.width() - 1 &&
        bottomRight.y() - topLeft.y() == origImageSize.height() - 1 && DrawSelectionAllowed )
      )
  { return DRAW_SELECTION; }
  //move entire selection
  //mouse is within MOVE_THRESH of center and outside
  //DRAG_THRESH of any side
  else if( p.x() >= QMAX( center.x() - MOVE_THRESHOLD, topLeft.x() ) &&
           p.x() <= QMIN( center.x() + MOVE_THRESHOLD, bottomRight.x() ) &&
           p.y() >= QMAX( center.y() - MOVE_THRESHOLD, topLeft.y() ) &&
           p.y() <= QMIN( center.y() + MOVE_THRESHOLD, bottomRight.y() ) &&
           p.x() > topLeft.x() + DRAG_THRESHOLD &&
           p.x() < bottomRight.x() - DRAG_THRESHOLD &&
           p.y() > topLeft.y() + DRAG_THRESHOLD &&
           p.y() < bottomRight.y() - DRAG_THRESHOLD )
{ return MOVE_SELECTION; } 
  //drag top left
  else if( QABS(topLeft.x() - p.x()) <= DRAG_THRESHOLD &&
           QABS(topLeft.y() - p.y()) <= DRAG_THRESHOLD )  
  { return MOVE_TOP_LEFT_CORNER; }
  //drag top right    
  else if( QABS(bottomRight.x() - p.x()) <= DRAG_THRESHOLD &&
           QABS(topLeft.y()     - p.y()) <= DRAG_THRESHOLD )  
  { return MOVE_TOP_RIGHT_CORNER; }
  //drag bottom left    
  else if( QABS(topLeft.x() - p.x()) <= DRAG_THRESHOLD &&
           QABS(bottomRight.y() - p.y()) <= DRAG_THRESHOLD )  
  { return MOVE_BOTTOM_LEFT_CORNER; }
  //drag bottom right
  else if( QABS(bottomRight.x() - p.x()) <= DRAG_THRESHOLD &&
           QABS(bottomRight.y() - p.y()) <= DRAG_THRESHOLD )  
  { return MOVE_BOTTOM_RIGHT_CORNER; }
  //drag left
  else if( QABS(topLeft.x() - p.x()) <= DRAG_THRESHOLD &&
           p.y() >= topLeft.y() &&
           p.y() <= bottomRight.y() )
  { return MOVE_LEFT_SIDE; }
  //drag right
  else if( QABS(bottomRight.x() - p.x()) <= DRAG_THRESHOLD &&
           p.y() >= topLeft.y() &&
           p.y() <= bottomRight.y() )
  { return MOVE_RIGHT_SIDE; }
  //drag top
  else if( QABS(topLeft.y() - p.y()) <= DRAG_THRESHOLD &&
           p.x() >= topLeft.x() &&
           p.x() <= bottomRight.x() )
  { return MOVE_TOP_SIDE; }
  //drag bottom
  else if( QABS(bottomRight.y() - p.y()) <= DRAG_THRESHOLD &&
           p.x() >= topLeft.x() &&
           p.x() <= bottomRight.x() )
  { return MOVE_BOTTOM_SIDE; }
  //else new selection
  else { 
		if (DrawSelectionAllowed)
			return DRAW_SELECTION; 
		else 
			return MOVE_SELECTION;
	}
}
//==============================================
void SelectionInterface::mousePressEvent(QMouseEvent *e)
{        
  //If CTRL pressed then emit ctrlClick event
  if( CTRL_Pressed) 
  {    
    emit ctrlClick();
    return;
  }
  
  //get mouse position in original image coordinates
  QPoint p = ConvertDisplayToImageCoordinate( e->pos() );

  //in draw line mode a mouse press has not immediate effect, 
  //mouse release is how a point is finally selected
  if( currentDragMode == DRAW_LINE )
  { return; }
  
  //construct topLeft and bottomRight points to make logic easier
  QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                  QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
  QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                      QMAX(mousePressPoint.y(), mouseDragPoint.y()) );  

  //determine action mouse click will cause
  currentDragMode = mouseActionByPosition(e->pos());

	switch(currentDragMode)
  {
    case DRAW_SELECTION:
      mousePressPoint = p;
      mouseDragPoint = p;
      cropMaxDimen = -1.0;
      emit aspectRatioChanged();
      break;
    case MOVE_SELECTION:
      //nothing done until mouse moves
      break;
    case MOVE_TOP_LEFT_CORNER:
      mousePressPoint = bottomRight;
      mouseDragPoint = topLeft;
      break;
    case MOVE_BOTTOM_RIGHT_CORNER:
      mousePressPoint = topLeft;
      mouseDragPoint = bottomRight;
      break;
    case MOVE_TOP_RIGHT_CORNER:
      mousePressPoint = QPoint( topLeft.x(), bottomRight.y() );
      mouseDragPoint = QPoint( bottomRight.x(), topLeft.y() );
      break;mousePressPoint = QPoint(-1,-1);
  mouseDragPoint = QPoint(-1,-1);
  
    case MOVE_BOTTOM_LEFT_CORNER:
      mousePressPoint = QPoint( bottomRight.x(), topLeft.y() );
      mouseDragPoint = QPoint( topLeft.x(), bottomRight.y() );
      break;
    case MOVE_LEFT_SIDE:
      mousePressPoint = bottomRight;
      mouseDragPoint  = topLeft;
      break;
    case MOVE_RIGHT_SIDE:
      mousePressPoint = topLeft;
      mouseDragPoint  = bottomRight;
      break;
    case MOVE_TOP_SIDE:
      mousePressPoint = bottomRight;
      mouseDragPoint  = topLeft;
      break;
    case MOVE_BOTTOM_SIDE:
      mousePressPoint = topLeft;
      mouseDragPoint  = bottomRight;
      break;
    //no effect
    default:
      return;
  }
//   qDebug(QString("\n\nCurrentDragMode:%0 , MousePressPoint: (%1, %2), MouseDragPoint (%3, %4) ").arg(currentDragMode).arg(mousePressPoint.x()).arg(mousePressPoint.y()).arg(mouseDragPoint.x()).arg(mouseDragPoint.y()));
  //repaint the selection  
  repaint(false);
  emit selectionChanged();
}
//==============================================
void SelectionInterface::updateCursorShape( QPoint rawP )
{  
  //if resizing a selection always use size all cursor
  if(SHIFT_Pressed) 
  { 
    setCursor( getCursor(SCALE_SELECTION_CURSOR) );
    return;
  }
  
  //if ctrl pressed show rotate selection cursor  
  if(CTRL_Pressed) 
  { 
    setCursor( getCursor(ROTATE_CURSOR) );
    return;
  }
   
  //if selecting a line the cursor is always the same
  if( currentDragMode == DRAW_LINE )
  {
    setCursor( getCursor(TARGET_CURSOR) );
    return;
  }
  
  //update mouse cursor based on coordinates
  currentMouseShape = mouseActionByPosition( rawP ); 
  switch(currentMouseShape)
  {
    case DRAW_SELECTION:
      setCursor( getCursor(CROSS_CURSOR) ); break;
    case MOVE_SELECTION:
      setCursor( getCursor(MOVE_SELECTION_CURSOR) ); break;
    case MOVE_TOP_LEFT_CORNER:
    case MOVE_BOTTOM_RIGHT_CORNER:
      setCursor( getCursor(MOVE_TL_CURSOR) ); break;
    case MOVE_TOP_RIGHT_CORNER:
    case MOVE_BOTTOM_LEFT_CORNER:
      setCursor( getCursor(MOVE_TR_CURSOR) ); break;
    case MOVE_LEFT_SIDE:
    case MOVE_RIGHT_SIDE:
      setCursor( getCursor(MOVE_HOR_CURSOR) ); break;
    case MOVE_TOP_SIDE:
    case MOVE_BOTTOM_SIDE:
      setCursor( getCursor(MOVE_VERT_CURSOR) ); break;
      break;
    default:
      setCursor( Qt::ArrowCursor );
      break;
  }  
}
//==============================================
bool SelectionInterface::scaleSelection( int delta )
{
  //construct topLeft and bottomRight points to make logic easier
  QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                  QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
  QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                      QMAX(mousePressPoint.y(), mouseDragPoint.y()) );     

  //compute new selection dimension
  int curSelectionWidth = bottomRight.x() - topLeft.x() + 1;
  int curSelectionHeight = bottomRight.y()- topLeft.y() + 1;      
  int newSelectionWidth = -1; 
  int newSelectionHeight = -1;      

  //adjust in dimension of greater resolution
  if( curSelectionWidth > curSelectionHeight )
  {
    newSelectionWidth = QMIN( curSelectionWidth + delta, origImageSize.width() );
    newSelectionHeight = (newSelectionWidth * cachedSelectionSize.height()) / cachedSelectionSize.width();
  }
  else
  {
    newSelectionHeight = QMIN( curSelectionHeight + delta, origImageSize.height() );
    newSelectionWidth = (newSelectionHeight * cachedSelectionSize.width()) / cachedSelectionSize.height();    
  }
  
  //if selection width or height already maxed out then ignore attempt to make selection bigger
  if(delta > 0 &&
     ( curSelectionWidth == origImageSize.width() ||
       curSelectionHeight == origImageSize.height() ) ) 
  { 
    return false;
  }  
  
  //again prevent from growing too big, now along y-axis
  if(newSelectionHeight > origImageSize.height())
  {
    newSelectionHeight = origImageSize.height();
    newSelectionWidth = (newSelectionHeight * cachedSelectionSize.width()) / cachedSelectionSize.height();
  }        
  
  //prevent selection becoming empty
  if(newSelectionWidth <= 0 || newSelectionHeight <= 0)
  {
    newSelectionWidth = curSelectionWidth;
    newSelectionHeight = curSelectionHeight;
  }
  
  //center new selection over current selection , move if necessary
  topLeft = QPoint(QMAX( cachedSelctionCenter.x() - newSelectionWidth/2, 0 ),
                   QMAX( cachedSelctionCenter.y() - newSelectionHeight/2, 0 ));
  bottomRight = QPoint( topLeft.x() + newSelectionWidth - 1,
                        topLeft.y() + newSelectionHeight - 1 );   
  
  //too far right
  if(bottomRight.x() > origImageSize.width() - 1 )
  {
    int diff = bottomRight.x() - (origImageSize.width() - 1);        
    topLeft.setX( topLeft.x() - diff );
    bottomRight.setX( bottomRight.x() - diff );

    //recompute center
    cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                   ( mouseDragPoint.y() + mousePressPoint.y() )/2 );          
  }                     
  
  //too far down
  if(bottomRight.y() > origImageSize.height() - 1 )
  {
    int diff = bottomRight.y() - (origImageSize.height() - 1);        
    topLeft.setY( topLeft.y() - diff );
    bottomRight.setY( bottomRight.y() - diff );
    
    //recompute center
    cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                   ( mouseDragPoint.y() + mousePressPoint.y() )/2 );      
  }                     
  
  //set new selection
  mousePressPoint = topLeft;
  mouseDragPoint = bottomRight;  
  
  //success
  return true;  
}
//==============================================
void SelectionInterface::mouseMoveEvent(QMouseEvent *e)
{
  //if the SHIFT_Pressed state variable is true but SHIFT is now not pressed
  //reset bool and mouse cursor!
  if( SHIFT_Pressed && !(e->state() & Qt::ShiftButton) )
  {
    SHIFT_Pressed = false;
    updateCursorShape( ConvertImageToDisplayCoordinate( e->pos() ) );   
  }
      
  //if the CTRL_Pressed state variable is true but CTRL is now not pressed
  //reset bool and mouse cursor!
  if( CTRL_Pressed && !(e->state() & Qt::ControlButton) )
  {
    CTRL_Pressed = false;
    updateCursorShape( ConvertImageToDisplayCoordinate( e->pos() ) );   
  }  
  
  //if mouse not pressed update mouse cursor if 
  //mode will change if user presses mouse button
  if(currentDragMode == NO_EFFECT)
  {  
    //only update the mouse cursor shape if it will change
    if( mouseActionByPosition( e->pos() ) != currentMouseShape )
      updateCursorShape( e->pos() );
    
    cachedMousePosition = ConvertDisplayToImageCoordinate( cropSelectedPoint(e->pos()) );
  }
  //if currently in draw line mode update the selected line
  else if(currentDragMode == DRAW_LINE)
  {
    //if the 1st point hasn't been set yet do nothing
    if( mousePressPoint.x() == -1 )
      return;
    
    //1st point set, set 2nd point to current mouse position and redraw
    mouseDragPoint = ConvertDisplayToImageCoordinate( cropSelectedPoint(e->pos()) );

    //repaint the line
    repaint(false);
  }  
  //update selection rectangle or selected line
  else
  {     
    //get mouse position in original image coordinates
    QPoint p = ConvertDisplayToImageCoordinate( cropSelectedPoint(e->pos()) );

    //construct topLeft and bottomRight points to make logic easier
    QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                    QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
    QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                        QMAX(mousePressPoint.y(), mouseDragPoint.y()) );     
    
    //if SHIFT pressed and both selction dimensions non-empty then scale selection
    if(SHIFT_Pressed && 
       bottomRight.x() > topLeft.x() && 
       bottomRight.y() > topLeft.y() )
    {
      //update width, prevent from growing beyond image boundaries
      int delta = p.x() - cachedMousePosition.x(); 
      if( !scaleSelection( delta ) )
      {
        //cache the current mouse position before returning without change
        cachedMousePosition = p;
        return;
      }
    }    
    //if draging the entire selection, update x and y coordinates for
    //both mousePress and mouseDrag points and update the dragSelectionPoint
    else if( currentDragMode == MOVE_SELECTION )
    {
      //compute offset
      QPoint offset( p.x() - cachedMousePosition.x(),
                     p.y() - cachedMousePosition.y() );
      
      //construct topLeft and bottomRight points to make logic easier
      QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                      QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
      QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                          QMAX(mousePressPoint.y(), mouseDragPoint.y()) );  
      
      //clip offset so selected area is always within image boundaries
      
      //left
      if(offset.x() < 0 && topLeft.x() + offset.x() < 0 )
        offset.setX( -topLeft.x() );
      
      //right
      if(offset.x() > 0 && bottomRight.x() + offset.x() >= origImageSize.width() )
        offset.setX( origImageSize.width() - 1 - bottomRight.x() );          
      
      //top
      if(offset.y() < 0 && topLeft.y() + offset.y() < 0 )
        offset.setY( -topLeft.y() );
      
      //bottom
      if(offset.y() > 0 && bottomRight.y() + offset.y() >= origImageSize.height() )
        offset.setY( origImageSize.height() - 1 - bottomRight.y() );            
      
      //update press and drag points
      mousePressPoint+= offset;
      mouseDragPoint+= offset;      
      
      //recompute selection center
      cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                     ( mouseDragPoint.y() + mousePressPoint.y() )/2 );          
    }        
    //if moving any corner or drawing the original selection, 
    //update x and y coords of drag point
    else if( currentDragMode == DRAW_SELECTION ||
             currentDragMode == MOVE_TOP_LEFT_CORNER ||
             currentDragMode == MOVE_TOP_RIGHT_CORNER ||
             currentDragMode == MOVE_BOTTOM_LEFT_CORNER ||
             currentDragMode == MOVE_BOTTOM_RIGHT_CORNER )
    {
      mouseDragPoint.setX( p.x() );
      mouseDragPoint.setY( p.y() );
      cropMaxDimen = -1.0;
      
      //cache selection size and center, necessary for computing next size
      cachedSelectionSize = QSize( QABS( mouseDragPoint.x() - mousePressPoint.x() )+1,
                                   QABS( mouseDragPoint.y() - mousePressPoint.y() )+1);
      cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                     ( mouseDragPoint.y() + mousePressPoint.y() )/2 );    
      
      
      emit aspectRatioChanged();
    }
    //else if moving the left or right sides just update the x coordinate of 
    //the drag point
    else if( currentDragMode == MOVE_LEFT_SIDE ||
             currentDragMode == MOVE_RIGHT_SIDE )
    {
      mouseDragPoint.setX( p.x() );
      cropMaxDimen = -1.0;
      emit aspectRatioChanged();
      
      //cache selection size and center, necessary for computing next size
      cachedSelectionSize = QSize( QABS( mouseDragPoint.x() - mousePressPoint.x() )+1,
                                   QABS( mouseDragPoint.y() - mousePressPoint.y() )+1);                                               
      cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                     ( mouseDragPoint.y() + mousePressPoint.y() )/2 );    
    }
    //else if moving the top or bottom sides just update the x coordinate of 
    //the drag point
    else if( currentDragMode == MOVE_TOP_SIDE ||
             currentDragMode == MOVE_BOTTOM_SIDE )
    {
      mouseDragPoint.setY( p.y() );
      cropMaxDimen = -1.0;
      emit aspectRatioChanged();
      
      //cache selection size and center, necessary for computing next size
      cachedSelectionSize = QSize( QABS( mouseDragPoint.x() - mousePressPoint.x() )+1,
                                   QABS( mouseDragPoint.y() - mousePressPoint.y() )+1);                                               
      cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                     ( mouseDragPoint.y() + mousePressPoint.y() )/2 );    
      
    }    
    
    //cache current mouse position
    cachedMousePosition = p;

    //repaint the selection
    repaint(false);
    emit selectionChanged();

    //only update the mouse cursor shape if it will change
    if( mouseActionByPosition( e->pos() ) != currentMouseShape )
      updateCursorShape( e->pos() );
  }
}
//==============================================
void SelectionInterface::mouseReleaseEvent(QMouseEvent *e)
{
  //if in draw line mode
  if( currentDragMode == DRAW_LINE )
  {
    //get mouse location in image space
    QPoint p = ConvertDisplayToImageCoordinate( e->pos() );

    //if first point has not been set set it now
    if( mousePressPoint.x() == -1 )
    {
      mousePressPoint = p;
      repaint(false);
    }
    //else first point has been set then we're setting the 2nd point, emit the selected line
    else
    {
      //get two points on line
      QPoint p1 = mousePressPoint;
      QPoint p2 = p;
      
      //reset mode, press and drag points so there is no selection
      currentDragMode = NO_EFFECT;
      selectNone();
      updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
      
      //emit the selected line
      emit lineSelected( p1, p2 );       
    }
    return; 
  }
  
  //disable move moving from having an effect on the selection
  currentDragMode = NO_EFFECT;
  
  //construct topLeft and bottomRight points to make logic easier
  QPoint topLeft( QMIN(mousePressPoint.x(), mouseDragPoint.x()),
                  QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
  QPoint bottomRight( QMAX(mousePressPoint.x(), mouseDragPoint.x()),
                      QMAX(mousePressPoint.y(), mouseDragPoint.y()) );  

  //convert selection region to display space to check for tiny selections to ignore
  topLeft = ConvertImageToDisplayCoordinate( topLeft );
  bottomRight = ConvertImageToDisplayCoordinate( bottomRight );
  
  //if selection is very small reset to select nothing
  if( bottomRight.x() - topLeft.x() + 1 < MINIMUM_SELECTION_SIZE ||
      bottomRight.y() - topLeft.y() + 1 < MINIMUM_SELECTION_SIZE ) { selectNone(); }  
}
//==============================================
QPoint SelectionInterface::ConvertDisplayToImageCoordinate( QPoint p )
{  
  QPoint newPoint = p;
  
  //remove display offset
  int xOffset = (width() - scaledImage.width()   ) / 2;
  int yOffset = (height() - scaledImage.height() ) / 2;  
  newPoint.setX( newPoint.x() - xOffset );
  newPoint.setY( newPoint.y() - yOffset );
  
  //if coordinate is at max set new value explicitly to avoid roundoff error,
  //otherwise scale to full image dimensions
  if(newPoint.x() == scaledImage.width() - 1)
    newPoint.setX( origImageSize.width() - 1);
  else 
    newPoint.setX( (int) (0.5 + ((double)(newPoint.x() * (origImageSize.width()-1))) / (scaledImage.width()-1) ) );
  
  if(newPoint.y() == scaledImage.height() - 1)
    newPoint.setY( origImageSize.height() - 1);
  else 
    newPoint.setY( (int) (0.5 + ((double)(newPoint.y() * (origImageSize.height()-1))) / (scaledImage.height()-1) ) );
  
  //return point in image coordinates
  return newPoint;
}
//==============================================
QPoint SelectionInterface::ConvertImageToDisplayCoordinate( QPoint p )
{       
  QPoint newPoint = p;
  
  //if coordinate is at max set new value explicitly to avoid roundoff error,
  //otherwise scale to full image dimensions
  if(newPoint.x() == origImageSize.width() - 1)
    newPoint.setX( scaledImage.width() - 1);
  else 
    newPoint.setX( (newPoint.x() * (scaledImage.width()-1)) / (origImageSize.width()-1) );
  
  if(newPoint.y() == origImageSize.height() - 1)
    newPoint.setY( scaledImage.height() - 1);
  else 
    newPoint.setY( (newPoint.y() * (scaledImage.height()-1)) / (origImageSize.height()-1) );
  
  //add display offset
  int xOffset = (width() - scaledImage.width()   ) / 2;
  int yOffset = (height() - scaledImage.height() ) / 2;  
  newPoint.setX( newPoint.x() + xOffset );
  newPoint.setY( newPoint.y() + yOffset );
    
  //return point in image coordinates
  return newPoint;
}
//==============================================
QPoint SelectionInterface::cropSelectedPoint(QPoint p)
{    
  int xOffset = (width() - scaledImage.width()   ) / 2;
  int yOffset = (height() - scaledImage.height() ) / 2;

  QPoint croppedPoint;
  croppedPoint.setX( QMIN( QMAX(xOffset, p.x()), xOffset + scaledImage.width() - 1 ) );
  croppedPoint.setY( QMIN( QMAX(yOffset, p.y()), yOffset + scaledImage.height() - 1 ) );
  return croppedPoint;
}
//==============================================
bool SelectionInterface::selectionEmpty()
{
  return (
          mousePressPoint.x() == -1 ||
          mouseDragPoint.x() - mousePressPoint.x() == 0 ||
          mouseDragPoint.y() - mousePressPoint.y() == 0
         );  
}
//==============================================
void SelectionInterface::selectAll()
{
  mousePressPoint.setX( 0 );
  mousePressPoint.setY( 0 );
  mouseDragPoint.setX( origImageSize.width() - 1 );
  mouseDragPoint.setY( origImageSize.height() - 1);

  //cache selection size and center, necessary for computing next size
  cachedSelectionSize = QSize( QABS( mouseDragPoint.x() - mousePressPoint.x() )+1,
                               QABS( mouseDragPoint.y() - mousePressPoint.y() )+1);           
  cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                 ( mouseDragPoint.y() + mousePressPoint.y() )/2 );      
  
  cropMaxDimen = -1.0;
  repaint(false);
  emit selectionChanged();
  emit aspectRatioChanged();
}
//==============================================
void SelectionInterface::selectNone()
{
  mousePressPoint = QPoint(-1,-1);
  mouseDragPoint = QPoint(-1,-1);
  cropMaxDimen = -1.0;
  repaint(false);
  emit selectionChanged();
  emit aspectRatioChanged();
}
//==============================================
void SelectionInterface::setSelection(QPoint topLeft, QPoint bottomRight,
                                      double cropMaxDimen)
{
  mousePressPoint = topLeft;
  mouseDragPoint = bottomRight;
  
  this->cropMaxDimen = cropMaxDimen;
  
  //cache selection size and center, necessary for computing next size
  cachedSelectionSize = QSize( QABS( mouseDragPoint.x() - mousePressPoint.x() )+1,
                               QABS( mouseDragPoint.y() - mousePressPoint.y() )+1);           
  cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                 ( mouseDragPoint.y() + mousePressPoint.y() )/2 );    
  
  repaint(false);
  emit selectionChanged();
}

void SelectionInterface::selectMaxByAspectRatio(const QSizeF _AspectRatio)
{
	//determine selected width and height;
	int SelectedWidth = 0;
	int SelectedHeight = 0;
	int ImageWidth = origImageSize.width();
	int ImageHeight = origImageSize.height();

	QSizeF CAspectRatio = _AspectRatio;

	//select region using same aspect ratio
	SelectedWidth = ImageWidth;
	SelectedHeight = (int)((((double)(ImageWidth * CAspectRatio.height())) / CAspectRatio.width()));

	calcScaledImageDimensions(SelectedWidth, SelectedHeight,
	                          ImageWidth, ImageHeight,
	                          SelectedWidth, SelectedHeight);
// 	SelectedWidth = SelectedWidth / AspectRatioZoom;
// 	SelectedHeight = SelectedHeight / AspectRatioZoom;
	//get current selection boundary
	QPoint CurTopLeft, CurBottomRight;
	getSelection(CurTopLeft, CurBottomRight);

	//get current selection center
	QPoint CurCenter;
	CurCenter.setX((CurTopLeft.x() + CurBottomRight.x()) / 2);
	CurCenter.setY((CurTopLeft.y() + CurBottomRight.y()) / 2);

	//if center is off image (no previous selection) then
	//fix center to center of image
	if (CurCenter.x() < 0 || CurCenter.y() < 0)
	{
		CurCenter.setX(ImageWidth / 2);
		CurCenter.setY(ImageHeight / 2);
	}

	//attempt to center new selection overold selection, only
	//offset if necessary
	QPoint NewTopLeft, NewBottomRight;

	NewTopLeft.setX(CurCenter.x() - SelectedWidth / 2);
	NewTopLeft.setY(CurCenter.y() - SelectedHeight / 2);
	//qDebug("NewTopLeft: (%d, %d)", NewTopLeft.x(), NewTopLeft.y());

	//push right/down if necessary
	if (NewTopLeft.x() < 0) NewTopLeft.setX(0);
	if (NewTopLeft.y() < 0) NewTopLeft.setY(0);

	//push left/up if necessary
	NewBottomRight.setX(NewTopLeft.x() + SelectedWidth - 1);
	if (NewBottomRight.x() >= ImageWidth)
	{
		NewBottomRight.setX(ImageWidth - 1);
		NewTopLeft.setX(NewBottomRight.x() - SelectedWidth + 1);
	}

	NewBottomRight.setY(NewTopLeft.y() + SelectedHeight - 1);
	if (NewBottomRight.y() >= ImageHeight)
	{
		NewBottomRight.setY(ImageHeight - 1);
		NewTopLeft.setY(NewBottomRight.y() - SelectedHeight + 1);
	}

	//NewBottomRight = NewBottomRight / AspectRatioZoom;

	//select region
	setSelection(NewTopLeft, NewBottomRight, pixelsToInch(qMax(CAspectRatio.width(), CAspectRatio.height()), 300));

}

QRect SelectionInterface::getSelectionRect() const 
{
	QPoint TopLeft, BottomRight; 
	getSelection(TopLeft, BottomRight); 
	return QRect(TopLeft, BottomRight);
}

//==============================================
void SelectionInterface::getSelection(QPoint &topLeft, QPoint &bottomRight) const
{
  //if none selected just return immediately
  if(mousePressPoint.x() == -1)
  {
    topLeft.setX(-1); topLeft.setY(-1);
    bottomRight.setX(-1); bottomRight.setY(-1);
    return;
  }

  //set coordinates based on raw selection  
  topLeft.setX( QMIN(mousePressPoint.x(), mouseDragPoint.x()) );
  topLeft.setY( QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
  bottomRight.setX( QMAX( mousePressPoint.x(), mouseDragPoint.x()) );                
  bottomRight.setY( QMAX( mousePressPoint.y(), mouseDragPoint.y()) );                  
}
//==============================================
void SelectionInterface::getDisplaySize(int &width, int &height)
{
  width = scaledImage.width();
  height = scaledImage.height();  
}
//==============================================
void SelectionInterface::keyPressEvent(QKeyEvent *e)
{
  //if currently drawing a line, allow the user
  //to escape the current acction by hiting escape, all other
  //key presses are ignored
  if( currentDragMode == DRAW_LINE )
  {
    if(e->key() == Qt::Key_Escape )
    {
      //reset mode, press and drag points so there is no selection, and mouse cursor
      currentDragMode = NO_EFFECT;
      selectNone();
      updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
    
      //emit an invalid line
      emit lineSelected( QPoint(-1,-1), QPoint(-1,-1) );       
    }

    return; 
  }
  
  //if user pressed SHIFT button and selection non-empty 
  //then note state change, 
  //this will effect resizing selections during drag events
  if(e->key() == Qt::Key_Shift && !selectionEmpty() )
  {    
    SHIFT_Pressed = true;
    updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
    return;
  }

  //if user pressed CTRL button and selection non-empty 
  //then note state change, 
  //this will effect mouse clicks by rotating the current selection
  if(e->key() == Qt::Key_Control && !selectionEmpty() && !SHIFT_Pressed )
  {    
    CTRL_Pressed = true;
    updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
    return;
  }
    
  //ignore keypress if selection is empty
  if(mousePressPoint.x() == -1 ||
     mousePressPoint.x() - mouseDragPoint.x() == 0 ||
     mousePressPoint.y()  - mouseDragPoint.y() == 0)
  {
    e->ignore();
    return;
  }

  //ignore keypresses while mouse button is down since
  //intended behavious is unclear
  if( currentDragMode != NO_EFFECT )
  {
    e->ignore();
    return;
  }
  
  //-------  
  //if += / -_ keys are pressed scale selection
  if( e->key() == Qt::Key_Plus ||
      e->key() == Qt::Key_Equal ||
      e->key() == Qt::Key_Minus ||
      e->key() == Qt::Key_Underscore )
  {
    int delta = 2*QMAX( origImageSize.width(), scaledImage.width() ) / scaledImage.width();
    
    delta = 1;
    
    //negate delta if decreasing size
    if( e->key() == Qt::Key_Minus ||
        e->key() == Qt::Key_Underscore )
      delta = -delta;
    
    if( scaleSelection( delta ) )
    {    
      //repaint the selection
      repaint(false);
      emit selectionChanged();
    }
    return;
  }
  //-------  
  
  //find topleft and bottom right of current selection
  QPoint topLeft, bottomRight;
  topLeft.setX( QMIN(mousePressPoint.x(), mouseDragPoint.x()) );
  topLeft.setY( QMIN(mousePressPoint.y(), mouseDragPoint.y()) );
  bottomRight.setX( QMAX( mousePressPoint.x(), mouseDragPoint.x()) );                
  bottomRight.setY( QMAX( mousePressPoint.y(), mouseDragPoint.y()) );                  
  
  //compute the number of pixels in the image correspond to one display pixel
  //this is the unit by which the selection will be moved
  int moveBy = 0;
  if( e->key() == Qt::Key_Left ||
      e->key() == Qt::Key_Right )
  {
    moveBy = QMAX( origImageSize.width(), scaledImage.width() ) / scaledImage.width();
  }
  else
  {
    moveBy = QMAX( origImageSize.height(), scaledImage.height() ) / scaledImage.height();
  }
  
  //boundary checked moved by value. must be decalred outside switch statement below
  int dx = 0;
  int dy = 0;
  switch( e->key() )
  {
    case Qt::Key_Left:
      dx = QMAX( topLeft.x() - moveBy, 0) - topLeft.x();
      break;
    case Qt::Key_Right:
      dx = QMIN( bottomRight.x() + moveBy, origImageSize.width() - 1) - bottomRight.x();
      break;
    case Qt::Key_Up:
      dy = QMAX( topLeft.y() - moveBy, 0) - topLeft.y();
      break;
    case Qt::Key_Down:
      dy = QMIN( bottomRight.y() + moveBy, origImageSize.height() - 1) - bottomRight.y();
      break;
    default:
      e->ignore();
      return;
  }
  
  //shift by dx
  mousePressPoint.setX( mousePressPoint.x() + dx );
  mouseDragPoint.setX( mouseDragPoint.x() + dx );      
 
  //shift by dy
  mousePressPoint.setY( mousePressPoint.y() + dy );
  mouseDragPoint.setY( mouseDragPoint.y() + dy );      

  //recompute center
  cachedSelctionCenter = QPoint( ( mouseDragPoint.x() + mousePressPoint.x() )/2,
                                 ( mouseDragPoint.y() + mousePressPoint.y() )/2 );    
  
  //only bother with repaint and updating cursor shape if shift actually occured
  if(dx != 0 || dy != 0) 
  { 
    repaint(false);
    updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );  
  }
}
//==============================================
void SelectionInterface::keyReleaseEvent(QKeyEvent *e)
{
  //if user released SHIFT button then note state change, 
  //this will effect resizing selections during drag events
  if(e->key() == Qt::Key_Shift)
  {
    SHIFT_Pressed = false;
    updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
  }
  //if user released CTRL button then note state change, 
  //this will effect mouse clicks
  else  if(e->key() == Qt::Key_Control)
  {
    CTRL_Pressed = false;
    updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
  }
  //unhandled key press: pass up the object tree
  else { e->ignore(); }
}
//==============================================
void SelectionInterface::enterDrawLineMode()
{
  //set the current mode
  currentDragMode = DRAW_LINE;

  //during draw line mode ignore control and shift keys, they are only useful for adjusting
  //selections, not lines
  SHIFT_Pressed = false;
  CTRL_Pressed = false;
  
  //reset 1st point
  mousePressPoint = QPoint( -1, -1 );
  mouseDragPoint = QPoint( -1, -1 );
  
  //repaint and reset the mouse cursor
  updateCursorShape( ConvertImageToDisplayCoordinate(cachedMousePosition) );   
  repaint(false);
}
//==============================================

void SelectionInterface::adjustBrightAndContrast( QImage* _img, int _bright, int _contrast)
{
	Q_ASSERT ( _img );
	
	int lumLeft   = 0; int lumRight   = 255;
	int redLeft   = 0; int redRight   = 255;
	int greenLeft = 0; int greenRight = 255;
	int blueLeft  = 0; int blueRight  = 255;
	
	//modify image
	double meanR = 0;
	double meanG = 0;
	double meanB = 0;
	double displayToOneScalar = 1.0/255.0;
	double scaledMeanR = displayToOneScalar* HistogramEditor::scaleColor( 255.0*meanR, redLeft, redRight );
	double scaledMeanG = displayToOneScalar* HistogramEditor::scaleColor( 255.0*meanG, greenLeft, greenRight );
	double scaledMeanB = displayToOneScalar* HistogramEditor::scaleColor( 255.0*meanB, blueLeft, blueRight );
	
	double brightnessScalar, addedBrightnessColor;
	if(_bright < 0)
	{
		brightnessScalar = ((double)(SLIDER_RADIUS + _bright))/SLIDER_RADIUS;
		addedBrightnessColor = 1.0 - brightnessScalar;
	}
	else
	{
		brightnessScalar = ((double)(SLIDER_RADIUS - _bright))/SLIDER_RADIUS;
		addedBrightnessColor = 0.0;
	}
	
	int x, y;  
	QRgb* rgb;
	double r,g,b;
	double h,s,v;
	int rPrime, gPrime, bPrime;
	uchar* scanLine;
	
	QImage* image = _img;
		
	for( y=0; y<image->height(); y++)
	{   
		scanLine = image->scanLine(y);
		for( x=0; x<image->width(); x++)
		{
			//get rgb value
				
			rgb = ((QRgb*)scanLine+x);
			r = qRed(*rgb);
			g = qGreen(*rgb);
			b = qBlue(*rgb);
			
			//apply histogram boundaries
			RGBtoHSV(r,g,b,&h,&s,&v);
			v = HistogramEditor::scaleColor( v, lumLeft, lumRight );
			HSVtoRGB( &r,&g,&b, h,s,v);         
			
			r = HistogramEditor::scaleColor( r, redLeft, redRight );
			g = HistogramEditor::scaleColor( g, greenLeft, greenRight );
			b = HistogramEditor::scaleColor( b, blueLeft, blueRight );
			
			//convert to 0-1 scale
			r = r*displayToOneScalar;
			g = g*displayToOneScalar;
			b = b*displayToOneScalar;
			
			//adjust contrast
			r = ( (r-scaledMeanR) * (SLIDER_RADIUS-_contrast) )/SLIDER_RADIUS + scaledMeanR;
			g = ( (g-scaledMeanG) * (SLIDER_RADIUS-_contrast) )/SLIDER_RADIUS + scaledMeanG;
			b = ( (b-scaledMeanB) * (SLIDER_RADIUS-_contrast) )/SLIDER_RADIUS + scaledMeanB;
			
			//apply brightness adjustment
			//http://www.sgi.com/misc/grafica/interp/
			r = brightnessScalar*r + addedBrightnessColor;
			g = brightnessScalar*g + addedBrightnessColor;
			b = brightnessScalar*b + addedBrightnessColor;
			
			//scale and clamp to 0-255 range
			rPrime = (int) QMIN( QMAX((int)(r*255), 0), 255 );
			gPrime = (int) QMIN( QMAX((int)(g*255), 0), 255 );
			bPrime = (int) QMIN( QMAX((int)(b*255), 0), 255 );
			
			//set adjusted color value
			*rgb = qRgb(rPrime, gPrime, bPrime);          
		} //x
	} //y  
}

void SelectionInterface::adjustGamma(QImage& _Image, int _Gamma)
{
	double DGamma = _Gamma + 50;
	DGamma = DGamma * 2.3 / 100;
	STImage GImage = _Image; 
	_Image = GImage.gamma(DGamma); 
}

void SelectionInterface::adjustGammaRGB(QImage& _Image, int _GammaR, int _GammaG, int _GammaB )
{
	double DGammaR = _GammaR + 50;
	DGammaR = DGammaR * 2.3 / 100;

	double DGammaG = _GammaG + 50;
	DGammaG = DGammaG * 2.3 / 100;

	double DGammaB = _GammaB + 50;
	DGammaB = DGammaB * 2.3 / 100;

	STImage GImage = _Image; 
	_Image = GImage.gammaRGB(DGammaR, DGammaG, DGammaB); 
}

void SelectionInterface::slotChangeBrightAndContrast ( int _bright, int _contrast )
{
	Bright = _bright;
	Contrast = _contrast;
	//Copiem la imatge original i apliquem el brillo i el contrast.
	scaledImage = LastScaledImage.copy();
	adjustBrightAndContrast( &scaledImage, Bright, Contrast );
	this->repaint();
}

void SelectionInterface::slotChangeGamma(int _Gamma)
{
	scaledImage = LastScaledImage.copy(); 
	adjustGamma(scaledImage, _Gamma);
	this->repaint();
}

void SelectionInterface::slotChangeGammaRGB(int _GammaR, int _GammaG, int _GammaB)
{
	scaledImage = LastScaledImage.copy(); 
	adjustGammaRGB(scaledImage, _GammaR, _GammaG, _GammaB);
	this->repaint();
}

void SelectionInterface::slotChangeBright(int _bright)
{
	Bright = _bright;
	//Copiem la imatge original i apliquem el brillo i el contrast.
	scaledImage = LastScaledImage.copy();
	adjustBrightAndContrast(&scaledImage, Bright, Contrast);
	repaint();
}

void SelectionInterface::slotChangeContrast(int _contrast)
{
	Contrast = _contrast;
	//Copiem la imatge original i apliquem el brillo i el contrast.
	scaledImage = LastScaledImage.copy();
	adjustBrightAndContrast(&scaledImage, Bright, Contrast);
	repaint();
}
