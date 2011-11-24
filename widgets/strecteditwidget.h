#ifndef STRECTEDITWIDGET_H
#define STRECTEDITWIDGET_H

#include <QWidget>
#include "stwidgetsexport.h"

class QLabel;
class QDoubleSpinBox;
class ST_WIDGETS_EXPORT STRectEditWidget : public QWidget
{
    Q_OBJECT
	QDoubleSpinBox* SPValueLeft;
	QDoubleSpinBox* SPValueRight;
	QDoubleSpinBox* SPValueTop;
	QDoubleSpinBox* SPValueBottom;
	QLabel* LabelLeft;
	QLabel* LabelRight;
	QLabel* LabelTop;
	QLabel* LabelBottom;

public:
    explicit STRectEditWidget(QWidget *parent = 0);
	void setRect(const QRectF& _Rect);
	QRectF rect() const;
	void setSuffix(const QString& _Suffix);

signals:

public slots:

};

#endif // STRECTEDITWIDGET_H
