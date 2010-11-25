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
#ifndef SICONFACTORY_H
#define SICONFACTORY_H
#include <QIcon>
#include "stwidgetsexport.h"


/**
Classe generadora de icones del gui. Consta d'un mètode per a instalar una nova generadora d'icones per defecte: setDefaultFactory que ens permetrà canviar qualsevol icona que utilitzi el GUI de flam.
La idea no és un estil dinàmic sinó la d'un estil per a l'usuari de la llibreria.
@author Shadow
*/
class ST_WIDGETS_EXPORT SIconFactory
{
public:
	enum IconType
	{
	    Accept, //Generals
	    Cancel,
	    Edit,
	    EditAdd,
	    EditDelete,
	    EditRemove,
	    EditUser,
	    EditClear,
	    EditCopy,
	    EditCut,
	    EditPaste,
	    EditTrash,
	    Exit,
	    Find,
	    Configure,
	    Help,
	    Print,
	    Start, //Posicionament
	    Rew,
	    Pause,
	    Stop,
	    Play,
	    Fwd,
	    End,
	    FileSave, //Fitxers
	    FileOpen,
	    FileClose,
	    FileSaveAs,
	    FileNew,
	    DropDown,
	    Advanced,
	    AdvancedUp,
	    Warning, //Info, warning, ...
	    Info,
	    Error,
	    Question,	
	    DatabaseSettings //Settings
	};
	ST_WIDGETS_EXPORT static const SIconFactory* DefaultFactory;

public:
	inline SIconFactory() {}
	virtual ~SIconFactory();
	virtual QIcon icon(IconType _Type) const = 0;
	static const SIconFactory* defaultFactory();
	static void setDefaultFactory(const SIconFactory* _Factory);
	friend struct SIconDefaultFactoryCleaner;
};

#define stIcon(Icon) SIconFactory::defaultFactory()->icon(Icon)
/**
Default icons factory.

@author Shadow
*/

class ST_WIDGETS_EXPORT FDefaultIconFactory : public SIconFactory
{
public:
	inline FDefaultIconFactory() {}
	QIcon icon(IconType _Type) const;
};

#endif
