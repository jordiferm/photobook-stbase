/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-08-20
 * Description : Image editor interface used by editor tools.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "editortooliface.h"

// Qt includes.

#include <QWidget>

// Local includes.

//#include "sidebar.h"
//#include "canvas.h"
//#include "statusprogressbar.h"
#include "editortool.h"
#include "editortoolsettings.h"
//#include "editorstackview.h"
//#include "editorwindow.h"

namespace Digikam
{

class EditorToolIfacePriv
{

public:

    EditorToolIfacePriv()
    {
        prevTab = 0;
        //editor  = 0;
        tool    = 0;
    }

    QWidget      *prevTab;

    EditorTool   *tool;

    //EditorWindow *editor;
};

EditorToolIface* EditorToolIface::m_iface = 0;

EditorToolIface* EditorToolIface::editorToolIface()
{
    return m_iface;
}

EditorToolIface::EditorToolIface(QObject* _Parent)
               : QObject(_Parent), d(new EditorToolIfacePriv)
{
    //d->editor = editor;
    m_iface   = this;
}

EditorToolIface::~EditorToolIface()
{
    delete d;
    if (m_iface == this)
        m_iface = 0;
}

EditorTool* EditorToolIface::currentTool() const
{
    return d->tool;
}

void EditorToolIface::loadTool(EditorTool* tool)
{
/*    if (d->tool) unLoadTool();

    d->tool = tool;
    d->editor->editorStackView()->setToolView(d->tool->toolView());
    d->editor->editorStackView()->setViewMode(EditorStackView::ToolViewMode);
    d->prevTab = d->editor->rightSideBar()->getActiveTab();
    d->editor->rightSideBar()->appendTab(d->tool->toolSettings(), d->tool->toolIcon(), d->tool->toolName());
    d->editor->rightSideBar()->setActiveTab(d->tool->toolSettings());
    d->editor->toggleActions(false);

    // If editor tool has zoomable preview, switch on zoom actions.
    if (d->editor->editorStackView()->previewWidget())
        d->editor->toggleZoomActions(true);*/
}

void EditorToolIface::unLoadTool()
{
/*    if (!d->tool) return;

    d->editor->editorStackView()->setViewMode(EditorStackView::CanvasMode);
    d->editor->editorStackView()->setToolView(0);
    d->editor->rightSideBar()->deleteTab(d->tool->toolSettings());
    d->editor->rightSideBar()->setActiveTab(d->prevTab);
    d->editor->toggleActions(true);
    // To restore canvas zoom level in zoom combobox.
    if (!d->editor->editorStackView()->canvas()->fitToWindow())
        d->editor->editorStackView()->setZoomFactor(d->editor->editorStackView()->canvas()->zoomFactor());
    delete d->tool;
    d->tool = 0;*/
}

void EditorToolIface::setToolStartProgress(const QString& toolName)
{
	emit toolStartProgress(toolName);
/*    d->editor->setToolStartProgress(toolName);
    if (d->editor->editorStackView()->previewWidget())
        d->editor->toggleZoomActions(false);*/
}

void EditorToolIface::setToolProgress(int progress)
{
	emit toolProgress(progress); 
    //d->editor->setToolProgress(progress);
}

void EditorToolIface::setToolStopProgress()
{
	emit toolStopProgress(); 
/*    d->editor->setToolStopProgress();
    if (d->editor->editorStackView()->previewWidget())
        d->editor->toggleZoomActions(true);*/
}

void EditorToolIface::slotToolAborted()
{
    EditorToolThreaded *tool = dynamic_cast<EditorToolThreaded*>(d->tool);
    if (tool) tool->slotAbort();
}

}  // namespace Digikam
