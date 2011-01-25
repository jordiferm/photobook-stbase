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
#ifndef TPIPHOTOEDITOR_H
#define TPIPHOTOEDITOR_H

#include <QUndoCommand> 
#include <QImage> 
#include "stimage.h" 



/**
Photo Editor interface and commands.

	@author
*/
class TPIPhotoEditor
{
public:
	virtual void setImage(const QImage& _Image) = 0; 
	virtual void crop(const QPoint& _TopLeft, const QPoint& _BottomRight) = 0;
	virtual void transform(STImage::EnTransformCodes _TransformCode) = 0;
	virtual void blackAndWhite() = 0;
	virtual void sepia() = 0; 
	virtual void autoColor() = 0;
	virtual void autoContrast() = 0; 
	virtual void applyWhiteMargin(const QSize& _Size) = 0; 
	virtual void redEyes(const QPoint& _TopLeft, const QPoint& _BottomRight) = 0; 
	virtual QImage currentImage() const = 0;
};


/**
TPOPSl Undo commands .

	@author Shadow
*/
class OImageEditor; 
class TPAbstractImageUndoCommand : public QUndoCommand
{
	QImage UndoImage;
	
protected:	
	TPIPhotoEditor* ImageEditor; 
	
public:
	TPAbstractImageUndoCommand(TPIPhotoEditor* _ImageEditor, const QString& _Text, QUndoCommand* _Parent = 0);
	void undo();
};

class TPCropImageUndoCommand : public TPAbstractImageUndoCommand
{
	QPoint TopLeft; 
	QPoint BottomRight; 
	
public:
	TPCropImageUndoCommand(TPIPhotoEditor* _ImageEditor, const QPoint& _TopLeft, const QPoint& _BottomRight, 
								QUndoCommand* _Parent = 0);
	void redo();
};

class TPTransformImageUndoCommand : public TPAbstractImageUndoCommand
{
	STImage::EnTransformCodes TransformCode; 
	
	QString transformCodeToString(STImage::EnTransformCodes _TransfCode );
	
public:
	TPTransformImageUndoCommand(TPIPhotoEditor* _ImageEditor,STImage::EnTransformCodes Transformation, 
								QUndoCommand* _Parent = 0); 
	void redo();	
};

class TPBlackAndWhiteUndoCommand : public TPAbstractImageUndoCommand
{

public:
	TPBlackAndWhiteUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent = 0);
	void redo();
};

class TPSepiaUndoCommand : public TPAbstractImageUndoCommand
{

public:
	TPSepiaUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent = 0);
	void redo();
};

class TPAutoColorImageUndoCommand : public TPAbstractImageUndoCommand
{
public:
	TPAutoColorImageUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent = 0);
	void redo();
};

class TPAutoContrastImageUndoCommand : public TPAbstractImageUndoCommand
{
public:
	TPAutoContrastImageUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent = 0);
	void redo();
};

class TPWhiteMarginUndoCommand : public TPAbstractImageUndoCommand
{

	QSize Size;
public:
	TPWhiteMarginUndoCommand(TPIPhotoEditor* _ImageEditor, const QSize& _Size, QUndoCommand* _Parent = 0);
	void redo();
};

class TPRedEyeImageUndoCommand : public TPAbstractImageUndoCommand
{
	QPoint TopLeft; 
	QPoint BottomRight; 
	
public:
	TPRedEyeImageUndoCommand(TPIPhotoEditor* _ImageEditor, const QPoint& _TopLeft, const QPoint& _BottomRight, 
								QUndoCommand* _Parent = 0);
	void redo();
};

#endif
