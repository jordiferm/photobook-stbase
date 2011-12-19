/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Starblitz reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef IMIMAGE_H
#define IMIMAGE_H
#include "Magick++.h"
#include "stimagelibexport.h"


/**
ImageMagick Image class.

	@author Jordi Fernàndez <shadow@softwarelliure.com>
*/

using namespace Magick;

class QImage;
 
class ST_IMAGELIB_EXPORT IMImage: public Magick::Image {
 public:
  IMImage():Image(Magick::Geometry(1,1), Magick::ColorRGB(0,0,0)){}
  IMImage(const QImage&);
  IMImage(const Image& img):Image(img){}
  IMImage(const IMImage&);
  ~IMImage(){}
  QImage image();
  void read(const char* f);
  //FIXME: There's no implementation !???
  void image(const QImage&);
};

#endif
