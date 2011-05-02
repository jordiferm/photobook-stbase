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
#include "tpphotoeditor.h"
#include <QLayout> 
#include <QToolButton> 
#include <QToolBar> 
#include <QAction> 
#include <QListView>
#include <QApplication> 

#include "selectionInterface.h" 
#include "tpproductlistview.h" 
#include "addremovewidget.h" 
#include "imageTools.h" 
#include "sprocessstatuswidget.h" 
#include "smessagebox.h" 
#include "sresetslider.h"
#include "dimagedoc.h"
#include "exifmetadata.h"


void TPPhotoEditor::updateSelection()
{
	//determine selected width and height;
	int SelectedWidth = 0;
	int SelectedHeight = 0;
	int ImageWidth = CurrImage.width();
	int ImageHeight = CurrImage.height();

	QSizeF CAspectRatio = AspectRatio;

	//select region using same aspect ratio
	if (SelectionRotated)
	{
		SelectedWidth = ImageHeight;
		SelectedHeight = (int)((((double)(ImageHeight * CAspectRatio.width())) / CAspectRatio.height()));
	}
	else
	{
		SelectedWidth = ImageWidth;
		SelectedHeight = (int)((((double)(ImageWidth * CAspectRatio.height())) / CAspectRatio.width()));
	}

	calcScaledImageDimensions(SelectedWidth, SelectedHeight,
	                          ImageWidth, ImageHeight,
	                          SelectedWidth, SelectedHeight);
	SelectedWidth = SelectedWidth / AspectRatioZoom;
	SelectedHeight = SelectedHeight / AspectRatioZoom;
	//get current selection boundary
	QPoint CurTopLeft, CurBottomRight;
	SelectionIface->getSelection(CurTopLeft, CurBottomRight);

	//get current selection center
	QPoint CurCenter;
	CurCenter.setX((CurTopLeft.x() + CurBottomRight.x()) / 2);
	CurCenter.setY((CurTopLeft.y() + CurBottomRight.y()) / 2);

	//if center is off image (no previous selection) then
	//fix center to center of image
	if (CurCenter.x() < 0 || CurCenter.y() < 0)
	{
		CurCenter.setX(ImageWidth / 2);
		CurCenter.setY(ImageHeight / 2);
	}

	//attempt to center new selection overold selection, only
	//offset if necessary
	QPoint NewTopLeft, NewBottomRight;

	NewTopLeft.setX(CurCenter.x() - SelectedWidth / 2);
	NewTopLeft.setY(CurCenter.y() - SelectedHeight / 2);

	//push right/down if necessary
	if (NewTopLeft.x() < 0) NewTopLeft.setX(0);
	if (NewTopLeft.y() < 0) NewTopLeft.setY(0);

	//push left/up if necessary
	NewBottomRight.setX(NewTopLeft.x() + SelectedWidth - 1);
	if (NewBottomRight.x() >= ImageWidth)
	{
		NewBottomRight.setX(ImageWidth - 1);
		NewTopLeft.setX(NewBottomRight.x() - SelectedWidth + 1);
	}

	NewBottomRight.setY(NewTopLeft.y() + SelectedHeight - 1);
	if (NewBottomRight.y() >= ImageHeight)
	{
		NewBottomRight.setY(ImageHeight - 1);
		NewTopLeft.setY(NewBottomRight.y() - SelectedHeight + 1);
	}

	//select region
	SelectionIface->setSelection(NewTopLeft, NewBottomRight,
								 STDom::DDocFormat::mmToInch( qMax(CAspectRatio.width(), CAspectRatio.height())));
}

void TPPhotoEditor::beginApplyEffect()
{
	setDisabled(true);
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

}

void TPPhotoEditor::endApplyEffect()
{
	setDisabled(false);
	qApp->restoreOverrideCursor();
	QApplication::processEvents();
}

bool TPPhotoEditor::thereIsAdvancedChanges()
{
	return (BrightSl->value() != 0 || ContrastSl->value() != 0) ||
			   (GammaSl->value() != 0) ||
			   (GammaRSl->value() != 0 || GammaGSl->value() != 0 || GammaBSl->value() != 0);
}

void TPPhotoEditor::resetAdvancedChanges()
{
	BrightSl->setValue(0);
	ContrastSl->setValue(0);
	GammaSl->setValue(0);
	GammaRSl->setValue(0);
	GammaGSl->setValue(0);
	GammaBSl->setValue(0);
}

void TPPhotoEditor::applyBrightAndContrast()
{
	if (BrightSl->value() != 0 || ContrastSl->value() != 0)
	{
		beginApplyEffect();
		SelectionIface->adjustBrightAndContrast(&CurrImage, BrightSl->value(), ContrastSl->value());
		endApplyEffect();
	}
}

void TPPhotoEditor::applyGamma()
{
	if (GammaSl->value() != 0)
	{
		beginApplyEffect();
		SelectionIface->adjustGamma(CurrImage, GammaSl->value());
		endApplyEffect();
	}
}

void TPPhotoEditor::applyGammaRGB()
{
	if (GammaRSl->value() != 0 ||
		GammaGSl->value() != 0 || GammaBSl->value() != 0)
	{
		beginApplyEffect();
		SelectionIface->adjustGammaRGB(CurrImage, GammaRSl->value(), GammaGSl->value(), GammaBSl->value());
		endApplyEffect();
	}
}

void TPPhotoEditor::retranslateUi()
{
	Rotate90Action->setText(tr("Rotate 90�")); 
	Rotate270Action->setText(tr("Rotate 270�"));
	ZoomMagAction->setText(tr("Zoom+"));
	ZoomMinAction->setText(tr("Zoom-"));
	CropAction->setText(tr("Crop"));
	RotateSelectionAction->setText(tr("Rotate Sel."));

	BlackAndWhiteEfAction->setText(tr("Black&White")); 
	SepiaEfAction->setText(tr("Sepia")); 
	AutoCorrectionEfAction->setText(tr("Auto correction")); 
	NoCutEfAction->setText(tr("No Cut")); 
	RedEyesAction->setText(tr("Red eyes")); 

	CancelBut->setText(tr("Cancel")); 
	AcceptBut->setText(tr("Accept")); 

	LabContrast->setText(tr("Contrast"));
	LabBright->setText(tr("Brightness"));
	LabGamma->setText(tr("Gamma"));
	LabGammaR->setText(tr("Red"));
	LabGammaG->setText(tr("Green"));
	LabGammaB->setText(tr("Blue"));
	adjustToolButtonWidth(ActToolBar);
	adjustToolButtonWidth(EfectsToolBar);
}

QToolButton* TPPhotoEditor::newActionButton(const QString& _Icon)
{
	QToolButton* Res = new QToolButton(this); 
	Res->setObjectName("ActionButton"); 
 	Res->setIcon(QIcon(_Icon));
 	Res->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); 
 	Res->setIconSize(QSize(64,64)); 
	return Res; 
}

QToolButton* TPPhotoEditor::newMiniActionButton(const QString& _Icon)
{
	QToolButton* Res = new QToolButton(this); 
	Res->setObjectName("MiniActionButton"); 
 	Res->setIcon(QIcon(_Icon));
 	Res->setIconSize(QSize(48,48)); 
	return Res; 
}

void TPPhotoEditor::updateCurrProductSelection()
{
	productActivated(LVProducts->listView()->currentIndex());
}

void TPPhotoEditor::adjustToolButtonWidth(QToolBar* _ToolBar)
{
	QList<QToolButton *> ToolButtons = _ToolBar->findChildren<QToolButton *>();
	QList<QToolButton *>::iterator it;
	int MinWidth = 0;
	for (it = ToolButtons.begin(); it != ToolButtons.end(); ++it)
	{
		MinWidth = qMax(MinWidth, (*it)->width());
	}
	for (it = ToolButtons.begin(); it != ToolButtons.end(); ++it)
	{
		(*it)->setMinimumWidth(MinWidth);
	}
}

void TPPhotoEditor::removeToolTips(QToolBar* _ToolBar)
{
	QList<QToolButton *> ToolButtons = _ToolBar->findChildren<QToolButton *>();
	QList<QToolButton *>::iterator it; 
	for (it = ToolButtons.begin(); it != ToolButtons.end(); ++it)
	{
		(*it)->setToolTip(""); 
	}
}

void TPPhotoEditor::setStatus(EnStatus _Status)
{
	//SelectionIface->setDrawSelectionAllowed(_Status == StatusSelectRedEyes);
	BlackAndWhiteEfAction->setEnabled(_Status != StatusSelectRedEyes);
	SepiaEfAction->setEnabled(_Status != StatusSelectRedEyes);
	AutoCorrectionEfAction->setEnabled(_Status != StatusSelectRedEyes);
	NoCutEfAction->setEnabled(_Status != StatusSelectRedEyes);
	ActToolBar->setEnabled(_Status != StatusSelectRedEyes);

	SelectionIface->setDrawSelectionAllowed(_Status == StatusSelectRedEyes);
	if (_Status == StatusSelectRedEyes)
	{
		SelectionIface->setSelection(QPoint(0,0), QPoint(0,0)); //Clears the selection.
		RedEyesAction->setIcon(QIcon(":/st/tpopsl/accept.png")); 
		RedEyesAction->setText(tr("Confirm Red eyes")); 
	}
	else 
	{
		RedEyesAction->setIcon(QIcon(":/st/tpopsl/xeyes.png")); 
		RedEyesAction->setText(tr("Red eyes")); 
	}

	Status = _Status; 
}

void TPPhotoEditor::saveCurrentImage()
{
	QDir TmpDir(tempPath());
	//Si el directori temporal no existeix el creem.
	if (!TmpDir.exists())
		Assert(TmpDir.mkpath(TmpDir.absolutePath()), 
			Error(QString(tr("Temp directory %1 could not be created")).arg(TmpDir.absolutePath())));

	QString TmpImageFileName(TmpDir.absolutePath() + "/" + CurrImage.hashString() + ".JPG");
	Assert(CurrImage.save(TmpImageFileName), Error(tr("Error saving file: %1").arg(TmpImageFileName)));
	//Change image in model.
	SavedImage = QFileInfo(TmpImageFileName);
	if (Model)
	{
		Model->changeDocFile(CurrentIndex, SavedImage);
		//DDocPrintList PList = Model->productCopies(CurrentIndex);
		//Model->sourceImageModel()->setImage(Model->mapToSource(CurrentIndex), ColImg);
		//Model->setProductCopies(CurrentIndex, PList);
	}
	//Model->sourceImageModel()->insertImage(Model->mapToSource(CurrentIndex), ColImg);  
}

void TPPhotoEditor::setMode(EnMode _Mode)
{
	ButPrevImage->setVisible(_Mode == ModeModel);
	LabImageInfo->setVisible(_Mode == ModeModel);
	ButNextImage->setVisible(_Mode == ModeModel);
	ARWidget->setVisible(_Mode == ModeModel);
	ReceiptLabel->setVisible(_Mode == ModeModel);	
	LVProducts->setVisible(_Mode == ModeModel);
	NoCutEfAction->setVisible(_Mode == ModeModel);
}

SResetSlider* TPPhotoEditor::newSlider(QWidget* _Parent)
{
	SResetSlider* Res = new SResetSlider(Qt::Horizontal, _Parent);
	Res->slider()->setMinimumHeight(40);
	Res->slider()->setMaximum(50);
	Res->slider()->setMinimum(-50);
	return Res;
}

TPPhotoEditor::TPPhotoEditor(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f), Model(0), AspectRatioZoom(1), Dpis(300), SelectionRotated(false)
{
	QVBoxLayout* MLayout = new QVBoxLayout(this); 
	QHBoxLayout* TopLayout = new QHBoxLayout; 
	MLayout->addLayout(TopLayout); 
	QHBoxLayout* BottomLayout = new QHBoxLayout;
	MLayout->addLayout(BottomLayout); 

	MLayout->setStretchFactor(TopLayout, 5); 
	MLayout->setStretchFactor(BottomLayout, 1); 

	//Top Layout (Actions, image and effects)
	ActToolBar = new QToolBar(this); 
	ActToolBar->setOrientation(Qt::Vertical); 
	ActToolBar->setIconSize(QSize(64,64)); 
	ActToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); 

	TopLayout->addWidget(ActToolBar); 

	Rotate90Action = ActToolBar->addAction(QIcon(":/st/tpopsl/object_rotate_left.png"), tr("Rotate 90�"));
	connect(Rotate90Action, SIGNAL(triggered( bool )), this, SLOT(doRotateLeftCommand())); 
	Rotate270Action = ActToolBar->addAction(QIcon(":/st/tpopsl/object_rotate_right.png"), tr("Rotate 270�"));
	connect(Rotate270Action, SIGNAL(triggered( bool )), this, SLOT(doRotateRightCommand())); 
	ZoomMagAction = ActToolBar->addAction(QIcon(":/st/tpopsl/viewmag+.png"), tr("Zoom+"));
	ZoomMagAction->setAutoRepeat(true); 
	connect(ZoomMagAction, SIGNAL(triggered( bool )), this, SLOT(zoomMag())); 
	ZoomMinAction = ActToolBar->addAction(QIcon(":/st/tpopsl/viewmag-.png"), tr("Zoom-"));
	ZoomMinAction->setAutoRepeat(true); 
	connect(ZoomMinAction, SIGNAL(triggered( bool )), this, SLOT(zoomMin())); 
	CropAction = ActToolBar->addAction(QIcon(":/st/tpopsl/cut.png"), tr("Crop"));
	connect(CropAction, SIGNAL(triggered( bool )), this, SLOT(doCropCommand())); 
	RotateSelectionAction = ActToolBar->addAction(QIcon(":/st/tpopsl/rotateselection.png"), tr("Rotate Sel."));
	connect(RotateSelectionAction, SIGNAL(triggered( bool )), this, SLOT(doRotateSelectionCommand()));

	removeToolTips(ActToolBar); 

	//Image Frame 
	QFrame* ImageFrame = new QFrame(this); 
	TopLayout->addWidget(ImageFrame); 
	QVBoxLayout* ImageFrameLayout = new QVBoxLayout(ImageFrame); 
	ImageFrameLayout->setMargin(0); 
	ImageFrameLayout->setSpacing(0); 

	ProcStatusW = new SProcessStatusWidget(ImageFrame); 
	ImageFrameLayout->addWidget(ProcStatusW); 
	ProcStatusW->setVisible(false); 

	SelectionIface = new SelectionInterface(ImageFrame);
	ImageFrameLayout->addWidget(SelectionIface);

	QHBoxLayout* IFrameBottomLayout = new QHBoxLayout;
	IFrameBottomLayout->setSpacing(2); 
	ImageFrameLayout->addLayout(IFrameBottomLayout); 
	ImageFrameLayout->setStretchFactor(SelectionIface, 5); 
	
	ButPrevImage = newMiniActionButton(":/st/tpopsl/previous.png"); 
	connect(ButPrevImage, SIGNAL(clicked( bool )), this, SLOT(previousImage())); 
	IFrameBottomLayout->addWidget(ButPrevImage); 

	LabImageInfo = new QLabel(tr("No Info"), this); 
	IFrameBottomLayout->addWidget(LabImageInfo); 

	ButNextImage = newMiniActionButton(":/st/tpopsl/next.png"); 
	connect(ButNextImage, SIGNAL(clicked( bool )), this, SLOT(nextImage())); 
	IFrameBottomLayout->addWidget(ButNextImage); 


	//Effects
	EfectsToolBar = new QToolBar(this);
	EfectsToolBar->setOrientation(Qt::Vertical); 
	EfectsToolBar->setIconSize(QSize(64,64)); 
	EfectsToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); 

	TopLayout->addWidget(EfectsToolBar); 

	BlackAndWhiteEfAction = EfectsToolBar->addAction(QIcon(":/st/tpopsl/blackandwhite.png"), tr("Black&White"));
	connect(BlackAndWhiteEfAction, SIGNAL(triggered( bool )), this, SLOT(doBlackAndWhiteCommand())); 
	SepiaEfAction = EfectsToolBar->addAction(QIcon(":/st/tpopsl/sepia.png"), tr("Sepia"));
	connect(SepiaEfAction, SIGNAL(triggered( bool )), this, SLOT(doSepiaCommand())); 
	AutoCorrectionEfAction = EfectsToolBar->addAction(QIcon(":/st/tpopsl/wand.png"), tr("Auto correction"));
	connect(AutoCorrectionEfAction, SIGNAL(triggered( bool )), this, SLOT(doAutoCorrectionCommand())); 
	NoCutEfAction = EfectsToolBar->addAction(QIcon(":/st/tpopsl/nocut.png"), tr("No Cut"));
	connect(NoCutEfAction, SIGNAL(triggered( bool )), this, SLOT(doWhiteMarginCommand())); 
	RedEyesAction = EfectsToolBar->addAction(QIcon(":/st/tpopsl/xeyes.png"), tr("Red eyes"));
	connect(RedEyesAction, SIGNAL(triggered(bool )), this, SLOT(redEyesTriggered()));
	AdvancedAction = EfectsToolBar->addAction(QIcon(":/st/tpopsl/show-advanced.png"), tr("Advanced"));
	AdvancedAction->setCheckable(true);
	connect(AdvancedAction, SIGNAL(toggled(bool )), this, SLOT(advancedToggled(bool )));

	removeToolTips(EfectsToolBar); 



	//Bottom widgets	
	CancelBut = newActionButton(":/st/tpopsl/cancel.png");
	connect(CancelBut, SIGNAL(clicked( bool )), this, SLOT(reject())); 
	BottomLayout->addWidget(CancelBut); 


	//Basic controls
	FrameBasicControls = new QFrame(this);
	BottomLayout->addWidget((FrameBasicControls));

	QHBoxLayout* BCLayout = new QHBoxLayout(FrameBasicControls);

	QToolBar* UndoToolBar = new QToolBar(FrameBasicControls);
	UndoToolBar->setOrientation(Qt::Vertical);

	UndoToolBar->setIconSize(QSize(32,32));
	UndoToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	BCLayout->addWidget(UndoToolBar);

	LVProducts = new TPProductListView(FrameBasicControls);
	connect(LVProducts, SIGNAL(clicked( const QModelIndex& )), this, SLOT(productActivated(const QModelIndex& )));
	BCLayout->addWidget(LVProducts);

	ARWidget = new AddRemoveWidget(FrameBasicControls);
	connect(ARWidget, SIGNAL(addClicked()), this, SLOT(incSelectedProduct())); 
	connect(ARWidget, SIGNAL(removeClicked()), this, SLOT(decSelectedProduct())); 
	BCLayout->addWidget(ARWidget);

	ReceiptLabel = new QLabel(tr("Receipt"), FrameBasicControls);
	ReceiptLabel->setObjectName("ReceiptLabel");
	BCLayout->addWidget(ReceiptLabel);
	

	//Advanced controls Frame
	FrameAdvanced = new QFrame(this);
	BottomLayout->addWidget(FrameAdvanced);

	QGridLayout* FrameAdbLayout = new QGridLayout(FrameAdvanced);
	LabContrast = new QLabel(FrameAdvanced);

	FrameAdbLayout->addWidget(LabContrast, FrameAdbLayout->rowCount(), 0);
	ContrastSl = newSlider(FrameAdvanced);
	connect(ContrastSl, SIGNAL(valueChanged(int )), this, SLOT(slotContrastChanged( int)));
	FrameAdbLayout->addWidget(ContrastSl, FrameAdbLayout->rowCount() - 1, 1);

	LabBright = new QLabel(FrameAdvanced);
	FrameAdbLayout->addWidget(LabBright, FrameAdbLayout->rowCount(), 0);
	BrightSl = newSlider(FrameAdvanced);
	connect(BrightSl, SIGNAL(valueChanged(int )), this, SLOT(slotBrightChanged( int)));
	FrameAdbLayout->addWidget(BrightSl, FrameAdbLayout->rowCount() - 1, 1);

	LabGamma = new QLabel(FrameAdvanced);
	FrameAdbLayout->addWidget(LabGamma, FrameAdbLayout->rowCount(), 0);
	GammaSl = newSlider(FrameAdvanced);
	connect(GammaSl, SIGNAL(valueChanged(int )), SelectionIface, SLOT(slotChangeGamma( int)));
	FrameAdbLayout->addWidget(GammaSl, FrameAdbLayout->rowCount() - 1, 1);

	LabGammaR = new QLabel(FrameAdvanced);
	FrameAdbLayout->addWidget(LabGammaR, FrameAdbLayout->rowCount(), 0);
	GammaRSl = newSlider(FrameAdvanced);
	connect(GammaRSl, SIGNAL(valueChanged(int )), this, SLOT(slotGammaRGBChanged()));
	FrameAdbLayout->addWidget(GammaRSl, FrameAdbLayout->rowCount() - 1, 1);

	LabGammaG = new QLabel(FrameAdvanced);
	FrameAdbLayout->addWidget(LabGammaG, FrameAdbLayout->rowCount(), 0);
	GammaGSl = newSlider(FrameAdvanced);
	connect(GammaGSl, SIGNAL(valueChanged(int )), this, SLOT(slotGammaRGBChanged()));
	FrameAdbLayout->addWidget(GammaGSl, FrameAdbLayout->rowCount() - 1, 1);

	LabGammaB = new QLabel(FrameAdvanced);
	FrameAdbLayout->addWidget(LabGammaB, FrameAdbLayout->rowCount(), 0);
	GammaBSl = newSlider(FrameAdvanced);
	connect(GammaBSl, SIGNAL(valueChanged(int )), this, SLOT(slotGammaRGBChanged()));
	FrameAdbLayout->addWidget(GammaBSl, FrameAdbLayout->rowCount() - 1, 1);


	//Accept button
	AcceptBut = newActionButton(":/st/tpopsl/accept.png");
	connect(AcceptBut, SIGNAL(clicked( bool )), this, SLOT(saveChanges()));
	connect(AcceptBut, SIGNAL(clicked( bool )), this, SLOT(accept()));
	BottomLayout->addWidget(AcceptBut);

	//Undo 
	UndoStack = new QUndoStack(this);
	QAction* UndoAction = UndoStack->createUndoAction(this); 
	UndoAction->setIcon(QIcon(":/st/tpopsl/edit_undo.png")); 
	UndoToolBar->addAction(UndoAction); 

	QAction* RedoAction = UndoStack->createRedoAction(this); 
	RedoAction ->setIcon(QIcon(":/st/tpopsl/edit_redo.png")); 
	UndoToolBar->addAction(RedoAction );
	setTempPath(QDir(QDir::tempPath() + "/tpopsledit_images")); //Default Temp path.
	setStatus(StatusEditing); 
	setMode(ModeNoModel); 
	showAdvanced(false);
}

void TPPhotoEditor::setModel(STDom::PrintJobModel* _Model)
{
	setMode(ModeModel); 
	Model = _Model; 
	connect(Model, SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )), this, SLOT(updateImageInfo())); 
}

void TPPhotoEditor::setProductsModel(QAbstractItemModel* _Model)
{
	LVProducts->setModel(_Model);
}

void TPPhotoEditor::setCurrentIndex(const QModelIndex& _Index)
{
	saveChanges();
	CurrentIndex = _Index; 
	if (Model)
	{
		STDom::DDoc* CurrDoc = Model->doc(_Index);
		//Non image docs isn't supported at the moment
		if (CurrDoc->type() == STDom::DImageDoc::Type) //If Doc is an Image
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			STDom::DImageDoc* Image = static_cast<STDom::DImageDoc*>(CurrDoc);
			STDom::DImageDoc::CInfo ImageInfo = Image->getInfo();
			QString CurrImageFile = CurrDoc->fileInfo().absoluteFilePath();


			STImage Img;
			Assert(Img.load(CurrImageFile), Error(tr("Error loading image: %1").arg(CurrImageFile)));
			//Correct orientation if we have this info and the doc is an image
			if (!ImageInfo.isNull())
			{
				Img = ExifMetadata::correctOrientation(Img, ImageInfo.orientation());
			}

			setImage(Img);
			updateImageInfo();

			ButNextImage->setVisible(_Index.row() < Model->rowCount() -1);
			ButPrevImage->setVisible(_Index.row() > 0);
			//SelectionIface->setPhoto(CurrImageFile, true);
			QApplication::restoreOverrideCursor();
		}
		else
			SMessageBox::warning(this, tr("Loading non image doc"), tr("Doc type not supported yet."));
	}
}

void TPPhotoEditor::init()
{
	SelectionRotated = false;
	CurrentIndex = QModelIndex(); 
	setStatus(StatusEditing); 
	UndoStack->clear();
	retranslateUi();
	resetAdvancedChanges();
	showAdvanced(false);
	AdvancedAction->setChecked(false);
}

void TPPhotoEditor::show()
{
	init(); 
	showFullScreen();	
}

void TPPhotoEditor::showAdvanced(bool _Value)
{
	Rotate270Action->setVisible(!_Value);
	RotateSelectionAction->setVisible(!_Value);
	NoCutEfAction->setVisible(!_Value);
	RedEyesAction->setVisible(!_Value);

	if (_Value)
	{
		FrameBasicControls->setVisible(!_Value && LVProducts->listView()->model());
		FrameAdvanced->setVisible(_Value);
		AdvancedAction->setIcon(QIcon(":/st/tpopsl/hide-advanced.png"));
		AdvancedAction->setText(tr("Basic"));
	}
	else
	{
		FrameAdvanced->setVisible(_Value);
		FrameBasicControls->setVisible(!_Value && LVProducts->listView()->model());
		AdvancedAction->setIcon(QIcon(":/st/tpopsl/show-advanced.png"));
		AdvancedAction->setText(tr("Advanced"));
	}
}


int TPPhotoEditor::exec(const QModelIndex& _CurrIndex)
{
	show(); 
	if (_CurrIndex.isValid())
		setCurrentIndex(_CurrIndex); 
	return QDialog::exec();
}

void TPPhotoEditor::setTempPath(const QDir& _TempPath)
{
	TempPath = _TempPath; 
}

QDir TPPhotoEditor::tempPath() const
{
	return TempPath;
}

void TPPhotoEditor::setMinDpis(int _Dpis)
{
	SelectionIface->setMinDpis(_Dpis);
}

void TPPhotoEditor::setImage(const QImage& _Image)
{
	CurrImage = _Image;
	SelectionIface->setPhoto(CurrImage, true);
	updateCurrProductSelection();
}

void TPPhotoEditor::crop(const QPoint& _TopLeft, const QPoint& _BottomRight)
{
	beginApplyEffect();
	setImage(CurrImage.crop(_TopLeft, _BottomRight));
	endApplyEffect();
}

void TPPhotoEditor::transform(STImage::EnTransformCodes _TransformCode)
{
	beginApplyEffect();
	setImage(CurrImage.transform(_TransformCode));
	endApplyEffect();	
}

void TPPhotoEditor::blackAndWhite()
{
	beginApplyEffect();
	ProcStatusW->show();
	setImage(CurrImage.blackAndWhite(ProcStatusW));
	ProcStatusW->hide();
	endApplyEffect();
}

void TPPhotoEditor::sepia()
{
	beginApplyEffect();
	ProcStatusW->show();
	setImage(CurrImage.sepia(ProcStatusW));
	ProcStatusW->hide();
	endApplyEffect();
}

void TPPhotoEditor::autoColor()
{
	beginApplyEffect();
	ProcStatusW->show();
	setImage(CurrImage.improveColorBalance(ProcStatusW));
	ProcStatusW->hide();
	endApplyEffect();
}

void TPPhotoEditor::autoContrast()
{
	beginApplyEffect();
	ProcStatusW->show();
	setImage(CurrImage.enhanceImageContrast(ProcStatusW));
	ProcStatusW->hide();
	endApplyEffect();
}

void TPPhotoEditor::applyWhiteMargin(const QSize& _Size)
{
	beginApplyEffect();
	setImage(CurrImage.applyMargin(_Size));
	endApplyEffect();
}

void TPPhotoEditor::redEyes(const QPoint& _TopLeft, const QPoint& _BottomRight)
{
	beginApplyEffect();
	ProcStatusW->show();
	setImage(CurrImage.correctRedEye(_TopLeft, _BottomRight, ProcStatusW));
	ProcStatusW->hide();
	endApplyEffect();
}

QImage TPPhotoEditor::currentImage() const
{
	return CurrImage; 
}


void TPPhotoEditor::setAspectRatio(const QSizeF& _AspectRatio)
{
	AspectRatio = _AspectRatio;
	updateSelection();
}

void TPPhotoEditor::setAspectRatioZoom(double _Value)
{
	AspectRatioZoom = _Value;
	updateSelection();
}

void TPPhotoEditor::setReceiptText(const QString& _Text)
{ 
	ReceiptLabel->setText(_Text); 
}

void TPPhotoEditor::setCurrentProductIndex(const QModelIndex& _Index)
{
	LVProducts->setCurrentIndex(_Index);
}

void TPPhotoEditor::productActivated(const QModelIndex& _Index)
{
	if (_Index.isValid())
	{
		AspectRatioZoom = 1; 
		STDom::DDocProduct CurrProduct = LVProducts->product(_Index);
		QSize ImgSize = SelectionIface->imageSize(); 
		QSizeF FormatSize = CurrProduct.format().size();
		if ((ImgSize.width() > ImgSize.height() && FormatSize.width() < FormatSize.height()) ||
				(ImgSize.width() < ImgSize.height() && FormatSize.width() > FormatSize.height()))
			FormatSize.transpose();
	
		setAspectRatio(FormatSize); 
	}
}

void TPPhotoEditor::zoomMag()
{
	if (AspectRatioZoom < 7)
	{
		AspectRatioZoom += 0.04;
		updateSelection();
	}
}

void TPPhotoEditor::zoomMin()
{
	if (AspectRatioZoom > 1)
	{
		AspectRatioZoom -= 0.04;
		updateSelection();
	}
}

void TPPhotoEditor::doCropCommand()
{	
	QPoint TopLeft, BottomRight;
	SelectionRotated = false;
	SelectionIface->getSelection(TopLeft, BottomRight);
	if (TopLeft.x() < BottomRight.x() &&
	      TopLeft.y() < BottomRight.y())
	{
		UndoStack->push(new TPCropImageUndoCommand(this, TopLeft, BottomRight));
	}
}

void TPPhotoEditor::doRotateSelectionCommand()
{
	SelectionRotated = ! SelectionRotated;
	updateSelection();
}

void TPPhotoEditor::doRotateRightCommand()
{
	UndoStack->push(new TPTransformImageUndoCommand(this, STImage::Rotate90));
}

void TPPhotoEditor::doRotateLeftCommand()
{
	UndoStack->push(new TPTransformImageUndoCommand(this, STImage::Rotate270));
}

void TPPhotoEditor::doBlackAndWhiteCommand()
{
	UndoStack->push(new TPBlackAndWhiteUndoCommand(this)); 
}

void TPPhotoEditor::doSepiaCommand()
{
	UndoStack->push(new TPSepiaUndoCommand(this)); 
}

void TPPhotoEditor::doAutoCorrectionCommand()
{
	UndoStack->push(new TPAutoColorImageUndoCommand(this)); 
	UndoStack->push(new TPAutoContrastImageUndoCommand(this)); 
}

void TPPhotoEditor::doWhiteMarginCommand()
{
	STDom::DDocProduct CurrProduct = LVProducts->currentProduct();
	UndoStack->push(new TPWhiteMarginUndoCommand(this, CurrProduct.format().pixelSize(Dpis))); 
}

void TPPhotoEditor::redEyesTriggered()
{
	if (Status == StatusEditing)
	{
		setStatus(StatusSelectRedEyes);
	}
	else 
	{
		QPoint TopLeft, BottomRight;
		SelectionIface->getSelection(TopLeft, BottomRight);
		if(TopLeft.x() < BottomRight.x() &&
			TopLeft.y() < BottomRight.y())
		UndoStack->push(new TPRedEyeImageUndoCommand(this, TopLeft, BottomRight)); 
		setStatus(StatusEditing); 
	}
}

void TPPhotoEditor::advancedToggled(bool _Value)
{
	showAdvanced(_Value);
}

void TPPhotoEditor::nextImage()
{
	if (Model)
	{
		if (CurrentIndex.row() + 1 < Model->rowCount()   )
			setCurrentIndex(Model->index(CurrentIndex.row() + 1, CurrentIndex.column())); 
	}	
}

void TPPhotoEditor::previousImage()
{
	if (Model)
	{
		if (CurrentIndex.row() > 0  )
			setCurrentIndex(Model->index(CurrentIndex.row() - 1, CurrentIndex.column())); 
	}	
}

void TPPhotoEditor::updateImageInfo()
{
	if (Model && CurrentIndex.isValid() && isVisible() )
	{
		LabImageInfo->setText(Model->data(CurrentIndex, Qt::DisplayRole).toString()); 
	}
}

void TPPhotoEditor::incSelectedProduct()
{
	if (Model)
	{
		Model->incProductCopies(CurrentIndex, 1, LVProducts->currentProduct());
		emit orderChanged();
	}

}


void TPPhotoEditor::decSelectedProduct()
{
	if (Model)
	{
		Model->incProductCopies(CurrentIndex, -1, LVProducts->currentProduct());
		emit orderChanged();
	}
}

void TPPhotoEditor::saveChanges()
{
	if (!UndoStack->isClean() || thereIsAdvancedChanges())
	{
		try
		{
			QApplication::setOverrideCursor( QCursor(Qt::WaitCursor));
			applyBrightAndContrast();
			applyGamma();
			applyGammaRGB();
			saveCurrentImage();
			UndoStack->clear(); 
			QApplication::restoreOverrideCursor();
		}
		catch (Error& _Error)
		{
			QApplication::restoreOverrideCursor();
			SMessageBox::critical(this, tr("Error saving changes"), _Error.description());
		}
	}
	resetAdvancedChanges();
} 

void TPPhotoEditor::slotContrastChanged( int _Value)
{
	SelectionIface->slotChangeContrast(_Value);
}

void TPPhotoEditor::slotBrightChanged( int _Value)
{
	SelectionIface->slotChangeBright(_Value);
}

void TPPhotoEditor::slotGammaRGBChanged()
{
	SelectionIface->slotChangeGammaRGB(GammaRSl->value(), GammaGSl->value(), GammaBSl->value());
}
