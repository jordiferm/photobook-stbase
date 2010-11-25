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
#ifndef FSTDACTIONS_H
#define FSTDACTIONS_H

#include <QAction>
#include <QString>
#include "stwidgetsexport.h"
#include "siconfactory.h"

/**
Standard action classes.
FAction inherits QAction to provide an Id. Ids of actions are interesting if we want to manage it in switch sentences.

We also define a collection of action classes using FLAM_ACTIONCLASS macro with corresponding Id, and icon. It Uses an iconfactory to retrieve action icons.

@author Shadow
*/

class ST_WIDGETS_EXPORT FAction : public QAction
{

	Q_OBJECT

public:
	enum EnActionId
	{
	    FAcceptActionId,
	    FCancelActionId,
	    FEditActionId,
	    FEditAddActionId,
	    FEditDeleteActionId,
	    FExitActionId,
	    FFindActionId,
	    FConfigureActionId,
	    FHelpActionId,
	    FPrintActionId,
	    FYesActionId,
	    FNoActionId,

	    // Seeking
	    FStartActionId,
	    FRewActionId,
	    FPreviousActionId,
	    FPauseActionId,
	    FStopActionId,
	    FRunActionId,
	    FNextActionId,
	    FFweActionId,
	    FEndActionId,
	    // Files
	    FFileSaveActionId,
	    FFileCloseActionId,
	    FFileSaveAsActionId,
	    // Others
	    FAdvancedActionId,
	    FCustomAction
	};
	int Id;

public:
	FAction(QObject* _Parent, int _Id) : QAction(_Parent), Id(_Id) {}
	int id() const { return Id;}
	static void addActions(const QActionGroup* _Source, QActionGroup* _Destination);
};


#define FLAM_ACTIONCLASS(Class, Icon, Text) \
   class Class : public FAction { \
public: \
   Class(QObject* _Parent = 0) : FAction(_Parent, Class##Id) \
{ \
	setIcon(Icon); \
	setText(Text); \
	setIconText(Text); \
} \
};

class ST_WIDGETS_EXPORT FYesAction : public FAction
{
Q_OBJECT
public:
	FYesAction(QObject* _Parent = 0) : FAction(_Parent, FYesActionId)
	{
		setIcon(SIconFactory::defaultFactory()->icon(SIconFactory::Accept));
		setText(tr("&Si"));
		setIconText(tr("&Si"));
		setShortcut(Qt::Key_Return);
	}
};


// Generals
FLAM_ACTIONCLASS(FAcceptAction, SIconFactory::defaultFactory()->icon(SIconFactory::Accept), QObject::tr("&Accept"))
FLAM_ACTIONCLASS(FCancelAction, SIconFactory::defaultFactory()->icon(SIconFactory::Cancel), QObject::tr("&Cancel"))
FLAM_ACTIONCLASS(FEditAction, SIconFactory::defaultFactory()->icon(SIconFactory::Edit), QObject::tr("&Edit"))
FLAM_ACTIONCLASS(FEditAddAction, SIconFactory::defaultFactory()->icon(SIconFactory::EditAdd), QObject::tr("Add"))
FLAM_ACTIONCLASS(FEditDeleteAction, SIconFactory::defaultFactory()->icon(SIconFactory::EditDelete), QObject::tr("Delete"))
FLAM_ACTIONCLASS(FExitAction, SIconFactory::defaultFactory()->icon(SIconFactory::Exit), QObject::tr("&Exit"))
FLAM_ACTIONCLASS(FFindAction, SIconFactory::defaultFactory()->icon(SIconFactory::Find), QObject::tr("&Find"))
FLAM_ACTIONCLASS(FConfigureAction, SIconFactory::defaultFactory()->icon(SIconFactory::Configure), QObject::tr("Configure"))
FLAM_ACTIONCLASS(FHelpAction, SIconFactory::defaultFactory()->icon(SIconFactory::Help), QObject::tr("Help"))
FLAM_ACTIONCLASS(FPrintAction, SIconFactory::defaultFactory()->icon(SIconFactory::Print), QObject::tr("&Print"))
//FLAM_ACTIONCLASS(FYesAction, SIconFactory::defaultFactory()->icon(SIconFactory::Accept), QObject::tr("&Yes"))
FLAM_ACTIONCLASS(FNoAction, SIconFactory::defaultFactory()->icon(SIconFactory::Cancel), QObject::tr("&No"))

// Posicionament
FLAM_ACTIONCLASS(FStartAction, SIconFactory::defaultFactory()->icon(SIconFactory::Start), QObject::tr("Start"))
FLAM_ACTIONCLASS(FRewAction, SIconFactory::defaultFactory()->icon(SIconFactory::Rew), QObject::tr("Rewind"))
FLAM_ACTIONCLASS(FPreviousAction, SIconFactory::defaultFactory()->icon(SIconFactory::Rew), QObject::tr("Previous"))
FLAM_ACTIONCLASS(FPauseAction, SIconFactory::defaultFactory()->icon(SIconFactory::Pause), QObject::tr("Pause"))
FLAM_ACTIONCLASS(FStopAction, SIconFactory::defaultFactory()->icon(SIconFactory::Stop), QObject::tr("Stop"))
FLAM_ACTIONCLASS(FRunAction, SIconFactory::defaultFactory()->icon(SIconFactory::Play), QObject::tr("Play"))
FLAM_ACTIONCLASS(FNextAction, SIconFactory::defaultFactory()->icon(SIconFactory::Fwd), QObject::tr("Next"))
FLAM_ACTIONCLASS(FFweAction, SIconFactory::defaultFactory()->icon(SIconFactory::Fwd), QObject::tr("Forward"))
FLAM_ACTIONCLASS(FEndAction, SIconFactory::defaultFactory()->icon(SIconFactory::End), QObject::tr("&End"))

// Fitxers
FLAM_ACTIONCLASS(FFileSaveAction, SIconFactory::defaultFactory()->icon(SIconFactory::FileSave), QObject::tr("Save File"))
FLAM_ACTIONCLASS(FFileCloseAction, SIconFactory::defaultFactory()->icon(SIconFactory::FileClose), QObject::tr("File Close"))
FLAM_ACTIONCLASS(FFileSaveAsAction, SIconFactory::defaultFactory()->icon(SIconFactory::FileSaveAs), QObject::tr("Save File as"))

//Altres
FLAM_ACTIONCLASS(FAdvancedAction, SIconFactory::defaultFactory()->icon(SIconFactory::Advanced), QObject::tr("Advanced"))


#define FAcceptCancelActions FGeneralActionGroup

class ST_WIDGETS_EXPORT FGeneralActionGroup : public QActionGroup
{
public:
	enum EnActions
	{
	    Accept = 1,
	    Cancel = 2,
	    Edit = 4,
	    EditAdd = 8,
	    EditDelete = 16,
	    Find = 32,
	    Configure = 64,
	    Help = 128,
	    Print = 256,
	    Exit = 512,
	    Yes = 1024,
	    No = 2048

	};

public :
	FGeneralActionGroup(QObject* _Parent, int _Actions = Accept | Cancel);
};

//! Accions d'edició
class FEditActions : public FGeneralActionGroup
{
public:
	inline FEditActions(QObject* _Parent, int _Actions = Edit | EditAdd | EditDelete)
			: FGeneralActionGroup(_Parent, _Actions) {}};

class ST_WIDGETS_EXPORT FPositionActionGroup : public QActionGroup
{
public:
	enum EnActions
	{
	    Start = 1,
	    Rew = 2,
	    Previous = 4,
	    Pause = 8,
	    Stop = 16,
	    Run = 32,
	    Next = 64,
	    Fwe = 128,
	    End = 256
	};

public :
	FPositionActionGroup(QObject* _Parent, int _Actions = Start | Previous | Next | End);
};

class ST_WIDGETS_EXPORT FFileActionGroup : public QActionGroup
{
	enum EnActions
	{
	    Save = 1,
	    Close = 2,
	    SaveAs = 4,
	};

public:
	FFileActionGroup(QObject* _Parent, int _Actions = Save | Close | SaveAs);
};


#endif

