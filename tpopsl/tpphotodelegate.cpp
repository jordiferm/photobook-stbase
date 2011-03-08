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
#include "tpphotodelegate.h"

// Qt
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>
#include <QStylePainter>
#include <QLabel> 

#include <QTextDocument>
#include <QAbstractTextDocumentLayout> 

#include "tpphotolistview.h"
#include "stpaintutils.h"


/**
 * Space between the item outer rect and the content, and between the
 * thumbnail and the caption
 */
const int ITEM_MARGIN = 14;

/** How darker is the border line around selection */
const int SELECTION_BORDER_DARKNESS = 140;

/** Radius of the selection rounded corners, in pixels */
const int SELECTION_RADIUS = 15;

/** Space between the item outer rect and the context bar */
const int CONTEXTBAR_MARGIN = -10;

/** How dark is the shadow, 0 is invisible, 255 is as dark as possible */
const int SHADOW_STRENGTH = 146;

/** How many pixels around the thumbnail are shadowed */
const int SHADOW_SIZE = 5;


//_____________________________________________________________________________
//
// Struct TPPhotoDelegatePrivate
//_____________________________________________________________________________

struct TPPhotoDelegatePrivate {
	/**
	 * Maps full text to elided text.
	 */
	mutable QHash<QString, QString> mElidedTextCache;

	// Key is height * 1000 + width
	typedef QHash<int, QPixmap> ShadowCache;
	mutable ShadowCache mShadowCache;

	TPPhotoDelegate* that;
	TPPhotoListView* mView;
	QWidget* mContextBar;
	QToolButton* mSaveButton;
	QPixmap mSaveButtonPixmap;

	QToolButton* mAddProductButton;
	QToolButton* mRemoveProductButton;
	QToolButton* mCopyImageButton;

	QModelIndex mIndexUnderCursor;
	int mThumbnailSize;
	TPPhotoDelegate::ThumbnailDetails mDetails;

	QLabel* mTipLabel;


	QToolButton* createContextBarButton(const char* iconName) {
		const int size = 48;

		//ContextBarButton* button = new ContextBarButton();
		QToolButton* button = new QToolButton();
		button->setObjectName("ContextBarButton"); 
		//button->setViewport(mView->viewport());
		button->setIcon(QIcon(iconName));
		button->setIconSize(QSize(size, size));
		button->setAutoRaise(true);
		return button;
	}

	void initSaveButtonPixmap() {
		if (!mSaveButtonPixmap.isNull()) {
			return;
		}
		// Necessary otherwise we won't see the save button itself
		mSaveButton->adjustSize();

		// This is hackish.
		// Show/hide the frame to make sure mSaveButton->render produces
		// something coherent.
		mSaveButton->show();
		mSaveButton->repaint();
		mSaveButton->hide();

		mSaveButtonPixmap = QPixmap(mSaveButton->sizeHint());
		mSaveButtonPixmap.fill(Qt::transparent);
		mSaveButton->render(&mSaveButtonPixmap, QPoint(), QRegion(), QWidget::DrawChildren);
	}


	void showContextBar(const QRect& rect, const QPixmap& thumbnailPix) {
		mContextBar->adjustSize();
		const int posX = (rect.width() - mContextBar->width()) / 2;
		const int posY = qMax(CONTEXTBAR_MARGIN, mThumbnailSize - thumbnailPix.height() - mContextBar->height());
		mContextBar->move(rect.topLeft() + QPoint(posX, CONTEXTBAR_MARGIN));//rect.topLeft() + QPoint(posX, posY));
		mContextBar->show();
	}


	void initTipLabel() {
		mTipLabel = new QLabel(mView);
		mTipLabel->setAutoFillBackground(true);
		mTipLabel->setFrameStyle(QFrame::Box | QFrame::Plain);
		mTipLabel->setPalette(QToolTip::palette());
		mTipLabel->hide();
	}


	bool hoverEventFilter(QHoverEvent* event) {
		QModelIndex index = mView->indexAt(event->pos());
		//if (mIndexUnderCursor.isValid()) {
		//	// Always repaint in case we are over the rating row
		//	mView->update(mIndexUnderCursor);
		//}
		if (index == mIndexUnderCursor) {
			// Same index, nothing to do
			return false;
		}
		//QModelIndex oldIndex = mIndexUnderCursor;
		mIndexUnderCursor = index;
		//mView->update(oldIndex);
		//!!!TODO: Single click in settings.
		//if (KGlobalSettings::singleClick() && KGlobalSettings::changeCursorOverIcon()) {
		//	mView->setCursor(mIndexUnderCursor.isValid() ? Qt::PointingHandCursor : Qt::ArrowCursor);
		//}

		if (index.isValid()) {

			const QRect rect = mView->visualRect(mIndexUnderCursor);
			const QPixmap thumbnailPix = mView->thumbnailForIndex(index);
			showContextBar(rect, thumbnailPix);

			showToolTip(index);
			mView->update(index);

		} else {
			mContextBar->hide();
			mTipLabel->hide();
		}
		return false;
	}

	QRect ratingRectFromIndexRect(const QRect& rect) const {
		return QRect(
			rect.left(),
			rect.bottom() - ratingRowHeight() - ITEM_MARGIN,
			rect.width(),
			ratingRowHeight());
	}

// #ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
// 	int ratingFromCursorPosition(const QRect& ratingRect) const {
// 		const QPoint pos = mView->viewport()->mapFromGlobal(QCursor::pos());
// 		return mRatingPainter.ratingFromPosition(ratingRect, pos);
// 	}
// #endif

	bool mouseButtonEventFilter(QEvent::Type type) {
// 	#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
// 		const QRect rect = ratingRectFromIndexRect(mView->visualRect(mIndexUnderCursor));
// 		const int rating = ratingFromCursorPosition(rect);
// 		if (rating == -1) {
// 			return false;
// 		}
// 		if (type == QEvent::MouseButtonRelease) {
// 			that->setDocumentRatingRequested(urlForIndex(mIndexUnderCursor) , rating);
// 		}
// 		return true;
// 	#else
		return false;
//	#endif
	}


	QPoint saveButtonPosition(const QRect& itemRect) const {
		QSize frameSize = mSaveButton->sizeHint();
		int textHeight = mView->fontMetrics().height();
		int posX = itemRect.right() - CONTEXTBAR_MARGIN - frameSize.width();
		int posY = itemRect.bottom() - CONTEXTBAR_MARGIN - textHeight - frameSize.height();

		return QPoint(posX, posY);
	}


	void showSaveButton(const QRect& itemRect) const {
		mSaveButton->move(saveButtonPosition(itemRect));
		mSaveButton->show();
	}


	void drawBackground(QPainter* painter, const QRect& rect, const QColor& bgColor, const QColor& borderColor) const {
		int bgH, bgS, bgV;
		int borderH, borderS, borderV, borderMargin;
	#ifdef FINETUNE_SELECTION_BACKGROUND
		QSettings settings(QDir::homePath() + "/colors.ini", QSettings::IniFormat);
		bgH = settings.value("bg/h").toInt();
		bgS = settings.value("bg/s").toInt();
		bgV = settings.value("bg/v").toInt();
		borderH = settings.value("border/h").toInt();
		borderS = settings.value("border/s").toInt();
		borderV = settings.value("border/v").toInt();
		borderMargin = settings.value("border/margin").toInt();
	#else
		bgH = 0;
		bgS = -150;
		bgV = 0;
		borderH = 0;
		borderS = -100;
		borderV = 60;
		borderMargin = 1;
	#endif
		painter->setRenderHint(QPainter::Antialiasing);

		QRectF rectF = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

		QPainterPath path = PaintUtils::roundedRectangle(rectF, SELECTION_RADIUS);

		QLinearGradient gradient(rectF.topLeft(), rectF.bottomLeft());
		gradient.setColorAt(0, PaintUtils::adjustedHsv(bgColor, bgH, bgS, bgV));
		gradient.setColorAt(1, bgColor);
		painter->fillPath(path, gradient);

		painter->setPen(borderColor);
		painter->drawPath(path);

		painter->setPen(PaintUtils::adjustedHsv(borderColor, borderH, borderS, borderV));
		rectF = rectF.adjusted(borderMargin, borderMargin, -borderMargin, -borderMargin);
		path = PaintUtils::roundedRectangle(rectF, SELECTION_RADIUS);
		painter->drawPath(path);
	}


	void drawShadow(QPainter* painter, const QRect& rect) const {
		const QPoint shadowOffset(-SHADOW_SIZE, -SHADOW_SIZE + 1);

		int key = rect.height() * 1000 + rect.width();

		ShadowCache::Iterator it = mShadowCache.find(key);
		if (it == mShadowCache.end()) {
			QSize size = QSize(rect.width() + 2*SHADOW_SIZE, rect.height() + 2*SHADOW_SIZE);
			QColor color(0, 0, 0, SHADOW_STRENGTH);
			QPixmap shadow = PaintUtils::generateFuzzyRect(size, color, SHADOW_SIZE);
			it = mShadowCache.insert(key, shadow);
		}
		painter->drawPixmap(rect.topLeft() + shadowOffset, it.value());
	}


	void drawText(QPainter* painter, const QRect& rect, const QRect& _OptionRect, const QColor& fgColor, const QString& fullText) const {
		QFontMetrics fm = mView->fontMetrics();

		// Elide text
/*		QString text;
		QHash<QString, QString>::const_iterator it = mElidedTextCache.constFind(fullText);
		if (it == mElidedTextCache.constEnd()) {
			text = fm.elidedText(fullText, Qt::ElideRight, rect.width());
			mElidedTextCache[fullText] = text;
		} else {
			text = it.value();
		}

		// Compute x pos
		int posX;
		if (text.length() == fullText.length()) {
			// Not elided, center text
			posX = (rect.width() - fm.width(text)) / 2;
		} else {
			// Elided, left align
			posX = 0;
		}

		// Draw text*/
		//painter->setPen(fgColor);
		//painter->drawText(rect.left() + posX, rect.top() + fm.ascent(), text);
		QTextDocument Doc; 
		Doc.setHtml(fullText);
		Doc.setPageSize(QSizeF(_OptionRect.width() - 10, _OptionRect.height())); 
		QAbstractTextDocumentLayout* DocLayout = Doc.documentLayout(); 
		

		//QRect TextBRect = fm.boundingRect(rect, Qt::TextWordWrap, fullText); 
		double DocHeight = DocLayout->documentSize().height();
		double UnfitYGap = qMax(0.0, DocHeight  - rect.height());
		QRectF TRect(rect.x(), rect.y() - UnfitYGap, rect.width(), rect.height() + UnfitYGap);
		painter->setPen(fgColor); 
		//painter->drawText(TRect, Qt::TextWordWrap, fullText);
		painter->save();  
		painter->translate(TRect.x(), TRect.y()); 
		Doc.drawContents(painter); 
		painter->restore(); 
		
		//painter->setPen(fgColor);
		//painter->drawText(QRectF(rect), Qt::TextWordWrap, fullText);
	}


	void drawRating(QPainter* painter, const QRect& rect, const QVariant& value) {
// 	#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
// 		const int rating = value.toInt();
// 		const QRect ratingRect = ratingRectFromIndexRect(rect);
// 		const int hoverRating = ratingFromCursorPosition(ratingRect);
// 		mRatingPainter.paint(painter, ratingRect, rating, hoverRating);
// 	#endif
	}


	bool isTextElided(const QString& text) const {
		QHash<QString, QString>::const_iterator it = mElidedTextCache.constFind(text);
		if (it == mElidedTextCache.constEnd()) {
			return false;
		}
		return it.value().length() < text.length();
	}


	/**
	 * Show a tooltip only if the item has been elided.
	 * This function places the tooltip over the item text.
	 */
	void showToolTip(const QModelIndex& index) {
		if (mDetails == 0 || mDetails == TPPhotoDelegate::RatingDetail) {
			// No text to display
			return;
		}

		// Gather tip text
		QStringList textList;
		bool elided = false;
		if (mDetails & TPPhotoDelegate::FileNameDetail) {
			const QString text = index.data().toString();
			elided |= isTextElided(text);
			textList << text;
		}
		if (mDetails & TPPhotoDelegate::DateDetail) {
			// FIXME: Duplicated from drawText
			
// 			const KFileItem fileItem = fileItemForIndex(index);
// 			const KDateTime dt = TimeUtils::dateTimeForFileItem(fileItem);
// 			const QString text = KGlobal::locale()->formatDateTime(dt);
// 			elided |= isTextElided(text);
// 			textList << text;
		}
		if (!elided) {
			return;
		}
		mTipLabel->setText(textList.join("\n"));

		// Compute tip position
		QRect rect = mView->visualRect(index);
		const int textX = ITEM_MARGIN;
		const int textY = ITEM_MARGIN + mThumbnailSize + ITEM_MARGIN;
		const int margin = mTipLabel->frameWidth();
		const QPoint tipPosition = rect.topLeft() + QPoint(textX - margin, textY - margin);

		// Show tip
		mTipLabel->adjustSize();
		mTipLabel->move(tipPosition);
		mTipLabel->show();
	}

	int itemWidth() const {
		return mThumbnailSize + 2 * ITEM_MARGIN;
	}

	int ratingRowHeight() const {
#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
		return mView->fontMetrics().ascent();
#endif
		return 0;
	}

	int itemHeight() const {
		const int lineHeight = mView->fontMetrics().height();
		int textHeight = 0;
		if (mDetails & TPPhotoDelegate::FileNameDetail) {
			textHeight += lineHeight;
		}
		if (mDetails & TPPhotoDelegate::DateDetail) {
			textHeight += lineHeight;
		}
		if (mDetails & TPPhotoDelegate::RatingDetail) {
			textHeight += ratingRowHeight();
		}
		if (textHeight == 0) {
			// Keep at least one row of text, so that we can show folder names
			textHeight = lineHeight;
		}
		//return mThumbnailSize + textHeight + 3*ITEM_MARGIN;
		return mThumbnailSize + textHeight + ITEM_MARGIN;
	}

	void selectIndexUnderCursorIfNoMultiSelection() {
		if (mView->selectionModel()->selectedIndexes().size() <= 1) {
			mView->setCurrentIndex(mIndexUnderCursor);
		}
	}

};

//_____________________________________________________________________________
//
// Class TPPhotoDelegate
//_____________________________________________________________________________

TPPhotoDelegate::TPPhotoDelegate(TPPhotoListView* view)
: QAbstractItemDelegate(view)
, d(new TPPhotoDelegatePrivate), SelectItemsWithPhotos(true) {
	d->that = this;
	d->mView = view;
	view->viewport()->installEventFilter(this);
	d->mThumbnailSize = view->thumbnailSize();
	d->mDetails = FileNameDetail;

/*#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
	d->mRatingPainter.setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	d->mRatingPainter.setLayoutDirection(view->layoutDirection());
	d->mRatingPainter.setMaxRating(10);
#endif*/

	connect(view, SIGNAL(thumbnailSizeChanged(int)),
		SLOT(setThumbnailSize(int)) );

	// Button frame
	d->mContextBar = new QWidget(d->mView->viewport());
	d->mContextBar->hide();

	d->mAddProductButton = d->createContextBarButton(":/st/edit_add.png");
	connect(d->mAddProductButton, SIGNAL(clicked()),
		SLOT(slotEditAddClicked()));

	d->mRemoveProductButton = d->createContextBarButton(":/st/edit_remove.png");
	connect(d->mRemoveProductButton, SIGNAL(clicked()),
		SLOT(slotEditDelClicked()) );

	d->mCopyImageButton = d->createContextBarButton(":/st/editcopy.png");
	connect(d->mCopyImageButton, SIGNAL(clicked()),
		SLOT(slotEditCloneClicked()) );


	QHBoxLayout* layout = new QHBoxLayout(d->mContextBar);
	layout->setMargin(2);
	layout->setSpacing(2);
	layout->addWidget(d->mAddProductButton);
	layout->addWidget(d->mRemoveProductButton);
	layout->addWidget(d->mCopyImageButton);

	// Save button
	d->mSaveButton = d->createContextBarButton("document-save");
	d->mSaveButton->adjustSize();
	d->mSaveButton->setParent(d->mView->viewport());
	d->mSaveButton->hide();
	connect(d->mSaveButton, SIGNAL(clicked()),
		SLOT(slotSaveClicked()) );

	d->initTipLabel();
}


TPPhotoDelegate::~TPPhotoDelegate() {
	delete d;
}


QSize TPPhotoDelegate::sizeHint( const QStyleOptionViewItem & /*option*/, const QModelIndex& _Index ) const {
	QSize Res = QSize( d->itemWidth(), d->itemHeight() );
	if (_Index.model())
		if (_Index.row() == _Index.model()->rowCount() -1)
			Res = QSize( d->itemWidth(), d->itemHeight() + 40);
	return Res;
}

void TPPhotoDelegate::setCopyActionVisible(bool _Visible)
{
	d->mCopyImageButton->setVisible(_Visible);
}

bool TPPhotoDelegate::eventFilter(QObject*, QEvent* event) {
	switch (event->type()) {
	case QEvent::ToolTip:
		return true;

	case QEvent::HoverMove:
	case QEvent::HoverLeave:
		return d->hoverEventFilter(static_cast<QHoverEvent*>(event));

	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
		return d->mouseButtonEventFilter(event->type());

	default:
		return false;
	}
}


void TPPhotoDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const {
	int thumbnailSize = d->mThumbnailSize;
	QPixmap thumbnailPix = d->mView->thumbnailForIndex(index);
	//const KFileItem fileItem = fileItemForIndex(index);
	const bool opaque = !thumbnailPix.hasAlphaChannel();
	//const bool isDirOrArchive = ArchiveUtils::fileItemIsDirOrArchive(fileItemForIndex(index));
	const bool isDirOrArchive = false;
	QRect rect = option.rect;
	bool selected = option.state & QStyle::State_Selected;
	if (SelectItemsWithPhotos)
		selected = selected || d->mView->hasPhotos(index);
	bool current = index == d->mView->currentIndex();
	const bool underMouse = option.state & QStyle::State_MouseOver;
	const QWidget* viewport = d->mView->viewport();

#ifdef DEBUG_RECT
	painter->setPen(Qt::red);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(rect);
#endif

	// Select color group
	QPalette::ColorGroup cg;

	if ( (option.state & QStyle::State_Enabled) && (option.state & QStyle::State_Active) ) {
		cg = QPalette::Normal;
	} else if ( (option.state & QStyle::State_Enabled)) {
		cg = QPalette::Inactive;
	} else {
		cg = QPalette::Disabled;
	}

	// Select colors
	QColor bgColor, borderColor, fgColor;
	if (selected || underMouse) {
		bgColor = option.palette.color(cg, QPalette::Highlight);
		borderColor = bgColor.dark(SELECTION_BORDER_DARKNESS);
	} else {
		bgColor = viewport->palette().color(viewport->backgroundRole());

		if (bgColor.value() < 128) {
			borderColor = bgColor.dark(200);
		} else {
			borderColor = bgColor.light(200);
		}
	}
	fgColor = viewport->palette().color(viewport->foregroundRole());

	// Compute thumbnailRect
	QRect thumbnailRect = QRect(
		rect.left() + (rect.width() - thumbnailPix.width())/2,
		rect.top() + ITEM_MARGIN /*+ (thumbnailSize - thumbnailPix.height()) + ITEM_MARGIN ,*/,
		thumbnailPix.width(),
		thumbnailPix.height());

	// Draw background
	//const QRect backgroundRect = thumbnailRect.adjusted(-ITEM_MARGIN, -ITEM_MARGIN, ITEM_MARGIN, ITEM_MARGIN);
	const QRect backgroundRect = option.rect;
	if (current) {
		QColor CurrColor = bgColor.darker(150);
		d->drawBackground(painter, backgroundRect, CurrColor, borderColor);
	} else if (selected) {
		d->drawBackground(painter, backgroundRect, bgColor, borderColor);
	} else 
	if (underMouse) {
		painter->setOpacity(0.5);
		d->drawBackground(painter, backgroundRect, bgColor, borderColor);
		painter->setOpacity(1.); 
	} else if (opaque) {
		d->drawShadow(painter, thumbnailRect);
	}

	// Draw thumbnail
	if (opaque) {
		painter->setPen(borderColor);
		painter->setRenderHint(QPainter::Antialiasing, false);
		QRect borderRect = thumbnailRect.adjusted(-1, -1, 0, 0);
		painter->drawRect(borderRect);
	}
	painter->drawPixmap(thumbnailRect.left(), thumbnailRect.top(), thumbnailPix);

	// Draw modified indicator
	bool isModified = d->mView->isModified(index);
	if (isModified) {
		// Draws a pixmap of the save button frame, as an indicator that
		// the image has been modified
		QPoint framePosition = d->saveButtonPosition(rect);
		d->initSaveButtonPixmap();
		painter->drawPixmap(framePosition, d->mSaveButtonPixmap);
	}

	if (index == d->mIndexUnderCursor) {
		// Show bar again: if the thumbnail has changed, we may need to update
		// its position. Don't do it if we are over rotate buttons, though: it
		// would not be nice to move the button now, the user may want to
		// rotate the image one more time.
		// The button will get moved when the mouse leaves.
		if (!d->mCopyImageButton->underMouse()) {
			d->showContextBar(rect, thumbnailPix);
		}
		if (isModified) {
			// If we just rotated the image with the buttons from the
			// button frame, we need to show the save button frame right now.
			d->showSaveButton(rect);
		} else {
			d->mSaveButton->hide();
		}
	}

	QRect textRect(
		rect.left() + ITEM_MARGIN,
		//rect.top() + 2 * ITEM_MARGIN + thumbnailSize,
		rect.top() + 2 * ITEM_MARGIN + thumbnailPix.height() , //+ thumbnailSize,
		rect.width() - 2 * ITEM_MARGIN,
		rect.height() - ( 2 * ITEM_MARGIN + thumbnailPix.height())  );
		//d->mView->fontMetrics().height() );
	if (isDirOrArchive || (d->mDetails & TPPhotoDelegate::FileNameDetail)) {
		d->drawText(painter, textRect, rect, fgColor, index.data().toString());
		textRect.moveTop(textRect.bottom());
	}

	/*if (!isDirOrArchive && (d->mDetails & TPPhotoDelegate::DateDetail)) {
		const KDateTime dt = TimeUtils::dateTimeForFileItem(fileItem);
		d->drawText(painter, textRect, fgColor, KGlobal::locale()->formatDateTime(dt));
	}*/

	/*if (!isDirOrArchive && (d->mDetails & TPPhotoDelegate::RatingDetail)) {
#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
		d->drawRating(painter, rect, index.data(SemanticInfoDirModel::RatingRole));
#endif
	}*/
}


void TPPhotoDelegate::setThumbnailSize(int value) {
	d->mThumbnailSize = value;

	const int width = d->itemWidth();
	const int buttonWidth = d->mCopyImageButton->sizeHint().width();
	d->mCopyImageButton->setVisible(width >= 3 * buttonWidth);
	//d->mRotateRightButton->setVisible(width >= 4 * buttonWidth);
	d->mContextBar->adjustSize();

	d->mElidedTextCache.clear();
}


void TPPhotoDelegate::slotSaveClicked() {
	//saveDocumentRequested(urlForIndex(d->mIndexUnderCursor));
}



void TPPhotoDelegate::slotEditDelClicked() 
{
	editDelRequested(d->mIndexUnderCursor); 
}

void TPPhotoDelegate::slotEditAddClicked() 
{
	editAddRequested(d->mIndexUnderCursor); 
// 	if (d->mView->selectionBehavior() == QAbstractItemView::SelectRows)
// 		d->mView->selectionModel()->select(d->mIndexUnderCursor, QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
// 	else 
// 	if (d->mView->selectionBehavior() == QAbstractItemView::SelectColumns)
// 		d->mView->selectionModel()->select(d->mIndexUnderCursor, QItemSelectionModel::Toggle | QItemSelectionModel::Columns);
// 	else 
// 		d->mView->selectionModel()->select(d->mIndexUnderCursor, QItemSelectionModel::Toggle);
}

void TPPhotoDelegate::slotEditCloneClicked() 
{
	editCloneRequested(d->mIndexUnderCursor); 

}

TPPhotoDelegate::ThumbnailDetails TPPhotoDelegate::thumbnailDetails() const {
	return d->mDetails;
}


void TPPhotoDelegate::setThumbnailDetails(TPPhotoDelegate::ThumbnailDetails details) {
	/*d->mDetails = details;
	d->mView->scheduleDelayedItemsLayout();*/
}
