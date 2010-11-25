/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-07-20
 * Description : Loader for thumbnails
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2003-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cstring>

// Qt includes.

#include <QImage>
#include <QDir>
#include <QCryptographicHash>


// C ANSI includes.

extern "C"
{
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#endif
#include <sys/time.h>
#ifndef ST_QIMAGELOADERONLY
	#include <png.h>
#endif 
}

// Local includes.

#include "thumbnailcreator.h"
#include "thumbnailcreator_p.h"

// Definitions

#define PNG_BYTES_TO_CHECK 4

namespace Digikam
{

// --- Static methods: Generate the thumbnail path according to FreeDesktop spec ---

	QString ThumbnailCreator::normalThumbnailDir()
	{
		return  QDir::homePath() + "/.thumbnails/normal/";
	}

	QString ThumbnailCreator::largeThumbnailDir()
	{
		return  QDir::homePath() + "/.thumbnails/large/";
	}

	QString ThumbnailCreator::thumbnailPath(const QString &filePath, const QString &basePath)
	{
		return thumbnailPathFromUri(thumbnailUri(filePath), basePath);
	}

	QString ThumbnailCreator::thumbnailUri(const QString &filePath)
	{
		return "file://" + QDir::cleanPath(filePath);
	}

	QString ThumbnailCreator::thumbnailPathFromUri(const QString &uri, const QString &basePath)
	{
//     KMD5 md5( QFile::encodeName(uri) );
//     return basePath + QFile::encodeName( md5.hexDigest() ) + ".png";

		return basePath + QFile::encodeName(QCryptographicHash::hash(uri.toAscii(), QCryptographicHash::Md5)) + ".png";
	}

// --- non-static methods ---

	void ThumbnailCreator::initThumbnailDirs()
	{
		d->smallThumbPath = normalThumbnailDir();
		d->bigThumbPath   = largeThumbnailDir();

		QDir STDir(d->smallThumbPath);
		STDir.mkdir(d->smallThumbPath);
		QFile::setPermissions(d->smallThumbPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
		QDir BTDir(d->bigThumbPath);
		BTDir.mkdir(d->bigThumbPath);
		QFile::setPermissions(d->bigThumbPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
	}

	QString ThumbnailCreator::thumbnailPath(const QString &filePath)
	{
		QString basePath = (d->cachedSize == 128) ? d->smallThumbPath : d->bigThumbPath;
		return thumbnailPath(filePath, basePath);
	}

// --- Basic PNG loading ---

	QImage ThumbnailCreator::loadPNG(const QString& path)
	{
#ifndef ST_QIMAGELOADERONLY
		png_uint_32  w32, h32;
		int          w, h;
		bool         has_alpha;
		bool         has_grey;
		FILE        *f;
		png_structp  png_ptr = NULL;
		png_infop    info_ptr = NULL;
		int          bit_depth, color_type, interlace_type;

		has_alpha = 0;
		has_grey  = 0;

		QImage qimage;

		f = fopen(path.toLatin1(), "rb");
		if (!f)
			return qimage;

		unsigned char buf[PNG_BYTES_TO_CHECK];

		fread(buf, 1, PNG_BYTES_TO_CHECK, f);
		if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
		{
			fclose(f);
			return qimage;
		}
		rewind(f);

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		{
			fclose(f);
			return qimage;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			fclose(f);
			return qimage;
		}

		if (setjmp(png_ptr->jmpbuf))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(f);
			return qimage;
		}

		png_init_io(png_ptr, f);
		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)(&w32),
		             (png_uint_32 *)(&h32), &bit_depth, &color_type,
		             &interlace_type, NULL, NULL);

		w = w32;
		h = h32;

		qimage = QImage(w, h, QImage::Format_ARGB32);

		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);

		if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
			has_alpha = 1;

		if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			has_alpha = 1;
			has_grey = 1;
		}

		if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
			has_grey = 1;

		unsigned char **lines;
		int             i;

		if (has_alpha)
			png_set_expand(png_ptr);

		if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
		{
			png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
			png_set_bgr(png_ptr);
		}
		else                                                         // PPC
		{
			png_set_swap_alpha(png_ptr);
			png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
		}

		/* 16bit color -> 8bit color */
		if (bit_depth == 16)
			png_set_strip_16(png_ptr);

		/* pack all pixels to byte boundaries */

		png_set_packing(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);

		lines = (unsigned char **)malloc(h * sizeof(unsigned char *));
		if (!lines)
		{
			png_read_end(png_ptr, info_ptr);
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			fclose(f);
			return qimage;
		}

		if (has_grey)
		{
			png_set_gray_to_rgb(png_ptr);
			if (png_get_bit_depth(png_ptr, info_ptr) < 8)
				png_set_gray_1_2_4_to_8(png_ptr);
		}

		int sizeOfUint = sizeof(unsigned int);
		for (i = 0 ; i < h ; i++)
			lines[i] = ((unsigned char *)(qimage.bits())) + (i * w * sizeOfUint);

		png_read_image(png_ptr, lines);
		free(lines);

		png_textp text_ptr;
		int num_text = 0;
		png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);
		while (num_text--)
		{
			qimage.setText(text_ptr->key, 0, text_ptr->text);
			text_ptr++;
		}

		png_read_end(png_ptr, info_ptr);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(f);

		return qimage;
#else 
		return QImage();
#endif //#ifndef ST_QIMAGELOADERONLY
	}
}  // namespace Digikam
