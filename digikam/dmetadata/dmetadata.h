/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : image metadata interface
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DMETADATA_H
#define DMETADATA_H

// Qt includes.

#include <QtCore/QByteArray>

// LibKExiv2 includes.

#include "kexiv2.h"

// Local includes.

#include "dimg.h"
#include "photoinfocontainer.h"
#include "metadatainfo.h"
#include "stdigikamexport.h"

namespace Digikam
{

class ST_DIGIKAM_EXPORT DMetadata : public KExiv2Iface::KExiv2
{

public:

    DMetadata();
    DMetadata(const QString& filePath);
    ~DMetadata();

    /** Re-implemented from libKexiv2 to use dcraw identify method if Exiv2 failed. */
    bool load(const QString& filePath) const;

    /** Try to extract metadata using dcraw identify method */
    bool loadUsingDcraw(const QString& filePath) const;

    /** Metadata manipulation methods */

    QString getImageComment() const;
    bool    setImageComment(const QString& comment) const;

    int  getImageRating() const;
    bool setImageRating(int rating) const;

    bool getImageTagsPath(QStringList& tagsPath) const;
    bool setImageTagsPath(const QStringList& tagsPath) const;

    bool setImagePhotographerId(const QString& author, const QString& authorTitle) const;
    bool setImageCredits(const QString& credit, const QString& source, const QString& copyright) const;

    /** Return a string with Lens mounted on the front of camera.
        There no standard Exif tag for Lens information.
        Camera makernotes and Xmp tags are parsed. 
        Take a care : lens information are not standardized and string content is not homogeneous between 
        camera model/maker.
     */
    QString getLensDescription() const;

    PhotoInfoContainer getPhotographInformation() const;

    /** Returns the requested metadata field as a QVariant. See metadatainfo.h for a specification
        of the format of the QVariant.
     */
    QVariant     getMetadataField(MetadataInfo::Field field);
    QVariantList getMetadataFields(const MetadataFields &fields);

    /** Convert a QVariant value of the specified field to a user-presentable, i18n'ed string.
        The QVariant must be of the type as specified in metadatainfo.h and as obtained by getMetadataField.
     */
    static QString     valueToString (const QVariant &value, MetadataInfo::Field field);
    static QStringList valuesToString(const QVariantList &list, const MetadataFields &fields);

    /** Returns a map of possible enum values and their user-presentable, i18n'ed representation.
        Valid fields are those which are described as "enum from" or "bit mask from" in metadatainfo.h.
     */
    static QMap<int, QString> possibleValuesForEnumField(MetadataInfo::Field field);

    /** Methods dedicated to record/read a private IPTC tag used to store digiKam image properties.
        Code tested but not used because XMP is more simple to use for that. */
    bool getXMLImageProperties(QString& comments, QDateTime& date,
                               int& rating, QStringList& tagsPath) const;
    bool setXMLImageProperties(const QString& comments, const QDateTime& date, 
                               int rating, const QStringList& tagsPath) const;

    static double apexApertureToFNumber(double aperture);
    static double apexShutterSpeedToExposureTime(double shutterSpeed);

// Scheduled to be moved to libkexiv2
    /** Set an Xmp tag content using a list of strings defined by the 'entriesToAdd' parameter.
        The existing entries are preserved. The method will compare
        all new with all already existing entries to prevent duplicates in the image.
        Return true if the entries have been added to metadata.
     */
    bool addToXmpTagStringBag(const char *xmpTagName, const QStringList& entriesToAdd,
                              bool setProgramName) const;

    /** Remove those Xmp tag entries that are listed in entriesToRemove from the entries in metadata.
        Return true if subjects are no longer contained in metadata.
        All other entries are preserved.
     */
    bool removeFromXmpTagStringBag(const char *xmpTagName, const QStringList& entriesToRemove,
                                   bool setProgramName) const;

    /** Return a strings list of Xmp keywords from image. Return an empty list if no keyword are set.
     */
    QStringList getXmpKeywords() const;

    /** Set Xmp keywords using a list of strings defined by 'newKeywords' parameter.
        The existing keywords from image are preserved. The method will compare
        all new keywords with all already existing keywords to prevent duplicate entries in image.
        Return true if keywords have been changed in metadata.
     */
    bool setXmpKeywords(const QStringList& newKeywords, bool setProgramName=true) const;

    /** Remove those Xmp keywords that are listed in keywordsToRemove from the keywords in metadata.
        Return true if keywords are no longer contained in metadata.
     */
    bool removeXmpKeywords(const QStringList& keywordsToRemove, bool setProgramName=true);

    /** Return a strings list of Xmp subjects from image. Return an empty list if no subject are set.
     */
    QStringList getXmpSubjects() const;

    /** Set Xmp subjects using a list of strings defined by 'newSubjects' parameter.
        The existing subjects from image are preserved. The method will compare
        all new subject with all already existing subject to prevent duplicate entries in image.
        Return true if subjects have been changed in metadata.
     */
    bool setXmpSubjects(const QStringList& newSubjects, bool setProgramName=true) const;

    /** Remove those Xmp subjects that are listed in subjectsToRemove from the subjects in metadata.
        Return true if subjects are no longer contained in metadata.
     */
    bool removeXmpSubjects(const QStringList& subjectsToRemove, bool setProgramName=true);

    /** Return a strings list of Xmp sub-categories from image. Return an empty list if no sub-category
        are set.
     */
    QStringList getXmpSubCategories() const;

    /** Set Xmp sub-categories using a list of strings defined by 'newSubCategories' parameter.
        The existing sub-categories from image are preserved. The method will compare
        all new sub-categories with all already existing sub-categories to prevent duplicate entries in image.
        Return true if sub-categories have been changed in metadata.
     */
    bool setXmpSubCategories(const QStringList& newSubCategories, bool setProgramName=true) const;

    /** Remove those Xmp sub-categories that are listed in categoriesToRemove from the sub-categories in metadata.
        Return true if subjects are no longer contained in metadata.
     */
    bool removeXmpSubCategories(const QStringList& categoriesToRemove, bool setProgramName=true);
// End: Scheduled to be moved to libkexiv2

private:

    bool setProgramId(bool on=true) const;
    bool setIptcTag(const QString& text, int maxLength, const char* debugLabel, const char* tagKey) const;
    QVariant fromExifOrXmp(const char *exifTagName, const char *xmpTagName) const;
    QVariant fromIptcOrXmp(const char *iptcTagName, const char *xmpTagName) const;
    QVariant fromXmpList(const char *xmpTagName) const;
    QVariant fromIptcEmulateList(const char *iptcTagName) const;
    QVariant fromXmpLangAlt(const char *xmpTagName) const;
    QVariant fromIptcEmulateLangAlt(const char *iptcTagName) const;
};

}  // namespace Digikam

#endif /* DMETADATA_H */
