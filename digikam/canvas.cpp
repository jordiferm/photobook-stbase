/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-09
 * Description : image editor canvas management class
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "canvas.h"

// C++ includes.

#include <cstdio>
#include <cmath>

// Qt includes.

#include <QCache>
#include <QFile>
#include <QString>
#include <QEvent>
#include <QPoint>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QStyle>
#include <QApplication>
#include <QCursor>
#include <QImage>
#include <QRegion>
#include <QTimer>
#include <QColor>
#include <QClipboard>
#include <QToolButton>
#include <QFrame>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWheelEvent>


// Local includes.

#include "imagehistogram.h"
#include "imagepaniconwidget.h"
#include "dimginterface.h"
#include "iccsettingscontainer.h"
#include "exposurecontainer.h"
#include "iofilesettingscontainer.h"
#include "loadingcacheinterface.h"
#include "drubberband.h"
#include "kpopupframe.h"

namespace Digikam
{

	class CanvasPrivate
	{

	public:

		CanvasPrivate() :
				tileSize(128), minZoom(0.1), maxZoom(12.0), zoomMultiplier(1.2)
		{
			pressedMoved     = false;
			pressedMoving    = false;
			ltActive         = false;
			rtActive         = false;
			lbActive         = false;
			rbActive         = false;
			midButtonPressed = false;
			midButtonX       = 0;
			midButtonY       = 0;
			panIconPopup     = 0;
			panIconWidget    = 0;
			cornerButton     = 0;
			parent           = 0;
			im               = 0;
			rubber           = 0;
			autoZoom         = false;
			fullScreen       = false;
			zoom             = 1.0;
			tileTmpPix       = new QPixmap(tileSize, tileSize);

			tileCache.setMaxCost((10*1024*1024) / (tileSize*tileSize*4));
		}

		bool                     autoZoom;
		bool                     fullScreen;
		bool                     pressedMoved;
		bool                     pressedMoving;
		bool                     ltActive;
		bool                     rtActive;
		bool                     lbActive;
		bool                     rbActive;
		bool                     midButtonPressed;

		const int                tileSize;
		int                      midButtonX;
		int                      midButtonY;

		double                   zoom;
		const double             minZoom;
		const double             maxZoom;
		const double             zoomMultiplier;

		QToolButton             *cornerButton;

		DRubberBand             *rubber;
		QRect                    pixmapRect;

		QCache<QString, QPixmap> tileCache;

		QPixmap*                 tileTmpPix;
		QPixmap                  qcheck;

		QColor                   bgColor;

		QWidget                 *parent;

		KPopupFrame             *panIconPopup;

		DImgInterface           *im;

		ImagePanIconWidget      *panIconWidget;
	};

	Canvas::Canvas(QWidget *parent)
			: Q3ScrollView(parent), d(new CanvasPrivate)
	{
		d->im     = new DImgInterface();
		d->parent = parent;
		d->bgColor.setRgb(0, 0, 0);

		d->qcheck = QPixmap(16, 16);
		QPainter p(&d->qcheck);
		p.fillRect(0, 0, 8, 8, QColor(144, 144, 144));
		p.fillRect(8, 8, 8, 8, QColor(144, 144, 144));
		p.fillRect(0, 8, 8, 8, QColor(100, 100, 100));
		p.fillRect(8, 0, 8, 8, QColor(100, 100, 100));
		p.end();

		d->cornerButton = new QToolButton(this);
		//!!!! Put an icon here !!!
		d->cornerButton->setIcon(QIcon(":/digikam/transform_move.png"));
		d->cornerButton->hide();
		d->cornerButton->setToolTip(tr("Pan the image to a region"));
		setCornerWidget(d->cornerButton);

		QPalette palette;
		palette.setColor(viewport()->backgroundRole(), Qt::NoBackground);
		viewport()->setPalette(palette);
		viewport()->setMouseTracking(false);
		setFrameStyle(QFrame::NoFrame);

		d->rubber = new DRubberBand(this);

		// ------------------------------------------------------------

		connect(this, SIGNAL(signalZoomChanged(double)),
		        this, SLOT(slotZoomChanged(double)));

		connect(d->cornerButton, SIGNAL(pressed()),
		        this, SLOT(slotCornerButtonPressed()));

		connect(d->im, SIGNAL(signalModified()),
		        this, SLOT(slotModified()));

		connect(d->im, SIGNAL(signalUndoStateChanged(bool, bool, bool)),
		        this, SIGNAL(signalUndoStateChanged(bool, bool, bool)));

		connect(d->im, SIGNAL(signalLoadingStarted(const QString&)),
		        this, SIGNAL(signalLoadingStarted(const QString&)));

		connect(d->im, SIGNAL(signalImageLoaded(const QString&, bool)),
		        this, SLOT(slotImageLoaded(const QString&, bool)));

		connect(d->im, SIGNAL(signalImageSaved(const QString&, bool)),
		        this, SLOT(slotImageSaved(const QString&, bool)));

		connect(d->im, SIGNAL(signalLoadingProgress(const QString&, float)),
		        this, SIGNAL(signalLoadingProgress(const QString&, float)));

		connect(d->im, SIGNAL(signalSavingProgress(const QString&, float)),
		        this, SIGNAL(signalSavingProgress(const QString&, float)));

		connect(this, SIGNAL(signalSelected(bool)),
		        this, SLOT(slotSelected()));
	}

	Canvas::~Canvas()
	{
		delete d->tileTmpPix;
		delete d->im;

		if (d->rubber)
			delete d->rubber;

		delete d;
	}

	void Canvas::resetImage()
	{
		reset();
		viewport()->setUpdatesEnabled(false);
		d->im->resetImage();
	}

	void Canvas::reset()
	{
		if (d->rubber->isActive())
		{
			d->rubber->setActive(false);
			if (d->im->imageValid())
				emit signalSelected(false);
		}

		d->tileCache.clear();
	}

	void Canvas::load(const QString& filename, IOFileSettingsContainer *IOFileSettings)
	{
		reset();

		viewport()->setUpdatesEnabled(false);

		d->im->load(filename, IOFileSettings, d->parent);
		emit signalPrepareToLoad();
	}

	void Canvas::slotImageLoaded(const QString& filePath, bool success)
	{
		d->im->zoom(d->zoom);

		if (d->autoZoom)
			updateAutoZoom();

		updateContentsSize(true);

		viewport()->setUpdatesEnabled(true);
		viewport()->update();

		emit signalZoomChanged(d->zoom);

		emit signalLoadingFinished(filePath, success);
	}

	void Canvas::preload(const QString& /*filename*/)
	{
//    d->im->preload(filename);
	}

	/*
	These code part are unused and untested
	void Canvas::save(const QString& filename, IOFileSettingsContainer *IOFileSettings)
	{
	    d->im->save(filename, IOFileSettings);
	    emit signalSavingStarted(filename);
	}

	void Canvas::saveAs(const QString& filename,IOFileSettingsContainer *IOFileSettings,
	                    const QString& mimeType)
	{
	    d->im->saveAs(filename, IOFileSettings, mimeType);
	    emit signalSavingStarted(filename);
	}
	*/

	void Canvas::saveAs(const QString& filename, IOFileSettingsContainer *IOFileSettings,
	                    bool setExifOrientationTag, const QString& mimeType)
	{
		d->im->saveAs(filename, IOFileSettings, setExifOrientationTag, mimeType);
		emit signalSavingStarted(filename);
	}

	void Canvas::slotImageSaved(const QString& filePath, bool success)
	{
		emit signalSavingFinished(filePath, success);
	}

	void Canvas::switchToLastSaved(const QString& newFilename)
	{
		d->im->switchToLastSaved(newFilename);
	}

	void Canvas::abortSaving()
	{
		d->im->abortSaving();
	}

	void Canvas::setModified()
	{
		d->im->setModified();
	}

	void Canvas::readMetadataFromFile(const QString &file)
	{
		d->im->readMetadataFromFile(file);
	}

	void Canvas::clearUndoHistory()
	{
		d->im->clearUndoManager();
	}

	void Canvas::setUndoHistoryOrigin()
	{
		d->im->setUndoManagerOrigin();
	}

	void Canvas::updateUndoState()
	{
		d->im->updateUndoState();
	}

	DImg Canvas::currentImage()
	{
		return DImg(*d->im->getImg());
	}

	QString Canvas::currentImageFileFormat()
	{
		return d->im->getImageFormat();
	}

	QString Canvas::currentImageFilePath()
	{
		return d->im->getImageFilePath();
	}

	int Canvas::imageWidth()
	{
		return d->im->origWidth();
	}

	int Canvas::imageHeight()
	{
		return d->im->origHeight();
	}

	bool Canvas::isReadOnly()
	{
		return d->im->isReadOnly();
	}

	QRect Canvas::getSelectedArea()
	{
		int x, y, w, h;
		d->im->getSelectedArea(x, y, w, h);
		return (QRect(x, y, w, h));
	}

	DImgInterface *Canvas::interface() const
		{
			return d->im;
		}

	void Canvas::makeDefaultEditingCanvas()
	{
		DImgInterface::setDefaultInterface(d->im);
	}

	double Canvas::calcAutoZoomFactor()
	{
		if (!d->im->imageValid()) return d->zoom;

		double srcWidth  = d->im->origWidth();
		double srcHeight = d->im->origHeight();
		double dstWidth  = contentsRect().width();
		double dstHeight = contentsRect().height();
		return qMin(dstWidth / srcWidth, dstHeight / srcHeight);
	}

	void Canvas::updateAutoZoom()
	{
		d->zoom = calcAutoZoomFactor();
		d->im->zoom(d->zoom);
		emit signalZoomChanged(d->zoom);
	}

	void Canvas::updateContentsSize(bool deleteRubber)
	{
		viewport()->setUpdatesEnabled(false);

		if (deleteRubber && d->rubber->isActive())
		{
			d->rubber->setActive(false);
			d->ltActive     = false;
			d->rtActive     = false;
			d->lbActive     = false;
			d->rbActive     = false;
			d->pressedMoved = false;
			viewport()->unsetCursor();
			viewport()->setMouseTracking(false);
			if (d->im->imageValid())
				emit signalSelected(false);
		}

		int wZ = d->im->width();
		int hZ = d->im->height();

		if (visibleWidth() > wZ || visibleHeight() > hZ)
		{
			// Center the image
			int centerx = contentsRect().width() / 2;
			int centery = contentsRect().height() / 2;
			int xoffset = int(centerx - wZ / 2);
			int yoffset = int(centery - hZ / 2);
			xoffset     = qMax(xoffset, 0);
			yoffset     = qMax(yoffset, 0);

			d->pixmapRect = QRect(xoffset, yoffset, wZ, hZ);
		}
		else
		{
			d->pixmapRect = QRect(0, 0, wZ, hZ);
		}

		// always restrict rubberband to area covered by image
		d->rubber->setRestrictionOnContents(d->pixmapRect);

		if (!deleteRubber && d->rubber->isActive())
		{
			int xSel, ySel, wSel, hSel;
			d->im->getSelectedArea(xSel, ySel, wSel, hSel);
			xSel = (int)((xSel * d->tileSize) / floor(d->tileSize / d->zoom));
			ySel = (int)((ySel * d->tileSize) / floor(d->tileSize / d->zoom));
			wSel = (int)((wSel * d->tileSize) / floor(d->tileSize / d->zoom));
			hSel = (int)((hSel * d->tileSize) / floor(d->tileSize / d->zoom));
			QRect rubberRect(xSel, ySel, wSel, hSel);
			rubberRect.translate(d->pixmapRect.x(), d->pixmapRect.y());
			d->rubber->setRectOnViewport(rubberRect);
		}

		d->tileCache.clear();
		resizeContents(wZ, hZ);
		viewport()->setUpdatesEnabled(true);
	}

	void Canvas::resizeEvent(QResizeEvent* e)
	{
		if (!e)
			return;

		Q3ScrollView::resizeEvent(e);

		if (d->autoZoom)
			updateAutoZoom();

		updateContentsSize(false);

		// No need to repaint. its called
		// automatically after resize

		// To be sure than corner widget used to pan image will be hide/show
		// accordingly with resize event.
		slotZoomChanged(d->zoom);
	}

	void Canvas::viewportPaintEvent(QPaintEvent *e)
	{
		QRect er(e->rect());
		er = QRect(qMax(er.x() - 1, 0),
		           qMax(er.y() - 1, 0),
		           qMin(er.width()  + 2, contentsRect().width()),
		           qMin(er.height() + 2, contentsRect().height()));

		paintViewport(er, (d->zoom <= 1.0) ? true : false);
	}

	void Canvas::paintViewport(const QRect& er, bool antialias)
	{
		QRect o_cr(viewportToContents(er.topLeft()), viewportToContents(er.bottomRight()));
		QRect cr = o_cr;

		QRegion clipRegion(er);
		cr = d->pixmapRect.intersect(cr);
		QPainter painter(viewport());

		if (!cr.isEmpty() && d->im->imageValid())
		{
			clipRegion -= QRect(contentsToViewport(cr.topLeft()), cr.size());

			QRect pr = QRect(cr.x() - d->pixmapRect.x(), cr.y() - d->pixmapRect.y(),
			                 cr.width(), cr.height());

			int x1 = (int)floor((double)pr.x()      / (double)d->tileSize) * d->tileSize;
			int y1 = (int)floor((double)pr.y()      / (double)d->tileSize) * d->tileSize;
			int x2 = (int)ceilf((double)pr.right()  / (double)d->tileSize) * d->tileSize;
			int y2 = (int)ceilf((double)pr.bottom() / (double)d->tileSize) * d->tileSize;

			QPixmap pix(d->tileSize, d->tileSize);
			int sx, sy, sw, sh;
			int step = (int)floor(d->tileSize / d->zoom);

			//bool hasRubber = (d->rubber->isVisible() && d->pressedMoved && d->pressedMoving && d->rubber->geometry().intersects(pr));

			for (int j = y1 ; j < y2 ; j += d->tileSize)
			{
				for (int i = x1 ; i < x2 ; i += d->tileSize)
				{
					QString key  = QString("%1,%2").arg(i).arg(j);
					QPixmap *pix = d->tileCache.object(key);

					if (!pix)
					{
						if (antialias)
						{
							pix = new QPixmap(d->tileSize, d->tileSize);
							d->tileCache.insert(key, pix);
						}
						else
						{
							pix = d->tileTmpPix;
						}

						if (d->im->hasAlpha())
						{
							QPainter p(pix);
							p.drawTiledPixmap(0, 0, d->tileSize, d->tileSize,
							                  d->qcheck, 0, 0);
							p.end();
						}
						else
						{
							pix->fill(d->bgColor);
						}

						// NOTE : with implementations <= 0.9.1, the canvas doesn't work properly using high zoom level (> 500).
						// The sx, sy, sw, sh values haven't be computed properly and "tile" artifacts been appears
						// over the image.
						// Note these "tile" artifacts are not the real tiles of canvas.
						// The new implementation below fix this problem to handle properly the areas to
						// use from the source image to generate the canvas pixmap tiles.

						sx = (int)floor((double)i / d->tileSize) * step;
						sy = (int)floor((double)j / d->tileSize) * step;
						sw = step;
						sh = step;

						if (d->rubber->isActive() && d->pressedMoved && !d->pressedMoving)
						{
							QRect rr(d->rubber->rubberBandAreaOnContents());
							rr = QRect(rr.x(), rr.y(), rr.width() - 1, rr.height() - 1);
							QRect r(i, j, d->tileSize, d->tileSize);

							d->im->paintOnDevice(pix, sx, sy, sw, sh,
							                     0, 0, d->tileSize, d->tileSize,
							                     rr.x() - i - d->pixmapRect.x(),
							                     rr.y() - j - d->pixmapRect.y(),
							                     rr.width(), rr.height(),
							                     antialias);

							rr.translate(-i - d->pixmapRect.x(), -j - d->pixmapRect.y());

							QPainter p(pix);
							p.setPen(d->rubber->palette().color(QPalette::Active, QPalette::Highlight));
							p.drawRect(rr);
							if (rr.width() >= 10 && rr.height() >= 10)
							{
								p.fillRect(QRect(rr.x(),              rr.y(),               5, 5), d->rubber->palette().highlight());
								p.fillRect(QRect(rr.x(),              rr.y() + rr.height() - 5, 5, 5), d->rubber->palette().highlight());
								p.fillRect(QRect(rr.x() + rr.width() - 5, rr.y() + rr.height() - 5, 5, 5), d->rubber->palette().highlight());
								p.fillRect(QRect(rr.x() + rr.width() - 5, rr.y(),               5, 5), d->rubber->palette().highlight());
							}
							p.end();
						}
						else
						{
							d->im->paintOnDevice(pix, sx, sy, sw, sh,
							                     0, 0, d->tileSize, d->tileSize,
							                     antialias);
						}
					}

					QRect  r(i, j, d->tileSize, d->tileSize);
					QRect  ir = pr.intersect(r);
					QPoint pt(contentsToViewport(QPoint(ir.x() + d->pixmapRect.x(),
					                                    ir.y() + d->pixmapRect.y())));

					painter.drawPixmap(pt.x(), pt.y(),
					                   *pix,
					                   ir.x() - r.x(), ir.y() - r.y(),
					                   ir.width(), ir.height());
				}
			}
		}

		painter.setClipRegion(clipRegion);
		painter.fillRect(er, d->bgColor);
		painter.end();
	}

	void Canvas::contentsMousePressEvent(QMouseEvent *e)
	{
		if (!e || e->button() == Qt::RightButton)
			return;

		d->midButtonPressed = false;

		if (e->button() == Qt::LeftButton)
		{
			if (d->ltActive || d->rtActive ||
			        d->lbActive || d->rbActive)
			{
				if (!d->rubber->isActive())
					return;

				// Set diagonally opposite corner as anchor

				QRect r(d->rubber->rubberBandAreaOnContents());
				QRect newRect(r);

				if (d->ltActive)
				{
					newRect.setTopLeft(r.bottomRight());
					newRect.setBottomRight(r.topLeft());
				}
				else if (d->rtActive)
				{
					newRect.setTopLeft(r.bottomLeft());
					newRect.setBottomRight(r.topRight());
				}
				else if (d->lbActive)
				{
					newRect.setTopLeft(r.topRight());
					newRect.setBottomRight(r.bottomLeft());
				}
				else if (d->rbActive)
				{
					newRect.setTopLeft(r.topLeft());
					newRect.setBottomRight(r.bottomLeft());
				}
				d->rubber->setRectOnContents(newRect);

				viewport()->setMouseTracking(false);
				d->pressedMoved  = false;
				d->pressedMoving = true;

				d->tileCache.clear();
				viewport()->repaint();

				return;
			}
		}
		else if (e->button() == Qt::MidButton)
		{
			if (visibleWidth()  < d->im->width() ||
			        visibleHeight() < d->im->height())
			{
				viewport()->setCursor(Qt::SizeAllCursor);
				d->midButtonPressed = true;
				d->midButtonX       = e->x();
				d->midButtonY       = e->y();
			}
			return;
		}

		d->rubber->setFirstPointOnViewport(e->pos());

		if (d->pressedMoved)
		{
			d->tileCache.clear();
			viewport()->update();
		}

		d->pressedMoved  = false;
		d->pressedMoving = true;

		viewport()->setMouseTracking(false);
	}

	void Canvas::contentsMouseMoveEvent(QMouseEvent *e)
	{
		if (!e)
			return;

		if (e->buttons() & Qt::MidButton)
		{
			if (d->midButtonPressed)
			{
				scrollBy(d->midButtonX - e->x(),
				         d->midButtonY - e->y());
			}
		}
		else if (!viewport()->hasMouseTracking())
		{
			if (!d->rubber->isActive())
				return;

			if (e->buttons() != Qt::LeftButton &&
			        !(d->ltActive || d->rtActive ||
			          d->lbActive || d->rbActive))
				return;

			// Move content if necessary.
			blockSignals(true);
			setUpdatesEnabled(false);
			ensureVisible(e->x(), e->y(), 10, 10);
			setUpdatesEnabled(true);
			blockSignals(false);

			// set the new rubber position.
			d->rubber->setSecondPointOnViewport(e->pos());

			d->pressedMoved  = true;
			d->pressedMoving = true;

			// To refresh editor status bar with current selection.
			emit signalSelectionChanged(calcSelectedArea());
		}
		else
		{
			if (!d->rubber->isActive())
				return;

			QRect r(d->rubber->rubberBandAreaOnContents());

			QRect lt(r.x() - 5,           r.y() - 5,            10, 10);
			QRect rt(r.x() + r.width() - 5, r.y() - 5,            10, 10);
			QRect lb(r.x() - 5,           r.y() + r.height() - 5, 10, 10);
			QRect rb(r.x() + r.width() - 5, r.y() + r.height() - 5, 10, 10);

			d->ltActive = false;
			d->rtActive = false;
			d->lbActive = false;
			d->rbActive = false;

			if (lt.contains(e->x(), e->y()))
			{
				viewport()->setCursor(Qt::SizeFDiagCursor);
				d->ltActive = true;
			}
			else if (rb.contains(e->x(), e->y()))
			{
				viewport()->setCursor(Qt::SizeFDiagCursor);
				d->rbActive = true;
			}
			else if (lb.contains(e->x(), e->y()))
			{
				viewport()->setCursor(Qt::SizeBDiagCursor);
				d->lbActive = true;
			}
			else if (rt.contains(e->x(), e->y()))
			{
				viewport()->setCursor(Qt::SizeBDiagCursor);
				d->rtActive = true;
			}
			else
				viewport()->unsetCursor();
		}
	}

	void Canvas::contentsMouseReleaseEvent(QMouseEvent *e)
	{
		if (!e)
			return;

		d->midButtonPressed = false;

		if (d->pressedMoving)
		{
			d->pressedMoving = false;
			viewport()->update();
		}

		if (d->pressedMoved && d->rubber->isActive())
		{
			d->tileCache.clear();
			viewport()->setMouseTracking(true);
			if (d->im->imageValid())
				emit signalSelected(true);
		}
		else
		{
			d->ltActive = false;
			d->rtActive = false;
			d->lbActive = false;
			d->rbActive = false;
			d->rubber->setActive(false);
			viewport()->setMouseTracking(false);
			viewport()->unsetCursor();
			if (d->im->imageValid())
				emit signalSelected(false);
		}

		if (e->button() != Qt::LeftButton)
		{
			viewport()->unsetCursor();
		}

		if (e->button() == Qt::RightButton)
		{
			emit signalRightButtonClicked();
		}
	}

	void Canvas::contentsWheelEvent(QWheelEvent *e)
	{
		e->accept();

		if (e->modifiers() & Qt::ShiftModifier)
		{
			if (e->delta() < 0)
				emit signalShowNextImage();
			else if (e->delta() > 0)
				emit signalShowPrevImage();
			return;
		}
		else if (e->modifiers() & Qt::ControlModifier)
		{
			if (e->delta() < 0)
				slotDecreaseZoom();
			else if (e->delta() > 0)
				slotIncreaseZoom();
			return;
		}

		Q3ScrollView::contentsWheelEvent(e);
	}

	bool Canvas::maxZoom()
	{
		return ((d->zoom * d->zoomMultiplier) >= d->maxZoom);
	}

	bool Canvas::minZoom()
	{
		return ((d->zoom / d->zoomMultiplier) <= d->minZoom);
	}

	bool Canvas::exifRotated()
	{
		return d->im->exifRotated();
	}

	double Canvas::snapZoom(double zoom)
	{
		// If the zoom value gets changed from d->zoom to zoom
		// across 50%, 100% or fit-to-window, then return the
		// the corresponding special value. Otherwise zoom is returned unchanged.
		double fit = calcAutoZoomFactor();
		QList<double> snapValues;
		snapValues.append(0.5);
		snapValues.append(1.0);
		snapValues.append(fit);

		if (d->zoom < zoom)
			qStableSort(snapValues);
		else
			qStableSort(snapValues.begin(), snapValues.end(), qGreater<double>());

		for (QList<double>::const_iterator it = snapValues.constBegin(); it != snapValues.constEnd(); ++it)
		{
			double z = *it;
			if ((d->zoom < z) && (zoom > z))
			{
				zoom = z;
				break;
			}
		}

		return zoom;
	}

	void Canvas::slotIncreaseZoom()
	{
		if (maxZoom())
			return;

		double zoom = d->zoom * d->zoomMultiplier;
		zoom        = snapZoom(zoom);
		setZoomFactor(zoom);
	}

	void Canvas::slotDecreaseZoom()
	{
		if (minZoom())
			return;

		double zoom = d->zoom / d->zoomMultiplier;
		zoom        = snapZoom(zoom);
		setZoomFactor(zoom);
	}

	void Canvas::setZoomFactorSnapped(double zoom)
	{
		double fit = calcAutoZoomFactor();

		if (fabs(zoom - fit) < 0.05)
		{
			// If 1.0 or 0.5 are even closer to zoom than fit, then choose these.
			if (fabs(zoom - fit) > fabs(zoom - 1.0))
			{
				zoom = 1.0;
			}
			else if (fabs(zoom - fit) > fabs(zoom - 0.5))
			{
				zoom = 0.5;
			}
			else
			{
				zoom = fit;
			}
		}
		else
		{
			if (fabs(zoom - 1.0) < 0.05)
			{
				zoom = 1.0;
			}
			if (fabs(zoom - 0.5) < 0.05)
			{
				zoom = 0.5;
			}
		}
		setZoomFactor(zoom);
	}

	double Canvas::zoomFactor()
	{
		return d->zoom;
	}

	void Canvas::setZoomFactor(double zoom)
	{
		if (d->autoZoom)
		{
			d->autoZoom = false;
			emit signalToggleOffFitToWindow();
		}

		// Zoom using center of canvas and given zoom factor.

		double cpx = contentsX() + visibleWidth()  / 2.0;
		double cpy = contentsY() + visibleHeight() / 2.0;

		cpx = (cpx / d->tileSize) * floor(d->tileSize / d->zoom);
		cpy = (cpy / d->tileSize) * floor(d->tileSize / d->zoom);

		d->zoom = zoom;

		d->im->zoom(d->zoom);
		updateContentsSize(false);

		viewport()->setUpdatesEnabled(false);
		center((int)((cpx * d->tileSize) / floor(d->tileSize / d->zoom)),
		       (int)((cpy * d->tileSize) / floor(d->tileSize / d->zoom)));
		viewport()->setUpdatesEnabled(true);
		viewport()->update();

		emit signalZoomChanged(d->zoom);
	}

	void Canvas::fitToSelect()
	{
		int xSel, ySel, wSel, hSel;
		d->im->getSelectedArea(xSel, ySel, wSel, hSel);

		if (wSel && hSel)
		{
			// If selected area, use center of selection
			// and recompute zoom factor accordingly.
			double cpx = xSel + wSel / 2.0;
			double cpy = ySel + hSel / 2.0;

			double srcWidth  = wSel;
			double srcHeight = hSel;
			double dstWidth  = contentsRect().width();
			double dstHeight = contentsRect().height();

			d->zoom = qMin(dstWidth / srcWidth, dstHeight / srcHeight);

			d->autoZoom = false;
			emit signalToggleOffFitToWindow();
			d->im->zoom(d->zoom);
			updateContentsSize(true);

			viewport()->setUpdatesEnabled(false);
			center((int)((cpx * d->tileSize) / floor(d->tileSize / d->zoom)),
			       (int)((cpy * d->tileSize) / floor(d->tileSize / d->zoom)));
			viewport()->setUpdatesEnabled(true);
			viewport()->update();

			emit signalZoomChanged(d->zoom);
		}
	}

	bool Canvas::fitToWindow()
	{
		return d->autoZoom;
	}

	void Canvas::toggleFitToWindow()
	{
		d->autoZoom = !d->autoZoom;

		if (d->autoZoom)
			updateAutoZoom();
		else
		{
			d->zoom = 1.0;
			emit signalZoomChanged(d->zoom);
		}

		d->im->zoom(d->zoom);
		updateContentsSize(false);
		slotZoomChanged(d->zoom);
		viewport()->update();
	}

	void Canvas::zoomFitToWindow()
	{
		d->autoZoom = true;
		updateAutoZoom();
		d->im->zoom(d->zoom);
		updateContentsSize(false);
		slotZoomChanged(d->zoom);
		viewport()->update();
	}

	void Canvas::slotRotate90()
	{
		d->im->rotate90();
	}

	void Canvas::slotRotate180()
	{
		d->im->rotate180();
	}

	void Canvas::slotRotate270()
	{
		d->im->rotate270();
	}

	void Canvas::slotFlipHoriz()
	{
		d->im->flipHoriz();
	}

	void Canvas::slotFlipVert()
	{
		d->im->flipVert();
	}

	void Canvas::slotCrop()
	{
		int x, y, w, h;
		d->im->getSelectedArea(x, y, w, h);

		if (!w && !h)   // No current selection.
			return;

		d->im->crop(x, y, w, h);
	}

	void Canvas::resizeImage(int w, int h)
	{
		d->im->resize(w, h);
	}

	void Canvas::setBackgroundColor(const QColor& color)
	{
		if (d->bgColor == color)
			return;

		d->bgColor = color;
		viewport()->update();
	}

	void Canvas::setICCSettings(ICCSettingsContainer *cmSettings)
	{
		d->im->setICCSettings(cmSettings);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::setExposureSettings(ExposureSettingsContainer *expoSettings)
	{
		d->im->setExposureSettings(expoSettings);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::setExifOrient(bool exifOrient)
	{
		d->im->setExifOrient(exifOrient);
		viewport()->update();
	}

	void Canvas::increaseGamma()
	{
		d->im->changeGamma(1);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::decreaseGamma()
	{
		d->im->changeGamma(-1);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::increaseBrightness()
	{
		d->im->changeBrightness(1);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::decreaseBrightness()
	{
		d->im->changeBrightness(-1);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::increaseContrast()
	{
		d->im->changeContrast(5);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::decreaseContrast()
	{
		d->im->changeContrast(-5);
		d->tileCache.clear();
		viewport()->update();
	}

	void Canvas::slotRestore()
	{
		d->im->restore();
	}

	void Canvas::slotUndo(int steps)
	{
		while (steps > 0)
		{
			d->im->undo();
			--steps;
		}
	}

	void Canvas::getUndoHistory(QStringList &titles)
	{
		d->im->getUndoHistory(titles);
	}

	void Canvas::getRedoHistory(QStringList &titles)
	{
		d->im->getRedoHistory(titles);
	}

	void Canvas::slotRedo(int steps)
	{
		while (steps > 0)
		{
			d->im->redo();
			--steps;
		}
	}

	void Canvas::slotCopy()
	{
		int x, y, w, h;
		d->im->getSelectedArea(x, y, w, h);

		if (!w && !h)   // No current selection.
			return;

		QApplication::setOverrideCursor(Qt::WaitCursor);
		uchar* data = d->im->getImageSelection();
		DImg selDImg = DImg(w, h, d->im->sixteenBit(), d->im->hasAlpha(), data);
		delete [] data;

		QImage selImg       = selDImg.copyQImage();
		QMimeData *mimeData = new QMimeData();
		mimeData->setImageData(selImg);
		QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
		QApplication::restoreOverrideCursor();
	}

	void Canvas::slotSelected()
	{
		int x = 0, y = 0, w = 0, h = 0;

		if (d->rubber->isActive() && d->pressedMoved)
		{
			QRect sel = calcSelectedArea();
			x = sel.x();
			y = sel.y();
			w = sel.width();
			h = sel.height();
		}

		d->im->setSelectedArea(x, y, w, h);
	}

	QRect Canvas::calcSelectedArea()
	{
		int x = 0, y = 0, w = 0, h = 0;
		QRect r(d->rubber->rubberBandAreaOnContents());

		if (r.isValid())
		{
			r.translate(- d->pixmapRect.x(), - d->pixmapRect.y());

			x = (int)(((double)r.x()      / d->tileSize) * floor(d->tileSize / d->zoom));
			y = (int)(((double)r.y()      / d->tileSize) * floor(d->tileSize / d->zoom));
			w = (int)(((double)r.width()  / d->tileSize) * floor(d->tileSize / d->zoom));
			h = (int)(((double)r.height() / d->tileSize) * floor(d->tileSize / d->zoom));

			x = qMin(imageWidth(),  qMax(x, 0));
			y = qMin(imageHeight(), qMax(y, 0));
			w = qMin(imageWidth(),  qMax(w, 0));
			h = qMin(imageHeight(), qMax(h, 0));

			// Avoid empty selection by rubberband - at least mark one pixel
			// At high zoom factors, the rubberband may operate at subpixel level!
			if (w == 0)
				w = 1;
			if (h == 0)
				h = 1;
		}

		return QRect(x, y, w, h);
	}

	void Canvas::slotModified()
	{
		if (d->autoZoom)
			updateAutoZoom();
		d->im->zoom(d->zoom);

		updateContentsSize(true);
		viewport()->update();

		// To be sure than corner widget used to pan image will be hide/show
		// accordingly with new image size (if changed).
		slotZoomChanged(d->zoom);

		emit signalChanged();
	}

	void Canvas::slotCornerButtonPressed()
	{
		if (d->panIconPopup)
		{
			d->panIconPopup->hide();
			d->panIconPopup->deleteLater();
			d->panIconPopup = 0;
		}

		d->panIconPopup         = new KPopupFrame(this);
		ImagePanIconWidget *pan = new ImagePanIconWidget(180, 120, d->panIconPopup);
		d->panIconPopup->setMainWidget(pan);

		QRect r((int)(contentsX()    / d->zoom), (int)(contentsY()     / d->zoom),
		        (int)(visibleWidth() / d->zoom), (int)(visibleHeight() / d->zoom));
		pan->setRegionSelection(r);
		pan->setMouseFocus();

		connect(pan, SIGNAL(signalSelectionMoved(const QRect&, bool)),
		        this, SLOT(slotPanIconSelectionMoved(const QRect&, bool)));

		connect(pan, SIGNAL(signalHidden()),
		        this, SLOT(slotPanIconHiden()));

		QPoint g = mapToGlobal(viewport()->pos());
		g.setX(g.x() + viewport()->size().width());
		g.setY(g.y() + viewport()->size().height());
		d->panIconPopup->popup(QPoint(g.x() - d->panIconPopup->width(),
		                              g.y() - d->panIconPopup->height()));

		pan->setCursorToLocalRegionSelectionCenter();
	}

	void Canvas::slotPanIconHiden()
	{
		d->cornerButton->blockSignals(true);
		d->cornerButton->animateClick();
		d->cornerButton->blockSignals(false);
	}

	void Canvas::slotPanIconSelectionMoved(const QRect& r, bool b)
	{
		setContentsPos((int)(r.x()*d->zoom), (int)(r.y()*d->zoom));

		if (b)
		{
			d->panIconPopup->hide();
			d->panIconPopup->deleteLater();
			d->panIconPopup = 0;
			slotPanIconHiden();
		}
	}

	void Canvas::slotZoomChanged(double /*zoom*/)
	{
		updateScrollBars();

		if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
			d->cornerButton->show();
		else
			d->cornerButton->hide();
	}

	void Canvas::slotSelectAll()
	{
		d->rubber->setRectOnContents(d->pixmapRect);
		d->pressedMoved = true;
		d->tileCache.clear();
		viewport()->setMouseTracking(true);
		viewport()->update();

		if (d->im->imageValid())
			emit signalSelected(true);
	}

	void Canvas::slotSelectNone()
	{
		reset();
		viewport()->update();
	}

}  // namespace Digikam
