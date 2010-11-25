/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-23
 * Description : a tab widget to display ICC profile infos
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

#include "iccprofilewidget.h"

// Qt includes.

#include <QComboBox>
#include <QFile>
#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QtDebug> 


// Lcms includes.

#include <lcms.h>
#if LCMS_VERSION < 114
#define cmsTakeCopyright(profile) "Unknown"
#endif // LCMS_VERSION < 114

// Local includes.

#include "cietonguewidget.h"
#include "metadatalistview.h"

namespace Digikam
{

static const char* ICCHumanList[] =
{
     "ColorSpace",
     "Copyright",
     "DeviceClass",
     "Name",
     "Description",
     "RenderingIntent",
     "-1"
};

// This entry list is only require for compatibility with MetadataWidget implementation.
static const char* ICCEntryList[] =
{
     "Header",
     "-1"
};

class ICCTagInfo
{

public:

    ICCTagInfo(){}

    ICCTagInfo(const QString& title, const QString& description)
        : m_title(title), m_description(description){}

    QString title()       const { return m_title;       }
    QString description() const { return m_description; }

private:

    QString m_title;
    QString m_description;
};

typedef QMap<QString, ICCTagInfo> ICCTagInfoMap;

class ICCProfileWidgetPriv
{

public:

    ICCProfileWidgetPriv()
    {
        cieTongue = 0;
    }

    QByteArray       profileData;

    QStringList      tagsfilter;
    QStringList      keysFilter;

    CIETongueWidget *cieTongue;

    ICCTagInfoMap    iccTagsDescription;
};

ICCProfileWidget::ICCProfileWidget(QWidget* parent, int w, int h)
                : MetadataWidget(parent), d(new ICCProfileWidgetPriv)
{
    cmsErrorAction(LCMS_ERROR_SHOW);

    // Set the translated ICC tags titles/descriptions list
    d->iccTagsDescription["Icc.Header.Name"]            = ICCTagInfo(tr("Name"),
                                                          tr("The ICC profile product name"));
    d->iccTagsDescription["Icc.Header.Description"]     = ICCTagInfo(tr("Description"),
                                                          tr("The ICC profile product description"));
    d->iccTagsDescription["Icc.Header.Information"]     = ICCTagInfo(tr("Information"),
                                                          tr("Additional ICC profile information"));
    d->iccTagsDescription["Icc.Header.Manufacturer"]    = ICCTagInfo(tr("Manufacturer"),
                                                          tr("Raw information about the ICC profile manufacturer"));
    d->iccTagsDescription["Icc.Header.Model"]           = ICCTagInfo(tr("Model"),
                                                          tr("Raw information about the ICC profile model"));
    d->iccTagsDescription["Icc.Header.Copyright"]       = ICCTagInfo(tr("Copyright"),
                                                          tr("Raw information about the ICC profile copyright"));
    d->iccTagsDescription["Icc.Header.ProfileID"]       = ICCTagInfo(tr("Profile ID"),
                                                          tr("The ICC profile ID number"));
    d->iccTagsDescription["Icc.Header.ColorSpace"]      = ICCTagInfo(tr("Color Space"),
                                                          tr("The color space used by the ICC profile"));
    d->iccTagsDescription["Icc.Header.ConnectionSpace"] = ICCTagInfo(tr("Connection Space"),
                                                          tr("The connection space used by the ICC profile"));
    d->iccTagsDescription["Icc.Header.DeviceClass"]     = ICCTagInfo(tr("Device Class"),
                                                          tr("The ICC profile device class"));
    d->iccTagsDescription["Icc.Header.RenderingIntent"] = ICCTagInfo(tr("Rendering Intent"),
                                                          tr("The ICC profile rendering intent"));
    d->iccTagsDescription["Icc.Header.ProfileVersion"]  = ICCTagInfo(tr("Profile Version"),
                                                          tr("The ICC version used to record the profile"));
    d->iccTagsDescription["Icc.Header.CMMFlags"]        = ICCTagInfo(tr("CMM Flags"),
                                                          tr("The ICC profile color management flags"));

    // Set the list of keys and tags filters.
    for (int i=0 ; QString(ICCEntryList[i]) != QString("-1") ; i++)
        d->keysFilter << ICCEntryList[i];

    for (int i=0 ; QString(ICCHumanList[i]) != QString("-1") ; i++)
        d->tagsfilter << ICCHumanList[i];

    // Add CIE tongue graph to the widget area

    d->cieTongue = new CIETongueWidget(w, h, this);
    d->cieTongue->setWhatsThis( tr("<p>This area contains a CIE or chromaticity diagram. "
                    "A CIE diagram is a representation of all the colors "
                    "that a person with normal vision can see. This is represented "
                    "by the colored sail-shaped area. In addition you will see a "
                    "triangle that is superimposed on the diagram outlined in white. "
                    "This triangle represents the outer boundaries of the color space "
                    "of the device that is characterized by the inspected profile. "
                    "This is called the device gamut.</p>"
                    "<p>In addition there are black dots and yellow lines on the diagram. "
                    "Each black dot represents one of the measurement points that were "
                    "used to create this profile. The yellow line represents the "
                    "amount that each point is corrected by the profile, and the "
                    "direction of this correction.</p>"));

    setUserAreaWidget(d->cieTongue);
    decodeMetadata();
}

ICCProfileWidget::~ICCProfileWidget()
{
    delete d;
}

bool ICCProfileWidget::setProfileData(const QByteArray& data)
{
    d->profileData = data;

    // Cleanup all metadata contents.
    setMetadataMap();

    if (d->profileData.isEmpty())
    {
        setMetadataEmpty();
        return false;
    }

    // Try to decode current metadata.
    if (decodeMetadata())
        enabledToolButtons(true);
    else
        enabledToolButtons(false);

    // Refresh view using decoded metadata.
    buildView();
    return true;
}

const QByteArray& ICCProfileWidget::getProfileData()
{
    return d->profileData;
}

void ICCProfileWidget::setDataLoading()
{
    d->cieTongue->loadingStarted();
}

void ICCProfileWidget::setLoadingFailed()
{
    d->cieTongue->loadingFailed();
}

QString ICCProfileWidget::getMetadataTitle()
{
    return tr("ICC Color Profile Information");
}

bool ICCProfileWidget::loadFromURL(const QUrl& url)
{
    setFileName(url.path());

    if (url.isEmpty())
    {
        setProfileData();
        d->cieTongue->setProfileData();
        return false;
    }
    else
    {
        QFile file(url.path());
        if ( !file.open(QIODevice::ReadOnly) )
        {
            setProfileData();
            d->cieTongue->setProfileData();
            return false;
        }

        QByteArray iccData;
        iccData.resize(file.size());

        QDataStream stream( &file );
        stream.readRawData(iccData.data(), iccData.size());
        file.close();

        if (iccData.isEmpty())
        {
            setProfileData();
            d->cieTongue->setProfileData();
            return false;
        }
        else
        {
            setProfileData(iccData);
            d->cieTongue->setProfileData(iccData);
        }
    }

    return true;
}

bool ICCProfileWidget::loadFromProfileData(const QString& fileName, const QByteArray& data)
{
    setFileName(fileName);
    return(setProfileData(data));
}

bool ICCProfileWidget::decodeMetadata()
{
    QByteArray iccData = getProfileData();
    if (iccData.isNull())
        return false;

    d->cieTongue->setProfileData(iccData);

    cmsHPROFILE hProfile = cmsOpenProfileFromMem(iccData.data(), (DWORD)iccData.size());

    if (!hProfile)
    {
        qDebug() << "Cannot parse ICC profile tags using LCMS" << endl;
        return false;
    }

    DMetadata::MetaDataMap metaDataMap;

    if ( !QString(cmsTakeProductName(hProfile)).isEmpty() )
        metaDataMap.insert("Icc.Header.Name", QString(cmsTakeProductName(hProfile)).replace('\n', ' '));

    if ( !QString(cmsTakeProductDesc(hProfile)).isEmpty() )
        metaDataMap.insert("Icc.Header.Description", QString(cmsTakeProductDesc(hProfile)).replace('\n', ' '));

    if ( !QString(cmsTakeProductInfo(hProfile)).isEmpty() )
        metaDataMap.insert("Icc.Header.Information", QString(cmsTakeProductInfo(hProfile)).replace('\n', ' '));

    if ( !QString(cmsTakeManufacturer(hProfile)).isEmpty() )
        metaDataMap.insert("Icc.Header.Manufacturer", QString(cmsTakeManufacturer(hProfile)).replace('\n', ' '));

    if ( !QString(cmsTakeModel(hProfile)).isEmpty() )
        metaDataMap.insert("Icc.Header.Model", QString(cmsTakeModel(hProfile)).replace('\n', ' '));

    if ( !QString(cmsTakeCopyright(hProfile)).isEmpty() )
        metaDataMap.insert("Icc.Header.Copyright", QString(cmsTakeCopyright(hProfile)).replace('\n', ' '));

    metaDataMap.insert("Icc.Header.ProfileID",      QString::number((uint)*cmsTakeProfileID(hProfile)));
    metaDataMap.insert("Icc.Header.ProfileVersion", QString::number((uint)cmsGetProfileICCversion(hProfile)));
    metaDataMap.insert("Icc.Header.CMMFlags",       QString::number((uint)cmsTakeHeaderFlags(hProfile)));

    QString colorSpace;
    switch (cmsGetColorSpace(hProfile))
    {
        case icSigLabData:
            colorSpace = tr("Lab");
            break;
        case icSigLuvData:
            colorSpace = tr("Luv");
            break;
        case icSigRgbData:
            colorSpace = tr("RGB");
            break;
        case icSigGrayData:
            colorSpace = tr("GRAY");
            break;
        case icSigHsvData:
            colorSpace = tr("HSV");
            break;
        case icSigHlsData:
            colorSpace = tr("HLS");
            break;
        case icSigCmykData:
            colorSpace = tr("CMYK");
            break;
        case icSigCmyData:
            colorSpace= tr("CMY");
            break;
        default:
            colorSpace = tr("Unknown");
            break;
    }
    metaDataMap.insert("Icc.Header.ColorSpace", colorSpace);

    QString connectionSpace;
    switch (cmsGetPCS(hProfile))
    {
        case icSigLabData:
            connectionSpace = tr("Lab");
            break;
        case icSigLuvData:
            connectionSpace = tr("Luv");
            break;
        case icSigRgbData:
            connectionSpace = tr("RGB");
            break;
        case icSigGrayData:
            connectionSpace = tr("GRAY");
            break;
        case icSigHsvData:
            connectionSpace = tr("HSV");
            break;
        case icSigHlsData:
            connectionSpace = tr("HLS");
            break;
        case icSigCmykData:
            connectionSpace = tr("CMYK");
            break;
        case icSigCmyData:
            connectionSpace= tr("CMY");
            break;
        default:
            connectionSpace = tr("Unknown");
            break;
    }
    metaDataMap.insert("Icc.Header.ConnectionSpace", connectionSpace);

    QString device;
    switch ((int)cmsGetDeviceClass(hProfile))
    {
        case icSigInputClass:
            device = tr("Input device");
            break;
        case icSigDisplayClass:
            device = tr("Display device");
            break;
        case icSigOutputClass:
            device = tr("Output device");
            break;
        case icSigColorSpaceClass:
            device = tr("Color space");
            break;
        case icSigLinkClass:
            device = tr("Link device");
            break;
        case icSigAbstractClass:
            device = tr("Abstract");
            break;
        case icSigNamedColorClass:
            device = tr("Named color");
            break;
        default:
            device = tr("Unknown");
            break;
    }
    metaDataMap.insert("Icc.Header.DeviceClass", device);

    QString intent;
    switch (cmsTakeRenderingIntent(hProfile))
    {
        case 0:
            intent = tr("Perceptual");
            break;
        case 1:
            intent = tr("Relative Colorimetric");
            break;
        case 2:
            intent = tr("Saturation");
            break;
        case 3:
            intent = tr("Absolute Colorimetric");
            break;
        default:
            intent = tr("Unknown");
            break;
    }
    metaDataMap.insert("Icc.Header.RenderingIntent", intent);

    cmsCloseProfile(hProfile);

    // Update all metadata contents.
    setMetadataMap(metaDataMap);
    return true;
}

void ICCProfileWidget::buildView()
{
    if (getMode() == SIMPLE)
    {
        setIfdList(getMetadataMap(), d->keysFilter, d->tagsfilter);
    }
    else
    {
        setIfdList(getMetadataMap(), d->keysFilter, QStringList());
    }

    MetadataWidget::buildView();
}

QString ICCProfileWidget::getTagTitle(const QString& key)
{
    ICCTagInfoMap::Iterator it = d->iccTagsDescription.find(key);
    if (it != d->iccTagsDescription.end())
        return(it.value().title());

    return key.section('.', 2, 2);
}

void ICCProfileWidget::slotSaveMetadataToFile()
{
    QUrl url = saveMetadataToFile(tr("ICC color profile File to Save"),
                                  QString("*.icc *.icm|"+tr("ICC Files (*.icc; *.icm)")));
    storeMetadataToFile(url, d->profileData);
}

QString ICCProfileWidget::getTagDescription(const QString& key)
{
    ICCTagInfoMap::Iterator it = d->iccTagsDescription.find(key);
    if (it != d->iccTagsDescription.end())
        return(it.value().description());

    return key.section('.', 2, 2);
}

}  // namespace Digikam
