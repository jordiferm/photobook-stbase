/****************************************************************************
**
** Copyright (C) 2006-2010 Aili Image. All rights reserved.
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
** Strablitz (c) 2010
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef OBIGIMAGEPREVIEWWIDGET_H
#define OBIGIMAGEPREVIEWWIDGET_H

#include <QWidget>

namespace STDom
{
	class DDocModel;
	class DDoc;
}
class QDir;
class QLabel;
class QModelIndex;
class QFileInfo;
class STThumbnailView;
class QToolButton;

namespace SPhotoBook
{
	class CheckedProxyModel;
}
class OBigImagePreviewWidget : public QWidget
{
	Q_OBJECT

	STDom::DDocModel* DocModel;
	SPhotoBook::CheckedProxyModel* CheckedDocModel;
	QLabel* ImageLabel;
	STThumbnailView* LVImages;
	QToolButton* TBInclude;

	void updateIncludeState();
	void setCurrentIndex(const QModelIndex& _Index);

public:
	OBigImagePreviewWidget(QWidget* _Parent = 0);
	void setLabel(const QString& _Label);
	void setDir(const QDir& _Dir);
	void clear();
	STDom::DDoc* doc(const QModelIndex& _Index);

private slots:
	void slotShowImage(const QModelIndex& _Index);
	void slotIncludeCurrentImage();
	void slotGoForward();
	void slotGoBackward();

signals:
	void includeImage(const QFileInfo& _ImageFile);
	void excludeImage(const QFileInfo& _ImageFile);
	void closeRequest();
};

#endif // OBIGIMAGEPREVIEWWIDGET_H
