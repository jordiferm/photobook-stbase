/* __________________________________________________________________________
//                        .__.
//       S^3.O.S.  _.--^^ ___ \
//             _../^   _.^   ^\!
//          _./     .-^    -_. |
//      _-/^     .-^        > \
//     /^--..   /  /|     ./   \._
//           ^^/  / ^_-./^        ^.__        >-.-. www.s3os.net 
//               <_____..--^^^---...__^..__..
//               Solucions i Serveis amb Sistemes Open Source
// __________________________________________________________________________
//
//    Revision             :- $Revision: 1.1.1.1 $
//    Init Date            : Fri Mar 2 2007
//    licence              : GNU/GPL-2
//    programmer           : Jordi Fern?dez, (C) 2007
//    email                : shadow@softwarelliure.com
//    File                 : imimage.cpp
//    Classification       :- C++ (Gcc 3.xx) Qt 4.xx
//                                                                    
// _________________________________________________________________________
//                                                                         
//   This program is free software; you can redistribute it and/or modify  
//   it under the terms of the GNU General Public License as published by  
//   the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
// _________________________________________________________________________
// ------------------------------------ CVS --------------------------------
// $Header: /home/cvs/s3os/swaiter2/templates/cpp,v 1.1.1.1 2006/03/10 14:03:54 shadow Exp $
// _________________________________________________________________________*/
#include "imimage.h"
#include <QImage> 
#include <QColor>

QImage IMImage::image()
{
  //std::cout << " IMImage::image()" << columns() << " " << rows() << std::endl;
  QImage img(columns(), rows(), QImage::Format_RGB32);
  const Magick::PixelPacket *pixels;
  Magick::ColorRGB rgb;
  for (int y = 0; y < img.height(); y++) {
    pixels = getConstPixels(0, y, img.width(), 1);
    for (int x = 0; x < img.width(); x++) {
      rgb = (*(pixels + x));
      img.setPixel(x, y, QColor((int) (255 * rgb.red())
                                , (int) (255 * rgb.green())
                                , (int) (255 * rgb.blue())).rgb());
    }
  }
  return img;
}

IMImage::IMImage(const IMImage& img):Image(img)
{
}


IMImage::IMImage(const QImage& img):Image(Magick::Geometry(img.width(), img.height()), Magick::ColorRGB(0.5, 0.2, 0.3))
{
  double scale = 1 / 256.0;
  modifyImage();
  Magick::PixelPacket *pixels;
  Magick::ColorRGB mgc;
  for (int y = 0; y < img.height(); y++) {
    pixels = setPixels(0, y, columns(), 1);
    for (int x = 0; x < img.width(); x++) {
      QColor pix = img.pixel(x, y);
      //      *pixels++ = Magick::ColorRGB(256 * pix.red(), 256 * pix.green(), 256 * pix.blue());
      mgc.red(scale *pix.red());
      mgc.green(scale *pix.green());
      mgc.blue(scale *pix.blue());
      //      *pixels++ = Magick::ColorRGB(scale *pix.red(), scale * pix.green(), scale * pix.blue());
      *pixels++ = mgc;
    }
    syncPixels();
  }
}

void IMImage::read(const char *f)
{
  Image i;
  try {
    i.read(f);
  }
  catch (...) {
    // OK, something wrong, sorting out later
  }
  *this = i;
}

