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
#ifndef OIMAGEEDITOR_H
#define OIMAGEEDITOR_H

#include <QDialog>
#include <QImage>
#include <QUndoCommand> 
#include <QModelIndex> 
#include "storomelexport.h"

//Image manipulation
#include "stimage.h"

//Digikam
#include "editortool.h" 
#include "iofilesettingscontainer.h"  

//STDom
#include "ddocmodel.h"
/**
Orome Undo commands .

	@author Shadow
*/
class OImageEditor; 
class AbstractImageUndoCommand : public QUndoCommand
{
	QImage UndoImage;
	
protected:	
	OImageEditor* ImageEditor; 
	
public:
	AbstractImageUndoCommand(OImageEditor* _ImageEditor, const QString& _Text, QUndoCommand* _Parent = 0);
	void undo();
};

class SetToolImageUndoCommand : public AbstractImageUndoCommand
{
	QImage RedoImage;
	
public:
	SetToolImageUndoCommand(OImageEditor* _ImageEditor, const QImage& _Image, const QString& _ImgChanges,
								QUndoCommand* _Parent = 0);
	void redo();
};


class CropImageUndoCommand : public AbstractImageUndoCommand
{
	QPoint TopLeft; 
	QPoint BottomRight; 
	
public:
	CropImageUndoCommand(OImageEditor* _ImageEditor, const QPoint& _TopLeft, const QPoint& _BottomRight, 
								QUndoCommand* _Parent = 0);
	void redo();
};

class SetImageUndoCommand : public AbstractImageUndoCommand
{
	QImage NewImage; 
	
public:
	SetImageUndoCommand(OImageEditor* _ImageEditor, const QImage& _NewImage, 
								QUndoCommand* _Parent = 0);
	void redo();
};

class TiltImageUndoCommand : public AbstractImageUndoCommand
{
	QPoint P1, P2; 
	
public:
	TiltImageUndoCommand(OImageEditor* _ImageEditor, const QPoint& _P1, const QPoint& _P2, 
								QUndoCommand* _Parent = 0);
	void redo();
};

class TransformImageUndoCommand : public AbstractImageUndoCommand
{
	STImage::EnTransformCodes TransformCode; 
	
	QString transformCodeToString(STImage::EnTransformCodes _TransfCode );
	
public:
	TransformImageUndoCommand(OImageEditor* _ImageEditor,STImage::EnTransformCodes Transformation, 
								QUndoCommand* _Parent = 0); 
	void redo();
	
};

class RedEyeImageUndoCommand : public AbstractImageUndoCommand
{
	QPoint TopLeft; 
	QPoint BottomRight; 
	
public:
	RedEyeImageUndoCommand(OImageEditor* _ImageEditor, const QPoint& _TopLeft, const QPoint& _BottomRight, 
								QUndoCommand* _Parent = 0);
	void redo();
};


class RevertToOriginalUndoCommand : public AbstractImageUndoCommand
{

public:
	RevertToOriginalUndoCommand(OImageEditor* _ImageEditor, QUndoCommand* _Parent = 0);
	void redo();
};

/**
Oromë image editor.

	@author
*/
namespace Digikam
{
	class Canvas;
}
class SelectionInterface; 
class QScrollArea;
class QShowEvent; 
class OEditorActionsWidget;
class QModelIndex;
class QUndoStack;
class SProcessStatusWidget; 
class ImageTunningWidget;
class OEditorBottomActionsWidget;
class QToolBar; 
class QTabWidget; 
class QVBoxLayout; 
class ODigikamToolsWidget; 
class ST_OROMEL_EXPORT OImageEditor : public QDialog
{
	Q_OBJECT
	QUndoStack* UndoStack;
	SelectionInterface* SelectionIface;
	STImage CurrImage; 
	QScrollArea* ScrollArea;
	STDom::DDocModel* SourceModel;
	OEditorActionsWidget* EditorActions;
	double AspectRatioZoom; 
	QSizeF AspectRatio; 
	SProcessStatusWidget* ProcStatusW;
	QModelIndex CurrentIndex; 
	QString CurrentImagePath;
	bool EditFromModel; 
	OEditorBottomActionsWidget* MBottomActWidget;
	Digikam::IOFileSettingsContainer* IOFileSettings;
	QToolBar* DigikamToolBar; 
	QTabWidget* ToolsTab;
	QVBoxLayout* ImgFrameLayout;
	QWidget* ViewerWidget;
	Digikam::Canvas* DigikamCanvas;
	QWidget* LastActiveCanvas; 

	void initEditor(bool _EditFromModel = true);
	int editModelImage(const QModelIndex& _Index);
	void updateSelection();
	void beginApplyEffect();
	void endApplyEffect();
	void addDigikamTool(ODigikamToolsWidget* _Tool, const QIcon _Icon, const QString& _Label); 
	void setActiveCanvas(QWidget* _Canvas);

public:
	OImageEditor(QWidget *parent = 0);
	~OImageEditor();
	void setImage(const QImage& _Image, bool _UpdateDigikamIface = true);
	void setImage(const QModelIndex& _Index);
	QImage currentImage() const { return CurrImage; }
	void setSourceModel(STDom::DDocModel* _Model);
	//! Opens image and disables EditFromModel mode.
	void openImage(const QString& _ImagePath);
	int editImage(const QString& _ImagePath);
	int editImage(const QModelIndex& _Index);
	QSizeF aspectRatio() const { return AspectRatio; }
	double aspectRatioZoom() const { return AspectRatioZoom; }
	void crop(const QPoint& _TopLeft, const QPoint& _BottomRight);
	void tilt( QPoint _P1, QPoint _P2);
	void transform(STImage::EnTransformCodes _TransformCode);
	void redEyes(const QPoint& _TopLeft, const QPoint& _BottomRight);
	void changeImage(const QImage& _Image);
	void revertToOriginal();
	void setImageViewer(QWidget* _Viewer);
	
protected:
	void closeEvent(QCloseEvent* _Event);
	
	
public slots:	
	void setAspectRatio(const QSizeF& _AspectRatio);
	void setAspectRatioZoom(double _Value);
	void enterDrawLineMode();

private slots:
	void doCropCommand();
	void doRedEyeCommand();
	void doTiltCommand(QPoint _P1, QPoint _P2);
	void doRotateRightCommand();
	void doRotateLeftCommand();
	void doFlipXCommand();
	void doFlipYCommand();
	void doRevertToOriginalCommand();
	void zoom1_1();
	void zoom_selection();
	void setZoomValue(double _Value); 
	void saveCurrentImage();
	void saveAndClose();
	void showHistogram();
	void editImageChanged(const QModelIndex& );
	void saveCurrentImageAs();
	void saveCurrentIndex();
	void openFile();
	void loadTool(Digikam::EditorTool* );
	void unloadTool();
	void slotToolsTabChanged(int _Index);
	void slotToolImageChanged(const QString& _Changes);
	void setToolStartProgress(const QString& toolName);
	void setToolProgress(int progress);
	void setToolStopProgress();
};

#endif
