/****************************************************************************
**
** Copyright (C) 2006-2010 Starblitz. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STPROGRESSINDICATOR_H
#define STPROGRESSINDICATOR_H
#include "stwidgetsexport.h"

class QProgressBar;
class QString;
class ST_WIDGETS_EXPORT STProgressIndicator
{
	QProgressBar* ProgressBar;


public:
	STProgressIndicator(QProgressBar* _ProgressBar = 0);
	void setProgressBar(QProgressBar* _ProgressBar) {  ProgressBar = _ProgressBar; }
	virtual void setRange(int _From, int _To);
	virtual void start(const QString& _Message);
	virtual void setValue(int _Value);
	virtual void incValue();
	virtual void stop();
};

#endif // STPROGRESSINDICATOR_H
