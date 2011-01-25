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

#ifndef STMASKMIMEDATA_H
#define STMASKMIMEDATA_H

#include <QMimeData>

class STMaskMimeData : public QMimeData
{
	Q_OBJECT
	QString MaskFilePath;

public:
	STMaskMimeData();
	QString maskFilePath() const { return MaskFilePath; }
	void setMaskFilePath(const QString& _Value) { MaskFilePath = _Value; }
};

#endif // STMASKMIMEDATA_H
