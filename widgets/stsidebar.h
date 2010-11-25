/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in GUI.
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

#ifndef STSIDEBAR_H
#define STSIDEBAR_H

// Qt includes.

#include <QtGui/QPixmap>
#include <QtGui/QSplitter>

#include "kmultitabbar.h"
#include "stwidgetsexport.h"


class QSplitter;


class SidebarPriv;
class SidebarSplitter;
class SidebarSplitterPriv;

/**
 * This class handles a sidebar view
 */
class ST_WIDGETS_EXPORT STSidebar : public KMultiTabBar
{
    Q_OBJECT

public:

    /**
     * Creates a new sidebar
     * @param parent sidebar's parent
     * @param sp sets the splitter, which should handle the width. The splitter normally
     *           is part of the main view. Internally, the width of the widget stack can
     *           be changed by a QSplitter.
     * @param side where the sidebar should be displayed. At the left or right border.
                   Use KMultiTabBar::Left or KMultiTabBar::Right.
     * @param minimizedDefault hide the sidebar when the program is started the first time.
     */
    STSidebar(QWidget *parent, SidebarSplitter *sp, KMultiTabBarPosition side=KMultiTabBar::Left,
            bool mimimizedDefault=false);

    virtual ~STSidebar();

    SidebarSplitter* splitter() const;

    /**
     * Appends a new tab to the sidebar
     * @param w widget which is activated by this tab
     * @param pic icon which is shown in this tab
     * @param title text which is shown it this tab
     */
    void appendTab(QWidget *w, const QPixmap &pic, const QString &title);

    /**
     * Deletes a tab from the tabbar
     */
    void deleteTab(QWidget *w);

    /**
     * Activates a tab
     */
    void setActiveTab(QWidget *w);

    /**
     * Returns the currently activated tab, or 0 if no tab is active
    */
    QWidget* getActiveTab();

    /**
     * Hides the sidebar (display only the activation buttons)
     */
    void shrink();

    /**
     * redisplays the whole sidebar
     */
    void expand();

    /**
     * load the last view state from disk
     */
    void loadViewState();

    /**
     * hide sidebar and backup minimized state.
     */
    void backup();

    /**
     * Hide sidebar and backup minimized state.
     * If there are other widgets in this splitter, stores
     * their sizes in the provided list.
     */
    void backup(const QList<QWidget*> thirdWidgetsToBackup, QList<int> *sizes);

    /**
     * show sidebar and restore minimized state.
     */
    void restore();

    /**
     * Show sidebar and restore minimized state.
     * Restores other widgets' sizes in splitter.
     */
    void restore(const QList<QWidget*> thirdWidgetsToRestore, const QList<int> &sizes);

    /**
     * return the visible status of current sidebar tab.
     */
    bool isExpanded();

private:

    /**
     * save the view state to disk
     */
    void saveViewState();
    bool eventFilter(QObject *o, QEvent *e);
    void switchTabAndStackToTab(int tab);

private Q_SLOTS:

    /**
     * Activates a tab
     */
    void clicked(int tab);

    void slotDragSwitchTimer();

Q_SIGNALS:

    /**
     * is emitted, when another tab is activated
     */
    void signalChangedTab(QWidget *w);

    /**
     * is emitted, when tab is shrink or expanded
     */
    void signalViewChanged();

private:

    friend class SidebarSplitter;

    SidebarPriv* const d;
};

// -----------------------------------------------------------------------------

class ST_WIDGETS_EXPORT SidebarSplitter : public QSplitter
{
    Q_OBJECT

public:

    /**
     *  This is a QSplitter with better support for storing its state
     *  in config files, especially if Sidebars are contained in the splitter.
     */
    SidebarSplitter(QWidget *parent = 0);
    SidebarSplitter(Qt::Orientation orientation, QWidget *parent = 0);

    ~SidebarSplitter();

    /**
     * Saves the splitter state to group, handling minimized sidebars correctly.
     * If key is 0 (default), the config key will be "SplitterState".
     */
//    void saveState(KConfigGroup &group, const char *key = 0);
    /**
     * Restores the splitter state from group, handling minimized sidebars correctly.
     * If key is 0 (default), the config key will be "SplitterState".
     */
//    void restoreState(KConfigGroup &group, const char *key = 0);

    /**
     * Returns the value of sizes() that corresponds to the given Sidebar or splitter child widget.
     */
    int size(STSidebar *bar) const;
    int size(QWidget *widget) const;
    /**
     * Sets the splitter size for the given sidebar or splitter child widget to size.
     * Special value -1: Sets the minimum size hint of the widget.
     */
    void setSize(STSidebar *bar, int size);
    void setSize(QWidget *widget, int size);

private Q_SLOTS:

    void slotSplitterMoved(int pos, int index);

private:

    friend class STSidebar;

    SidebarSplitterPriv* const d;
};


#endif // SIDEBAR_H
