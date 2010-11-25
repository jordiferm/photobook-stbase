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

#ifndef HTMLEDITWIDGET_H
#define HTMLEDITWIDGET_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QEventLoop> 
#include "stwidgetsexport.h"

class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;

class ST_WIDGETS_EXPORT HtmlEditWidget : public QMainWindow
{
	Q_OBJECT
	QEventLoop EventLoop;
	bool Accepted; 
	 
public:
	HtmlEditWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	QString html(const QByteArray & encoding = QByteArray()) const; 
	void setHtml(const QString& _Html); 
	QString plainText() const; 
	void setPlainText(const QString& _PlainText);
	QTextCharFormat  textCharFormat () const; 
	void setTextCharFormat (const QTextCharFormat & _Font); 
	QColor textForegroundColor() const;
	void setTextForegroundColor(const QColor& _Color);
	bool needsHtml() const;
	//! \return the same set of results of QDialog::exec
	int exec();
	bool hasMultiLineText() const;

protected:
	virtual void closeEvent(QCloseEvent *e);
		 
private:
    void setupEditActions();
    void setupTextActions();

private slots:

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
	void textBackground();
	void noTextBackground();
	void textOutlineColor();
	void noTextOutlineColor();
    void textAlign(QAction *a);

    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();

public slots: 
	void accepted(); 

private:
    void mergeFormatOnLineOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    QAction *actionTextBold,
        *actionTextUnderline,
        *actionTextItalic,
        *actionTextColor,
		*actionTextBackgroundColor,
		*actionNoTextBackgroundColor,
		*actionTextOutLineColor,
		*actionNoTextOutLineColor,
        *actionAlignLeft,
        *actionAlignCenter,
        *actionAlignRight,
        *actionAlignJustify,
        *actionUndo,
        *actionRedo,
        *actionCut,
        *actionCopy,
        *actionPaste;

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    QToolBar *tb;
    QTextEdit* textEdit;
};

#endif
