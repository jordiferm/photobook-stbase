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

#ifndef SPROCESSSTATUSWIDGET_H
#define SPROCESSSTATUSWIDGET_H

#include <QWidget>
#include "stwidgetsexport.h"

/**
Widget per mostrar l'estat d'un process.

@author shadow
*/
class QLabel;
class QProgressBar;
class QxtProgressLabel;
class ST_WIDGETS_EXPORT SProcessStatusWidget : public QWidget
{
Q_OBJECT
	QxtProgressLabel* LabStatus;
	QProgressBar* MProgressBar;
	QString CurrStatusMessage; 
	QLabel* LabPixmap;

public:
	SProcessStatusWidget(QWidget* _Parent = 0, Qt::WindowFlags _Flags = 0, Qt::Orientation _Orientation = Qt::Vertical);
	QProgressBar* progressBar() const;
	void moveToScreenCenter();
	void showProgressBar(QString _Message, int _NumSteps, bool _ShowTimes = true);
	void hideProgressBar();
	void restartTimes();
	int currentProgress();
	void incrementProgress();
	void updateProgress(int _Progress, QString _NewMessage = "");
	void showPixmap(const QPixmap& _Pixmap);

public slots: 
	void statusChanged(const QString& _StatusMsg, int _Progress = -1, bool _ShowTimes = true);
};

#endif
