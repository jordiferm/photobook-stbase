/****************************************************************************
**
** Copyright (C) 2012 Aili Image S.L. All rights reserved.
**
** This file is part of Aili Image Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.
**
** Aili Image reserves all rights not expressly granted herein.
**
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "siconfactory.h"

SIconFactory::~SIconFactory()
{}

const SIconFactory* SIconFactory::DefaultFactory = 0;

struct SIconDefaultFactoryCleaner
{
	inline SIconDefaultFactoryCleaner() {}
	~SIconDefaultFactoryCleaner() { delete SIconFactory::DefaultFactory; SIconFactory::DefaultFactory = 0; }
};

/*!
Returns the default item editor factory.

\sa setDefaultFactory()*/
const SIconFactory* SIconFactory::defaultFactory()
{
	static const FDefaultIconFactory Factory;
	if (DefaultFactory)
		return DefaultFactory;
	return &Factory;
}

/*!
    Sets the default item editor factory to the given \a factory.

    \sa defaultFactory()
*/
void SIconFactory::setDefaultFactory(const SIconFactory* _Factory)
{
	static const SIconDefaultFactoryCleaner Cleaner;
	if (DefaultFactory)
		delete DefaultFactory;
	DefaultFactory = _Factory;
}


QIcon FDefaultIconFactory::icon(IconType _Type) const
{
	switch (_Type)
	{
		case Accept :
			return QIcon(":/st/accept.png");
			break;
		case Cancel :
			return QIcon(":/st/cancel.png");
			break;
		case Edit :
			return QIcon(":/st/edit.png");
			break;
		case EditAdd :
			return QIcon(":/st/edit_add.png");
			break;
		case EditDelete :
			return QIcon(":/st/editdelete.png");
			break;
		case EditRemove:
			return QIcon(":/st/edit_remove.png");
			break;
		case EditUser:
			return QIcon(":/st/edit_user.png");
			break;
		case EditClear:
			return QIcon(":/st/editclear.png");
			break;
		case EditCopy:
			return QIcon(":/st/editcopy.png");
			break;
		case EditCut:
			return QIcon(":/st/editcut.png");
			break;
		case EditPaste:
			return QIcon(":/st/editpaste.png");
			break;
		case EditTrash:
			return QIcon(":/st/edittrash.png");
			break;
		case Exit :
			return QIcon(":/st/exit.png");
			break;
		case Find :
			return QIcon(":/st/filefind.png");
			break;
		case Configure :
			return QIcon(":/st/configure.png");
			break;
		case Help :
			return QIcon(":/st/help.png");
			break;
		case Print :
			return QIcon(":/st/print.png");
			break;
		case Start : //Seeking
			return QIcon(":/st/player_start.png");
			break;
		case Rew :
			return QIcon(":/st/player_rew.png");
			break;
		case Pause :
			return QIcon(":/st/player_pause.png");
			break;
		case Stop :
			return QIcon(":/st/player_stop.png");
			break;
		case Play :
			return QIcon(":/st/player_play.png");
			break;
		case Fwd :
			return QIcon(":/st/player_fwd.png");
			break;
		case End :
			return QIcon(":/st/player_end.png");
			break;
		case FileSave : //Files
			return QIcon(":/st/filesave.png");
			break;
		case FileOpen : 
			return QIcon(":/st/fileopen.png");
			break;
		case FileClose :
			return QIcon(":/st/fileclose.png");
			break;
		case FileSaveAs :
			return QIcon(":/st/filesaveas.png");
			break;
		case FileNew :
			return QIcon(":/st/filenew.png");
			break;		
		case DropDown :
			return QIcon(":/st/dropdown.png");
			break;
		case Advanced :
			return QIcon(":/st/advanced.png");
			break;
		case AdvancedUp :
			return QIcon(":/st/advancedup.png");
			break;
		case Warning :
			return QIcon(":/st/warning.png");
			break;
		case Info :
			return QIcon(":/st/info.png");
			break;
		case Error :
			return QIcon(":/st/error.png");
			break;
		case Question :
			return QIcon(":/st/question.png");
			break;
		case DatabaseSettings :
			return QIcon(":/st/databasesettings.png");
			break;
		default :
			return QIcon();
	}
}

