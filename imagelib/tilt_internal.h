//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

#ifndef BACKEND_ENHANCEMENTS_TILT_INTERNAL_H
#define BACKEND_ENHANCEMENTS_TILT_INTERNAL_H

//In orded to prevent roundoff error from propogating
//across multiple pixel computations I've created
//my own DPoint class that resembles a QPoint but uses doubles instead.
class DPoint
{
public: 
  DPoint();
  DPoint( double x, double y );
  double x() const;
  double y() const;
private:
    double xpos, ypos;
};

//finds the 2-dimensionally interpolated pixel color
//using the supplied double coordinates and image object
QRgb interpolatedPixelValue( double xp, double yp, QImage* image );

//blends two rgb colors using a weight between 0-1, used by interpolate function
QRgb blendColors( QRgb color1, QRgb color2, double alpha );

//finds the intersection between two lines described by p1-p2 and p3-p4
DPoint findTwoLineIntersection(DPoint p1, DPoint p2, DPoint p3, DPoint p4);

#endif //BACKEND_ENHANCEMENTS_TILT_INTERNAL_H
