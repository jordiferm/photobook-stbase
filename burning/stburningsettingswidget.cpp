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
#include "stburningsettingswidget.h"
#include <QLayout> 
#include <QLabel> 
#include <QDataWidgetMapper> 


#include "stburningsettings.h" 
#include "fileselectlineedit.h" 
#include "qxtgroupbox.h"


STBurningSettingsWidget::STBurningSettingsWidget(STBurningSettings& _Settings, QDataWidgetMapper* _Mapper, QWidget* _Parent)
 : SMappedWidget(_Settings.model(), _Mapper, _Parent)
{
	QGridLayout* MLayout = new QGridLayout(this); 
	SAppSettingsModel* Model = _Settings.model(); 

	QxtGroupBox* GBDBWidget = new QxtGroupBox(headerLabelText(Model->index("stburning/enabled")) , this);
	_Mapper->addMapping(GBDBWidget, Model->index("stburning/enabled").column());
	MLayout->addWidget(GBDBWidget, MLayout->rowCount(), 0);

	QGridLayout* GBLayout = new QGridLayout(GBDBWidget);

	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/mkisofsbinary")), GBLayout->rowCount(), 0);
	FileSelectLineEdit* LEPath = new FileSelectLineEdit(FileSelectLineEdit::GetOpenFileName, this);
	_Mapper->addMapping(LEPath, Model->index("stburning/mkisofsbinary").column());
	GBLayout->addWidget(LEPath, GBLayout->rowCount(), 0);
	
	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/tmpisofile")), GBLayout->rowCount(), 0);
	LEPath = new FileSelectLineEdit(FileSelectLineEdit::GetSaveFileName, this);
	_Mapper->addMapping(LEPath, Model->index("stburning/tmpisofile").column());
	GBLayout->addWidget(LEPath, GBLayout->rowCount(), 0);

	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/cdrecordbinary")), GBLayout->rowCount(), 0);
	LEPath = new FileSelectLineEdit(FileSelectLineEdit::GetOpenFileName, this);
	_Mapper->addMapping(LEPath, Model->index("stburning/cdrecordbinary").column());
	GBLayout->addWidget(LEPath, GBLayout->rowCount(), 0);

	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/device")), GBLayout->rowCount(), 0);
	LEPath = new FileSelectLineEdit(FileSelectLineEdit::GetOpenFileName, this);
	_Mapper->addMapping(LEPath, Model->index("stburning/device").column());
	GBLayout->addWidget(LEPath, GBLayout->rowCount(), 0);

	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/speed")), GBLayout->rowCount() , 0);
	GBLayout->addWidget(createEditor(Model->index("stburning/speed")), GBLayout->rowCount(), 0);

	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/simulationmode")), GBLayout->rowCount(), 0);
	GBLayout->addWidget(createEditor(Model->index("stburning/simulationmode")), GBLayout->rowCount(), 0);

	GBLayout->addWidget(createHeaderLabel(Model->index("stburning/writedebuginfo")), GBLayout->rowCount() , 0);
	GBLayout->addWidget(createEditor(Model->index("stburning/writedebuginfo")), GBLayout->rowCount(), 0);

	GBLayout->addItem(new QSpacerItem(0,10, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding), GBLayout->rowCount(), 0);
}


