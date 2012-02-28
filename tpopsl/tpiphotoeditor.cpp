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
#include "tpiphotoeditor.h"

//_____________________________________________________________________________
//
// TPAbstractImageUndoCommand
//_____________________________________________________________________________

TPAbstractImageUndoCommand::TPAbstractImageUndoCommand(TPIPhotoEditor* _ImageEditor, const QString& _Text, QUndoCommand* _Parent)
	: QUndoCommand(_Text, _Parent), ImageEditor(_ImageEditor)
{
	UndoImage = _ImageEditor->currentImage();
	UndoImage.detach();
}
	
void TPAbstractImageUndoCommand::undo()
{
	ImageEditor->setImage(UndoImage);
}

//_____________________________________________________________________________
//
// TPCropImageUndoCommand
//_____________________________________________________________________________

TPCropImageUndoCommand::TPCropImageUndoCommand(TPIPhotoEditor* _ImageEditor, 
const QPoint& _TopLeft, const QPoint& _BottomRight, QUndoCommand* _Parent) : 
		TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("Crop Image"), _Parent), TopLeft(_TopLeft), BottomRight(_BottomRight) 
{}

void TPCropImageUndoCommand::redo()
{
	ImageEditor->crop(TopLeft, BottomRight);
}

//_____________________________________________________________________________
//
// Class TPTransformImageUndoCommand
//_____________________________________________________________________________

QString TPTransformImageUndoCommand::transformCodeToString(STImage::EnTransformCodes _TransfCode )
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

TPTransformImageUndoCommand::TPTransformImageUndoCommand(TPIPhotoEditor* _ImageEditor,STImage::EnTransformCodes _TransformCode, 
							QUndoCommand* _Parent): TPAbstractImageUndoCommand(_ImageEditor, transformCodeToString(_TransformCode), _Parent), TransformCode(_TransformCode)
{
}

void TPTransformImageUndoCommand::redo()
{
	ImageEditor->transform(TransformCode);
}



//_____________________________________________________________________________
//
// Class TPBlackAndWhiteUndoCommand
//_____________________________________________________________________________

TPBlackAndWhiteUndoCommand::TPBlackAndWhiteUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent )
	: TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("Black And White"), _Parent)
{
	
}

void TPBlackAndWhiteUndoCommand::redo()
{
	ImageEditor->blackAndWhite();
}

//_____________________________________________________________________________
//
// Class TPSepiaUndoCommand
//_____________________________________________________________________________

TPSepiaUndoCommand::TPSepiaUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent)
	: TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("Sepia"), _Parent)
{}

void TPSepiaUndoCommand::redo()
{
	ImageEditor->sepia();
}

//_____________________________________________________________________________
//
// Class TPAutoColorImageUndoCommand
//_____________________________________________________________________________

TPAutoColorImageUndoCommand::TPAutoColorImageUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent) : 
		TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("Auto Color"), _Parent)
{}

void TPAutoColorImageUndoCommand::redo()
{
	ImageEditor->autoColor();
}

//_____________________________________________________________________________
//
// Class TPAutoContrastImageUndoCommand
//_____________________________________________________________________________

TPAutoContrastImageUndoCommand::TPAutoContrastImageUndoCommand(TPIPhotoEditor* _ImageEditor, QUndoCommand* _Parent) : 
		TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("Auto Contrast"), _Parent)
{}

void TPAutoContrastImageUndoCommand::redo()
{
	ImageEditor->autoContrast();
}


//_____________________________________________________________________________
//
// Class TPAutoContrastImageUndoCommand
//_____________________________________________________________________________

TPWhiteMarginUndoCommand::TPWhiteMarginUndoCommand(TPIPhotoEditor* _ImageEditor, const QSize& _Size, QUndoCommand* _Parent) :
TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("No Cut"), _Parent), Size(_Size)
{}

void TPWhiteMarginUndoCommand::redo()
{
	ImageEditor->applyWhiteMargin(Size);
}

//_____________________________________________________________________________
//
// Class TPRedEyeImageUndoCommand
//_____________________________________________________________________________

TPRedEyeImageUndoCommand::TPRedEyeImageUndoCommand(TPIPhotoEditor* _ImageEditor, 
const QPoint& _TopLeft, const QPoint& _BottomRight, QUndoCommand* _Parent) : 
		TPAbstractImageUndoCommand(_ImageEditor, QObject::tr("Red Eyes"), _Parent), TopLeft(_TopLeft), BottomRight(_BottomRight) 
{}

void TPRedEyeImageUndoCommand::redo()
{
	ImageEditor->redEyes(TopLeft, BottomRight);
}
