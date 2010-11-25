/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-14
 * Description : A JPEG2000 IO file for DImg framework
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef JP2KLOADER_H
#define JP2KLOADER_H

// C ANSI includes.

#ifdef ST_JPEG2000SUPPORT
	#ifndef Q_CC_MSVC
	extern "C"
	{
	#endif
	#include <jasper/jasper.h>
	#ifndef Q_CC_MSVC
	}
	#endif
#endif 
// Local includes.

#include "dimgloader.h"
#include "stdigikamexport.h"

namespace Digikam
{
class DImg;

class ST_DIGIKAM_EXPORT JP2KLoader : public DImgLoader
{

public:

    JP2KLoader(DImg* image);

    bool load(const QString& filePath, DImgLoaderObserver *observer);
    bool save(const QString& filePath, DImgLoaderObserver *observer);

    virtual bool hasAlpha()   const;
    virtual bool sixteenBit() const;
    virtual bool isReadOnly() const { return false; };

private:

    bool m_sixteenBit;
    bool m_hasAlpha;
};

}  // namespace Digikam

#endif /* JP2KLOADER_H */
