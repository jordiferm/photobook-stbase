/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-02-03
 * Description : Loading parameters for multithreaded loading
 *
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#include "loadingdescription.h"

namespace Digikam
{

	bool LoadingDescription::PreviewParameters::operator==(const PreviewParameters &other) const
	{
		return type          == other.type
		       && size       == other.size
		       && exifRotate == other.exifRotate;
	}

	LoadingDescription::LoadingDescription(const QString &filePath)
			: filePath(filePath)
	{
		rawDecodingSettings = DRawDecoding();
	}

	LoadingDescription::LoadingDescription(const QString &filePath, DRawDecoding settings)
			: filePath(filePath), rawDecodingSettings(settings)
	{
	}

	LoadingDescription::LoadingDescription(const QString &filePath, int size, bool exifRotate,
	                                       LoadingDescription::PreviewParameters::PreviewType type)
			: filePath(filePath)
	{
		rawDecodingSettings          = DRawDecoding();
		previewParameters.type       = type;
		previewParameters.size       = size;
		previewParameters.exifRotate = exifRotate;
	}

	QString LoadingDescription::cacheKey() const
	{
		// Here we have the knowledge which LoadingDescriptions / RawFileDecodingSettings
		// must be cached separately.

		// Thumbnail loading. This one is easy.
		if (previewParameters.type == PreviewParameters::Thumbnail)
			return filePath + "-thumbnail-" + QString::number(previewParameters.size);

		// DImg loading
		// Current assumption:
		// Eight-bit images are needed for LightTable, and if 16-bit is enabled,
		// 16-bit half size images for the histogram sidebar,
		// and 16-bit full size images for the image editor.
		// Use previewParameters.size, not isPreview - if it is 0, full loading is used.

#ifdef ST_RAWSUPPORT
		QString suffix = rawDecodingSettings.sixteenBitsImage ? "-16" : "-8";
#else
		QString suffix = "-8";
#endif

#ifdef ST_RAWSUPPORT
		if (rawDecodingSettings.halfSizeColorImage)
			return filePath + suffix + "-halfSizeColorImage";
		else
#endif

		if (previewParameters.size)
			return filePath + suffix + "-previewImage" + QString::number(previewParameters.size);
		else
			return filePath + suffix;
	}

	QStringList LoadingDescription::lookupCacheKeys() const
	{
		// Build a hierarchy which cache entries may be used for this LoadingDescription.

		// Thumbnail loading. No other cache key included!
		if (previewParameters.type == PreviewParameters::Thumbnail)
			return QStringList() << cacheKey();

		// DImg loading.
		// Typically, the first is the best, but an actual loading operation may use a
		// lower-quality loading and will effectively only add the last entry of the
		// list to the cache, although it can accept the first if already available.
		// Sixteen-bit images cannot be used used instead of eight-bit ones because
		// color management is needed to display them.

#ifdef ST_RAWSUPPORT
		QString suffix = rawDecodingSettings.sixteenBitsImage ? "-16" : "-8";
#else 
		QString suffix = "-8";
#endif 

		QStringList keys;
		keys << filePath + suffix;
#ifdef ST_RAWSUPPORT
		if (rawDecodingSettings.halfSizeColorImage)
			keys << filePath + suffix + "-halfSizeColorImage";
#endif
		if (previewParameters.size)
			keys << filePath + suffix + "-previewImage";
		return keys;
	}

	bool LoadingDescription::isReducedVersion() const
	{
		// return true if this loads anything but the full version
#ifdef ST_RAWSUPPORT
		return rawDecodingSettings.halfSizeColorImage
		       || previewParameters.type != PreviewParameters::NoPreview;
#else 
		return previewParameters.type != PreviewParameters::NoPreview;
#endif 
	}

	bool LoadingDescription::operator==(const LoadingDescription &other) const
	{
#ifdef ST_RAWSUPPORT
		return filePath == other.filePath &&
		       rawDecodingSettings == other.rawDecodingSettings &&
		       previewParameters == other.previewParameters;
#else 
		return filePath == other.filePath &&
		       previewParameters == other.previewParameters;
#endif 
	}

	bool LoadingDescription::equalsIgnoreReducedVersion(const LoadingDescription &other) const
	{
		return filePath == other.filePath;
	}

	bool LoadingDescription::equalsOrBetterThan(const LoadingDescription &other) const
	{
		// This method is similar to operator==. But it returns true as well if other
		// Loads a "better" version than this.
		// Preview parameters must have the same size, or other has no size restriction.
		// All raw decoding settings must be equal, only the half size parameter is allowed to vary.

#ifdef ST_RAWSUPPORT
		DRawDecoding fullSize = other.rawDecodingSettings;
		fullSize.halfSizeColorImage = false;
		return filePath == other.filePath &&
		       (
		           rawDecodingSettings == other.rawDecodingSettings ||
		           rawDecodingSettings == fullSize
		       ) &&
		       (
		           (previewParameters.size == other.previewParameters.size) ||
		           other.previewParameters.size
		       );
#else 
		return filePath == other.filePath &&
		       (
		           (previewParameters.size == other.previewParameters.size) ||
		           other.previewParameters.size
		       );
#endif
	}

	bool LoadingDescription::isThumbnail() const
	{
		return previewParameters.type == PreviewParameters::Thumbnail;
	}

	bool LoadingDescription::isPreviewImage() const
	{
		return previewParameters.type == PreviewParameters::PreviewImage;
	}

	QStringList LoadingDescription::possibleCacheKeys(const QString &filePath)
	{
		QStringList keys;
		keys << filePath + "-16";
		keys << filePath + "-16-halfSizeColorImage";
		keys << filePath + "-16-previewImage";
		keys << filePath + "-8";
		keys << filePath + "-8-halfSizeColorImage";
		keys << filePath + "-8-previewImage";
		return keys;
	}

	QStringList LoadingDescription::possibleThumbnailCacheKeys(const QString &filePath)
	{
		QStringList keys;
		// there are 256 possible keys...
		QString path = filePath + "-thumbnail-";
		for (int i = 1; i <= 256; i++)
			keys << path + QString::number(i);
		return keys;
	}

} // namespace Digikam
