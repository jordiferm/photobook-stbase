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
#include "plusminusitemdelegate.h"

// Qt
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QToolButton>
#include <QToolTip>
#include <QStylePainter>
#include <QLabel>

#include "stthumbnailview.h"
#include "stpaintutils.h"


/**
 * Space between the item outer rect and the content, and between the
 * thumbnail and the caption
 */
const int ITEM_MARGIN = 5;

/** How darker is the border line around selection */
const int SELECTION_BORDER_DARKNESS = 140;

/** Radius of the selection rounded corners, in pixels */
const int SELECTION_RADIUS = 5;

/** Space between the item outer rect and the context bar */
const int CONTEXTBAR_MARGIN = 1;

/** How lighter is the border of context bar buttons */
const int CONTEXTBAR_BORDER_LIGHTNESS = 140;

/** How darker is the background of context bar buttons */
const int CONTEXTBAR_BACKGROUND_DARKNESS = 170;

/** How lighter are context bar buttons when under mouse */
const int CONTEXTBAR_MOUSEOVER_LIGHTNESS = 115;

/** Radius of ContextBarButtons */
const int CONTEXTBAR_RADIUS = 5;

/** How dark is the shadow, 0 is invisible, 255 is as dark as possible */
const int SHADOW_STRENGTH = 128;

/** How many pixels around the thumbnail are shadowed */
const int SHADOW_SIZE = 4;


/*static KFileItem fileItemForIndex(const QModelIndex& index) {
	Q_ASSERT(index.isValid());
	QVariant data = index.data(KDirModel::FileItemRole);
	return qvariant_cast<KFileItem>(data);
}


static KUrl urlForIndex(const QModelIndex& index) {
	KFileItem item = fileItemForIndex(index);
	return item.url();
}*/


class ContextBarButton : public QToolButton
{
public:
	ContextBarButton() : mViewport(0) {}

	/**
	 * The viewport is used to pick the right colors
	 */
	void setViewport(QWidget* viewPort)
	{
		mViewport = viewPort;
	}

protected:
	virtual void paintEvent(QPaintEvent*)
	{
		Q_ASSERT(mViewport);
		QStylePainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		QStyleOptionToolButton opt;
		initStyleOption(&opt);

		const QColor bgColor = mViewport->palette().color(mViewport->backgroundRole());
		QColor color = bgColor.dark(CONTEXTBAR_BACKGROUND_DARKNESS);
		QColor borderColor = bgColor.light(CONTEXTBAR_BORDER_LIGHTNESS);

		if (opt.state & QStyle::State_MouseOver && opt.state & QStyle::State_Enabled)
		{
			color = color.light(CONTEXTBAR_MOUSEOVER_LIGHTNESS);
			borderColor = borderColor.light(CONTEXTBAR_MOUSEOVER_LIGHTNESS);
		}

		const QRectF rectF = QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5);
		const QPainterPath path = PaintUtils::roundedRectangle(rectF, CONTEXTBAR_RADIUS);

		// Background
		painter.fillPath(path, color);

		// Top shadow
		QLinearGradient gradient(rectF.topLeft(), rectF.topLeft() + QPoint(0, 5));
		gradient.setColorAt(0, QColor::fromHsvF(0, 0, 0, .3));
		gradient.setColorAt(1, Qt::transparent);
		painter.fillPath(path, gradient);

		// Left shadow
		gradient.setFinalStop(rectF.topLeft() + QPoint(5, 0));
		painter.fillPath(path, gradient);

		// Border
		painter.setPen(borderColor);
		painter.drawPath(path);

		// Content
		painter.drawControl(QStyle::CE_ToolButtonLabel, opt);
	}

private:
	QWidget* mViewport;
};


struct PlusMinusItemDelegatePrivate
{
	/**
	 * Maps full text to elided text.
	 */
	mutable QHash<QString, QString> mElidedTextCache;

	// Key is height * 1000 + width
	typedef QHash<int, QPixmap> ShadowCache;
	mutable ShadowCache mShadowCache;

	PlusMinusItemDelegate* that;
	STThumbnailView* mView;
	QWidget* mContextBar;

	QToolButton* mPlusButton;
	QToolButton* mMinusButton;

	QModelIndex mIndexUnderCursor;
	int mThumbnailSize;

	QLabel* mTipLabel;


	QToolButton* createContextBarButton(const char* iconName)
	{
		const int size = 22;

		ContextBarButton* button = new ContextBarButton();
		button->setViewport(mView->viewport());
		button->setIcon(QIcon(iconName));
		button->setIconSize(QSize(size, size));
		button->setAutoRaise(true);
		return button;
	}


	void showContextBar(const QRect& rect, const QPixmap& thumbnailPix)
	{
		mContextBar->adjustSize();
		const int posX = (rect.width() - mContextBar->width()) / 2;
		const int posY = qMax(CONTEXTBAR_MARGIN, mThumbnailSize - thumbnailPix.height() - mContextBar->height());
		mContextBar->move(rect.topLeft() + QPoint(posX, posY));
		mContextBar->show();
	}


	void initTipLabel()
	{
		mTipLabel = new QLabel(mView);
		mTipLabel->setObjectName("ToolTipLabel");
		mTipLabel->setAutoFillBackground(true);
		mTipLabel->setFrameStyle(QFrame::Box | QFrame::Plain);
		mTipLabel->setPalette(QToolTip::palette());
		mTipLabel->hide();
	}


	bool hoverEventFilter(QHoverEvent* event)
	{
		QModelIndex index = mView->indexAt(event->pos());
		if (mIndexUnderCursor.isValid())
		{
			// Always repaint in case we are over the rating row
			mView->update(mIndexUnderCursor);
		}
		if (index == mIndexUnderCursor)
		{
			// Same index, nothing to do
			return false;
		}
		QModelIndex oldIndex = mIndexUnderCursor;
		mIndexUnderCursor = index;
		mView->update(oldIndex);
		//!!!TODO: Single click in settings.
		//if (KGlobalSettings::singleClick() && KGlobalSettings::changeCursorOverIcon()) {
		mView->setCursor(mIndexUnderCursor.isValid() ? Qt::PointingHandCursor : Qt::ArrowCursor);
		//}

		if (mIndexUnderCursor.isValid())
		{
			const QRect rect = mView->visualRect(mIndexUnderCursor);
			const QPixmap thumbnailPix = mView->thumbnailForIndex(index);
			showContextBar(rect, thumbnailPix);

			showToolTip(index);
			mView->update(mIndexUnderCursor);

		}
		else
		{
			mContextBar->hide();
			mTipLabel->hide();
		}
		return false;
	}



	bool mouseButtonEventFilter(QEvent::Type type)
	{
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


	void drawBackground(QPainter* painter, const QRect& rect, const QColor& bgColor, const QColor& borderColor) const
	{
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


	void drawShadow(QPainter* painter, const QRect& rect) const
	{
		const QPoint shadowOffset(-SHADOW_SIZE, -SHADOW_SIZE + 1);

		int key = rect.height() * 1000 + rect.width();

		ShadowCache::Iterator it = mShadowCache.find(key);
		if (it == mShadowCache.end())
		{
			QSize size = QSize(rect.width() + 2 * SHADOW_SIZE, rect.height() + 2 * SHADOW_SIZE);
			QColor color(0, 0, 0, SHADOW_STRENGTH);
			QPixmap shadow = PaintUtils::generateFuzzyRect(size, color, SHADOW_SIZE);
			it = mShadowCache.insert(key, shadow);
		}
		painter->drawPixmap(rect.topLeft() + shadowOffset, it.value());
	}


	void drawText(QPainter* painter, const QRect& rect, const QColor& fgColor, const QString& fullText) const
	{
		QFontMetrics fm = mView->fontMetrics();

		// Elide text
		QString text;
		QHash<QString, QString>::const_iterator it = mElidedTextCache.constFind(fullText);
		if (it == mElidedTextCache.constEnd())
		{
			text = fm.elidedText(fullText, Qt::ElideRight, rect.width());
			mElidedTextCache[fullText] = text;
		}
		else
		{
			text = it.value();
		}

		// Compute x pos
		int posX;
		if (text.length() == fullText.length())
		{
			// Not elided, center text
			posX = (rect.width() - fm.width(text)) / 2;
		}
		else
		{
			// Elided, left align
			posX = 0;
		}

		// Draw text
		painter->setPen(fgColor);
		painter->drawText(rect.left() + posX, rect.top() + fm.ascent(), text);
	}


	bool isTextElided(const QString& text) const
	{
		QHash<QString, QString>::const_iterator it = mElidedTextCache.constFind(text);
		if (it == mElidedTextCache.constEnd())
		{
			return false;
		}
		return it.value().length() < text.length();
	}


	/**
	 * Show a tooltip only if the item has been elided.
	 * This function places the tooltip over the item text.
	 */
	void showToolTip(const QModelIndex& index)
	{

		// Gather tip text
		QStringList textList;
		const QString text = index.data().toString();
		bool elided = false;
		elided |= isTextElided(text);
		textList << text;
		if (!elided)
		{
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

	int itemWidth() const
	{
		return mThumbnailSize + 2 * ITEM_MARGIN;
	}

	int ratingRowHeight() const
	{
#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
		return mView->fontMetrics().ascent();
#endif
		return 0;
	}

	int itemHeight() const
	{
		const int lineHeight = mView->fontMetrics().height();
		int textHeight = lineHeight;
		return mThumbnailSize + textHeight + 3*ITEM_MARGIN;
	}

	void selectIndexUnderCursorIfNoMultiSelection()
	{
		if (mView->selectionModel()->selectedIndexes().size() <= 1)
		{
			mView->setCurrentIndex(mIndexUnderCursor);
		}
	}

};


PlusMinusItemDelegate::PlusMinusItemDelegate(STThumbnailView* view)
		: QAbstractItemDelegate(view)
		, d(new PlusMinusItemDelegatePrivate)
{
	d->that = this;
	d->mView = view;
	view->viewport()->installEventFilter(this);
	d->mThumbnailSize = view->thumbnailSize();

	/*#ifndef GWENVIEW_SEMANTICINFO_BACKEND_NONE
		d->mRatingPainter.setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
		d->mRatingPainter.setLayoutDirection(view->layoutDirection());
		d->mRatingPainter.setMaxRating(10);
	#endif*/

	connect(view, SIGNAL(thumbnailSizeChanged(int)),
	        SLOT(setThumbnailSize(int)));

	// Button frame
	d->mContextBar = new QWidget(d->mView->viewport());
	d->mContextBar->hide();

	d->mPlusButton = d->createContextBarButton(":/st/edit_add.png");
	connect(d->mPlusButton , SIGNAL(clicked()),
	        SLOT(slotPlusButtonClicked()));

	d->mMinusButton = d->createContextBarButton(":/st/edit_remove.png");
	connect(d->mMinusButton, SIGNAL(clicked()),
	        SLOT(slotMinusButtonClicked()));

	QHBoxLayout* layout = new QHBoxLayout(d->mContextBar);
	layout->setMargin(2);
	layout->setSpacing(2);
	layout->addWidget(d->mPlusButton);
	layout->addWidget(d->mMinusButton);

	d->initTipLabel();
}


PlusMinusItemDelegate::~PlusMinusItemDelegate()
{
	delete d;
}


QSize PlusMinusItemDelegate::sizeHint(const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const
{
	return QSize(d->itemWidth(), d->itemHeight());
}


bool PlusMinusItemDelegate::eventFilter(QObject*, QEvent* event)
{
	switch (event->type())
	{
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


void PlusMinusItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	int thumbnailSize = d->mThumbnailSize;
	QPixmap thumbnailPix = d->mView->thumbnailForIndex(index);
	//const KFileItem fileItem = fileItemForIndex(index);
	const bool opaque = !thumbnailPix.hasAlphaChannel();
	//const bool isDirOrArchive = ArchiveUtils::fileItemIsDirOrArchive(fileItemForIndex(index));
	const bool isDirOrArchive = false;
	QRect rect = option.rect;
	const bool selected = option.state & QStyle::State_Selected;
	const bool underMouse = option.state & QStyle::State_MouseOver;
	const QWidget* viewport = d->mView->viewport();

#ifdef DEBUG_RECT
	painter->setPen(Qt::red);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(rect);
#endif

	// Select color group
	QPalette::ColorGroup cg;

	if ((option.state & QStyle::State_Enabled) && (option.state & QStyle::State_Active))
	{
		cg = QPalette::Normal;
	}
	else if ((option.state & QStyle::State_Enabled))
	{
		cg = QPalette::Inactive;
	}
	else
	{
		cg = QPalette::Disabled;
	}

	// Select colors
	QColor bgColor, borderColor, fgColor;
	if (selected || underMouse)
	{
		bgColor = option.palette.color(cg, QPalette::Highlight);
		borderColor = bgColor.dark(SELECTION_BORDER_DARKNESS);
	}
	else
	{
		bgColor = viewport->palette().color(viewport->backgroundRole());

		if (bgColor.value() < 128)
		{
			borderColor = bgColor.dark(200);
		}
		else
		{
			borderColor = bgColor.light(200);
		}
	}
	fgColor = viewport->palette().color(viewport->foregroundRole());

	// Compute thumbnailRect
	QRect thumbnailRect = QRect(
	                          rect.left() + (rect.width() - thumbnailPix.width()) / 2,
	                          rect.top() + (thumbnailSize - thumbnailPix.height()) + ITEM_MARGIN,
	                          thumbnailPix.width(),
	                          thumbnailPix.height());

	// Draw background
	//const QRect backgroundRect = thumbnailRect.adjusted(-ITEM_MARGIN, -ITEM_MARGIN, ITEM_MARGIN, ITEM_MARGIN);
	const QRect backgroundRect = option.rect;
	if (selected)
	{
		d->drawBackground(painter, backgroundRect, bgColor, borderColor);
	}
	else if (underMouse)
	{
		painter->setOpacity(0.2);
		d->drawBackground(painter, backgroundRect, bgColor, borderColor);
		painter->setOpacity(1.);
	}
	else if (opaque)
	{
		d->drawShadow(painter, thumbnailRect);
	}

	// Draw thumbnail
	if (opaque)
	{
		painter->setPen(borderColor);
		painter->setRenderHint(QPainter::Antialiasing, false);
		QRect borderRect = thumbnailRect.adjusted(-1, -1, 0, 0);
		painter->drawRect(borderRect);
	}
	painter->drawPixmap(thumbnailRect.left(), thumbnailRect.top(), thumbnailPix);


	if (index == d->mIndexUnderCursor)
	{
		// Show bar again: if the thumbnail has changed, we may need to update
		// its position. Don't do it if we are over rotate buttons, though: it
		// would not be nice to move the button now, the user may want to
		// rotate the image one more time.
		// The button will get moved when the mouse leaves.
// 		if (!d->mRotateLeftButton->underMouse() && !d->mRotateRightButton->underMouse())
// 		{
// 			d->showContextBar(rect, thumbnailPix);
// 		}
	}

	QRect textRect(
	    rect.left() + ITEM_MARGIN,
	    rect.top() + 2 * ITEM_MARGIN + thumbnailSize,
	    rect.width() - 2 * ITEM_MARGIN,
	    d->mView->fontMetrics().height());
	d->drawText(painter, textRect, fgColor, index.data().toString());
	textRect.moveTop(textRect.bottom());
}


void PlusMinusItemDelegate::setThumbnailSize(int value)
{
	d->mThumbnailSize = value;

	const int width = d->itemWidth();
	const int buttonWidth = d->mMinusButton->sizeHint().width();
	//d->mRotateLeftButton->setVisible(width >= 3 * buttonWidth);
	//d->mRotateRightButton->setVisible(width >= 4 * buttonWidth);
	d->mContextBar->adjustSize();

	d->mElidedTextCache.clear();
}



void PlusMinusItemDelegate::slotPlusButtonClicked()
{
 	d->selectIndexUnderCursorIfNoMultiSelection();
 	emit plusButtonClicked(d->mIndexUnderCursor);
}

void PlusMinusItemDelegate::slotMinusButtonClicked()
{
 	d->selectIndexUnderCursorIfNoMultiSelection();
 	emit minusButtonClicked(d->mIndexUnderCursor);
}

// void PlusMinusItemDelegate::slotRotateLeftClicked() {
// 	d->selectIndexUnderCursorIfNoMultiSelection();
// 	rotateDocumentLeftRequested(d->mIndexUnderCursor);
// }





// void PlusMinusItemDelegate::slotToggleSelectionClicked()
// {
// 	if (d->mView->selectionBehavior() == QAbstractItemView::SelectRows)
// 		d->mView->selectionModel()->select(d->mIndexUnderCursor, QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
// 	else
// 	if (d->mView->selectionBehavior() == QAbstractItemView::SelectColumns)
// 		d->mView->selectionModel()->select(d->mIndexUnderCursor, QItemSelectionModel::Toggle | QItemSelectionModel::Columns);
// 	else
// 		d->mView->selectionModel()->select(d->mIndexUnderCursor, QItemSelectionModel::Toggle);
// 	d->updateToggleSelectionButton();
// }


