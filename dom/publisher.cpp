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

#include "publisher.h"

using namespace STDom;

QString Publisher::DefaultUserName = PUBLISHER_LOGIN;
QString Publisher::DefaultUserPassword = PUBLISHER_PASSWD;

Publisher::Publisher()
{
	UserName = DefaultUserName;
	Password = DefaultUserPassword;
}

Publisher::Publisher(const QString& _Id, const QString& _Name, const QString& _Url, const QString& _UserName, const QString& _Password)
	: Id(_Id), Name(_Name), FtpUrl(_Url), UserName(_UserName), Password(_Password)
{
}

