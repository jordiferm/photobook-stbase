/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "htmleditwidget.h"

#include <QAction>
#include <QFrame>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QTextCursor>
#include <QTextList>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QToolBar>
#include <QLayout> 

#include "fstdactions.h" 


const QString rsrcPath = ":/st";

HtmlEditWidget::HtmlEditWidget(QWidget *parent,  Qt::WindowFlags flags)
		: QMainWindow(parent, flags)
{
	setupEditActions();
	setupTextActions();
	
	textEdit = new QTextEdit(this);

	QFrame* CFrame = new QFrame(this); 
	QVBoxLayout* MLayout = new QVBoxLayout(CFrame); 
	MLayout->addWidget(textEdit); 
	
	QToolBar* BottomToolBar = new QToolBar(this); 
	BottomToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon); 
	
	MLayout->addWidget(BottomToolBar); 
	MLayout->setAlignment(BottomToolBar, Qt::AlignRight); 
	
	QAction* AcceptAction = new FAcceptAction(this);
	connect(AcceptAction, SIGNAL(triggered()), this, SLOT(accepted())); 
	BottomToolBar->addAction(AcceptAction); 

	QAction* CancelAction = new FCancelAction(this);
	connect(CancelAction, SIGNAL(triggered()), this, SLOT(close())); 
	BottomToolBar->addAction(CancelAction); 

	connect(textEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
	        this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));
	connect(textEdit, SIGNAL(cursorPositionChanged()),
	        this, SLOT(cursorPositionChanged()));

	setCentralWidget(CFrame);
	textEdit->setFocus();

	fontChanged(textEdit->font());
	colorChanged(textEdit->textColor());
	alignmentChanged(textEdit->alignment());

	connect(textEdit->document(), SIGNAL(modificationChanged(bool)),
	        this, SLOT(setWindowModified(bool)));
	connect(textEdit->document(), SIGNAL(undoAvailable(bool)),
	        actionUndo, SLOT(setEnabled(bool)));
	connect(textEdit->document(), SIGNAL(redoAvailable(bool)),
	        actionRedo, SLOT(setEnabled(bool)));

	setWindowModified(textEdit->document()->isModified());
	actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
	actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

	connect(actionUndo, SIGNAL(triggered()), textEdit, SLOT(undo()));
	connect(actionRedo, SIGNAL(triggered()), textEdit, SLOT(redo()));

	actionCut->setEnabled(false);
	actionCopy->setEnabled(false);

	connect(actionCut, SIGNAL(triggered()), textEdit, SLOT(cut()));
	connect(actionCopy, SIGNAL(triggered()), textEdit, SLOT(copy()));
	connect(actionPaste, SIGNAL(triggered()), textEdit, SLOT(paste()));

	connect(textEdit, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
	connect(textEdit, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
}


QString HtmlEditWidget::html(const QByteArray& _Encoding) const
{
	return textEdit->document()->toHtml(_Encoding);
}

void HtmlEditWidget::setHtml(const QString& _Html)
{
	textEdit->document()->setHtml(_Html);
}

QString HtmlEditWidget::plainText() const
{
	return textEdit->document()->toPlainText();
}

void HtmlEditWidget::setPlainText(const QString& _PlainText)
{
	textEdit->document()->setPlainText(_PlainText);
}

QTextCharFormat HtmlEditWidget::textCharFormat() const
{
	QTextCharFormat Res = textEdit->textCursor().charFormat();
	if (Res.foreground().style() == Qt::NoBrush)
	{
		QBrush ForegroundBrush = Res.foreground(); 
		ForegroundBrush.setStyle(Qt::SolidPattern); 
		Res.setForeground(ForegroundBrush);
	}
	return Res; 
}

void HtmlEditWidget::setTextCharFormat(const QTextCharFormat & _Format)
{
	textEdit->setCurrentCharFormat(_Format); 
	QTextCursor cursor = textEdit->textCursor();
	cursor.select(QTextCursor::Document);
	cursor.mergeCharFormat(_Format);
	textEdit->mergeCurrentCharFormat(_Format);
}

QColor HtmlEditWidget::textForegroundColor() const 
{
	return textEdit->currentCharFormat().foreground().color(); 
}

void HtmlEditWidget::setTextForegroundColor(const QColor& _Color) 
{
	QTextCharFormat CurrFormat = textEdit->currentCharFormat(); 
	CurrFormat.setForeground(_Color); 
	textEdit->setCurrentCharFormat(CurrFormat); 
}

bool HtmlEditWidget::hasMultiLineText() const
{
	return plainText().count("\n") > 0; 
}


/*!
	\return True if the text needs Html.
*/
bool HtmlEditWidget::needsHtml() const 
{
	bool Res = true; 
	if (!hasMultiLineText())
	{
		QTextCursor cursor = textEdit->textCursor();
		cursor.setPosition(QTextCursor::Start); 
		cursor.clearSelection();
		cursor.movePosition(QTextCursor::NextCharacter);
		QTextCharFormat CurrFormat = cursor.charFormat();
		bool Found = false; 
		while (!cursor.atEnd() && ! Found)
		{
			cursor.clearSelection();
			cursor.movePosition(QTextCursor::NextCharacter);
			Found = cursor.charFormat() != CurrFormat; 
		}
		if (!Found)
			Res = false; 
	}
	return Res; 
}

int HtmlEditWidget::exec()
{
	Accepted = false; 
	bool wasShowModal = testAttribute(Qt::WA_ShowModal);
	setAttribute(Qt::WA_ShowModal, true);
	//textEdit->document()->clear();
	
	show();
	
	
	EventLoop.exec();
	
	setAttribute(Qt::WA_ShowModal, wasShowModal);
	int Res; 
	if (Accepted)
		Res = QDialog::Accepted; 
	else 
		Res = QDialog::Rejected; 
	return Res; 
}

void HtmlEditWidget::closeEvent(QCloseEvent *e)
{
	EventLoop.exit();
}


void HtmlEditWidget::setupEditActions()
{
	QToolBar *tb = new QToolBar(this);
	tb->setWindowTitle(tr("Edit Actions"));
	addToolBar(tb);

	QMenu *menu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(menu);

	QAction *a;
	a = actionUndo = new QAction(QIcon(rsrcPath + "/editundo.png"), tr("&Undo"), this);
	a->setShortcut(QKeySequence::Undo);
	tb->addAction(a);
	menu->addAction(a);
	a = actionRedo = new QAction(QIcon(rsrcPath + "/editredo.png"), tr("&Redo"), this);
	a->setShortcut(QKeySequence::Redo);
	tb->addAction(a);
	menu->addAction(a);
	menu->addSeparator();
	a = actionCut = new QAction(QIcon(rsrcPath + "/editcut.png"), tr("Cu&t"), this);
	a->setShortcut(QKeySequence::Cut);
	tb->addAction(a);
	menu->addAction(a);
	a = actionCopy = new QAction(QIcon(rsrcPath + "/editcopy.png"), tr("&Copy"), this);
	a->setShortcut(QKeySequence::Copy);
	tb->addAction(a);
	menu->addAction(a);
	a = actionPaste = new QAction(QIcon(rsrcPath + "/editpaste.png"), tr("&Paste"), this);
	a->setShortcut(QKeySequence::Paste);
	tb->addAction(a);
	menu->addAction(a);
	actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void HtmlEditWidget::setupTextActions()
{
	QToolBar *tb = new QToolBar(this);
	tb->setWindowTitle(tr("Format Actions"));
	addToolBar(tb);

	QMenu *menu = new QMenu(tr("F&ormat"), this);
	menuBar()->addMenu(menu);

	actionTextBold = new QAction(QIcon(rsrcPath + "/textbold.png"), tr("&Bold"), this);
	actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
	QFont bold;
	bold.setBold(true);
	actionTextBold->setFont(bold);
	connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
	tb->addAction(actionTextBold);
	menu->addAction(actionTextBold);
	actionTextBold->setCheckable(true);

	actionTextItalic = new QAction(QIcon(rsrcPath + "/textitalic.png"), tr("&Italic"), this);
	actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
	QFont italic;
	italic.setItalic(true);
	actionTextItalic->setFont(italic);
	connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
	tb->addAction(actionTextItalic);
	menu->addAction(actionTextItalic);
	actionTextItalic->setCheckable(true);

	actionTextUnderline = new QAction(QIcon(rsrcPath + "/textunder.png"), tr("&Underline"), this);
	actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
	QFont underline;
	underline.setUnderline(true);
	actionTextUnderline->setFont(underline);
	connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
	tb->addAction(actionTextUnderline);
	menu->addAction(actionTextUnderline);
	actionTextUnderline->setCheckable(true);

	menu->addSeparator();

	QActionGroup *grp = new QActionGroup(this);
	connect(grp, SIGNAL(triggered(QAction *)), this, SLOT(textAlign(QAction *)));

	actionAlignLeft = new QAction(QIcon(rsrcPath + "/textleft.png"), tr("&Left"), grp);
	actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
	actionAlignLeft->setCheckable(true);
	actionAlignCenter = new QAction(QIcon(rsrcPath + "/textcenter.png"), tr("C&enter"), grp);
	actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
	actionAlignCenter->setCheckable(true);
	actionAlignRight = new QAction(QIcon(rsrcPath + "/textright.png"), tr("&Right"), grp);
	actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
	actionAlignRight->setCheckable(true);
	actionAlignJustify = new QAction(QIcon(rsrcPath + "/textjustify.png"), tr("&Justify"), grp);
	actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
	actionAlignJustify->setCheckable(true);

	tb->addActions(grp->actions());
	menu->addActions(grp->actions());

	menu->addSeparator();

	QPixmap pix(16, 16);
	pix.fill(Qt::black);
	actionTextColor = new QAction(pix, tr("&Color..."), this);
	connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
	tb->addAction(actionTextColor);
	menu->addAction(actionTextColor);

	actionTextBackgroundColor = new QAction(QIcon(rsrcPath + "/colorfill.png"), tr("&Background Color"), grp);
	connect(actionTextBackgroundColor, SIGNAL(triggered()), this, SLOT(textBackground()));
	tb->addAction(actionTextBackgroundColor);
	menu->addAction(actionTextBackgroundColor);

	actionNoTextBackgroundColor = new QAction(QIcon(rsrcPath + "/nocolorfill.png"), tr("&No Background Color"), grp);
	connect(actionNoTextBackgroundColor, SIGNAL(triggered()), this, SLOT(noTextBackground()));
	tb->addAction(actionNoTextBackgroundColor);
	menu->addAction(actionNoTextBackgroundColor);


	actionTextOutLineColor = new QAction(QIcon(rsrcPath + "/color_line.png"), tr("&Outline Color"), grp);
	connect(actionTextOutLineColor, SIGNAL(triggered()), this, SLOT(textOutlineColor()));
	tb->addAction(actionTextOutLineColor);
	menu->addAction(actionTextOutLineColor);

	actionNoTextOutLineColor = new QAction(QIcon(rsrcPath + "/nocolor_line.png"), tr("&No Outline Color"), grp);
	connect(actionNoTextOutLineColor, SIGNAL(triggered()), this, SLOT(noTextOutlineColor()));
	tb->addAction(actionNoTextOutLineColor);
	menu->addAction(actionNoTextOutLineColor);

	tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	tb->setWindowTitle(tr("Format Actions"));
	addToolBarBreak(Qt::TopToolBarArea);
	addToolBar(tb);

	comboStyle = new QComboBox(tb);
	tb->addWidget(comboStyle);
	comboStyle->addItem("Standard");
	comboStyle->addItem("Bullet List (Disc)");
	comboStyle->addItem("Bullet List (Circle)");
	comboStyle->addItem("Bullet List (Square)");
	comboStyle->addItem("Ordered List (Decimal)");
	comboStyle->addItem("Ordered List (Alpha lower)");
	comboStyle->addItem("Ordered List (Alpha upper)");
	connect(comboStyle, SIGNAL(activated(int)),
	        this, SLOT(textStyle(int)));

	comboFont = new QFontComboBox(tb);
	tb->addWidget(comboFont);
	connect(comboFont, SIGNAL(activated(const QString &)),
	        this, SLOT(textFamily(const QString &)));

	comboSize = new QComboBox(tb);
	comboSize->setObjectName("comboSize");
	tb->addWidget(comboSize);
	comboSize->setEditable(true);

	QFontDatabase db;
	foreach(int size, db.standardSizes())
	comboSize->addItem(QString::number(size));

	connect(comboSize, SIGNAL(activated(const QString &)),
	        this, SLOT(textSize(const QString &)));
	comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font()
	                           .pointSize())));
}

void HtmlEditWidget::textBold()
{
	QTextCharFormat fmt;
	fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textUnderline()
{
	QTextCharFormat fmt;
	fmt.setFontUnderline(actionTextUnderline->isChecked());
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textItalic()
{
	QTextCharFormat fmt;
	fmt.setFontItalic(actionTextItalic->isChecked());
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textFamily(const QString &f)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(f);
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textSize(const QString &p)
{
	QTextCharFormat fmt;
	fmt.setFontPointSize(p.toFloat());
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textStyle(int styleIndex)
{
	QTextCursor cursor = textEdit->textCursor();

	if (styleIndex != 0)
	{
		QTextListFormat::Style style = QTextListFormat::ListDisc;

		switch (styleIndex)
		{
			default:
			case 1:
			style = QTextListFormat::ListDisc;
			break;
			case 2:
			style = QTextListFormat::ListCircle;
			break;
			case 3:
			style = QTextListFormat::ListSquare;
			break;
			case 4:
			style = QTextListFormat::ListDecimal;
			break;
			case 5:
			style = QTextListFormat::ListLowerAlpha;
			break;
			case 6:
			style = QTextListFormat::ListUpperAlpha;
			break;
		}

		cursor.beginEditBlock();

		QTextBlockFormat blockFmt = cursor.blockFormat();

		QTextListFormat listFmt;

		if (cursor.currentList())
		{
			listFmt = cursor.currentList()->format();
		}
		else
		{
			listFmt.setIndent(blockFmt.indent() + 1);
			blockFmt.setIndent(0);
			cursor.setBlockFormat(blockFmt);
		}

		listFmt.setStyle(style);

		cursor.createList(listFmt);

		cursor.endEditBlock();
	}
	else
	{
		// ####
		QTextBlockFormat bfmt;
		bfmt.setObjectIndex(-1);
		cursor.mergeBlockFormat(bfmt);
	}
}

void HtmlEditWidget::textColor()
{
	QColor col = QColorDialog::getColor(textEdit->textColor(), this);
	if (!col.isValid())
		return;
	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnLineOrSelection(fmt);
	colorChanged(col);
}

void HtmlEditWidget::textBackground()
{
	QColor col = QColorDialog::getColor(textEdit->textColor(), this);
	if (!col.isValid())
		return;
	QTextCharFormat fmt;
	fmt.setBackground(col);
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::noTextBackground()
{
	QTextCharFormat fmt;
	fmt.setBackground(QBrush());
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textOutlineColor()
{
	QColor col = QColorDialog::getColor(textEdit->textColor(), this);
	if (!col.isValid())
		return;
	QTextCharFormat fmt;
	fmt.setTextOutline(col);
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::noTextOutlineColor()
{
	QTextCharFormat fmt;
	fmt.setTextOutline(QPen(Qt::NoPen));
	mergeFormatOnLineOrSelection(fmt);
}

void HtmlEditWidget::textAlign(QAction *a)
{
	if (a == actionAlignLeft)
		textEdit->setAlignment(Qt::AlignLeft);
	else if (a == actionAlignCenter)
		textEdit->setAlignment(Qt::AlignHCenter);
	else if (a == actionAlignRight)
		textEdit->setAlignment(Qt::AlignRight);
	else if (a == actionAlignJustify)
		textEdit->setAlignment(Qt::AlignJustify);
}

void HtmlEditWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
	fontChanged(format.font());
	colorChanged(format.foreground().color());
}

void HtmlEditWidget::cursorPositionChanged()
{
	alignmentChanged(textEdit->alignment());
}

void HtmlEditWidget::clipboardDataChanged()
{
	actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void HtmlEditWidget::accepted()
{
	Accepted = true; 
	close(); 
}


void HtmlEditWidget::mergeFormatOnLineOrSelection(const QTextCharFormat &format)
{
	QTextCursor cursor = textEdit->textCursor();
	if (!cursor.hasSelection())
		cursor.select(QTextCursor::LineUnderCursor);
	cursor.mergeCharFormat(format);
	textEdit->mergeCurrentCharFormat(format);
}

void HtmlEditWidget::fontChanged(const QFont &f)
{
	comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
	comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
	actionTextBold->setChecked(f.bold());
	actionTextItalic->setChecked(f.italic());
	actionTextUnderline->setChecked(f.underline());
}

void HtmlEditWidget::colorChanged(const QColor &c)
{
	QPixmap pix(16, 16);
	pix.fill(c);
	actionTextColor->setIcon(pix);
}

void HtmlEditWidget::alignmentChanged(Qt::Alignment a)
{
	if (a & Qt::AlignLeft)
	{
		actionAlignLeft->setChecked(true);
	}
	else if (a & Qt::AlignHCenter)
	{
		actionAlignCenter->setChecked(true);
	}
	else if (a & Qt::AlignRight)
	{
		actionAlignRight->setChecked(true);
	}
	else if (a & Qt::AlignJustify)
	{
		actionAlignJustify->setChecked(true);
	}
}

