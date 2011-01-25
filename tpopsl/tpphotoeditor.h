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
#ifndef TPPHOTOEDITOR_H
#define TPPHOTOEDITOR_H

#include <QDialog>
#include <QModelIndex> 
#include <QDir>
#include "tpiphotoeditor.h" 
#include "stimage.h" 
#include "sterror.h" 
#include "sttpopslexport.h"
#include "printjobmodel.h"

/**
Photo editor Touch screen.

	@author
*/
class QToolButton;
class SelectionInterface; 
class QModelIndex; 
class QAction; 
class TPProductListView; 
class QAbstractItemModel; 
class SProcessStatusWidget; 
class QToolBar; 
class QLabel; 
class AddRemoveWidget;
class QSlider;
class QFrame;
class SResetSlider;
class ST_TPOPSL_EXPORT TPPhotoEditor : public QDialog, public TPIPhotoEditor
{
	Q_OBJECT

public:
	enum EnStatus{
		StatusSelectRedEyes, 
		StatusEditing
	};

	enum EnMode{
		ModeModel,
		ModeNoModel
	};

ST_DECLARE_ERRORCLASS(); 

private:

	STImage CurrImage; 
	QFileInfo SavedImage;
	QUndoStack* UndoStack;
	QSizeF AspectRatio; 
	double AspectRatioZoom;
	int Dpis; 
	EnStatus Status; 
	QModelIndex CurrentIndex; 

	SelectionInterface* SelectionIface; 
	STDom::PrintJobModel* Model;
	TPProductListView* LVProducts;
	SProcessStatusWidget* ProcStatusW; 
	QToolBar* ActToolBar;
	QToolBar* EfectsToolBar;
	QLabel* LabImageInfo;
	QLabel* ReceiptLabel;
	QLabel* LabContrast;
	QLabel* LabBright;
	QLabel* LabGamma;
	QLabel* LabGammaR;
	QLabel* LabGammaG;
	QLabel* LabGammaB;

	QAction* Rotate90Action;
	QAction* Rotate270Action;
	QAction* ZoomMagAction;
	QAction* ZoomMinAction;
	QAction* CropAction; 

	QAction* BlackAndWhiteEfAction;
	QAction* SepiaEfAction;
	QAction* AutoCorrectionEfAction;
	QAction* NoCutEfAction;
	QAction* RedEyesAction;
	QAction* AdvancedAction;

	QToolButton* CancelBut;
	QToolButton* AcceptBut;
	QToolButton* ButPrevImage;
	QToolButton* ButNextImage;

	QFrame* FrameBasicControls;
	QFrame* FrameAdvanced;

	SResetSlider* ContrastSl;
	SResetSlider* BrightSl;
	SResetSlider* GammaSl;
	SResetSlider* GammaRSl;
	SResetSlider* GammaGSl;
	SResetSlider* GammaBSl;



	AddRemoveWidget* ARWidget;

	QDir TempPath;

	void updateSelection();
	void beginApplyEffect();
	void endApplyEffect();
	bool thereIsAdvancedChanges();
	void resetAdvancedChanges();
	void applyBrightAndContrast();
	void applyGamma();
	void applyGammaRGB();
	void retranslateUi();
	QToolButton* newActionButton(const QString& _Icon);
	QToolButton* newMiniActionButton(const QString& _Icon);
	void updateCurrProductSelection();
	void adjustToolButtonWidth(QToolBar* _ToolBar);

	void removeToolTips(QToolBar* _ToolBar); 
	void setStatus(EnStatus _Status);
	void saveCurrentImage(); 
	void setMode(EnMode _Mode); 
	SResetSlider* newSlider(QWidget* _Parent);

public:
	TPPhotoEditor(QWidget* parent = 0, Qt::WindowFlags f = 0);
	void setModel(STDom::PrintJobModel* _Model);
	void setProductsModel(QAbstractItemModel* _Model);
	void setCurrentIndex(const QModelIndex& _Index);
	void init();
	void show();
	void showAdvanced(bool _Value);
	int exec(const QModelIndex& _CurrIndex = QModelIndex());
	void setTempPath(const QDir& _TempPath);
	QDir tempPath() const;

	virtual void setImage(const QImage& _Image); 
	virtual void crop(const QPoint& _TopLeft, const QPoint& _BottomRight);
	virtual void transform(STImage::EnTransformCodes _TransformCode);
	virtual void applyWhiteMargin(const QSize& _Size);
	virtual void redEyes(const QPoint& _TopLeft, const QPoint& _BottomRight);
	virtual void blackAndWhite(); 
	virtual void sepia();
	virtual void autoColor();
	virtual void autoContrast(); 
	virtual QImage currentImage() const;
	QFileInfo savedImage() const { return SavedImage; }

	void setAspectRatio(const QSizeF& _AspectRatio);
	void setAspectRatioZoom(double _Value);
	void setDpis(int _Value) { Dpis = _Value; }
	int dpis() const { return Dpis; }
	void setReceiptText(const QString& _Text);

private slots: 
	void productActivated(const QModelIndex& );
	void zoomMag();
	void zoomMin();
	void doCropCommand();
	void doRotateRightCommand();
	void doRotateLeftCommand();
	void doBlackAndWhiteCommand();
	void doSepiaCommand();
	void doAutoCorrectionCommand();
	void doWhiteMarginCommand();
	void redEyesTriggered();
	void advancedToggled(bool _Value);
	void nextImage(); 
	void previousImage();
	void updateImageInfo();
	void incSelectedProduct(); 
	void decSelectedProduct(); 
	void saveChanges();
	void slotContrastChanged(int _Value);
	void slotBrightChanged( int _Value);
	void slotGammaRGBChanged();

signals:
	void orderChanged();

};

#endif
