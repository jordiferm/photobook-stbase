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
#include "oimageeditor.h"
#include <QLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QScrollArea>
#include <QApplication>
#include <QUndoStack> 
#include <QCloseEvent> 
#include <QFileDialog> 
#include <QToolBar> 

#include "oeditoractionswidget.h"
#include "selectionInterface.h"
#include "basicfixeswidget.h"
#include "oeditorbottomactionswidget.h"
#include "oimagenavigatorwidget.h"
#include "sprocessstatuswidget.h"

//Digikam tools 
#include "odigikamtoolswidget.h" 
#include "ocolortoolswidget.h" 
#include "ofiltertoolswidget.h" 
#include "editortooliface.h" 

#include "imageTools.h"
#include "smessagebox.h"

#include "histogramEditor.h" 

//Digikam
#include "dimginterface.h"
#include "iccsettingscontainer.h" 
#include "canvas.h" 
#include "exposurecontainer.h"
#include "dimagedoc.h"
using namespace Digikam;

//_____________________________________________________________________________
//
// AbstractImageUndoCommand
//_____________________________________________________________________________

AbstractImageUndoCommand::AbstractImageUndoCommand(OImageEditor* _ImageEditor, const QString& _Text, QUndoCommand* _Parent)
	: QUndoCommand(_Text, _Parent), ImageEditor(_ImageEditor)
{
	UndoImage = _ImageEditor->currentImage();
	UndoImage.detach();
}
	
void AbstractImageUndoCommand::undo()
{
	ImageEditor->setImage(UndoImage);
}


//_____________________________________________________________________________
//
// CropImageUndoCommand
//_____________________________________________________________________________

SetToolImageUndoCommand::SetToolImageUndoCommand(OImageEditor* _ImageEditor, const QImage& _Image, const QString& _ImgChanges,
								QUndoCommand* _Parent) : 
		AbstractImageUndoCommand(_ImageEditor, _ImgChanges, _Parent)
{
	RedoImage = _Image; 
	RedoImage.detach(); 
}


void SetToolImageUndoCommand::redo()
{
	ImageEditor->setImage(RedoImage, false);
}

//_____________________________________________________________________________
//
// CropImageUndoCommand
//_____________________________________________________________________________

CropImageUndoCommand::CropImageUndoCommand(OImageEditor* _ImageEditor, 
const QPoint& _TopLeft, const QPoint& _BottomRight, QUndoCommand* _Parent) : 
		AbstractImageUndoCommand(_ImageEditor, QObject::tr("Crop Image"), _Parent), TopLeft(_TopLeft), BottomRight(_BottomRight) 
{}

void CropImageUndoCommand::redo()
{
	ImageEditor->crop(TopLeft, BottomRight);
}


SetImageUndoCommand::SetImageUndoCommand(OImageEditor* _ImageEditor, const QImage& _NewImage, 
							QUndoCommand* _Parent): 
		AbstractImageUndoCommand(_ImageEditor, QObject::tr("Change Image"), _Parent), NewImage(_NewImage)
{}

void SetImageUndoCommand::redo()
{
	ImageEditor->changeImage(NewImage); 
}


//_____________________________________________________________________________
//
// TiltImageUndoCommand
//_____________________________________________________________________________

TiltImageUndoCommand::TiltImageUndoCommand(OImageEditor* _ImageEditor, const QPoint& _P1, const QPoint& _P2, 
								QUndoCommand* _Parent) : AbstractImageUndoCommand(_ImageEditor, QObject::tr("Tilt"), _Parent),
		P1(_P1), P2(_P2)
{
}
	
void TiltImageUndoCommand::redo()
{
	ImageEditor->tilt(P1, P2);
}

QString TransformImageUndoCommand::transformCodeToString(STImage::EnTransformCodes _TransfCode )
{
	QString Res; 
	switch (_TransfCode)
	{
		case STImage::Rotate90 :
			Res = Res = QObject::tr("Rotate Right");
		break; 
		case STImage::Rotate270 : 
			Res = QObject::tr("Rotate Left");
		break; 
		case STImage::FlipX : 
			Res = QObject::tr("Flip X");
		break; 
		case STImage::FlipY :
			Res = QObject::tr("Flip Y");
		break; 
	}
	return Res; 
}

TransformImageUndoCommand::TransformImageUndoCommand(OImageEditor* _ImageEditor,STImage::EnTransformCodes _TransformCode, 
							QUndoCommand* _Parent): AbstractImageUndoCommand(_ImageEditor, transformCodeToString(_TransformCode), _Parent), TransformCode(_TransformCode)
{}


void TransformImageUndoCommand::redo()
{
	ImageEditor->transform(TransformCode);
}

//_____________________________________________________________________________
//
// RedEyeImageUndoCommand
//_____________________________________________________________________________

RedEyeImageUndoCommand::RedEyeImageUndoCommand(OImageEditor* _ImageEditor, 
const QPoint& _TopLeft, const QPoint& _BottomRight, QUndoCommand* _Parent) : 
		AbstractImageUndoCommand(_ImageEditor, QObject::tr("Red Eyes"), _Parent), TopLeft(_TopLeft), BottomRight(_BottomRight) 
{}

void RedEyeImageUndoCommand::redo()
{
	ImageEditor->redEyes(TopLeft, BottomRight);
}




//_____________________________________________________________________________
//
// Class RevertToOriginalUndoCommand
//_____________________________________________________________________________

RevertToOriginalUndoCommand::RevertToOriginalUndoCommand(OImageEditor* _ImageEditor, QUndoCommand* _Parent)
	: AbstractImageUndoCommand(_ImageEditor, QObject::tr("Revert to Original"), _Parent)
{
}

void RevertToOriginalUndoCommand::redo()
{
	ImageEditor->revertToOriginal();
}


//_____________________________________________________________________________
//
// OImageEditor
//_____________________________________________________________________________

void OImageEditor::initEditor(bool _EditFromModel)
{
	EditorActions->imageNavigator()->setVisible(_EditFromModel);
	EditorActions->setOriginalVisible(_EditFromModel);
	EditorActions->setAcceptVisible(!_EditFromModel);
	ScrollArea->setWidgetResizable(true);
	UndoStack->clear();
	EditorActions->init();
	EditFromModel = _EditFromModel;
	MBottomActWidget->resetZoom(); 
}

int OImageEditor::editModelImage(const QModelIndex& _Index)
{
	initEditor();
	setImage(_Index);
	showMaximized();
	zoom1_1();
	return exec();
}

void OImageEditor::beginApplyEffect()
{
	setDisabled(true);
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

}

void OImageEditor::endApplyEffect()
{
	setDisabled(false);
	qApp->restoreOverrideCursor();
	QApplication::processEvents();
}

void OImageEditor::updateSelection()
{
	//determine selected width and height;
	int SelectedWidth = 0;
	int SelectedHeight = 0;
	int ImageWidth = CurrImage.width();
	int ImageHeight = CurrImage.height();

	QSizeF CAspectRatio = AspectRatio;

	//select region using same aspect ratio
	SelectedWidth = ImageWidth;
	SelectedHeight = (int)((((double)(ImageWidth * CAspectRatio.height())) / CAspectRatio.width()));

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
	//qDebug("NewTopLeft: (%d, %d)", NewTopLeft.x(), NewTopLeft.y());

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

	//NewBottomRight = NewBottomRight / AspectRatioZoom;

	//select region
	SelectionIface->setSelection(NewTopLeft, NewBottomRight,
	                             qMax(CAspectRatio.width(), CAspectRatio.height()));

}

void OImageEditor::addDigikamTool(ODigikamToolsWidget* _Tool, const QIcon _Icon, const QString& _Label)
{
	connect(_Tool, SIGNAL(toolActivated(Digikam::EditorTool*)), this, SLOT(loadTool(Digikam::EditorTool* ))); 
	connect(_Tool, SIGNAL(toolClosed(Digikam::EditorTool*)), this, SLOT(unloadTool())); 
	connect(_Tool, SIGNAL(imageChanged(const QString&)), this, SLOT(slotToolImageChanged(const QString&))); 
	ToolsTab->addTab(_Tool, _Icon, _Label); 
}

void OImageEditor::setActiveCanvas(QWidget* _Canvas)
{
	if (_Canvas) //Defensive
	{
		if (_Canvas != LastActiveCanvas)
		{
			setImageViewer(_Canvas); 
			LastActiveCanvas = _Canvas;
		}
	}
}


OImageEditor::OImageEditor(QWidget *parent)
		: QDialog(parent), SourceModel(0), AspectRatioZoom(1), EditFromModel(false), ViewerWidget(0), LastActiveCanvas(0)
{
	//Digikam initializations 
	DigikamCanvas = new Digikam::Canvas(this);
	DigikamCanvas->hide(); 
	DigikamCanvas->makeDefaultEditingCanvas();
	DigikamCanvas->setExposureSettings( new ExposureSettingsContainer());

	//Digikam::DImgInterface* DefInterface = new Digikam::DImgInterface(); 
	//Digikam::DImgInterface::setDefaultInterface(DefInterface);
   ICCSettingsContainer* ICCSettings = new ICCSettingsContainer();
	IOFileSettings = new IOFileSettingsContainer();
	Digikam::DImgInterface::defaultInterface()->setICCSettings(ICCSettings);
	

	QVBoxLayout* MLayout = new QVBoxLayout(this);
	UndoStack = new QUndoStack(this);
	
	ProcStatusW = new SProcessStatusWidget; 
	ProcStatusW->setVisible(false); 
	
	//Actions Widget
	EditorActions = new OEditorActionsWidget(UndoStack, this);
	MLayout->addWidget(EditorActions);
	connect(EditorActions->imageNavigator(), SIGNAL(currentItemChanged(const QModelIndex&)),
	        this, SLOT(editImageChanged(const QModelIndex&)));
	connect(EditorActions, SIGNAL(saveClicked()), this, SLOT(saveCurrentImage())); 
	connect(EditorActions, SIGNAL(saveAsClicked()), this, SLOT(saveCurrentImageAs())); 
	connect(EditorActions, SIGNAL(openClicked()), this, SLOT(openFile()));
	connect(EditorActions, SIGNAL(revertOriginalClicked()), this, SLOT(doRevertToOriginalCommand())); 
	connect(EditorActions, SIGNAL(backClicked()), this, SLOT(close()));
	connect(EditorActions, SIGNAL(acceptClicked()), this, SLOT(saveAndClose()));
	connect(EditorActions, SIGNAL(histogramClicked()), this, SLOT(showHistogram()));

	DigikamToolBar = new QToolBar(this); 
	DigikamToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	MLayout->addWidget(DigikamToolBar); 
	DigikamToolBar->hide(); 

	QSplitter* Splitter = new QSplitter(this);
	MLayout->addWidget(Splitter);

	//Tools Widget
	ToolsTab = new QTabWidget(this);
	ToolsTab->setIconSize(QSize(32,32)); 
	ToolsTab->setTabPosition(QTabWidget::West); 


	// Digikam tools 
	addDigikamTool(new OColorToolsWidget(this), QIcon(":/st/oromel/colortools.png"), tr("Color"));
	addDigikamTool(new OFilterToolsWidget(this), QIcon(":/st/oromel/filter.png"), tr("Filters"));
	EditorToolIface* EditorIface = new EditorToolIface(this); 
	connect(EditorIface, SIGNAL(toolStartProgress(const QString&)), 
				this, SLOT(setToolStartProgress(const QString&))); 
	connect(EditorIface, SIGNAL(toolStopProgress()), 
				this, SLOT(setToolStopProgress())); 
	connect(EditorIface, SIGNAL(toolProgress(int)), 
				this, SLOT(setToolProgress(int))); 


	//ToolsTab->setTabShape(QTabWidget::Triangular);
	// Old tools tab
	//!!! Deprecated tools 
	BasicFixesWidget* BFixesWidg = new BasicFixesWidget(this);
	ToolsTab->addTab(BFixesWidg, QIcon(":/st/oromel/basic.png"), tr("Basic Fixes"));
	connect(ToolsTab, SIGNAL(currentChanged( int )), this, SLOT(slotToolsTabChanged(int ))); 
	connect(BFixesWidg, SIGNAL(cropSizeChanged(const QSizeF&)), this, SLOT(setAspectRatio(const QSizeF&)));
	connect(BFixesWidg, SIGNAL(applyCrop()), this, SLOT(doCropCommand()));
	connect(BFixesWidg, SIGNAL(cropZoomChanged(double)), this, SLOT(setAspectRatioZoom(double)));
	connect(BFixesWidg, SIGNAL(applyStraighten()), this, SLOT(enterDrawLineMode()));
	connect(BFixesWidg, SIGNAL(rotateRight()), this, SLOT(doRotateRightCommand()));
	connect(BFixesWidg, SIGNAL(rotateLeft()), this, SLOT(doRotateLeftCommand()));
	connect(BFixesWidg, SIGNAL(flipX()), this, SLOT(doFlipXCommand()));
	connect(BFixesWidg, SIGNAL(flipY()), this, SLOT(doFlipYCommand()));
	connect(BFixesWidg, SIGNAL(applyRedEyes()), this, SLOT(doRedEyeCommand()));

	Splitter->addWidget(ToolsTab);

	//Image Selection Interface.
	QFrame* ImageFrame = new QFrame(this);
	ImgFrameLayout = new QVBoxLayout(ImageFrame);
	ImgFrameLayout->setSpacing(1);
	ImgFrameLayout->setMargin(0);

	SelectionIface = new SelectionInterface(ImageFrame);
	connect(SelectionIface, SIGNAL(lineSelected(QPoint, QPoint)),
	        this, SLOT(doTiltCommand(QPoint, QPoint)));

	ScrollArea = new QScrollArea(this);
	//ScrollArea->setWidgetResizable(true);
	ScrollArea->setBackgroundRole(QPalette::Dark);
	ScrollArea->setWidget(SelectionIface);

	//ImgFrameLayout->addWidget(ScrollArea);
	//setActiveCanvas(ScrollArea); 
	ScrollArea->setVisible(false); 
	MBottomActWidget = new OEditorBottomActionsWidget(UndoStack, ImageFrame);
	connect(MBottomActWidget, SIGNAL(zoomValueChanged(double )), this, SLOT(setZoomValue(double )));
	connect(MBottomActWidget, SIGNAL(zoomFitToWindow()), this, SLOT(zoom1_1())); 
	connect(MBottomActWidget, SIGNAL(zoomFitToSelection()), this, SLOT(zoom_selection())); 

	ImgFrameLayout->addWidget(MBottomActWidget);


	Splitter->addWidget(ImageFrame);
	Splitter->setStretchFactor(0, 0);
	Splitter->setStretchFactor(1, 1);
	
	//Initialitzations	
	slotToolsTabChanged(ToolsTab->currentIndex()); 
}

OImageEditor::~OImageEditor()
{
	delete ProcStatusW;
}


void OImageEditor::setImage(const QImage& _Image, bool _UpdateDigikamIface)
{
	CurrImage = _Image;
	SelectionIface->setPhoto(CurrImage);
	Digikam::DImgInterface::defaultInterface()->setImage(CurrentImagePath, CurrImage, this); 
}

void OImageEditor::setImage(const QModelIndex& _Index)
{
	if (_Index.isValid()) //Defensive
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		STDom::DDoc* CurrentDoc = SourceModel->doc(_Index);
		if (CurrentDoc->type() == STDom::DImageDoc::Type)
		{
			STDom::DImageDoc* ColImage = static_cast<STDom::DImageDoc*>(CurrentDoc);
			//TODO: Load image using digikam !!! (We'll have to change defaultInterface()->setImage by defaultInterface()->load
			QString CFilePath = CurrentDoc->fileInfo().absoluteFilePath();
			CurrImage.load(CFilePath);
			if (!CurrImage.isNull())
			{
				SelectionIface->setPhoto(CurrImage);
				//Use Thumbnail for image preview.
				CurrentIndex = _Index;
				CurrentImagePath = CFilePath;
				EditorActions->setOriginalEnabled( ColImage->originalSaved());
				setWindowTitle(QString(tr("Editing image: %1 ")).arg(CFilePath));
				Digikam::DImgInterface::defaultInterface()->setImage(CFilePath, CurrImage, this);

				//setImage(CurrImage);
			}
		}
		QApplication::restoreOverrideCursor(); 
	}
}

void OImageEditor::setSourceModel(STDom::DDocModel* _Model)
{
	SourceModel = _Model;
	EditorActions->imageNavigator()->setSourceModel(_Model);
}

int OImageEditor::editImage(const QString& _ImagePath)
{
	openImage(_ImagePath);
	showMaximized();
	return exec();	
}

void OImageEditor::openImage(const QString& _ImagePath)
{
	CurrentImagePath = _ImagePath;
	initEditor(false);
	setWindowTitle(QString(tr("Editing image: %1 ")).arg(_ImagePath));
	//TODO: Load Image using digikam.
	//Digikam::DImgInterface::defaultInterface()->load(_ImagePath, IOFileSettings, this); 
	setImage(STImage(_ImagePath));		
}

int OImageEditor::editImage(const QModelIndex& _Index)
{
	if (SourceModel) //Defensive.
	{
		EditorActions->imageNavigator()->setCurrentItem(_Index);
		editModelImage(_Index);
	}
}

void OImageEditor::crop(const QPoint& _TopLeft, const QPoint& _BottomRight)
{
	beginApplyEffect();
	setImage(CurrImage.crop(_TopLeft, _BottomRight));
	endApplyEffect();
}

void OImageEditor::tilt(QPoint _P1, QPoint _P2)
{
	beginApplyEffect();
	setImage(CurrImage.correctTilt(_P1, _P2));
	endApplyEffect();
}

void OImageEditor::transform(STImage::EnTransformCodes _TransformCode)
{
	beginApplyEffect();
	setImage(CurrImage.transform(_TransformCode));
	endApplyEffect();	
}

void OImageEditor::redEyes(const QPoint& _TopLeft, const QPoint& _BottomRight)
{
	beginApplyEffect();
	ProcStatusW->show();
	setImage(CurrImage.correctRedEye(_TopLeft, _BottomRight, ProcStatusW));
	ProcStatusW->hide();
	endApplyEffect();
}


//!!!! Deprecated method
void OImageEditor::revertToOriginal()
{
	if (CurrentIndex.isValid() && SourceModel) //Defensive
	{
		if (SourceModel->doc(CurrentIndex)->type() == STDom::DImageDoc::Type)
		{
			STDom::DImageDoc* ColImage = static_cast<STDom::DImageDoc*>(SourceModel->doc(CurrentIndex));
			setImage(ColImage->originalImage());
		}
	}
}

void OImageEditor::setImageViewer(QWidget* _Viewer)
{
	if (ViewerWidget)
	{
		ViewerWidget->hide(); 
		ImgFrameLayout->removeWidget(ViewerWidget); 
	}
	ViewerWidget = _Viewer; 
	ImgFrameLayout->insertWidget(0, _Viewer); 
	_Viewer->show(); 
}

void OImageEditor::closeEvent(QCloseEvent* _Event)
{
	if (!UndoStack->isClean())
	{
		if ( SMessageBox::question(this, tr("Unconfirmed changes"), tr("The image has been modified. Do you want to close without saving changes ?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
			_Event->accept();
		else 
			_Event->ignore();
	}	
	else 
		_Event->accept();
}


void OImageEditor::changeImage(const QImage& _Image)
{
	beginApplyEffect();
	setImage(_Image);
	endApplyEffect();
}


void OImageEditor::setAspectRatio(const QSizeF& _AspectRatio)
{
	AspectRatio = _AspectRatio;
	updateSelection();
}

void OImageEditor::setAspectRatioZoom(double _Value)
{
	AspectRatioZoom = _Value;
	updateSelection();
}


//! Sets the drawLineMode. When there is mouse release event
void OImageEditor::enterDrawLineMode()
{
	SelectionIface->enterDrawLineMode();
}

void OImageEditor::doCropCommand()
{
	QPoint TopLeft, BottomRight;
	SelectionIface->getSelection(TopLeft, BottomRight);
	if (TopLeft.x() < BottomRight.x() &&
	      TopLeft.y() < BottomRight.y())
	{
		UndoStack->push(new CropImageUndoCommand(this, TopLeft, BottomRight));
	}
}

void OImageEditor::doRedEyeCommand()
{
	QPoint TopLeft, BottomRight;
	SelectionIface->getSelection(TopLeft, BottomRight);
	if(TopLeft.x() < BottomRight.x() &&
		TopLeft.y() < BottomRight.y())
	{
		UndoStack->push(new RedEyeImageUndoCommand(this, TopLeft, BottomRight));
	}	
}

void OImageEditor::doTiltCommand(QPoint _P1, QPoint _P2)
{
	UndoStack->push(new TiltImageUndoCommand(this, _P1, _P2));
}

void OImageEditor::doRotateRightCommand()
{
	UndoStack->push(new TransformImageUndoCommand(this, STImage::Rotate90));
}

void OImageEditor::doRotateLeftCommand()
{
	UndoStack->push(new TransformImageUndoCommand(this, STImage::Rotate270));
}

void OImageEditor::doFlipXCommand()
{
	UndoStack->push(new TransformImageUndoCommand(this, STImage::FlipX));
}

void OImageEditor::doFlipYCommand()
{
	UndoStack->push(new TransformImageUndoCommand(this, STImage::FlipY));
}


void OImageEditor::doRevertToOriginalCommand()
{
	UndoStack->push(new RevertToOriginalUndoCommand(this)); 
}

void OImageEditor::zoom1_1()
{
	ScrollArea->setWidgetResizable(true);
	SelectionIface->resize(ScrollArea->size());
	
	DigikamCanvas->zoomFitToWindow();
	MBottomActWidget->setZoomValue(DigikamCanvas->zoomFactor()); 
}

void OImageEditor::zoom_selection()
{
	DigikamCanvas->fitToSelect();
	MBottomActWidget->setZoomValue(DigikamCanvas->zoomFactor()); 
}

void OImageEditor::setZoomValue(double _Value)
{
	ScrollArea->setWidgetResizable(false);
	QSize ResSize(ScrollArea->size()); 
	ResSize.setWidth(ResSize.width() * _Value); 
	ResSize.setHeight(ResSize.height() * _Value); 
	SelectionIface->resize(ResSize);
	ScrollArea->update();
	DigikamCanvas->setZoomFactor(_Value);
}


void OImageEditor::editImageChanged(const QModelIndex& _Index)
{
	setImage(_Index);
}

void OImageEditor::saveCurrentImage()
{
	if (EditFromModel)
		saveCurrentIndex(); 
	else 
	{
		beginApplyEffect();
		if (CurrImage.save(CurrentImagePath))
			UndoStack->setClean();
		else 
			SMessageBox::critical(this, tr("Saving image"), QString(tr("Error saving image %1")).arg(CurrentImagePath));
		endApplyEffect();
	}
}

void OImageEditor::saveAndClose()
{
	saveCurrentImage(); 
	accept();
}

void OImageEditor::showHistogram()
{
	qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
	HistogramEditor HEditorDialog(CurrentImagePath);
	qApp->restoreOverrideCursor();
	if (HEditorDialog.exec() == QDialog::Accepted)
	{
		UndoStack->push(new SetImageUndoCommand(this, *HEditorDialog.getModifiedImage()));
	}
}

void OImageEditor::saveCurrentImageAs()
{
	//TODO: Ask to user if he would like to add the new image to collection and if we add it then remain in EditFromModel Mode.
	QString NewFileName = QFileDialog::getSaveFileName(this, tr("Select filename to save"), CurrentImagePath);
	if (!NewFileName.isEmpty())
	{
		beginApplyEffect();
		if (CurrImage.save(NewFileName))
		{
			openImage(NewFileName);
			emit savedAs(NewFileName);
		}
		else 
			SMessageBox::critical(this, tr("Saving image"), QString(tr("Error saving image %1")).arg(CurrentImagePath));
		endApplyEffect();
	}
}

void OImageEditor::saveCurrentIndex()
{
	if (CurrentIndex.isValid() && SourceModel) //Defensive
	{
		STDom::DDoc* CurrentDoc = SourceModel->doc(CurrentIndex);
		if (CurrentDoc->type() == STDom::DImageDoc::Type)
		{
			STDom::DImageDoc* CImage = static_cast<STDom::DImageDoc*>(CurrentDoc);
			try
			{
				beginApplyEffect();
				CImage->save(CurrImage, !CImage->originalSaved());
				UndoStack->setClean();
				endApplyEffect();
			}
			catch (STDom::DImageDoc::Error& _Error)
			{
				SMessageBox::critical(this, tr("Error saving image"), _Error.description());
			}
		}
	}
}

void OImageEditor::openFile()
{
	QString FileToOpen = QFileDialog::getOpenFileName(this, tr("Select file to open"), QDir::homePath(), STImage::supportedFormatsToReadFilter());
	if (!FileToOpen.isEmpty())
	{
		beginApplyEffect();
		openImage(FileToOpen);
		endApplyEffect();
	}
}



/*!
	Loads digikamTool 
*/
void OImageEditor::loadTool(Digikam::EditorTool* _Tool)
{
	setImageViewer(_Tool->toolView());
	//ScrollArea->setWidget(_Tool->toolView()); 
	EditorActions->setEnabled(false); 
	//MBottomActWidget->setEnabled(false); 
}

/*!
	Unloads digikamTool 
*/
void OImageEditor::unloadTool()
{
	//ImgFrameLayout->removeWidget( ImgFrameLayout->widget()
	setImageViewer(LastActiveCanvas);
	EditorActions->setEnabled(true); 
	MBottomActWidget->setEnabled(true); 
}

void OImageEditor::slotToolsTabChanged(int _Index)
{
	QWidget* CurrWidget = ToolsTab->widget(_Index); 
	if (ODigikamToolsWidget* ToolsW  = qobject_cast<ODigikamToolsWidget*>(CurrWidget))
	{
		DigikamToolBar->clear(); 
		ToolsW->setupActions(DigikamToolBar);
		DigikamToolBar->setVisible(true); 
		setActiveCanvas(DigikamCanvas);
		//DigikamCanvas->zoomFitToWindow();
	}
	else 
	{
		if (isVisible())
			unloadTool(); 
		DigikamToolBar->hide(); 
		setActiveCanvas(ScrollArea);
	}
	zoom1_1();
}

void OImageEditor::slotToolImageChanged(const QString& _Changes)
{
	QImage ToolModifImage = Digikam::DImgInterface::defaultInterface()->getImg()->copyQImage();
	//setImage( ToolModifImage); 
	UndoStack->push(new SetToolImageUndoCommand(this, ToolModifImage, _Changes));
}


void OImageEditor::setToolStartProgress(const QString& _ToolName)
{
	ProcStatusW->show(); 
	ProcStatusW->showProgressBar(_ToolName, 100, true); 
}

void OImageEditor::setToolProgress(int _Progress)
{
	ProcStatusW->updateProgress(_Progress); 
}

void OImageEditor::setToolStopProgress()
{
	ProcStatusW->hide(); 
}
