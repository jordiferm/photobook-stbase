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
#ifndef BASICFIXESWIDGET_H
#define BASICFIXESWIDGET_H

#include <QToolBox>
#include <QRadioButton> 
#include "fstdactions.h"
#include "storomelexport.h"


class QVBoxLayout; 
class ST_OROMEL_EXPORT ApplyEfectBasicWidget : public QWidget
{
	Q_OBJECT
	QVBoxLayout* MLayout;
	
public:
	ApplyEfectBasicWidget(const QString& _HelpText, QWidget* _Parent = 0, 
								 const QString& _ActionText = tr("Apply"), const QIcon& _ActionIcon = stIcon(SIconFactory::Accept));
	void setMainWidget(QWidget* _MainWidget);
signals:
	void applyEffect();	
};

/**
RadioButton with DimensionData.

	@author 
*/

class ST_OROMEL_EXPORT DimensionRadioButton : public QRadioButton
{
	QSize Dimension; 
	
	Q_OBJECT
	
public:
	DimensionRadioButton(const QSize& _Dimension, QWidget* _Parent)
	: QRadioButton(QString::number(_Dimension.width()) + "x" + QString::number(_Dimension.height()),
						 _Parent), Dimension(_Dimension) {}
	QSize dimension()	{ return Dimension; }
};

/**
Widget crop actions.

	@author 
*/
class QPushButton; 
class ST_OROMEL_EXPORT CropFixesWidget : public ApplyEfectBasicWidget
{
	Q_OBJECT 	
public:	
	CropFixesWidget(QWidget* _Parent = 0);
	QPushButton* RotateButton;
	QButtonGroup* RButtonGroup;
	
private slots:
	void rotateToggled(bool );
	void cropSizeClicked(QAbstractButton* );
	void zoomSliderChanged(int _Value);
	
signals:
	void cropSizeChanged(const QSizeF& _SizeInInches);
	void cropZoomChanged(double _Value);
};

/**
Widget straighten actions.

	@author 
*/
class QPushButton; 
class ST_OROMEL_EXPORT StraightenFixesWidget : public ApplyEfectBasicWidget
{
	Q_OBJECT 	
public:	
	StraightenFixesWidget(QWidget* _Parent = 0); 
};

/**
Widget for simple transformations.

	@author 
*/
class ST_OROMEL_EXPORT TransformationsWidget : public QWidget
{
	Q_OBJECT
public: 
	TransformationsWidget(QWidget* _Parent = 0);
	
signals:
	void rotateLeft();
	void rotateRight();
	void flipX();
	void flipY();
};

/**
Widget straighten actions.

	@author 
*/
class QPushButton; 
class ST_OROMEL_EXPORT RedEyeFixesWidget : public ApplyEfectBasicWidget
{
	Q_OBJECT
			
public:	
	RedEyeFixesWidget(QWidget* _Parent = 0); 
};


/**
Widget for basic fixes actions.

	@author 
*/
class ST_OROMEL_EXPORT BasicFixesWidget : public QToolBox
{
Q_OBJECT
public:
    BasicFixesWidget(QWidget *parent = 0);

    ~BasicFixesWidget();

signals:
	void cropSizeChanged(const QSizeF& _SizeInInches);
	void applyCrop(); 
	void cropZoomChanged(double _Value);
	void applyStraighten();
	void rotateLeft();
	void rotateRight();
	void flipX();
	void flipY();
	void applyRedEyes(); 
};

#endif
