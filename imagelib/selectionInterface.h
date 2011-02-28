//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef GUI_EDITING_SELECTIONINTERFACE_H
#define GUI_EDITING_SELECTIONINTERFACE_H
#include "stimagelibexport.h"

///current drag mode, effect of mouse movement on current selection
typedef enum
{
    DRAW_SELECTION,
    MOVE_SELECTION,
    MOVE_TOP_LEFT_CORNER,
    MOVE_TOP_RIGHT_CORNER,
    MOVE_BOTTOM_LEFT_CORNER,
    MOVE_BOTTOM_RIGHT_CORNER,
    MOVE_LEFT_SIDE,
    MOVE_RIGHT_SIDE,
    MOVE_TOP_SIDE,
    MOVE_BOTTOM_SIDE,
    SCALE_SELECTION,
    DRAW_LINE,
    NO_EFFECT
} DRAG_MODE;

//--------------------
//forward declarations
class QPoint;
class QImage;
//--------------------

#include <QLabel>
#include <qimage.h>

//=====================================
/*! \brief Display widget for photos. Used by the PhotoEditWidget.
 */
//=====================================
class ST_IMAGELIB_EXPORT SelectionInterface : public QLabel
{
	Q_OBJECT

public:
	///Creates layout
	SelectionInterface(QWidget *parent=0);
	void resetValues();

	///Deletes objects
	~SelectionInterface();

	///Updates displayed photo
	void setPhoto(const QImage& img, bool resetSelection=true);
	void setPhoto(QString imageFilename, bool resetSelection=true);

	QRect getSelectionRect() const;
	///Returns the current selected coordinates (actual slideshow image
	///space, aka not including buffered whitespace or widget offsets)
	void getSelection(QPoint &topLeft, QPoint &bottomRight) const;
	//! \return the current cropMaxDimen \sa setSelection
	double getCropMaxDimen() { return cropMaxDimen; }
	///Sets the current selection
	///cropMaxDimen specifies the idealized dimension in dominant direction in inches (eg 7")
	void setSelection(QPoint topLeft, QPoint bottomRight, double cropMaxDimen=-1.0);
	//! _AspectRatio must be in pixels.
	void selectMaxByAspectRatio(const QSizeF _AspectRatio);

	///returns true if selection is empty
	bool selectionEmpty();

	///returns the current photo display size (in screen pixels)
	void getDisplaySize(int &width, int &height);

	///enter draw line mode - used for tilt correction
	void enterDrawLineMode();

	///Aplica el nivell de brillo i contrast al preview. TODO: Treure histogram del projecte o posar-ho a part.
	static void adjustBrightAndContrast( QImage* _img, int _bright, int _contrast);

	void adjustGamma(QImage& _Image, int _Gamma);
	void adjustGammaRGB(QImage& _Image, int _GammaR, int _GammaG, int _GammaB );

	//! Indica si es permet dibuixar seleccions. (Per defecte false)
	void setDrawSelectionAllowed(bool _Value) { DrawSelectionAllowed = _Value; }

	//! Size of original image.
	QSize imageSize() { return origImageSize; }

	void setMinDpis(int _Value) {   MinDpiSettings = _Value; }
	int minDpis() const { return MinDpiSettings; }

	//----------------------
signals:
	///emitted when a user CTRL-clicks a selection indicating
	///the selection needs to be rotated intelligently
	void ctrlClick();

	///emitted when the user changed the selected region
	void selectionChanged();

	///emitted when the user changed the aspect ratio of the selected region
	void aspectRatioChanged();

	///emitted once line has been selected, on or the other points
	///will be set to -1,-1 if the user escaped out the selected a line
	void lineSelected( QPoint p1, QPoint p2 );
	
	//! Emitted when resizeEvent is called.
	void resized(QResizeEvent* _Event);
	
	//----------------------
protected:
	QSize sizeHint();
	void paintEvent( QPaintEvent *e);
	void mousePressEvent( QMouseEvent *e);
	void mouseReleaseEvent( QMouseEvent *);
	void mouseMoveEvent( QMouseEvent *e);
	void resizeEvent( QResizeEvent * );
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	//----------------------
public slots:
	///selects none of the image
	void selectNone();

	///selects all of the image
	void selectAll();

	///Canvia el brillo i contrast del preview.
	void slotChangeBrightAndContrast ( int _bright, int _contrast );
	void slotChangeGamma(int _Gamma);
	void slotChangeGammaRGB(int _GammaR, int _GammaG, int _GammaB);

	void slotChangeBright(int _bright);
	void slotChangeContrast(int _contrast);

	
	//----------------------
private:

	///determine action based on mouse position
	DRAG_MODE mouseActionByPosition(QPoint p);

	///crops a selected point to within the photo
	QPoint cropSelectedPoint(QPoint p);

	///converts a point from display coordinates to original image coordinates
	QPoint ConvertDisplayToImageCoordinate( QPoint p );

	///converts a point from original image coordinates to display coordinates
	QPoint ConvertImageToDisplayCoordinate( QPoint p );

	///construct scaled image and unselected images for drawing purposes
	void constructDisplayImages();

	///update mouse cursor based on position over widget and selected region
	void updateCursorShape( QPoint p );

	///increase/decrease selection while maintaining aspect ratio by changing selected width by delta
	bool scaleSelection( int delta );

	///Full screen version of image
	QImage fullScreenImage;

	///Scaled image used for display purposes
	QImage scaledImage;

	///Last scaled image, used as original for apply brightness.
	QImage LastScaledImage;

	///Grayscale version of scaled image, used for drawing non-selected regions
	QImage unselectedScaledImage;

	///first corner of selection, where mouse first clicked
	QPoint mousePressPoint;

	///second corner of selection, where mouse moved to
	QPoint mouseDragPoint;

	///cached mouse position, used to scale or drag around selection area
	QPoint cachedMousePosition;

	///method of dragging out, either new selection or resizing selection corner or side
	DRAG_MODE currentDragMode;

	///current mouse shape. by caching this value we avoid
	///resetting the mouse cursor every time it moves etc.
	DRAG_MODE currentMouseShape;

	///original image dimensions
	QSize origImageSize;

	///state of SHIFT button, effects if mouse drags adjust or scale the current selection
	bool SHIFT_Pressed;

	///state of CTRL button, effects if mouse clicks rotate current selection
	bool CTRL_Pressed;

	///cache the aspect ratio when pressing the control buttion, this helps aleviate
	///numerical error that builds up when scaling up and down selections
	QPoint cachedSelctionCenter;
	QSize cachedSelectionSize;

	///Current crop max dimension (in inches)
	double cropMaxDimen;

	int Bright;
	int Contrast;
	//----------------------
	bool DrawSelectionAllowed;
	int MinDpiSettings;
};
//======================

#endif //GUI_EDITING_SELECTIONINTERFACE_H
