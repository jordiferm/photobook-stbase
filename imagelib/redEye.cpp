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
#include <qimage.h>
#include <qstring.h>
#include <qapplication.h>
#include <qpoint.h>

//Projectwide includes
#include "redEye.h"
#include "sprocessstatuswidget.h"

//----------------------------------------------
// Inputs:
// -------
// QString filename - location of original image on disk
// QPoint topLeftExtreme - top left constraint
// QPoint bottomRightExtreme - botth right constraint
// SProcessStatusWidget* status - widget for making progress visible to user
//
// Outputs:
// --------
// QImage* returned - enhanced image
//
// Description:
// ------------
// There are a lot of programs out there that provide some sort of
// red eye tool, but to put it bluntly, most of them really suck.
// To be fair, the red-eye flash function on most digital cameras (my own
// Olympus 3030z included) suck too. 
//
// "Such foolishness, what can men do against such reckless stupidity?"
//                                         -unknown
//
// Well, here I try to provide a better red-eye tool by studying those that suck,
// those that suck less, drawing some conclusions, and coming up with a few tricks
// of my own...
//
// The worst red eye tools suck for two reasons:
// -False positives
// -Horrid red channel desaturation
//
// I've encountered red eye tools that claim to just work 
// by clicking a single button. Guess what, they don't. The sad thing is that
// while you can do a pretty good job figuring out where the red eyes are
// in a picture, the programs to provide these brain-dead interfaces usually don't
// do anything complicated at all and gunk up non-red eye regions all over the place!
//
// The second problem I'd say most programs suffer from is doing a poor job of actually
// correcting the red eye region, which is a shame but also stems from their generally
// poor understanding of where the red eyes are.
//
// Algorithm:
// ----------
// I've developed my own red-eye reduction algorithm that tries to surpass all
// others by:
// -finding the red eyes and
// -carefully fixing the color of these regions only
//
// The second step involving desaturing the red channel of offending pixels
// is largely based on Gaubtz and Ulichney's 2002 IEEE paper titled:
// "Automatic Red-Eye Detection and Correction"
//
// http://www.crl.hpl.hp.com/who/people/ulichney/bib/papers/2002-redeye%20-%20ICIP.pdf
// 
// Gaubtz and Ulichney base their techinque on a complicated face-detection model.
// I know such approaches are error prone, and guess what, we have a semi (if not
// very) intelligent user sitting in front of the screen, why not put them to work!
//
// Instead of detecting face elements automatically, we first have the user select
// a region of the image that two red eyes exist within. Before continuing, we
// attempt to shrink this selection as much as possible by thresholding pixels and
// tightening the boundary as long as no above threshold pixels are cut out.
// 
// threshmet = r > 2*g AND r > MIN_RED_VAL
//
// Red eyes tend to be red, but not nearly as green or blue. The second
// half of the threshold helps throw out low-lying noise by requiring 
// the red channel to be above a minimum threshold.
//
// Many programs JUST use the first half of this test (r > 2*g) to pick pixels
// within a region to fix. I suppose you can get away without the noise test but
// fudging up all these other pixels, even if it isn't very noticable, really bugs me.
// I did extensive testing and tuned that second paramters to filter such changes out.
//
// Once we've shrunk the selected area, we proceed with the heart of the algorithm:
// 1.) finding blobs
// 2.) sorting blobs
// 3.) picking best two blobs
//
// and finally...
//
// 4.) desaturating the best two blobs OR desaturating the entire selected
//     region if good blobs could not be found.
//
// Under the best conditions (most cases) the algorithm finds the offending
// eyes and reduces them only. In the worst case scenario the algorithm
// applying the desaturing procedure of all thresholded pixels within the
// selected area, which is still better than other algorithms in the wild
// since we'll employ a smarter desaturating techinque, but more on that in a bit.
//
// Let's examine each step in detail:
//
// Finding Blobs:
// --------------
// The finding blobs algorithm is actually pretty straight forward.
// An initial pass over the selected region constructs a integer mask where
// 0 indicates a pixel did not met and 1 indictes a pixel that did met the 
// same red threshold test we applied earlier.
//
// If the integer mask is set to 0 move on.
// If the integer mask is set to 1 assign the next unique ID, push all 8 neighbors
// that are 1's in the integer masl into a list and asssociate that pixel in 
// the list with the unique ID we just set.
//
// At the top of the loop we pop pixels off the list while the list is not empty. For each
// pixel we check the current integer mask value it has. If it is 1 we set it to the 
// tagged unique ID and push all it's neighbors that have 1's in the integer 
// mask and move. Below is an example of what the integer mask might look like
// before and after blobs are found.
//
// 0000000000000000000      0000000000000000000
// 0011000111100000100      0022000333300000400
// 0111100000111000110  --> 0222200000333000440
// 0100000000110000010      0200000000330000040
// 0000000000000000000      0000000000000000000
//
// Every time a new pixel is used to start a new blob the old
// blob and a few statistics are pushed into a list. In addition to
// knowledge of the blob ID and inherantly all tagged pixels (we keep around
// the integer mask), we also store the pixel count and the blobs aspect ratio (w/h).
// These stats are useful during the next step.
//
// Sorting Blobs:
// --------------
// At this point we've found all the above threshold blobs which consist of
// connected above threshold pixels, but it is often the case not all blobs
// are eyes. Acne, lipstick, moles, or plain old poor selection by the user, can
// result in a number of false positive blobs getting pushed into our lists.
// Fortunately, eyes are:
// -round
// -roughly the same size and shape
//
// To make actually picking blobs easier, we first sort the blob list by 
// decreasing size, so the biggest ones are up front. You tend to run into a lot more
// small false positives than large ones, and the large ones tend to not be
// very round at all (like lips), so thorwing them out is a lot easier.
//
// Picking Blobs:
// --------------
// Picking the two best blobs is fairly straight forward. If only two 
// blobs are found use those. If more blobs are found then start walking 
// down the list of blobs starting with the largest ones. The first two 
// consequtive blobls that are roughly circular (0.75 < aspect ratio < 2.0), 
// roughly similar in shape (larger aspect ratio / smaller aspect ratio < 2), 
// roughly similar in size (biggerSize / smallersize < 1.5), and both blobs 
// meet a minimum size threshold (20 pixels) are chosen as the best two blobs.
//
// That's all just fine and dandy, but what if two blobs can't be found that
// meet those constraints? Easy, we'll work on the entire region, but usually we
// find the eyes without much trouble, while throwing out the other stuff
// like lips etc.
//
// Desaturing:
// -----------
// There are two aspects of the desaturation process that make
// the results provided by this techinque far better than most of the
// other programs out there.
//
// First, we only desaturate the red channel. A lot of programs convert
// the pixcel color to grayscale, then dim is slightly. This is bad for two
// reasons. First, you lose the true pupil color. Second, dimming the pixel
// causes you to lose the glint that often reflects off the center of the
// eyeball. Instead, we desaturate the red channel only, and instead
// of simply decreasing it, we estimate it's true value using the green and
// and blue components, which tends to look more natural:
//
// r' = 0.05*r + 0.6*g + 0.3*b
//
// The problem with directly desaturing the red channel is that you get seams at
// the blob border. To prevent seams from occuring, we blend the updated
// red channel color with the original using an alpha term based on
// the percentage of pixels within a centered 5x5 grid that were marked as
// blob pixels.
//
// The result of seamless red channel correction for the offending red eyes only.
// The glint in a persons eyes are preserved mainly because of the blob based 
// approach we take (pixels in the center of a blob are not necessary tagged
// since the white glint does not pass the intial threshold test).
//
// A final note, in the situation where two good blobs could not be found
// we simply desaturate all pixels that meet the less stringent r > 2*g
// test using the same r' approach techinque.
//
//----------------------------------------------

//==============================================

QImage* RedEye::removeRedeyeRegions ( QString filename, 
                             QPoint topLeftExtreme, QPoint bottomRightExtreme,
                             SProcessStatusWidget* SProcessStatusWidget )
{
	//load original image
  QImage img( filename );
	return removeRedeyeRegions( &img, topLeftExtreme, bottomRightExtreme, SProcessStatusWidget );
}

QImage* RedEye::removeRedeyeRegions( QImage* image, 
                             QPoint topLeftExtreme, QPoint bottomRightExtreme,
                             SProcessStatusWidget* SProcessStatusWidget )
{
	Q_ASSERT ( image );
	rawImage = *image;
  //store handle to status widget
  status = SProcessStatusWidget;
  
  //sanity check: unable to load image
  if(rawImage.isNull()) { return NULL; }

  //convert to 32-bit depth if necessary
  if( rawImage.depth() < 32 ) { rawImage = rawImage.convertDepth( 32, Qt::AutoColor ); }
   
  //sanity check: make sure topLeftExtreme and bottomRightExtreme are within image boundary
  topLeftExtreme.setX( QMAX( topLeftExtreme.x(), 0 ) );
  topLeftExtreme.setY( QMAX( topLeftExtreme.y(), 0 ) );
  bottomRightExtreme.setX( QMIN( bottomRightExtreme.x(), rawImage.width()-1 ) );
  bottomRightExtreme.setY( QMIN( bottomRightExtreme.y(), rawImage.height()-1 ) );

  //setup progress bar
  QString statusMessage = qApp->translate( "removeRedeyeRegions", "Removing Red-Eye:" );
  status->showProgressBar( statusMessage, 100 );
  qApp->processEvents();  
  
  //update progress bar for every 1% of completion
  updateIncrement = (int) ( 0.01 * 
                            ( bottomRightExtreme.x() - topLeftExtreme.x() + 1 ) *
                            ( bottomRightExtreme.y() - topLeftExtreme.y() + 1 ) );
  newProgress = 0;   

  //find region of interest: constrain search box to boundary that actually contains red enough pixels
  findRegionOfInterest(topLeftExtreme, bottomRightExtreme);

  //if no pixels were found then immediately return a NULL pointer signaling no change
  if(topLeft.x() == -1) 
  { 
    //hide progress bar
    status->hideProgressBar();
    qApp->processEvents();

    return NULL; 
  }

  //load an editing image
  //two images mus be loaded becuase pixel values are replaced
  //using a compbination of niehgbors and their own in order
  //to avoid sharp lines at the edge of the saturated region
  editedImage = new QImage( *image );
  
  //sanity check: unable to allocated edited image
  if( editedImage == NULL) 
  { 
    //hide progress bar
    status->hideProgressBar();
    qApp->processEvents();

    return NULL; 
  }

  //convert to 32-bit depth if necessary
  if( editedImage->depth() < 32 )
  {
    QImage* tmp = editedImage;
    editedImage = new QImage( tmp->convertDepth( 32, Qt::AutoColor ) );
    delete tmp; tmp=NULL;
  }
  
  findBlobs();
  sortBlobsByDecreasingSize();
  findBestTwoBlobs();

  //if we found two good blobs then desaturate those only
  if(id1 != -1)
  {
    desaturateBlobs();
  }
  //else desaturate all pixels above thresh within selection area
  else
  {
    desaturateEntireImage(topLeftExtreme, bottomRightExtreme);
  }

  //remove status bar
  status->hideProgressBar();
  qApp->processEvents();

  //return pointer to edited image
  return editedImage;                                                                                                 
}
//==============================================

// 40 = 15.6% of red channel, a good heuristic for false positives
//at border of face on a dark background.
#define MIN_RED_VAL 40

//==============================================
void RedEye::findRegionOfInterest(QPoint topLeftExtreme, QPoint bottomRightExtreme)
{
  topLeft = QPoint(-1,-1);
  bottomRight = QPoint(-1,-1);
  
  int x, y;
  QRgb* rgb;
  uchar* scanLine;
  for( y=topLeftExtreme.y(); y<=bottomRightExtreme.y(); y++)
  {
    scanLine = rawImage.scanLine(y);
    for( x=topLeftExtreme.x(); x<=bottomRightExtreme.x(); x++)
    {
      rgb = ((QRgb*)scanLine+x);
      
      bool threshMet = qRed(*rgb) > 2*qGreen(*rgb) &&
                      qRed(*rgb) > MIN_RED_VAL;
      if(threshMet)
      {
        //first pixel
        if(topLeft.x() == -1) 
        {
          topLeft = QPoint(x,y);
          bottomRight = QPoint(x,y);
        }
        
        if(x < topLeft.x() ) topLeft.setX( x );
        if(y < topLeft.y() ) topLeft.setY( y );
        if(x > bottomRight.x() ) bottomRight.setX( x );
        if(y > bottomRight.y() ) bottomRight.setY( y );
      }
      
      //update status bar if significant progress has been made since last update
      newProgress++;
      if(newProgress >= updateIncrement)
      {
        newProgress = 0;
        status->incrementProgress();
        qApp->processEvents();  
      }
      
    }
  }  
}
//==============================================
void RedEye::pushPixel(int x, int y, int id)
{
  //if pixel off image or below thresh ignore push attempt
  if(  x < 0  || 
       y <  0 ||
       x >= regionWidth ||
       y >= regionHeight ||
       regionOfInterest[ x + y*regionWidth ] != 1 )
    return;
  
  //passes! set id and actually put pixel onto stack
  regionOfInterest[ x + y*regionWidth] = id;  
  spreadablePixels.push( QPoint( x, y ) );
  
  //increase blob pixel count and update topLeft and bottomRight
  blobPixelCount++;
  blobTopLeft.setX( QMIN( x, blobTopLeft.x() ) );
  blobTopLeft.setY( QMIN( y, blobTopLeft.y() ) );
  blobBottomRight.setX( QMAX( x, blobBottomRight.x() ) );
  blobBottomRight.setY( QMAX( y, blobBottomRight.y() ) );
}
//==============================================
void RedEye::findBlobs()
{
  //create small matrix for region of interest
  regionWidth = bottomRight.x() - topLeft.x() + 1;
  regionHeight = bottomRight.y() - topLeft.y() + 1;  
  regionOfInterest = new int[ regionWidth * regionHeight ];
  
  //set all pixels that meet thresh to 1, all others to 0
  int x, y;
  int x2, y2;
  QRgb* rgb;
  uchar* scanLine;
  for( y=topLeft.y(); y<=bottomRight.y(); y++)
  {
    y2 = y - topLeft.y();
    
    scanLine = rawImage.scanLine(y);
    for( x=topLeft.x(); x<=bottomRight.x(); x++)
    {
    
      x2 = x - topLeft.x();
      
      rgb = ((QRgb*)scanLine+x);
      
      bool threshMet = qRed(*rgb) > 2*qGreen(*rgb) &&
                       qRed(*rgb) > MIN_RED_VAL;
      
      if(threshMet)
        regionOfInterest[ x2 + y2*regionWidth ] = 1;
      else
        regionOfInterest[ x2 + y2*regionWidth ] = 0;
    }
  } 
  
  //walk over region of interest and propogate blobs
  int nextValidID = 2;
  for(x = 0; x<regionWidth; x++)
  {
    for(y = 0; y<regionHeight; y++)
    {
      //if any blobs can be propogated handle them first
      while( !spreadablePixels.empty() )
      {
        QPoint point = spreadablePixels.pop();
        int id = regionOfInterest[ point.x() + point.y()*regionWidth ];
        
        pushPixel( point.x()-1, point.y()-1, id );
        pushPixel( point.x(),   point.y()-1, id );
        pushPixel( point.x()+1, point.y()-1, id );
        pushPixel( point.x()-1, point.y(), id );
        pushPixel( point.x()+1, point.y(), id );
        pushPixel( point.x()-1, point.y()+1, id );
        pushPixel( point.x(),   point.y()+1, id );
        pushPixel( point.x()+1, point.y()+1, id );
      }
      
      //if this pixel has met thresh and has not yet been assigned a unique ID,
      //assign it the next unique id and push all valid neighbors
      if( regionOfInterest[ x + y*regionWidth ] == 1 )
      {
        //print last blob stats
        if( nextValidID > 2)
        {
          blobIDs.push( (nextValidID - 1) );
          blobSizes.push( blobPixelCount );
          blobAspectRatios.push( ((double)(blobBottomRight.x() - blobTopLeft.x()+1)) / 
                                          (blobBottomRight.y() - blobTopLeft.y()+1) );
        }
        
        regionOfInterest[x + y*regionWidth] = nextValidID;
        pushPixel( x-1, y-1, nextValidID );
        pushPixel( x,   y-1, nextValidID );
        pushPixel( x+1, y-1, nextValidID );
        pushPixel( x-1, y, nextValidID );
        pushPixel( x+1, y, nextValidID );
        pushPixel( x-1, y+1, nextValidID );
        pushPixel( x,   y+1, nextValidID );
        pushPixel( x+1, y+1, nextValidID );
        nextValidID++;        
        
        blobPixelCount = 1;
        blobTopLeft = QPoint( x, y );
        blobBottomRight = QPoint( x, y );
      }
    } //y
  } //x
  
  //insert last blob stats
  if( nextValidID > 2)
  {
    blobIDs.push( (nextValidID - 1) );
    blobSizes.push( blobPixelCount );
    blobAspectRatios.push( ((double)(blobBottomRight.x() - blobTopLeft.x()+1)) / (blobBottomRight.y() - blobTopLeft.y()+1) );
  }
}
//==============================================
void RedEye::sortBlobsByDecreasingSize()
{
  blobCount = blobIDs.count();
  ids = new int[blobCount];
  sizes = new int[blobCount];
  ratios = new double[blobCount];
  
  int i,j;
  for(i=0; i<blobCount; i++)
  {
    ids[i] = blobIDs.pop();
    sizes[i] = blobSizes.pop();
    ratios[i] = blobAspectRatios.pop();
  }
  
  //quick and dirty bubble sort
  for(j = blobCount-1; j>0; j--)
  {
    for(i=0; i<j; i++)
    {
      if( sizes[i+1] > sizes[i] )
      {
        int t = sizes[i+1];
        sizes[i+1] = sizes[i];
        sizes[i] = t;
        
        t = ids[i+1];
        ids[i+1] = ids[i];
        ids[i] = t;
        
        double tR = ratios[i+1];
        ratios[i+1] = ratios[i];
        ratios[i] = tR;        
      }
    }
  }
}
//==============================================
void RedEye::findBestTwoBlobs()
{
  id1 = -1;
  id2 = -1;
  int i;
  
  //special case: 2 blobs found, both larger than 1 pixel
  if(blobCount == 2 &&
     sizes[0] > 1 &&
     sizes[1] > 1)
  {
    id1 = ids[0];
    id2 = ids[1];
  }
  else
  {
    for(i=0; i<blobCount-2; i++)
    {
      //once we hit blobs that are only one pixel large stop because they are probably just noise
      if( sizes[i+1] <= 1 ) break;
      
      double as1 = ratios[i];
      double as2 = ratios[i+1];

      if(as1 < 1) as1 = 1.0/as1;
      if(as2 < 1) as2 = 1.0/as2;
      
      if( //both blobs must be semi-circular, prefer those that are wider
          ratios[i] > 0.75 &&   ratios[i] < 2 &&
          ratios[i+1] > 0.75 && ratios[i+1] < 2 &&
          //both blobs must be similar in shape
          QMAX(as2,as1)/QMIN(as2,as1) < 2 &&
          //both blobs must be similar in size
          ((double)QMAX( sizes[i], sizes[i+1] )) / QMIN( sizes[i], sizes[i+1] ) < 1.5 &&
          //both blobs must be above a certain thresh size, this prevents selecting blobs that are very very tiny
          //if only tiny blobs are around we'll end up desaturating entire region
          QMAX( sizes[i], sizes[i+1] ) > 20 )
      {
        id1 = ids[i];
        id2 = ids[i+1];
        break;
      }    
    }
  }
  
  //Comment this sectionin to see what blobs were found and selected
/* cout << "-----\n";
  for(i=0; i<blobCount-1; i++)
  {
    if( ids[i] == id1 || ids[i] == id2 )
      cout << "--->";
    cout << "ID: " << ids[i] << "count: " << sizes[i] << " w:h: " << ratios[i] << "\n";      
  }*/
}
//==============================================
bool RedEye::IDedPixel( int x, int y)
{
  if( x < topLeft.x() || y < topLeft.y() ||
      x > bottomRight.x() || y > bottomRight.y() )
    return false;
  
  int regionIndex = x - topLeft.x() + (y-topLeft.y())*regionWidth;
  return ( regionOfInterest[regionIndex] == id1 ||
           regionOfInterest[regionIndex] == id2 );
}
//==============================================
double RedEye::desaturateAlpha(int x, int y)
{
  int n = 0;
  if( IDedPixel(x  ,y  ) ) n++;
  
  if(n == 1)
    return 1.0;
  
  if( IDedPixel(x-1,y-1) ) n++;
  if( IDedPixel(x  ,y-1) ) n++;
  if( IDedPixel(x+1,y-1) ) n++;
  if( IDedPixel(x-1,y  ) ) n++;
  if( IDedPixel(x+1,y  ) ) n++;
  if( IDedPixel(x-1,y+1) ) n++;
  if( IDedPixel(x  ,y+1) ) n++;
  if( IDedPixel(x+1,y+1) ) n++;
  
  if( IDedPixel(x-2,y-2) ) n++;
  if( IDedPixel(x-1,y-2) ) n++;
  if( IDedPixel(x  ,y-2) ) n++;
  if( IDedPixel(x+1,y-2) ) n++;
  if( IDedPixel(x+2,y-2) ) n++;
  
  if( IDedPixel(x-2,y-1) ) n++;
  if( IDedPixel(x+2,y-1) ) n++;
  if( IDedPixel(x-2,y  ) ) n++;
  if( IDedPixel(x+2,y  ) ) n++;
  if( IDedPixel(x-2,y+1) ) n++;
  if( IDedPixel(x+2,y+1) ) n++;
  
  if( IDedPixel(x-2,y+2) ) n++;
  if( IDedPixel(x-1,y+2) ) n++;
  if( IDedPixel(x  ,y+2) ) n++;
  if( IDedPixel(x+1,y+2) ) n++;
  if( IDedPixel(x+2,y+2) ) n++;
  
  
  return ((double)n) / 25;
}
//==============================================
void RedEye::desaturateBlobs()
{
  //desaturate bad pixels
  int x, y;
  double r;
  QRgb* rgb;
  uchar* scanLine;
  for( y = QMAX( topLeft.y()-1, 0); 
       y<= QMIN( bottomRight.y()+1, editedImage->height()-1 ); 
       y++)
  {
    scanLine = editedImage->scanLine(y);
    for( x =  QMAX( topLeft.x()-1, 0); 
         x <= QMIN( bottomRight.x()+1, editedImage->width()-1 ); 
         x++)
    {      
      double alpha = desaturateAlpha( x, y );
      if( alpha > 0)
      {
        rgb = ((QRgb*)scanLine+x);
        
        r = alpha*(0.05*qRed(*rgb) + 0.6*qGreen(*rgb) + 0.3*qBlue(*rgb)) +
          (1-alpha)*qRed(*rgb);
        *rgb = qRgb( (int)r,
                     qGreen(*rgb),
                     qBlue(*rgb) );
      } //alpha > 0
    } //x
  } //y  
}
//==============================================
void RedEye::desaturateEntireImage(QPoint topLeftExtreme, QPoint bottomRightExtreme)
{
  //desaturate bad pixels
  int x, y;
  QRgb* rgb;
  uchar* scanLine;
  for( y=topLeftExtreme.y(); y<=bottomRightExtreme.y(); y++)
  {
    scanLine = editedImage->scanLine(y);
    for( x=topLeftExtreme.x(); x<=bottomRightExtreme.x(); x++)
    {
      rgb = ((QRgb*)scanLine+x);
      if( qRed(*rgb) > 2*qGreen(*rgb) )
      {
        *rgb = qRgb( (int) (0.05*qRed(*rgb) + 0.6*qGreen(*rgb) + 0.3*qBlue(*rgb)),
                     qGreen(*rgb),
                     qBlue(*rgb) );
      } // > thresh
    } //x
  } //y
}
//==============================================





