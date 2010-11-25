/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-11-01
 * Description : A digital camera RAW files loader for DImg 
 *               framework using an external dcraw instance.
 *
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com> 
 * Copyright (C) 2005-2008 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#ifndef RAWLOADER_H
#define RAWLOADER_H

// LibKDcraw includes.

//TODO: Add RAW support
// #include <libkdcraw/version.h>
// #include <libkdcraw/kdcraw.h>

// Local includes.

#include "dimgloader.h"
#ifdef ST_RAWSUPPORT
	#include "drawdecoding.h"
#endif 
#include "stdigikamexport.h"

namespace Digikam
{
class DImg;

class ST_DIGIKAM_EXPORT RAWLoader 
#ifdef ST_RAWSUPPORT
: public KDcrawIface::KDcraw, public DImgLoader
#else 
: public QObject, public DImgLoader
#endif 
{
    Q_OBJECT

public:

    RAWLoader(DImg* image, DRawDecoding rawDecodingSettings=DRawDecoding());

    bool load(const QString& filePath, DImgLoaderObserver *observer=0);

    // NOTE: RAW files are always Read only.
    bool save(const QString& /*filePath*/, DImgLoaderObserver * /*observer=0*/) { return false; };

    bool hasAlpha()   const { return false;                                  };
    bool isReadOnly() const { return true;                                   };
    bool sixteenBit() const { 
#ifdef ST_RAWSUPPORT
		return m_rawDecodingSettings.sixteenBitsImage; 
#else 
		return false;
#endif 
		};

private:

    // Methods to load RAW image using external dcraw instance.

    bool loadedFromDcraw(QByteArray data, int width, int height, int rgbmax,
                         DImgLoaderObserver *observer);

    bool checkToCancelWaitingData();
    void setWaitingDataProgress(double value);
    void postProcessing(DImgLoaderObserver *observer);

#if KDCRAW_VERSION < 0x000400
    bool checkToCancelReceivingData();
    void setReceivingDataProgress(double value);
#endif

private:

    DImgLoaderObserver *m_observer;
    DRawDecoding        m_customRawSettings;
};

}  // namespace Digikam

#endif /* RAWLOADER_H */
