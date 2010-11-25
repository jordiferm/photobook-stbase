/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-07-21
 * Description : a widget to display an image histogram.
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Some code parts are inspired from from gimp 2.0
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

#include "histogramwidget.h"

// C++ includes.

#include <cmath>

// Qt includes.

#include <qpixmap.h>
#include <qpainter.h>
#include <qpen.h>
#include <qevent.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qbrush.h>
#include <qrect.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qtooltip.h>
#include <QPaintEvent>
#include <QMouseEvent>


// Local includes.
#include "ditemtooltip.h"
#include "imagehistogram.h"

namespace Digikam
{

class HistogramWidgetPriv
{
public:

    enum RepaintType
    {
        HistogramNone = 0,        // No current histogram values calculation.
        HistogramDataLoading,     // The image is being loaded
        HistogramStarted,         // Histogram values calculation started.
        HistogramCompleted,       // Histogram values calculation completed.
        HistogramFailed           // Histogram values calculation failed.
    };

    HistogramWidgetPriv()
    {
        renderingType        = HistogramWidget::FullImageHistogram;
        blinkTimer           = 0;
        sixteenBits          = false;
        inSelected           = false;
        clearFlag            = HistogramNone;
        xmin                 = 0.0;
        xmax                 = 0.0;
        range                = 255;
        guideVisible         = false;
        inInitialRepaintWait = false;
    }

    int     renderingType;       // Using full image or image selection for histogram rendering.

    // Current selection information.
    double  xmin;
    double  xminOrg;
    double  xmax;
    int     range;
    int     clearFlag;          // Clear drawing zone with message.
    int     pos;                // Position of animation during loading/calculation.

    bool    sixteenBits;
    bool    guideVisible;       // Display color guide.
    bool    statisticsVisible;  // Display tooltip histogram statistics.
    bool    inSelected;
    bool    selectMode;         // If true, a part of the histogram can be selected !
    bool    showProgress;       // If true, a message will be displayed during histogram computation,
                                // else nothing (limit flicker effect in widget especially for small
                                // image/computation time).
    bool    inInitialRepaintWait;

    QTimer *blinkTimer;

    DColor  colorGuide;
};

// Constructor without image data (needed to use updateData() method after instance created).

HistogramWidget::HistogramWidget(int w, int h,
                                 QWidget *parent, bool selectMode,
                                 bool showProgress, bool statisticsVisible)
               : QWidget(parent), d(new HistogramWidgetPriv)
{
    setup(w, h, selectMode, showProgress, statisticsVisible);
    setAttribute(Qt::WA_DeleteOnClose);

    m_imageHistogram     = 0;
    m_selectionHistogram = 0;
}

// Constructor without image selection.

HistogramWidget::HistogramWidget(int w, int h,
                                 uchar *i_data, uint i_w, uint i_h,
                                 bool i_sixteenBits,
                                 QWidget *parent, bool selectMode,
                                 bool showProgress, bool statisticsVisible)
               : QWidget(parent), d(new HistogramWidgetPriv)
{
    d->sixteenBits = i_sixteenBits;
    setup(w, h, selectMode, showProgress, statisticsVisible);
    setAttribute(Qt::WA_DeleteOnClose);

    m_imageHistogram     = new ImageHistogram(i_data, i_w, i_h, i_sixteenBits);
    m_selectionHistogram = 0L;

    connectHistogram(m_imageHistogram);

    m_imageHistogram->calculateInThread();
}

// Constructor with image selection.

HistogramWidget::HistogramWidget(int w, int h,
                                 uchar *i_data, uint i_w, uint i_h,
                                 uchar *s_data, uint s_w, uint s_h,
                                 bool i_sixteenBits,
                                 QWidget *parent, bool selectMode,
                                 bool showProgress, bool statisticsVisible)
               : QWidget(parent), d(new HistogramWidgetPriv)
{
    d->sixteenBits = i_sixteenBits;
    setup(w, h, selectMode, showProgress, statisticsVisible);
    setAttribute(Qt::WA_DeleteOnClose);

    m_imageHistogram     = new ImageHistogram(i_data, i_w, i_h, i_sixteenBits);
    m_selectionHistogram = new ImageHistogram(s_data, s_w, s_h, i_sixteenBits);

    connectHistogram(m_imageHistogram);
    connectHistogram(m_selectionHistogram);

    m_imageHistogram->calculateInThread();
}

HistogramWidget::~HistogramWidget()
{
    d->blinkTimer->stop();

    if (m_imageHistogram)
       delete m_imageHistogram;

    if (m_selectionHistogram)
       delete m_selectionHistogram;

    delete d;
}

void HistogramWidget::setup(int w, int h, bool selectMode, bool showProgress, bool statisticsVisible)
{
    m_channelType        = ValueHistogram;
    m_scaleType          = LogScaleHistogram;
    m_colorType          = RedColor;
    d->statisticsVisible = statisticsVisible;
    d->selectMode        = selectMode;
    d->showProgress      = showProgress;

    setMouseTracking(true);
    setMinimumSize(w, h);

    d->blinkTimer = new QTimer( this );

    connect( d->blinkTimer, SIGNAL(timeout()),
             this, SLOT(slotBlinkTimerDone()) );
}

void HistogramWidget::connectHistogram(const ImageHistogram *histogram)
{
    connect(histogram, SIGNAL(calculationStarted(const ImageHistogram *)),
            this, SLOT(slotCalculationStarted(const ImageHistogram *)));

    connect(histogram, SIGNAL(calculationFinished(const ImageHistogram *, bool)),
            this, SLOT(slotCalculationFinished(const ImageHistogram *, bool)));
}

void HistogramWidget::setHistogramGuideByColor(const DColor& color)
{
    d->guideVisible = true;
    d->colorGuide   = color;
    repaint();
}

void HistogramWidget::setRenderingType(HistogramRenderingType type)
{
    if (type != d->renderingType)
    {
        d->renderingType = type;

        ImageHistogram *nowUsedHistogram;
        if (d->renderingType == ImageSelectionHistogram && m_selectionHistogram)
            nowUsedHistogram = m_selectionHistogram;
        else
            nowUsedHistogram = m_imageHistogram;

        // already calculated?
        if (!nowUsedHistogram->isValid())
        {
            // still computing, or need to start it?
            if (nowUsedHistogram->isCalculating())
                slotCalculationStarted(nowUsedHistogram);
            else
                nowUsedHistogram->calculateInThread();
        }
        else
            update();
    }
}

void HistogramWidget::reset()
{
    d->guideVisible = false;
    repaint();
}

void HistogramWidget::slotCalculationStarted(const ImageHistogram *histogram)
{
    if (histogram != m_imageHistogram && histogram != m_selectionHistogram)
        return;

    // only react to the histogram that the user is currently waiting for
    if (d->renderingType == ImageSelectionHistogram && m_selectionHistogram)
    {
        if (histogram == m_imageHistogram)
            return;
    }
    else
    {
        if (histogram == m_selectionHistogram)
            return;
    }

    setCursor( Qt::WaitCursor );
    d->clearFlag = HistogramWidgetPriv::HistogramStarted;
    if (!d->inInitialRepaintWait)
    {
        if (d->clearFlag != HistogramWidgetPriv::HistogramDataLoading)
        {
            // enter initial repaint wait, repaint only after waiting
            // a short time so that very fast computation does not create flicker
            d->inInitialRepaintWait = true;
            d->blinkTimer->start( 100 );
        }
        else
        {
            // For data loading, the initial wait has been set in setDataLoading.
            // After the initial repaint, we can repaint immediately.
            repaint();
            d->blinkTimer->start( 200 );
        }
    }
}

void HistogramWidget::slotCalculationFinished(const ImageHistogram *histogram, bool success)
{
    if (histogram != m_imageHistogram && histogram != m_selectionHistogram)
        return;

    if (d->renderingType == ImageSelectionHistogram && m_selectionHistogram)
    {
        if (histogram == m_imageHistogram)
            return;
    }
    else
    {
        if (histogram == m_selectionHistogram)
            return;
    }

    if (success)
    {
        // Repaint histogram
        d->clearFlag = HistogramWidgetPriv::HistogramCompleted;
        d->blinkTimer->stop();
        d->inInitialRepaintWait = false;
        setCursor( Qt::ArrowCursor );

        // Send signals to refresh information if necessary.
        // The signals may trigger multiple repaints, avoid this,
        // we repaint once afterwards.
        setUpdatesEnabled(false);

        notifyValuesChanged();
        emit signalHistogramComputationDone(d->sixteenBits);

        setUpdatesEnabled(true);
        repaint();
    }
    else
    {
        d->clearFlag = HistogramWidgetPriv::HistogramFailed;
        d->blinkTimer->stop();
        d->inInitialRepaintWait = false;
        repaint();
        setCursor( Qt::ArrowCursor );
            // Remove old histogram data from memory.
        if (m_imageHistogram)
        {
            delete m_imageHistogram;
            m_imageHistogram = 0;
        }
        if (m_selectionHistogram)
        {
            delete m_selectionHistogram;
            m_selectionHistogram = 0;
        }
        emit signalHistogramComputationFailed();
    }
}

void HistogramWidget::setDataLoading()
{
    if (d->clearFlag != HistogramWidgetPriv::HistogramDataLoading)
    {
        setCursor( Qt::WaitCursor );
        d->clearFlag = HistogramWidgetPriv::HistogramDataLoading;
        // enter initial repaint wait, repaint only after waiting
        // a short time so that very fast computation does not create flicker
        d->inInitialRepaintWait = true;
        d->pos                  = 0;
        d->blinkTimer->start( 100 );
    }
}

void HistogramWidget::setLoadingFailed()
{
    d->clearFlag = HistogramWidgetPriv::HistogramFailed;
    d->pos       = 0;
    d->blinkTimer->stop();
    d->inInitialRepaintWait = false;
    repaint();
    setCursor( Qt::ArrowCursor );
}

void HistogramWidget::stopHistogramComputation()
{
    if (m_imageHistogram)
       m_imageHistogram->stopCalculation();

    if (m_selectionHistogram)
       m_selectionHistogram->stopCalculation();

    d->blinkTimer->stop();
    d->pos = 0;
}

void HistogramWidget::updateData(uchar *i_data, uint i_w, uint i_h,
                                 bool i_sixteenBits,
                                 uchar *s_data, uint s_w, uint s_h,
                                 bool showProgress)
{
    d->showProgress = showProgress;
    d->sixteenBits  = i_sixteenBits;

    // We are deleting the histogram data, so we must not use it to draw any more.
    d->clearFlag = HistogramWidgetPriv::HistogramNone;

    // Do not using ImageHistogram::getHistogramSegments()
    // method here because histogram hasn't yet been computed.
    d->range = d->sixteenBits ? 65535 : 255;
    emit signalMaximumValueChanged( d->range );


    // Remove old histogram data from memory.
    if (m_imageHistogram)
       delete m_imageHistogram;

    if (m_selectionHistogram)
       delete m_selectionHistogram;

    // Calc new histogram data
    m_imageHistogram = new ImageHistogram(i_data, i_w, i_h, i_sixteenBits);
    connectHistogram(m_imageHistogram);

    if (s_data && s_w && s_h)
    {
        m_selectionHistogram = new ImageHistogram(s_data, s_w, s_h, i_sixteenBits);
        connectHistogram(m_selectionHistogram);
    }
    else
        m_selectionHistogram = 0L;

    if (d->renderingType == ImageSelectionHistogram && m_selectionHistogram)
        m_selectionHistogram->calculateInThread();
    else
        m_imageHistogram->calculateInThread();
}

void HistogramWidget::updateSelectionData(uchar *s_data, uint s_w, uint s_h,
                                          bool i_sixteenBits,
                                          bool showProgress)
{
    d->showProgress = showProgress;

    // Remove old histogram data from memory.

    if (m_selectionHistogram)
       delete m_selectionHistogram;

    // Calc new histogram data
    m_selectionHistogram = new ImageHistogram(s_data, s_w, s_h, i_sixteenBits);
    connectHistogram(m_selectionHistogram);

    if (d->renderingType == ImageSelectionHistogram)
        m_selectionHistogram->calculateInThread();
}

void HistogramWidget::slotBlinkTimerDone()
{
    d->inInitialRepaintWait = false;
    repaint();
    d->blinkTimer->start( 200 );
}

void HistogramWidget::paintEvent(QPaintEvent*)
{
    // Widget is disabled, not initialized,
    // or loading, but no message shall be drawn:
    // Drawing grayed frame.
    if ( !isEnabled() ||
         d->clearFlag == HistogramWidgetPriv::HistogramNone ||
         (!d->showProgress && (d->clearFlag == HistogramWidgetPriv::HistogramStarted ||
                               d->clearFlag == HistogramWidgetPriv::HistogramDataLoading))
       )
    {
       QPixmap pm(size());
       QPainter p1;
       p1.begin(&pm);
       p1.initFrom(this);
       p1.fillRect(0, 0, width(), height(), palette().color(QPalette::Disabled, QPalette::Background));
       p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
       p1.drawRect(0, 0, width()-1, height()-1);
       QPen pen(palette().color(QPalette::Disabled, QPalette::Foreground));
       pen.setStyle(Qt::SolidLine);
       pen.setWidth(1);

       p1.setPen(pen);
       p1.drawRect(0, 0, width(), height());
       p1.end();

       QPainter p2(this);
       p2.drawPixmap(0, 0, pm);
       p2.end();
       return;
    }
    // Image data is loading or histogram is being computed:
    // Draw message.
    else if (  d->showProgress &&
              (d->clearFlag == HistogramWidgetPriv::HistogramStarted ||
               d->clearFlag == HistogramWidgetPriv::HistogramDataLoading)
            )
    {
       // In first, we draw an animation.

       int asize = 24;
       QPixmap anim(asize, asize);
       QPainter p0;
       p0.begin(&anim);
       p0.setRenderHint(QPainter::Antialiasing);
       p0.fillRect(0, 0, asize, asize, palette().color(QPalette::Active, QPalette::Background));
       p0.translate(asize/2, asize/2);

       d->pos = (d->pos + 10) % 360;
       p0.setPen(palette().color(QPalette::Active, QPalette::Text));
       p0.rotate(d->pos);
       for ( int i=0 ; i<12 ; i++ )
       {
           p0.drawLine(asize/2-5, 0, asize/2-2, 0);
           p0.rotate(30);
       }
       p0.end();

       // ... and we render busy text.

       QPixmap pm(size());
       QPainter p1;
       p1.begin(&pm);
       p1.initFrom(this);
       p1.fillRect(0, 0, width(), height(), palette().color(QPalette::Active, QPalette::Background));
       p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
       p1.drawRect(0, 0, width()-1, height()-1);
       p1.drawPixmap(width()/2 - asize /2, asize, anim);
       p1.setPen(palette().color(QPalette::Active, QPalette::Text));

       if (d->clearFlag == HistogramWidgetPriv::HistogramDataLoading)
           p1.drawText(0, 0, width(), height(), Qt::AlignCenter,
                       tr("Loading image..."));
       else
           p1.drawText(0, 0, width(), height(), Qt::AlignCenter,
                       tr("Histogram calculation..."));
       p1.end();

       QPainter p2(this);
       p2.drawPixmap(0, 0, pm);
       p2.end();
       return;
    }
    // Histogram computation failed:
    // Draw message.
    else if (d->clearFlag == HistogramWidgetPriv::HistogramFailed)
    {
       QPixmap pm(size());
       QPainter p1;
       p1.begin(&pm);
       p1.initFrom(this);
       p1.fillRect(0, 0, width(), height(), palette().color(QPalette::Active, QPalette::Background));
       p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
       p1.drawRect(0, 0, width()-1, height()-1);
       p1.setPen(palette().color(QPalette::Active, QPalette::Text));
       p1.drawText(0, 0, width(), height(), Qt::AlignCenter,
                   tr("Histogram\ncalculation\nfailed."));
       p1.end();

       QPainter p2(this);
       p2.drawPixmap(0, 0, pm);
       p2.end();
       return;
    }

    int    x, y;
    int    yr, yg, yb;             // For all color channels.
    int    wWidth  = width();
    int    wHeight = height();
    double max;
    class  ImageHistogram *histogram;

    if (d->renderingType == ImageSelectionHistogram && m_selectionHistogram)
       histogram = m_selectionHistogram;
    else
       histogram = m_imageHistogram;

    if (!histogram)
        return;

    x   = 0; y  = 0;
    yr  = 0; yg = 0; yb = 0;
    max = 0.0;

    switch(m_channelType)
    {
       case HistogramWidget::GreenChannelHistogram:    // Green channel.
          max = histogram->getMaximum(ImageHistogram::GreenChannel);
          break;

       case HistogramWidget::BlueChannelHistogram:     // Blue channel.
          max = histogram->getMaximum(ImageHistogram::BlueChannel);
          break;

       case HistogramWidget::RedChannelHistogram:      // Red channel.
          max = histogram->getMaximum(ImageHistogram::RedChannel);
          break;

       case HistogramWidget::AlphaChannelHistogram:    // Alpha channel.
          max = histogram->getMaximum(ImageHistogram::AlphaChannel);
          break;

       case HistogramWidget::ColorChannelsHistogram:   // All color channels.
          max = qMax (qMax (histogram->getMaximum(ImageHistogram::RedChannel),
                            histogram->getMaximum(ImageHistogram::GreenChannel)),
                      histogram->getMaximum(ImageHistogram::BlueChannel));
          break;

       case HistogramWidget::ValueHistogram:           // Luminosity.
          max = histogram->getMaximum(ImageHistogram::ValueChannel);
          break;
    }

    switch (m_scaleType)
    {
       case HistogramWidget::LinScaleHistogram:
          break;

       case HistogramWidget::LogScaleHistogram:
          if (max > 0.0)
              max = log (max);
          else
              max = 1.0;
          break;
    }

    // A QPixmap is used to enable the double buffering.

    QPixmap pm(size());
    QPainter p1;
    p1.begin(&pm);
    p1.initFrom(this);
    p1.fillRect(0, 0, width(), height(), palette().color(QPalette::Active, QPalette::Background));

    // Drawing selection or all histogram values.

    for (x = 0 ; x < wWidth ; x++)
    {
      double value   = 0.0;
      double value_r = 0.0, value_g = 0.0, value_b = 0.0; // For all color channels.
      int    i, j;

      i = (x       * histogram->getHistogramSegments()) / wWidth;
      j = ((x + 1) * histogram->getHistogramSegments()) / wWidth;

      do
      {
          double v;
          double vr, vg, vb;                              // For all color channels.

          v  = 0.0;
          vr = 0.0; vg = 0.0; vb = 0.0;

          switch(m_channelType)
          {
             case HistogramWidget::GreenChannelHistogram:    // Green channel.
                v = histogram->getValue(ImageHistogram::GreenChannel, i++);
                break;

             case HistogramWidget::BlueChannelHistogram:     // Blue channel.
                v = histogram->getValue(ImageHistogram::BlueChannel, i++);
                break;

             case HistogramWidget::RedChannelHistogram:      // Red channel.
                v = histogram->getValue(ImageHistogram::RedChannel, i++);
                break;

             case HistogramWidget::AlphaChannelHistogram:    // Alpha channel.
                v = histogram->getValue(ImageHistogram::AlphaChannel, i++);
                break;

             case HistogramWidget::ColorChannelsHistogram:   // All color channels.
                vr = histogram->getValue(ImageHistogram::RedChannel, i++);
                vg = histogram->getValue(ImageHistogram::GreenChannel, i);
                vb = histogram->getValue(ImageHistogram::BlueChannel, i);
                break;

             case HistogramWidget::ValueHistogram:           // Luminosity.
                v = histogram->getValue(ImageHistogram::ValueChannel, i++);
                break;
          }

          if ( m_channelType != HistogramWidget::ColorChannelsHistogram )
          {
             if (v > value)
                value = v;
          }
          else
          {
             if (vr > value_r)
                value_r = vr;
             if (vg > value_g)
                value_g = vg;
             if (vb > value_b)
                value_b = vb;
          }
      }
      while (i < j);

      if ( m_channelType != HistogramWidget::ColorChannelsHistogram )
      {
         switch (m_scaleType)
         {
            case HistogramWidget::LinScaleHistogram:
              y = (int) ((wHeight * value) / max);
              break;

            case HistogramWidget::LogScaleHistogram:
              if (value <= 0.0) value = 1.0;
              y = (int) ((wHeight * log (value)) / max);
              break;

            default:
              y = 0;
              break;
         }
      }
      else
      {
         switch (m_scaleType)
         {
            case HistogramWidget::LinScaleHistogram:
              yr = (int) ((wHeight * value_r) / max);
              yg = (int) ((wHeight * value_g) / max);
              yb = (int) ((wHeight * value_b) / max);
              break;

            case HistogramWidget::LogScaleHistogram:
              if (value_r <= 0.0) value_r = 1.0;
              if (value_g <= 0.0) value_g = 1.0;
              if (value_b <= 0.0) value_b = 1.0;
              yr = (int) ((wHeight * log (value_r)) / max);
              yg = (int) ((wHeight * log (value_g)) / max);
              yb = (int) ((wHeight * log (value_b)) / max);
              break;

            default:
              yr = 0;
              yg = 0;
              yb = 0;
              break;
         }
      }

      // Drawing the histogram + selection or only the histogram.

      if ( m_channelType != HistogramWidget::ColorChannelsHistogram )
      {
         if ( d->selectMode == true )   // Selection mode enable ?
         {
            if ( x >= (int)(d->xmin * wWidth) && x <= (int)(d->xmax * wWidth) )
            {
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
               p1.drawLine(x, wHeight, x, 0);
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
               p1.drawLine(x, wHeight, x, wHeight - y);
            }
            else
            {
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
               p1.drawLine(x, wHeight, x, wHeight - y);
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
               p1.drawLine(x, wHeight - y, x, 0);

               if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
               {
                  p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                  p1.drawLine(x, wHeight, x, 0);
               }
            }
         }
         else
         {
            p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
            p1.drawLine(x, wHeight, x, wHeight - y);
            p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
            p1.drawLine(x, wHeight - y, x, 0);

            if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
            {
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
               p1.drawLine(x, wHeight, x, 0);
            }
         }
      }
      else
      {
         if ( d->selectMode == true )   // Histogram selection mode enable ?
         {
             if ( x >= (int)(d->xmin * wWidth) && x <= (int)(d->xmax * wWidth) )
            {
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
               p1.drawLine(x, wHeight, x, 0);
               p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));

               // Witch color must be used on the foreground with all colors channel mode?
               switch (m_colorType)
               {
                  case HistogramWidget::RedColor:
                    p1.drawLine(x, wHeight, x, wHeight - yr);
                    break;

                  case HistogramWidget::GreenColor:
                    p1.drawLine(x, wHeight, x, wHeight - yg);
                    break;

                  default:
                    p1.drawLine(x, wHeight, x, wHeight - yb);
                    break;
               }
            }
            else
            {
               // Which color must be used on the foreground with all colors channel mode?
               switch (m_colorType)
               {
                  case HistogramWidget::RedColor:
                    p1.setPen(QPen(Qt::green, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yg);
                    p1.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yb);
                    p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yr);

                    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight - qMax(qMax(yr, yg), yb), x, 0);
                    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight - yg -1, x, wHeight - yg);
                    p1.drawLine(x, wHeight - yb -1, x, wHeight - yb);
                    p1.drawLine(x, wHeight - yr -1, x, wHeight - yr);

                    if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
                    {
                       p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                       p1.drawLine(x, wHeight, x, 0);
                    }

                    break;

                  case HistogramWidget::GreenColor:
                    p1.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yb);
                    p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yr);
                    p1.setPen(QPen(Qt::green, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yg);

                    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight - qMax(qMax(yr, yg), yb), x, 0);
                    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight - yb -1, x, wHeight - yb);
                    p1.drawLine(x, wHeight - yr -1, x, wHeight - yr);
                    p1.drawLine(x, wHeight - yg -1, x, wHeight - yg);

                    if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
                    {
                       p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                       p1.drawLine(x, wHeight, x, 0);
                    }

                    break;

                  default:
                    p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yr);
                    p1.setPen(QPen(Qt::green, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yg);
                    p1.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight, x, wHeight - yb);

                    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight - qMax(qMax(yr, yg), yb), x, 0);
                    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
                    p1.drawLine(x, wHeight - yr -1, x, wHeight - yr);
                    p1.drawLine(x, wHeight - yg -1, x, wHeight - yg);
                    p1.drawLine(x, wHeight - yb -1, x, wHeight - yb);

                    if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
                    {
                       p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                       p1.drawLine(x, wHeight, x, 0);
                    }

                    break;
               }
            }
         }
         else
         {
            // Which color must be used on the foreground with all colors channel mode?
            switch (m_colorType)
            {
               case HistogramWidget::RedColor:
                 p1.setPen(QPen(Qt::green, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yg);
                 p1.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yb);
                 p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yr);

                 p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight - qMax(qMax(yr, yg), yb), x, 0);
                 p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight - yg -1, x, wHeight - yg);
                 p1.drawLine(x, wHeight - yb -1, x, wHeight - yb);
                 p1.drawLine(x, wHeight - yr -1, x, wHeight - yr);

                 if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
                 {
                     p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                     p1.drawLine(x, wHeight, x, 0);
                 }

                 break;

               case HistogramWidget::GreenColor:
                 p1.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yb);
                 p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yr);
                 p1.setPen(QPen(Qt::green, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yg);

                 p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight - qMax(qMax(yr, yg), yb), x, 0);
                 p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight - yb -1, x, wHeight - yb);
                 p1.drawLine(x, wHeight - yr -1, x, wHeight - yr);
                 p1.drawLine(x, wHeight - yg -1, x, wHeight - yg);

                 if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
                 {
                     p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                     p1.drawLine(x, wHeight, x, 0);
                 }

                 break;

               default:
                 p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yr);
                 p1.setPen(QPen(Qt::green, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yg);
                 p1.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight, x, wHeight - yb);

                 p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Background), 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight - qMax(qMax(yr, yg), yb), x, 0);
                 p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
                 p1.drawLine(x, wHeight - yr -1, x, wHeight - yr);
                 p1.drawLine(x, wHeight - yg -1, x, wHeight - yg);
                 p1.drawLine(x, wHeight - yb -1, x, wHeight - yb);

                 if ( x == wWidth/4 || x == wWidth/2 || x == 3*wWidth/4 )
                 {
                     p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Base), 1, Qt::SolidLine));
                     p1.drawLine(x, wHeight, x, 0);
                 }

                 break;
             }
          }
       }
    }

    // Drawing color guide.

    p1.setPen(QPen(Qt::red, 1, Qt::DotLine));
    int guidePos;

    if (d->guideVisible)
    {
       switch(m_channelType)
       {
          case HistogramWidget::RedChannelHistogram:
             guidePos = d->colorGuide.red();
             break;

          case HistogramWidget::GreenChannelHistogram:
             guidePos = d->colorGuide.green();
             break;

          case HistogramWidget::BlueChannelHistogram:
             guidePos = d->colorGuide.blue();
             break;

          case HistogramWidget::ValueHistogram:
             guidePos = qMax(qMax(d->colorGuide.red(), d->colorGuide.green()), d->colorGuide.blue());
             break;

          case HistogramWidget::ColorChannelsHistogram:
          {
             switch(m_channelType)
             {
                 case HistogramWidget::RedChannelHistogram:
                     guidePos = d->colorGuide.red();
                     break;

                 case HistogramWidget::GreenChannelHistogram:
                     guidePos = d->colorGuide.green();
                     break;

                 case HistogramWidget::BlueChannelHistogram:
                     guidePos = d->colorGuide.blue();
                     break;
             }
          }

          default:
             guidePos = d->colorGuide.alpha();
             break;
       }

       if (guidePos != -1)
       {
          int xGuide = (guidePos * wWidth) / histogram->getHistogramSegments();
          p1.drawLine(xGuide, 0, xGuide, wHeight);

          QString string = tr("x:%1").arg(guidePos);
          QFontMetrics fontMt( string );
          QRect rect = fontMt.boundingRect(0, 0, wWidth, wHeight, 0, string);
          p1.setPen(QPen(Qt::red, 1, Qt::SolidLine));
          rect.moveTop(1);

          if (xGuide < wWidth/2)
          {
             rect.moveLeft(xGuide);
             p1.fillRect(rect, QBrush(QColor(250, 250, 255)) );
             p1.drawRect(rect);
             rect.moveLeft(xGuide+3);
             p1.drawText(rect, Qt::AlignLeft, string);
          }
          else
          {
             rect.moveRight(xGuide);
             p1.fillRect(rect, QBrush(QColor(250, 250, 255)) );
             p1.drawRect(rect);
             rect.moveRight(xGuide-3);
             p1.drawText(rect, Qt::AlignRight, string);
          }
       }
    }

    if (d->statisticsVisible)
    {
       DToolTipStyleSheet cnt;
       QString            tipText, value;
       tipText = "<qt><table cellspacing=0 cellpadding=0>";

       tipText += cnt.cellBeg + tr("Mean:") + cnt.cellMid;
       double mean = histogram->getMean(m_channelType, 0, histogram->getHistogramSegments()-1);
       tipText += value.setNum(mean, 'f', 1) + cnt.cellEnd;

       tipText += cnt.cellBeg + tr("Pixels:") + cnt.cellMid;
       double pixels = histogram->getPixels();
       tipText += value.setNum((float)pixels, 'f', 0) + cnt.cellEnd;

       tipText += cnt.cellBeg + tr("Std dev.:") + cnt.cellMid;
       double stddev = histogram->getStdDev(m_channelType, 0, histogram->getHistogramSegments()-1);
       tipText += value.setNum(stddev, 'f', 1) + cnt.cellEnd;

       tipText += cnt.cellBeg + tr("Count:") + cnt.cellMid;
       double counts = histogram->getCount(m_channelType, 0, histogram->getHistogramSegments()-1);
       tipText += value.setNum((float)counts, 'f', 0) + cnt.cellEnd;

       tipText += cnt.cellBeg + tr("Median:") + cnt.cellMid;
       double median = histogram->getMedian(m_channelType, 0, histogram->getHistogramSegments()-1);
       tipText += value.setNum(median, 'f', 1) + cnt.cellEnd;

       tipText += cnt.cellBeg + tr("Percent:") + cnt.cellMid;
       double percentile = (pixels > 0 ? (100.0 * counts / pixels) : 0.0);
       tipText += value.setNum(percentile, 'f', 1) + cnt.cellEnd;

       tipText += "</table></qt>";

       this->setToolTip( tipText);
    }

    p1.setPen(QPen(palette().color(QPalette::Active, QPalette::Foreground), 1, Qt::SolidLine));
    p1.drawRect(0, 0, width()-1, height()-1);
    p1.end();

    QPainter p2(this);
    p2.drawPixmap(0, 0, pm);
    p2.end();
}

void HistogramWidget::mousePressEvent ( QMouseEvent * e )
{
    if ( d->selectMode == true && d->clearFlag == HistogramWidgetPriv::HistogramCompleted )
    {
       if (!d->inSelected)
       {
          d->inSelected = true;
          repaint();
       }

       d->xmin = ((double)e->pos().x()) / ((double)width());
       d->xminOrg = d->xmin;
       notifyValuesChanged();
       //emit signalValuesChanged( (int)(d->xmin * d->range),  );
       d->xmax = 0.0;
    }
}

void HistogramWidget::mouseReleaseEvent ( QMouseEvent * )
{
    if ( d->selectMode == true  && d->clearFlag == HistogramWidgetPriv::HistogramCompleted )
    {
        d->inSelected = false;
        // Only single click without mouse move? Remove selection.
        if (d->xmax == 0.0)
        {
            d->xmin = 0.0;
            //emit signalMinValueChanged( 0 );
            //emit signalMaxValueChanged( d->range );
            notifyValuesChanged();
            repaint();
        }
    }
}

void HistogramWidget::mouseMoveEvent ( QMouseEvent * e )
{
    if ( d->selectMode == true && d->clearFlag == HistogramWidgetPriv::HistogramCompleted )
    {
       setCursor( Qt::CrossCursor );

       if (d->inSelected)
       {
          double max = ((double)e->pos().x()) / ((double)width());
          //int max = (int)(e->pos().x()*((float)m_imageHistogram->getHistogramSegments()/(float)width()));

          if (max < d->xminOrg)
          {
             d->xmax = d->xminOrg;
             d->xmin = max;
             //emit signalMinValueChanged( (int)(d->xmin * d->range) );
          }
          else
          {
             d->xmin = d->xminOrg;
             d->xmax = max;
          }

          notifyValuesChanged();
          //emit signalMaxValueChanged( d->xmax == 0.0 ? d->range : (int)(d->xmax * d->range) );

          repaint();
       }
    }
}

void HistogramWidget::notifyValuesChanged()
{
    emit signalIntervalChanged( (int)(d->xmin * d->range), d->xmax == 0.0 ? d->range : (int)(d->xmax * d->range) );
}

void HistogramWidget::slotMinValueChanged(int min)
{
    if ( d->selectMode == true && d->clearFlag == HistogramWidgetPriv::HistogramCompleted )
    {
        if (min == 0 && d->xmax == 1.0)
        {
            // everything is selected means no selection
            d->xmin = 0.0;
            d->xmax = 0.0;
        }
        if (min >= 0 && min < d->range)
        {
           d->xmin = ((double)min)/d->range;
        }
        repaint();
    }
}

void HistogramWidget::slotMaxValueChanged(int max)
{
    if ( d->selectMode == true && d->clearFlag == HistogramWidgetPriv::HistogramCompleted )
    {
        if (d->xmin == 0.0 && max == d->range)
        {
            // everything is selected means no selection
            d->xmin = 0.0;
            d->xmax = 0.0;
        }
        else if (max > 0 && max <= d->range)
        {
            d->xmax = ((double)max)/d->range;
        }
        repaint();
    }
}

}  // namespace Digikam
