/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-09-12
 * Description : Schema update
 * 
 * Copyright (C) 2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef METADATAINFO_H
#define METADATAINFO_H

namespace Digikam
{

namespace MetadataInfo
{

enum Field
{
    Comment,                          /// String (one of the following three values)
    CommentJfif,                      /// String
    CommentExif,                      /// String
    CommentIptc,                      /// String (see also IptcCoreDescription)
    Description,                      /// Map language -> String
    Title,                            /// Map language -> String
    Headline,                         /// String
    DescriptionWriter,                /// String

    Keywords,                         /// StringList

    Rating,                           /// Int
    CreationDate,                     /// DateTime
    DigitizationDate,                 /// DateTime
    Orientation,                      /// Int, enum from libkexiv2

    Make,                             /// String
    Model,                            /// String
    Lens,                             /// String
    Aperture,                         /// Double, FNumber
    FocalLength,                      /// Double, mm
    FocalLengthIn35mm,                /// Double, mm
    ExposureTime,                     /// Double, s
    ExposureProgram,                  /// Int, enum from Exif
    ExposureMode,                     /// Int, enum from Exif
    Sensitivity,                      /// Int, ISO sensitivity
    FlashMode,                        /// Int, bit mask from Exif
    WhiteBalance,                     /// Int, enum from Exif
    WhiteBalanceColorTemperature,     /// double, color temperature in K
    MeteringMode,                     /// Int, enum from Exif
    SubjectDistance,                  /// double, m
    SubjectDistanceCategory,          /// int, enum from Exif

    Latitude,                         /// String (as XMP GPSCoordinate)
    LatitudeNumber,                   /// double, degrees
    Longitude,                        /// String (as XMP GPSCoordinate)
    LongitudeNumber,                  /// double, degrees
    Altitude,                         /// double, m
    PositionOrientation,              /// double, ?
    PositionTilt,                     /// double, ?
    PositionRoll,                     /// double, ?
    PositionAccuracy,                 /// double, m
    PositionDescription,              /// String

    IptcCoreCopyrightNotice,          /// Map language -> String
    IptcCoreCreator,                  /// List of type String
    IptcCoreProvider,                 /// String
    IptcCoreRightsUsageTerms,          /// Map language -> String
    IptcCoreSource,                   /// String

    IptcCoreCreatorJobTitle,          /// String
    IptcCoreInstructions,             /// String

    IptcCoreCountryCode,              /// String
    IptcCoreCountry,                  /// String
    IptcCoreCity,                     /// String
    IptcCoreLocation,                 /// String
    IptcCoreProvinceState,            /// String
    IptcCoreIntellectualGenre,        /// String
    IptcCoreJobID,                    /// String
    IptcCoreScene,                    /// List of type String
    IptcCoreSubjectCode               /// List of type String

    // Description, DescriptionWriter, Headline, Title: see above
    // DateCreated: see above, CreationDate
    // Keywords: see above, Keywords
    // not supported: CreatorContactInfo

    // Dublin Core: Description, Title, Subject (keywords) see above
};

} // namespace MetadataInfo

typedef QList<MetadataInfo::Field> MetadataFields;

} // namespace Digikam

#endif // METADATAINFO_H
