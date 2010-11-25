/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage STSidebar in GUI.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern.ahrens@kdemail.net>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "stsidebar.h"

// Qt includes.

#include <QDataStream>
#include <QDragEnterEvent>
#include <QEvent>
#include <QPixmap>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>



class SidebarState
{

public:

    SidebarState() : activeWidget(0),size(0) {}
    SidebarState(QWidget *w, int size) : activeWidget(w),size(size) {}

    QWidget *activeWidget;
    int      size;
};

class SidebarPriv
{

public:

    SidebarPriv()
    {
        minimizedDefault = false;
        minimized        = false;
        isMinimized      = false;
        tabs             = 0;
        activeTab        = -1;
        dragSwitchId     = -1;
        stack            = 0;
        dragSwitchTimer  = 0;
        restoreSize      = 0;
    }

    bool             minimizedDefault;
    bool             minimized;
    bool             isMinimized;      // Backup of minimized status (used with Fullscreen)

    int              tabs;
    int              activeTab;
    int              dragSwitchId;

    int              restoreSize;

    QStackedWidget  *stack;
    SidebarSplitter *splitter;
    QTimer          *dragSwitchTimer;

    QHash<QWidget*, SidebarState>
                     appendedTabsStateCache;
};

class SidebarSplitterPriv
{
public:

    QList<STSidebar*> sidebars;
};

STSidebar::STSidebar(QWidget *parent, SidebarSplitter *sp, KMultiTabBarPosition side, bool minimizedDefault)
       : KMultiTabBar(side, parent), d(new SidebarPriv)
{
    d->minimizedDefault = minimizedDefault;
    d->stack            = new QStackedWidget(sp);
    d->splitter         = sp;
    d->dragSwitchTimer  = new QTimer(this);

    d->splitter->d->sidebars << this;

    connect(d->dragSwitchTimer, SIGNAL(timeout()),
            this, SLOT(slotDragSwitchTimer()));

    setStyle(KMultiTabBar::VSNET);
}

STSidebar::~STSidebar()
{
    saveViewState();
    if (d->splitter)
        d->splitter->d->sidebars.removeAll(this);
    delete d;
}

SidebarSplitter* STSidebar::splitter() const
{
    return d->splitter;
}

void STSidebar::loadViewState()
{
/*    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(QString("%1").arg(objectName()));
    int tab                   = group.readEntry("ActiveTab", 0);
    bool minimized            = group.readEntry("Minimized", d->minimizedDefault);
    d->restoreSize            = group.readEntry("RestoreSize", -1);

    // validate
    if(tab >= d->tabs || tab < 0)
        tab = 0;

    if (minimized)
    {
        d->activeTab = tab;
        setTab(d->activeTab, false);
        d->stack->setCurrentIndex(d->activeTab);
        shrink();
        emit signalChangedTab(d->stack->currentWidget());
        return;
    }

    d->activeTab = -1;
    clicked(tab);*/
}

void STSidebar::saveViewState()
{
/*    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(QString("%1").arg(objectName()));
    group.writeEntry("ActiveTab", d->activeTab);
    group.writeEntry("Minimized", d->minimized);
    group.writeEntry("RestoreSize", d->minimized ? d->restoreSize : -1);
    config->sync();*/
}

void STSidebar::backup()
{
    d->isMinimized = d->minimized;

    if (!d->isMinimized)
        shrink();

    KMultiTabBar::hide();
}

void STSidebar::backup(const QList<QWidget*> thirdWidgetsToBackup, QList<int> *sizes)
{
    sizes->clear();
    foreach (QWidget *widget, thirdWidgetsToBackup)
        *sizes << d->splitter->size(widget);

    backup();
}

void STSidebar::restore()
{
    if (!d->isMinimized)
        expand();

    KMultiTabBar::show();
}

void STSidebar::restore(const QList<QWidget*> thirdWidgetsToRestore, const QList<int> &sizes)
{
    restore();

    if (thirdWidgetsToRestore.size() == sizes.size())
    {
        for (int i=0; i<thirdWidgetsToRestore.size(); i++)
            d->splitter->setSize(thirdWidgetsToRestore[i], sizes[i]);
    }
}

void STSidebar::appendTab(QWidget *w, const QPixmap &pic, const QString &title)
{
    // Store state (but not on initialization)
    if (isVisible())
        d->appendedTabsStateCache[w] = SidebarState(d->stack->currentWidget(), d->splitter->size(this));

    // Add tab
    w->setParent(d->stack);
    KMultiTabBar::appendTab(pic, d->tabs, title);
    d->stack->insertWidget(d->tabs, w);

    tab(d->tabs)->setAcceptDrops(true);
    tab(d->tabs)->installEventFilter(this);

    connect(tab(d->tabs), SIGNAL(clicked(int)),
            this, SLOT(clicked(int)));

    d->tabs++;
}

void STSidebar::deleteTab(QWidget *w)
{
    int tab = d->stack->indexOf(w);
    if(tab < 0)
        return;

    bool removingActiveTab = (tab == d->activeTab);
    if (removingActiveTab)
        d->activeTab = -1;

    d->stack->removeWidget(d->stack->widget(tab));
    // delete widget
    removeTab(tab);
    d->tabs--;

    // restore or reset active tab and width
    if (!d->minimized)
    {
        // restore to state before adding tab
        // using a hash is simple, but does not handle well multiple add/removal operations at a time
        SidebarState state = d->appendedTabsStateCache.take(w);
        if (state.activeWidget)
        {
            int tab = d->stack->indexOf(state.activeWidget);
            if(tab != -1)
            {
                switchTabAndStackToTab(tab);
                emit signalChangedTab(d->stack->currentWidget());

                if (state.size == 0)
                {
                    d->minimized = true;
                    setTab(d->activeTab, false);
                }
                d->splitter->setSize(this, state.size);
            }
        }
        else
        {
            if (removingActiveTab)
                clicked(d->tabs - 1);
            d->splitter->setSize(this, -1);
        }
    }
    else
    {
        d->restoreSize = -1;
    }
}

void STSidebar::clicked(int tab)
{
    if(tab >= d->tabs || tab < 0)
        return;

    if(tab == d->activeTab)
    {
        d->stack->isHidden() ? expand() : shrink();
    }
    else
    {
        switchTabAndStackToTab(tab);

        if(d->minimized)
            expand();

        emit signalChangedTab(d->stack->currentWidget());
    }
}

void STSidebar::setActiveTab(QWidget *w)
{
    int tab = d->stack->indexOf(w);
    if(tab < 0)
        return;

    switchTabAndStackToTab(tab);

    if(d->minimized)
        expand();

    emit signalChangedTab(d->stack->currentWidget());
}

void STSidebar::switchTabAndStackToTab(int tab)
{
    if(d->activeTab >= 0)
        setTab(d->activeTab, false);

    d->activeTab = tab;
    setTab(d->activeTab, true);
    d->stack->setCurrentIndex(d->activeTab);
}

QWidget* STSidebar::getActiveTab()
{
    if (d->splitter)
        return d->stack->currentWidget();
    else
        return 0;
}

void STSidebar::shrink()
{
    d->minimized = true;

    // store the size that we had. We may later need it when we restore to visible.
    int currentSize = d->splitter->size(this);
    if (currentSize)
        d->restoreSize = currentSize;

    d->stack->hide();
    emit signalViewChanged();
}

void STSidebar::expand()
{
    d->minimized = false;
    d->stack->show();

    // Do not expand to size 0 (only splitter handle visible)
    // but either to previous size, or the minimum size hint
    if (d->splitter->size(this) == 0)
    {
        setTab(d->activeTab, true);
        d->splitter->setSize(this, d->restoreSize ? d->restoreSize : -1);
    }

    emit signalViewChanged();
}

bool STSidebar::isExpanded()
{
    return !d->minimized;
}

bool STSidebar::eventFilter(QObject *obj, QEvent *ev)
{
    for (int i = 0 ; i < d->tabs; i++)
    {
        if ( obj == tab(i) )
        {
            if ( ev->type() == QEvent::DragEnter)
            {
                QDragEnterEvent *e = static_cast<QDragEnterEvent *>(ev);
                enterEvent(e);
                e->accept();
                return false;
            }
            else if (ev->type() == QEvent::DragMove)
            {
                if (!d->dragSwitchTimer->isActive())
                {
                    d->dragSwitchTimer->setSingleShot(true);
                    d->dragSwitchTimer->start(800);
                    d->dragSwitchId = i;
                }
                return false;
            }
            else if (ev->type() == QEvent::DragLeave)
            {
                d->dragSwitchTimer->stop();
                QDragLeaveEvent *e = static_cast<QDragLeaveEvent *>(ev);
                leaveEvent(e);
                return false;
            }
            else if (ev->type() == QEvent::Drop)
            {
                d->dragSwitchTimer->stop();
                QDropEvent *e = static_cast<QDropEvent *>(ev);
                leaveEvent(e);
                return false;
            }
            else
            {
                return false;
            }
        }
    }

    // Else, pass the event on to the parent class
    return KMultiTabBar::eventFilter(obj, ev);
}

void STSidebar::slotDragSwitchTimer()
{
    clicked(d->dragSwitchId);
}

// -----------------------------------------------------------------------------

SidebarSplitter::SidebarSplitter(QWidget *parent)
               : QSplitter(parent), d(new SidebarSplitterPriv)
{
    connect(this, SIGNAL(splitterMoved(int,int)),
            this, SLOT(slotSplitterMoved(int,int)));
}

SidebarSplitter::SidebarSplitter(Qt::Orientation orientation, QWidget *parent)
               : QSplitter(orientation, parent), d(new SidebarSplitterPriv)
{
    connect(this, SIGNAL(splitterMoved(int,int)),
            this, SLOT(slotSplitterMoved(int,int)));
}

SidebarSplitter::~SidebarSplitter()
{
    // retreat cautiously from sidebars that live longer
    foreach(STSidebar *STSidebar, d->sidebars)
        STSidebar->d->splitter = 0;

    delete d;
}
/*
void SidebarSplitter::restoreState(KConfigGroup &group, const char *key)
{
    if (!key)
        key = "SplitterState";

    if (group.hasKey(key))
    {
        QByteArray state;
        state = group.readEntry(key, state);
        QSplitter::restoreState(QByteArray::fromBase64(state));
    }
}

void SidebarSplitter::saveState(KConfigGroup &group, const char *key)
{
    if (!key)
        key = "SplitterState";
    group.writeEntry(key, QSplitter::saveState().toBase64());
}*/

int SidebarSplitter::size(STSidebar *bar) const
{
    return size(bar->d->stack);
}

int SidebarSplitter::size(QWidget *widget) const
{
    int index = indexOf(widget);
    if (index == -1)
        return -1;

    return sizes()[index];
}

void SidebarSplitter::setSize(STSidebar *bar, int size)
{
    setSize(bar->d->stack, size);
}

void SidebarSplitter::setSize(QWidget *widget, int size)
{
    int index = indexOf(widget);
    if (index == -1)
        return;

    // special case: Use minimum size hint
    if (size == -1)
    {
        if (orientation() == Qt::Horizontal)
            size = widget->minimumSizeHint().width();
        if (orientation() == Qt::Vertical)
            size = widget->minimumSizeHint().height();
    }

    QList<int> sizeList = sizes();
    sizeList[index] = size;
    setSizes(sizeList);
}

void SidebarSplitter::slotSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos);

    // When the user moves the splitter so that size is 0 (collapsed),
    // it may be difficult to restore the STSidebar as clicking the buttons
    // has no effect (only hides/shows the splitter handle)
    // So we want to transform this size-0-STSidebar
    // to a STSidebar that is shrunk (d->stack hidden)
    // and can be restored by clicking a tab bar button

    // We need to look at the widget between index-1 and index
    // and the one between index and index+1

    QList<int> sizeList = sizes();
    // Is there a STSidebar with size 0 before index
    if (index > 0 && sizeList[index-1] == 0)
    {
        QWidget *w = widget(index-1);
        foreach(STSidebar *STSidebar, d->sidebars)
        {
            if (w == STSidebar->d->stack)
            {
                if (!STSidebar->d->minimized)
                {
                    STSidebar->setTab(STSidebar->d->activeTab, false);
                    STSidebar->shrink();
                }
                break;
            }
        }
    }

    // Is there a STSidebar with size 0 after index?
    if (sizeList[index] == 0)
    {
        QWidget *w = widget(index);
        foreach(STSidebar *STSidebar, d->sidebars)
        {
            if (w == STSidebar->d->stack)
            {
                if (!STSidebar->d->minimized)
                {
                    STSidebar->setTab(STSidebar->d->activeTab, false);
                    STSidebar->shrink();
                }
                break;
            }
        }
    }
}

