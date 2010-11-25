/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-07-21
 * Description : image histogram manipulation methods.
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Some code parts are inspired from gimp 2.0
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
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

#include "imagehistogram.h"

// C++ includes.

#include <cstdio>
#include <cmath>
#include <cstring>

// Qt includes.

#include <QObject>
#include <QtDebug> 


// Local includes.

#include "dimg.h"

namespace Digikam
{

class ImageHistogramPriv
{

public:

    // Using a structure instead a class is more fast
    // (access with memset() and bytes manipulation).

    struct double_packet
    {
        double value;
        double red;
        double green;
        double blue;
        double alpha;
    };

public:

    ImageHistogramPriv()
    {
        imageData   = 0;
        histogram   = 0;
        valid       = false;
        runningFlag = true;
    }

    /** The histogram data.*/
    struct double_packet *histogram;
    bool                  valid;

    /** Image information.*/
    uchar   *imageData;
    uint     imageWidth;
    uint     imageHeight;

    /** Numbers of histogram segments depending of image bytes depth*/
    int      histoSegments;

    /** Used to stop thread during calculations.*/
    bool     runningFlag;
};

ImageHistogram::ImageHistogram(const DImg& image, QObject *parent)
              : QThread(parent), d(new ImageHistogramPriv)
{
    setup(image.bits(), image.width(), image.height(), image.sixteenBit());
}

ImageHistogram::ImageHistogram(uchar *i_data, uint i_w, uint i_h, bool i_sixteenBits, QObject *parent)
              : QThread(parent), d(new ImageHistogramPriv)
{
    setup(i_data, i_w, i_h, i_sixteenBits);
}

void ImageHistogram::setup(uchar *i_data, uint i_w, uint i_h, bool i_sixteenBits)
{
    d->imageData     = i_data;
    d->imageWidth    = i_w;
    d->imageHeight   = i_h;
    d->histoSegments = i_sixteenBits ? 65536 : 256;
}

ImageHistogram::~ImageHistogram()
{
    stopCalculation();

    if (d->histogram)
       delete [] d->histogram;

    delete d;
}

int ImageHistogram::getHistogramSegments(void)
{
    return d->histoSegments;
}

void ImageHistogram::calculateInThread()
{
    // this is done in an extra method and not in the constructor
    // to allow to connect to the signals, which is only possible after construction
    if (d->imageData && d->imageWidth && d->imageHeight)
    {
        start();
    }
    else
    {
       emit calculationFinished(this, false);
    }
}

void ImageHistogram::stopCalculation()
{
    d->runningFlag = false;
    wait();
}

bool ImageHistogram::isValid()
{
    return d->valid;
}

bool ImageHistogram::isCalculating()
{
    return isRunning();
}

// List of threaded operations.

void ImageHistogram::run()
{
    calculate();
}

void ImageHistogram::calculate()
{
    if (!d->imageData || !d->imageWidth || !d->imageHeight)
    {
        emit calculationFinished(this, false);
        return;
    }

    register uint  i;
    int            max;

    emit calculationStarted(this);

    d->histogram = new ImageHistogramPriv::double_packet[d->histoSegments];

    if ( !d->histogram )
    {
        qWarning() << ("HistogramWidget::calcHistogramValues: Unable to allocate memory!") << endl;
        emit calculationFinished(this, false);
        return;
    }

    memset(d->histogram, 0, d->histoSegments*sizeof(struct ImageHistogramPriv::double_packet));

    if (d->histoSegments == 65536)         // 16 bits image.
    {
        unsigned short  blue, green, red, alpha;
        unsigned short *data = (unsigned short*)d->imageData;

        for (i = 0 ; (i < d->imageHeight*d->imageWidth*4) && d->runningFlag ; i+=4)
        {
            blue  = data[ i ];
            green = data[i+1];
            red   = data[i+2];
            alpha = data[i+3];

            d->histogram[blue].blue++;
            d->histogram[green].green++;
            d->histogram[red].red++;
            d->histogram[alpha].alpha++;

            max = (blue > green) ? blue : green;

            if (red > max)
                d->histogram[red].value++;
            else
                d->histogram[max].value++;
        }
    }
    else                                  // 8 bits images.
    {
        uchar blue, green, red, alpha;
        uchar *data = d->imageData;

        for (i = 0 ; (i < d->imageHeight*d->imageWidth*4) && d->runningFlag ; i+=4)
        {
            blue  = data[ i ];
            green = data[i+1];
            red   = data[i+2];
            alpha = data[i+3];

            d->histogram[blue].blue++;
            d->histogram[green].green++;
            d->histogram[red].red++;
            d->histogram[alpha].alpha++;

            max = (blue > green) ? blue : green;

            if (red > max)
                d->histogram[red].value++;
            else
                d->histogram[max].value++;
        }
    }

    if (d->runningFlag)
    {
        d->valid = true;
        emit calculationFinished(this, true);
    }
}

double ImageHistogram::getCount(int channel, int start, int end)
{
    int    i;
    double count = 0.0;

    if ( !d->histogram || start < 0 ||
        end > d->histoSegments-1 || start > end )
        return 0.0;

    switch(channel)
    {
    case ImageHistogram::ValueChannel:
        for (i = start ; i <= end ; i++)
            count += d->histogram[i].value;
        break;

    case ImageHistogram::RedChannel:
        for (i = start ; i <= end ; i++)
            count += d->histogram[i].red;
        break;

    case ImageHistogram::GreenChannel:
        for (i = start ; i <= end ; i++)
            count += d->histogram[i].green;
        break;

    case ImageHistogram::BlueChannel:
        for (i = start ; i <= end ; i++)
            count += d->histogram[i].blue;
        break;

    case ImageHistogram::AlphaChannel:
        for (i = start ; i <= end ; i++)
            count += d->histogram[i].alpha;
        break;

    default:
        return 0.0;
        break;
    }

  return count;
}

double ImageHistogram::getPixels()
{
  if ( !d->histogram )
       return 0.0;

  return(d->imageWidth * d->imageHeight);
}

double ImageHistogram::getMean(int channel, int start, int end)
{
    int    i;
    double mean = 0.0;
    double count;

    if ( !d->histogram || start < 0 ||
        end > d->histoSegments-1 || start > end )
        return 0.0;

    switch(channel)
    {
        case ImageHistogram::ValueChannel:
            for (i = start ; i <= end ; i++)
                mean += i * d->histogram[i].value;
            break;

        case ImageHistogram::RedChannel:
            for (i = start ; i <= end ; i++)
                mean += i * d->histogram[i].red;
            break;

        case ImageHistogram::GreenChannel:
            for (i = start ; i <= end ; i++)
                mean += i * d->histogram[i].green;
            break;

        case ImageHistogram::BlueChannel:
            for (i = start ; i <= end ; i++)
                mean += i * d->histogram[i].blue;
            break;

        case ImageHistogram::AlphaChannel:
            for (i = start ; i <= end ; i++)
                mean += i * d->histogram[i].alpha;
            break;

        default:
            return 0.0;
            break;
    }

    count = getCount(channel, start, end);

    if (count > 0.0)
        return mean / count;

    return mean;
}

int ImageHistogram::getMedian(int channel, int start, int end)
{
    int    i;
    double sum = 0.0;
    double count;

    if ( !d->histogram || start < 0 ||
        end > d->histoSegments-1 || start > end )
        return 0;

    count = getCount(channel, start, end);

    switch(channel)
    {
        case ImageHistogram::ValueChannel:
            for (i = start ; i <= end ; i++)
            {
                sum += d->histogram[i].value;
                if (sum * 2 > count) return i;
            }
            break;

        case ImageHistogram::RedChannel:
            for (i = start ; i <= end ; i++)
            {
                sum += d->histogram[i].red;
                if (sum * 2 > count) return i;
            }
            break;

        case ImageHistogram::GreenChannel:
            for (i = start ; i <= end ; i++)
            {
                sum += d->histogram[i].green;
                if (sum * 2 > count) return i;
            }
            break;

        case ImageHistogram::BlueChannel:
            for (i = start ; i <= end ; i++)
            {
                sum += d->histogram[i].blue;
                if (sum * 2 > count) return i;
            }
            break;

        case ImageHistogram::AlphaChannel:
            for (i = start ; i <= end ; i++)
            {
                sum += d->histogram[i].alpha;
                if (sum * 2 > count) return i;
            }
            break;

        default:
            return 0;
            break;
    }

    return -1;
}

double ImageHistogram::getStdDev(int channel, int start, int end)
{
    int    i;
    double dev = 0.0;
    double count;
    double mean;

    if ( !d->histogram || start < 0 ||
        end > d->histoSegments-1 || start > end )
        return 0.0;

    mean  = getMean(channel, start, end);
    count = getCount(channel, start, end);

    if (count == 0.0)
        count = 1.0;

    switch(channel)
    {
        case ImageHistogram::ValueChannel:
            for (i = start ; i <= end ; i++)
                dev += (i - mean) * (i - mean) * d->histogram[i].value;
            break;

        case ImageHistogram::RedChannel:
            for (i = start ; i <= end ; i++)
                dev += (i - mean) * (i - mean) * d->histogram[i].red;
            break;

        case ImageHistogram::GreenChannel:
            for (i = start ; i <= end ; i++)
                dev += (i - mean) * (i - mean) * d->histogram[i].green;
            break;

        case ImageHistogram::BlueChannel:
            for (i = start ; i <= end ; i++)
                dev += (i - mean) * (i - mean) * d->histogram[i].blue;
            break;

        case ImageHistogram::AlphaChannel:
            for (i = start ; i <= end ; i++)
                dev += (i - mean) * (i - mean) * d->histogram[i].alpha;
            break;

        default:
            return 0.0;
            break;
    }

    return sqrt(dev / count);
}

double ImageHistogram::getValue(int channel, int bin)
{
    double value;

    if ( !d->histogram || bin < 0 || bin > d->histoSegments-1 )
        return 0.0;

    switch(channel)
    {
       case ImageHistogram::ValueChannel:
          value = d->histogram[bin].value;
          break;

       case ImageHistogram::RedChannel:
          value = d->histogram[bin].red;
          break;

       case ImageHistogram::GreenChannel:
          value = d->histogram[bin].green;
          break;

       case ImageHistogram::BlueChannel:
          value = d->histogram[bin].blue;
          break;

       case ImageHistogram::AlphaChannel:
          value = d->histogram[bin].alpha;
          break;

       default:
          return 0.0;
          break;
    }

    return value;
}

double ImageHistogram::getMaximum(int channel)
{
    double max = 0.0;
    int    x;

    if ( !d->histogram )
        return 0.0;

    switch(channel)
    {
       case ImageHistogram::ValueChannel:
          for (x = 0 ; x < d->histoSegments ; x++)
             if (d->histogram[x].value > max)
                max = d->histogram[x].value;
          break;

       case ImageHistogram::RedChannel:
          for (x = 0 ; x < d->histoSegments ; x++)
             if (d->histogram[x].red > max)
                max = d->histogram[x].red;
          break;

       case ImageHistogram::GreenChannel:
          for (x = 0 ; x < d->histoSegments ; x++)
             if (d->histogram[x].green > max)
                max = d->histogram[x].green;
          break;

       case ImageHistogram::BlueChannel:
          for (x = 0 ; x < d->histoSegments ; x++)
             if (d->histogram[x].blue > max)
                max = d->histogram[x].blue;
          break;

       case ImageHistogram::AlphaChannel:
          for (x = 0 ; x < d->histoSegments ; x++)
             if (d->histogram[x].alpha > max)
                max = d->histogram[x].alpha;
          break;

       default:
          return 0.0;
          break;
    }

    return max;
}

}  // namespace Digikam
