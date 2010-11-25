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
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qslider.h>
#include <qtooltip.h>
#include <qsizegrip.h>
#include <QKeyEvent>

//Projectwide includes
#include "histogramEditor.h"
#include "scaledPreviewInterface.h"
#include "histogramInterface.h"
#include "clickableLabel.h"
#include "dynamicSlider.h"
#include "imageTools.h"

#define SLIDER_RADIUS 40

//==============================================
HistogramEditor::HistogramEditor( QString fileName, QWidget *parent, const char* name ) : QDialog(parent,name,true)
{
  //set pointer to null to make sure no 
  //random data is ever accessed by the preview interface
  histogramInterface = NULL;

  //record filename
  this->fileName = fileName;
  
  //-----
  //find mean color values
  meanR = 0;
  meanG = 0;
  meanB = 0;
  int x, y;
  QRgb* rgb;
  uchar* scanLine;
  QImage image = QImage( fileName );
  for( y=0; y<image.height(); y++)
  {   
    scanLine = image.scanLine(y);
    for( x=0; x<image.width(); x++)
    {
      rgb = ((QRgb*)scanLine+x);
      double r = ((double)qRed(*rgb)   )/255.0;
      double g = ((double)qGreen(*rgb) )/255.0;
      double b = ((double)qBlue(*rgb)  )/255.0;
      
      meanR+=r;
      meanG+=g;
      meanB+=b;
    } //x
  } //y
  meanR = meanR / ( image.width() * image.height() );
  meanG = meanG / ( image.width() * image.height() );
  meanB = meanB / ( image.width() * image.height() );
  
  QFrame* visibleFrame = new QFrame( this, "visible widgets" );
  //--------------  
  //Preview frame: 
  previewInterface = new ScaledPreviewInterface( fileName, visibleFrame, 
                                                 "previewInterface" );
  connect( previewInterface, SIGNAL(resized()), 
           this, SLOT(generateAdjustedPreviewImage()) );  

  previewSelection = new QComboBox( visibleFrame, "previewSelction" );
  previewSelection->insertItem( tr("Split View") );
  previewSelection->insertItem( tr("Original Image") );
  previewSelection->insertItem( tr("Adjusted Image") );
  connect( previewSelection, SIGNAL(activated(int)), this, SLOT(selectPreviewImageType(int)) );  
  //--------------
  //Adjust frame:   
  histogramInterface = new HistogramInterface( fileName, visibleFrame );  

  //connect adjustments in histogram to generateAdjustedPreviewImage
  connect( histogramInterface, SIGNAL( selectedRangeChanged() ),
           SLOT( generateAdjustedPreviewImage() ) );
  
  QToolTip::add( histogramInterface, tr("Click and drag to select tonal range") );  

  histogramType = new QComboBox( visibleFrame, "histogramType" );
  histogramType->insertItem( tr("Luminosity") );
  histogramType->insertItem( tr("Red") );
  histogramType->insertItem( tr("Green") );
  histogramType->insertItem( tr("Blue") );  
  connect( histogramType, SIGNAL(activated(int)), this, SLOT(selectHistogramType(int)) );  
  QToolTip::add( histogramType, tr("Histogram channel displayed") );  
  //--------------
  //Slider frame:
  QString noChange = QString( tr("No change") );
  
  brightness = new DynamicSlider( Qt::Vertical, visibleFrame );
  brightness->setZeroString( noChange );
  brightness->setPrefixes("", "+");
  brightness->setMinValue( -SLIDER_RADIUS );
  brightness->setMaxValue( SLIDER_RADIUS );
  connect( brightness, SIGNAL(valueChanged(int)),
           this, SLOT(generateAdjustedPreviewImage()) );;
  QToolTip::add( brightness, tr("Drag to adjust image brightness") );  
           
  brightnessIcon = new ClickableLabel( visibleFrame );
  brightnessIcon->setPixmap( QPixmap(QString::fromUtf8(":/brightness.png")));
  connect( brightnessIcon, SIGNAL(clicked()), SLOT(resetBrightness()) );    
  QToolTip::add( brightnessIcon, tr("Reset brightness") );

  contrast = new DynamicSlider( Qt::Vertical, visibleFrame );
  contrast->setZeroString( noChange );
  contrast->setPrefixes("", "+");
  contrast->setMinValue( -SLIDER_RADIUS );
  contrast->setMaxValue( SLIDER_RADIUS );
  connect( contrast, SIGNAL(valueChanged(int)),
           this, SLOT(generateAdjustedPreviewImage()) );
  QToolTip::add( contrast, tr("Drag to adjust image contrast") );  

  contrastIcon = new ClickableLabel( visibleFrame );
  contrastIcon->setPixmap( QPixmap(QString::fromUtf8(":/contrast.png")));
  connect( contrastIcon, SIGNAL(clicked()), SLOT(resetContrast()) );    
  QToolTip::add( contrastIcon, tr("Reset contrast") );
  //--------------
  //Dialog buttons:  
  buttonsFrame =   new QFrame( visibleFrame, "dialogButtons" );

  QPushButton* applyButton = new QPushButton( tr("Apply"), buttonsFrame );
  applyButton->setDefault(true);
  applyButton->setFocus();
  connect( applyButton, SIGNAL(clicked()), SLOT(applyAction()) );
                                
  QPushButton* cancelButton = new QPushButton( tr("Cancel"), buttonsFrame );
  connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );

  QPushButton* resetButton = new QPushButton( tr("Reset"), buttonsFrame );
  connect( resetButton, SIGNAL(clicked()), SLOT(resetAction()) );

  QGridLayout* buttonsGrid = new QGridLayout( buttonsFrame );
  buttonsGrid->setColStretch( 0, 1 );
  buttonsGrid->addWidget( applyButton,  0, 1 );
  buttonsGrid->addWidget( cancelButton, 0, 2 );
  buttonsGrid->addWidget( resetButton, 0, 3 );
  buttonsGrid->setColStretch( 4, 1 );  
//  buttonsGrid->setSpacing( WIDGET_SPACING );
  //--------------
  QGridLayout* mainGrid = new QGridLayout( visibleFrame, 5, 3, 0 );
  
  mainGrid->addMultiCellWidget( previewInterface,     0,0, 0,2 );
  mainGrid->addMultiCellWidget( previewSelection, 1,1, 0,2, Qt::AlignHCenter );  
   
  mainGrid->addWidget( histogramInterface,    2, 0 ); 
  mainGrid->addWidget( brightness,            2, 1 );
  mainGrid->addWidget( contrast,              2, 2 );
  
  //make sure sliders have enough space so all slider units are settable
  mainGrid->setRowSpacing( 2, 2*SLIDER_RADIUS + 5) ;
    
  mainGrid->addWidget( histogramType,    3, 0, Qt::AlignHCenter );
  mainGrid->addWidget( brightnessIcon,   3, 1 );  
  mainGrid->addWidget( contrastIcon,     3, 2 );  
  
  mainGrid->addMultiCellWidget( buttonsFrame, 4,4, 0,2 );

  mainGrid->setRowStretch( 0, 1 );
  mainGrid->setColStretch( 0, 1 );
  
//  mainGrid->setSpacing( WIDGET_SPACING );
//  mainGrid->setMargin( WIDGET_SPACING );
  
  QGridLayout* invisibleGrid = new QGridLayout( this, 2, 1, 0 );
  invisibleGrid->addWidget( visibleFrame, 0, 0 );
  invisibleGrid->setRowStretch( 0, 1 );
  
  //PLATFORM_SPECIFIC_CODE
  //windows users expect a grip, but qt doesn't put one in by default. we'll add
  //it for them too. :-)
#if defined(Q_OS_WIN)
  QSizeGrip* sizeGrip = new QSizeGrip( this );
  invisibleGrid->addWidget( sizeGrip, 1, 0, Qt::AlignRight | Qt::AlignBottom );
#endif
  
  
  
  
  //Window caption
  setCaption( tr("Histogram Editor") );
  //-------------------------------
}
//==============================================
HistogramEditor::~HistogramEditor() { }
//==============================================
void HistogramEditor::applyAction()
{
  //check if user has adjusted brightness, contrast, or histogram ranges.
  //if any changes have taken place call "accept", else "reject" so image is not
  //updated and appear modified
  int lumLeft, lumRight, redLeft, redRight, greenLeft, greenRight, blueLeft, blueRight;
  histogramInterface->getHistBoundaries( lumLeft, lumRight,
                                         redLeft, redRight,
                                         greenLeft, greenRight,
                                         blueLeft, blueRight );  
  if( brightness->value() != 0 || contrast->value()   != 0 ||
      lumLeft != 0 || lumRight != 255 ||
      redLeft !=0 || redRight != 255 ||
      greenLeft != 0 || greenRight != 255 ||
      blueLeft != 0 || blueRight != 255 )
  { accept(); }
  else
  { reject(); }  
}
//==============================================
void HistogramEditor::resetAction()
{
  histogramInterface->resetBoundaries();
  resetBrightness();
  resetContrast();
}
//==============================================
QImage* HistogramEditor::getModifiedImage()
{ 
  QImage* adjustedImage = new QImage(fileName);
  
  //convert to 32-bit depth if necessary
  if( adjustedImage->depth() < 32 )
  {
    QImage* tmp = adjustedImage;
    adjustedImage = new QImage( tmp->convertDepth( 32, Qt::AutoColor ) );
    delete tmp; tmp=NULL;
  }
  
  adjustImage( *adjustedImage );
  return adjustedImage;  
}
//==============================================
void HistogramEditor::selectPreviewImageType( int selection )
{
  previewInterface->setPreviewMode( (PREVIEW_MODE)selection );
}
//==============================================
void HistogramEditor::selectHistogramType( int selection )
{
  histogramInterface->setDisplayChannel( (DISPLAYED_CHANNEL) selection );
}
//==============================================
void HistogramEditor::resetBrightness()
{ brightness->setValue( 0 ); }
//==============================================
void HistogramEditor::resetContrast()
{ contrast->setValue( 0 ); }
//==============================================
void HistogramEditor::getHistBoundaries(int &lumLeft, int &lumRight,
                                        int &redLeft, int &redRight,
                                        int &greenLeft, int &greenRight,
                                        int &blueLeft, int &blueRight)
{
  //sanity check
  if( histogramInterface )
  {
    histogramInterface->getHistBoundaries( lumLeft, lumRight,
                                           redLeft, redRight,
                                           greenLeft, greenRight,
                                           blueLeft, blueRight );
  }
  else
  {
    lumLeft = 0; lumRight = 255;
    redLeft = 0; redRight = 255;
    greenLeft = 0; greenRight = 255;
    blueLeft = 0; blueRight = 255;
  }
}
//==============================================
void HistogramEditor::generateAdjustedPreviewImage()
{
  //get original image
  QImage origImage = previewInterface->getOrigImage();

  //construct adjusted image
  QImage adjustedImage = origImage.copy();
  adjustImage( adjustedImage );
  
  //set adjusted image
  previewInterface->setAdjustedImage( adjustedImage );
}
//==============================================
void HistogramEditor::adjustImage( QImage &image )
{
  //obtain histogram left and right boundaries
  //sanity check
  int lumLeft, lumRight, redLeft, redRight, greenLeft, greenRight, blueLeft, blueRight;
  if( histogramInterface )
  {
    histogramInterface->getHistBoundaries( lumLeft, lumRight,
                                           redLeft, redRight,
                                           greenLeft, greenRight,
                                           blueLeft, blueRight );
  }
  else
  {
    lumLeft   = 0; lumRight   = 255;
    redLeft   = 0; redRight   = 255;
    greenLeft = 0; greenRight = 255;
    blueLeft  = 0; blueRight  = 255;
  }
  
  //modify image
  double displayToOneScalar = 1.0/255.0;
  double scaledMeanR = displayToOneScalar*scaleColor( 255.0*meanR, redLeft, redRight );
  double scaledMeanG = displayToOneScalar*scaleColor( 255.0*meanG, greenLeft, greenRight );
  double scaledMeanB = displayToOneScalar*scaleColor( 255.0*meanB, blueLeft, blueRight );
  
  double brightnessScalar, addedBrightnessColor;
  if(brightness->value() < 0)
  {
    brightnessScalar = ((double)(SLIDER_RADIUS + brightness->value()))/SLIDER_RADIUS;
    addedBrightnessColor = 1.0 - brightnessScalar;
  }
  else
  {
    brightnessScalar = ((double)(SLIDER_RADIUS - brightness->value()))/SLIDER_RADIUS;
    addedBrightnessColor = 0.0;
  }
  
  int x, y;  
  QRgb* rgb;
  double r,g,b;
  double h,s,v;
  int rPrime, gPrime, bPrime;
  uchar* scanLine;
  
  for( y=0; y<image.height(); y++)
  {   
    scanLine = image.scanLine(y);
    for( x=0; x<image.width(); x++)
    {
      //get rgb value
      rgb = ((QRgb*)scanLine+x);
      r = qRed(*rgb);
      g = qGreen(*rgb);
      b = qBlue(*rgb);
      
      //apply histogram boundaries
      RGBtoHSV(r,g,b,&h,&s,&v);
      v = scaleColor( v, lumLeft, lumRight );
      HSVtoRGB( &r,&g,&b, h,s,v);         
      
      r = scaleColor( r, redLeft, redRight );
      g = scaleColor( g, greenLeft, greenRight );
      b = scaleColor( b, blueLeft, blueRight );
      
      //convert to 0-1 scale
      r = r*displayToOneScalar;
      g = g*displayToOneScalar;
      b = b*displayToOneScalar;
      
      //adjust contrast
      r = ( (r-scaledMeanR) * (SLIDER_RADIUS-contrast->value()) )/SLIDER_RADIUS + scaledMeanR;
      g = ( (g-scaledMeanG) * (SLIDER_RADIUS-contrast->value()) )/SLIDER_RADIUS + scaledMeanG;
      b = ( (b-scaledMeanB) * (SLIDER_RADIUS-contrast->value()) )/SLIDER_RADIUS + scaledMeanB;
      
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
//==============================================
double HistogramEditor::scaleColor( double color, int left, int right )
{
  return QMAX( QMIN( (int)((255.0*(color-left)) / (right-left)), 255), 0 );  
}
//==============================================
void HistogramEditor::keyPressEvent(QKeyEvent *e)
{
  if(e->key() == Qt::Key_Control )
  {
    PREVIEW_MODE curMode = (PREVIEW_MODE) previewSelection->currentItem();
    if(curMode == ORIGINAL_IMAGE)
      previewInterface->setPreviewMode( ADJUSTED_IMAGE, true );
    else if(curMode == ADJUSTED_IMAGE)
      previewInterface->setPreviewMode( ORIGINAL_IMAGE, true );
    else
      previewInterface->setPreviewMode( INV_SPLIT_VIEW );
  }
  else { QDialog::keyPressEvent(e); }
}
//==============================================
void HistogramEditor::keyReleaseEvent(QKeyEvent *e)
{
  if(e->key() == Qt::Key_Control )
  {
    previewInterface->setPreviewMode( (PREVIEW_MODE) previewSelection->currentItem(), 
                                      false );
  }
  else { QDialog::keyReleaseEvent(e); }
}
//==============================================


