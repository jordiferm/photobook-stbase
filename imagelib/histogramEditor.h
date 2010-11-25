//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef GUI_EDITING_HISTOGRAMEDITOR_H
#define GUI_EDITING_HISTOGRAMEDITOR_H
#include "stimagelibexport.h"

//--------------------
//forward declarations
class QFrame;
class QComboBox;
class ClickableLabel;
class DynamicSlider;
class HistogramInterface;
class ScaledPreviewInterface;
//--------------------

#include <qdialog.h>

//=====================================
/*! \brief Interface for adjusting image historgram using 
 upper and lower bounds and bright and contrast controls. */
//=====================================

class ST_IMAGELIB_EXPORT HistogramEditor : public QDialog
{
Q_OBJECT

public:
  ///Constructs layout
  HistogramEditor(QString filename,
                  QWidget *parent=0, const char* name=0);

  ~HistogramEditor();
  
  ///get modified image that resulted from adjustments
  QImage* getModifiedImage();

  ///returns histogram boundaries
  void getHistBoundaries(int &lumLeft, int &lumRight,
                         int &redLeft, int &redRight,
                         int &greenLeft, int &greenRight,
                         int &blueLeft, int &blueRight);                         
  
	//scale between two colors
  static double scaleColor( double color, int left, int right );
	
	//----------------------
protected:
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  //----------------------
private slots:
  ///updates preview image
  void selectPreviewImageType(int selection);

  ///updates historgram interface
  void selectHistogramType(int selection);    

  ///reset brightness
  void resetBrightness();

  ///reset contrast
  void resetContrast();
  
  ///check for changes to settings, if so 
  //accepts changes, otherwise calls reject
  void applyAction();  
  
  ///resets all adjustments
  void resetAction();

  ///updates adjusted preview image
  void generateAdjustedPreviewImage();
  //----------------------
private:
  //adjust image using current contrast, brightness, and histogram settings
  void adjustImage( QImage& image );

  //original image filename
  QString fileName;
    
  ///Mean color values
  double meanR, meanG, meanB;
  
  ///Preview image
  ScaledPreviewInterface* previewInterface;
    
  ///Histogram view
  HistogramInterface* histogramInterface;
      
  QComboBox* previewSelection;
  QComboBox* histogramType;
  ClickableLabel* brightnessIcon;
  ClickableLabel* contrastIcon;
  QFrame* buttonsFrame;  
  
  
  ///Brightness slider
  DynamicSlider* brightness;

  ///Contrast slider
  DynamicSlider* contrast;
};
//======================

#endif //GUI_EDITING_HISTOGRAMEDITOR_H
