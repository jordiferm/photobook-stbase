/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2007 John Layt <john@layt.net>
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kpopupframe.h"
#include <QKeyEvent>
#include <QDesktopWidget> 
#include <QEventLoop> 


class KPopupFrame::KPopupFramePrivate
{
public:
	KPopupFramePrivate(KPopupFrame *q):
			q(q),
			result(0),   // rejected
			main(0)
	{
	}

	KPopupFrame *q;

	/**
	 * The result. It is returned from exec() when the popup window closes.
	 */
	int result;

	/**
	 * The only subwidget that uses the whole dialog window.
	 */
	QWidget *main;
};



// JPL Shouldn't this be in own file as is used in a couple of places?  Or moved to private in KDE5?

KPopupFrame::KPopupFrame(QWidget* parent)
		: QFrame(parent, Qt::Popup), d(new KPopupFramePrivate(this))
{
	setFrameStyle(QFrame::Box | QFrame::Raised);
	setMidLineWidth(2);
}

KPopupFrame::~KPopupFrame()
{
	delete d;
}

void KPopupFrame::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Escape)
	{
		d->result = 0; // rejected
		emit leaveModality();
		//qApp->exit_loop();
	}
}

void KPopupFrame::close(int r)
{
	d->result = r;
	emit leaveModality();
	//qApp->exit_loop();
}

void KPopupFrame::setMainWidget(QWidget *m)
{
	d->main = m;
	if (d->main)
	{
		resize(d->main->width() + 2 * frameWidth(), d->main->height() + 2 * frameWidth());
	}
}

void KPopupFrame::resizeEvent(QResizeEvent *e)
{
	Q_UNUSED(e);

	if (d->main)
	{
		d->main->setGeometry(frameWidth(), frameWidth(),
		                     width() - 2 * frameWidth(), height() - 2 * frameWidth());
	}
}

void KPopupFrame::popup(const QPoint &pos)
{
	// Make sure the whole popup is visible.
	QDesktopWidget Desktop; 
	QRect desktopGeometry = Desktop.availableGeometry(pos);

	int x = pos.x();
	int y = pos.y();
	int w = width();
	int h = height();
	if (x + w > desktopGeometry.x() + desktopGeometry.width())
	{
		x = desktopGeometry.width() - w;
	}
	if (y + h > desktopGeometry.y() + desktopGeometry.height())
	{
		y = desktopGeometry.height() - h;
	}
	if (x < desktopGeometry.x())
	{
		x = 0;
	}
	if (y < desktopGeometry.y())
	{
		y = 0;
	}

	// Pop the thingy up.
	move(x, y);
	show();
	d->main->setFocus();
}

int KPopupFrame::exec(const QPoint &pos)
{
	popup(pos);
	repaint();
	QEventLoop eventLoop;
	connect(this, SIGNAL(leaveModality()),
	        &eventLoop, SLOT(quit()));
	eventLoop.exec();

	hide();
	return d->result;
}

int KPopupFrame::exec(int x, int y)
{
	return exec(QPoint(x, y));
}

