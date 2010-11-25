/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-05-15
 * Description : a dialog to see preview ICC color correction
 *               before to apply color profile.
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "colorcorrectiondlg.h"

// Qt includes.

#include <QLabel>
#include <QFrame>
#include <QString>
#include <QFileInfo>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtDebug>
#include <QDialogButtonBox>


// Local includes.

#include "dimg.h"
#include "icctransform.h"
#include "iccprofileinfodlg.h"

namespace Digikam
{

	ColorCorrectionDlg::ColorCorrectionDlg(QWidget* parent, DImg *preview,
	                                       IccTransform *iccTrans, const QString& file)
			: QDialog(parent)
	{
		m_iccTrans = iccTrans;
		m_parent   = parent;
	
		QDialogButtonBox* BBox = new QDialogButtonBox(this); 
		BBox->addButton(QDialogButtonBox::Help); 
		BBox->addButton(QDialogButtonBox::Ok); 
		BBox->addButton(QDialogButtonBox::Apply); 
		BBox->addButton(QDialogButtonBox::Cancel); 
		//setButtons(Help | Ok | Apply | Cancel);
		BBox->button(QDialogButtonBox::Ok)->setDefault(true);
		//setDefaultButton(Ok);
		setModal(true);
		//TODO: Help path and topic.
		//setHelp("iccprofile.anchor", "digikam");
		BBox->button(QDialogButtonBox::Ok)->setText(tr("Convert"));
		BBox->button(QDialogButtonBox::Ok)->setToolTip(tr("Apply the default color workspace profile to the image"));
		BBox->button(QDialogButtonBox::Cancel)->setText(tr("Do Nothing"));
		BBox->button(QDialogButtonBox::Cancel)->setToolTip(tr("Do not change the image"));
		BBox->button(QDialogButtonBox::Apply)->setText(tr("Assign"));
		BBox->button(QDialogButtonBox::Apply)->setToolTip(tr("Only embed the color workspace profile in the image, "
		                            "do not change the image"));

		QFileInfo fi(file);
		setWindowTitle(fi.fileName()); 
		//setCaption(fi.fileName());

		QWidget *page     = new QWidget(this);
		QVBoxLayout* MLayout = new QVBoxLayout(this); 
		QGridLayout* grid = new QGridLayout(page);
		MLayout->addWidget(page); 
		MLayout->addWidget(BBox); 

		QLabel *originalTitle         = new QLabel(tr("Original Image:"), page);
		QLabel *previewOriginal       = new QLabel(page);
		QLabel *targetTitle           = new QLabel(tr("Corrected Image:"), page);
		QLabel *previewTarget         = new QLabel(page);
		QLabel *logo                  = new QLabel(page);
		QLabel *message               = new QLabel(page);
		QLabel *currentProfileTitle   = new QLabel(tr("Current workspace color profile:"), page);
		QLabel *currentProfileDesc    = new QLabel(QString("<b>%1</b>")
		        .arg(m_iccTrans->getOutpoutProfileDescriptor()), page);
		QPushButton *currentProfInfo  = new QPushButton(tr("Info..."), page);
		QLabel *embeddedProfileTitle  = new QLabel(tr("Embedded color profile:"), page);
		QLabel *embeddedProfileDesc   = new QLabel(QString("<b>%1</b>")
		        .arg(m_iccTrans->getEmbeddedProfileDescriptor()), page);
		QPushButton *embeddedProfInfo = new QPushButton(tr("Info..."), page);
		//KSeparator *line              = new KSeparator(Qt::Horizontal, page);

		if (m_iccTrans->embeddedProfile().isEmpty())
		{
			message->setText(tr("<p>This image has not been assigned a color profile.</p>"
			                    "<p>Do you want to convert it to your workspace color profile?</p>"));

			//line->hide();
			embeddedProfileTitle->hide();
			embeddedProfileDesc->hide();
			embeddedProfInfo->hide();
		}
		else
		{
			message->setText(tr("<p>This image has been assigned to a color profile that does not "
			                    "match your default workspace color profile.</p>"
			                    "<p>Do you want to convert it to your workspace color profile?</p>"));
		}

		originalTitle->setWordWrap(true);
		targetTitle->setWordWrap(true);
		message->setWordWrap(true);
		currentProfileTitle->setWordWrap(true);
		currentProfileDesc->setWordWrap(true);
		embeddedProfileTitle->setWordWrap(true);
		embeddedProfileDesc->setWordWrap(true);

		previewOriginal->setPixmap(preview->convertToPixmap());
		previewTarget->setPixmap(preview->convertToPixmap(m_iccTrans));
		//TODO:put a pixmap here 
		//logo->setPixmap(QPixmap(KStandardDirs::locate("data", "digikam/data/logo-digikam.png"))
		//                .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

		QHBoxLayout *hlay1 = new QHBoxLayout();
		hlay1->setSpacing(1);
		hlay1->addWidget(currentProfInfo);
		hlay1->addStretch();

		QHBoxLayout *hlay2 = new QHBoxLayout();
		hlay2->setSpacing(1);
		hlay2->addWidget(embeddedProfInfo);
		hlay2->addStretch();

		QVBoxLayout *vlay = new QVBoxLayout();
		vlay->setSpacing(1);
		vlay->addWidget(logo);
		vlay->addWidget(message);
		//vlay->addWidget(new KSeparator(Qt::Horizontal, page));
		vlay->addWidget(currentProfileTitle);
		vlay->addWidget(currentProfileDesc);
		vlay->addLayout(hlay1);
		//vlay->addWidget(line);
		vlay->addWidget(embeddedProfileTitle);
		vlay->addWidget(embeddedProfileDesc);
		vlay->addLayout(hlay2);
		vlay->addStretch();

		grid->setMargin(0);
		grid->setSpacing(1);
		grid->addWidget(originalTitle,      0, 0, 1, 1);
		grid->addWidget(previewOriginal,    1, 0, 1, 1);
		grid->addWidget(targetTitle,        2, 0, 1, 1);
		grid->addWidget(previewTarget,      3, 0, 1, 1);
		grid->addItem(new QSpacerItem(1, 1,
		                              QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 1, 3, 1);
		grid->addLayout(vlay, 0, 2, 4, 1);

		// --------------------------------------------------------------------

		connect(currentProfInfo, SIGNAL(clicked()),
		        this, SLOT(slotCurrentProfInfo()));

		connect(embeddedProfInfo, SIGNAL(clicked()),
		        this, SLOT(slotEmbeddedProfInfo()));

		connect(this, SIGNAL(applyClicked()),
		        this, SLOT(slotApplyClicked()));
	}

	ColorCorrectionDlg::~ColorCorrectionDlg()
	{
	}

	void ColorCorrectionDlg::slotCurrentProfInfo()
	{
		if (m_iccTrans->outputProfile().isEmpty())
			return;

		ICCProfileInfoDlg infoDlg(m_parent, QString(), m_iccTrans->outputProfile());
		infoDlg.exec();
	}

	void ColorCorrectionDlg::slotEmbeddedProfInfo()
	{
		if (m_iccTrans->embeddedProfile().isEmpty())
			return;

		ICCProfileInfoDlg infoDlg(m_parent, QString(), m_iccTrans->embeddedProfile());
		infoDlg.exec();
	}

	void ColorCorrectionDlg::slotApplyClicked()
	{
		done(-1);
	}

}  // namespace Digikam
