/* __________________________________________________________________________
//                        .__.
//       S^3.O.S.  _.--^^ ___ \
//             _../^   _.^   ^\!
//          _./     .-^    -_. |
//      _-/^     .-^        > \
//     /^--..   /  /|     ./   \._
//           ^^/  / ^_-./^        ^.__        >-.-. www.s3os.net 
//               <_____..--^^^---...__^..__..
//               Solucions i Serveis amb Sistemes Open Source
// __________________________________________________________________________
//
//    Revision             :- $Revision: 1.3 $
//    Init Date            : Thu Feb 2 2006
//    licence              : GNU/GPL-2
//    programmer           : Shadow, (C) 2006
//    email                : shadow@s3os.net
//    File                 : SConfigWidget.h
//    Classification       :- C++ (Gcc 3.xx) Qt 4.xx
//                                                                    
// _________________________________________________________________________
//                                                                         
//   This program is free software; you can redistribute it and/or modify  
//   it under the terms of the GNU General Public License as published by  
//   the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
// _________________________________________________________________________
// ------------------------------------ CVS --------------------------------
// $Header: /home/cvs/s3os/flam/gui/SConfigWidget.h,v 1.3 2006/02/11 02:44:45 shadow Exp $
// _________________________________________________________________________*/
#ifndef SCONFIGWIDGET_H
#define SCONFIGWIDGET_H
#include <QWidget>
#include <QList>

#include "stwidgetsexport.h"


/**
Widget amb un iconview i un stacked widget que va molt bé per exemple per les configuracions del programa.
 
@author Shadow
*/
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class SAbstractConfigPage;
class ST_WIDGETS_EXPORT SConfigWidget : public QWidget
{
Q_OBJECT

	QListWidget* contentsWidget;
	QStackedWidget *pagesWidget;

public:
	SConfigWidget(QWidget* _Parent = 0);
	void addWidget(QWidget* _Widget, const QIcon& _Icon, const QString& _Label);
	
private slots:
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif
