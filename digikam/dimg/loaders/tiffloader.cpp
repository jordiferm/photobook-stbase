/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-06-17
 * Description : A TIFF IO file for DImg framework
 *
 * Copyright (C) 2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Specifications & references:
 * - TIFF 6.0  : http://partners.adobe.com/public/developer/en/tiff/TIFF6.pdf
 * - TIFF/EP   : http://www.map.tu.chiba-u.ac.jp/IEC/100/TA2/recdoc/N4378.pdf
 * - TIFF/Tags : http://www.awaresystems.be/imaging/tiff/tifftags.html
 * - DNG       : http://www.adobe.com/products/dng/pdfs/dng_spec.pdf
 *
 * Others Linux Tiff Loader implementation using libtiff:
 * - http://websvn.kde.org/trunk/koffice/filters/krita/tiff/kis_tiff_converter.cc
 * - http://artis.inrialpes.fr/Software/TiffIO/
 * - http://cvs.graphicsmagick.org/cgi-bin/cvsweb.cgi/GraphicsMagick/coders/tiff.c
 * - http://freeimage.cvs.sourceforge.net/freeimage/FreeImage/Source/FreeImage/PluginTIFF.cpp
 * - http://freeimage.cvs.sourceforge.net/freeimage/FreeImage/Source/Metadata/XTIFF.cpp
 * - https://subversion.imagemagick.org/subversion/ImageMagick/trunk/coders/tiff.c
 *
 * Test images repository:
 * - http://www.remotesensing.org/libtiff/images.html
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

// This line must be commented to prevent any latency time
// when we use threaded image loader interface for each image
// files io. Uncomment this line only for debugging.
//#define ENABLE_DEBUG_MESSAGES


// C ANSI includes.
extern "C"
{
#include <tiffvers.h>
}

// C++ includes.

#include <cstdio>

// Qt includes.

#include <QFile>
#include <QByteArray>
#include <QtDebug> 


// Local includes.

#include "version.h"
#include "dimg.h"
#include "dimgloaderobserver.h"
#include "dmetadata.h"
#include "tiffloader.h"

namespace Digikam
{

// To manage Errors/Warnings handling provide by libtiff

	void TIFFLoader::dimg_tiff_warning(const char* module, const char* format, va_list warnings)
	{
#ifdef ENABLE_DEBUG_MESSAGES
		char message[4096];
		vsnprintf(message, 4096, format, warnings);
		//kDebug(50003) << module <<  "::" <<  message << endl;
#else
		Q_UNUSED(module);
		Q_UNUSED(format);
		Q_UNUSED(warnings);
#endif
	}

	void TIFFLoader::dimg_tiff_error(const char* module, const char* format, va_list errors)
	{
#ifdef ENABLE_DEBUG_MESSAGES
		char message[4096];
		vsnprintf(message, 4096, format, errors);
		//kDebug(50003) << module << "::" << message << endl;
#else
		Q_UNUSED(module);
		Q_UNUSED(format);
		Q_UNUSED(errors);
#endif
	}

	TIFFLoader::TIFFLoader(DImg* image)
			: DImgLoader(image)
	{
		m_hasAlpha   = false;
		m_sixteenBit = false;
	}

	bool TIFFLoader::load(const QString& filePath, DImgLoaderObserver *observer)
	{
		readMetadata(filePath, DImg::TIFF);

		// -------------------------------------------------------------------
		// TIFF error handling. If an errors/warnings occurs during reading,
		// libtiff will call these methods

		TIFFSetWarningHandler(dimg_tiff_warning);
		TIFFSetErrorHandler(dimg_tiff_error);

		// -------------------------------------------------------------------
		// Open the file

		TIFF* tif = TIFFOpen(QFile::encodeName(filePath), "r");
		if (!tif)
		{
			//kDebug(50003) << "Cannot open image file." << endl;
			return false;
		}

#ifdef ENABLE_DEBUG_MESSAGES
		TIFFPrintDirectory(tif, stdout, 0);
#endif

		// -------------------------------------------------------------------
		// Get image information.

		uint32    w, h;
		uint16    bits_per_sample;
		uint16    samples_per_pixel;
		uint16    photometric;
		uint32    rows_per_strip;
		tsize_t   strip_size;
		tstrip_t  num_of_strips;

		TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGELENGTH, &h);

		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
		TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);

		if (TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip) == 0
		        || rows_per_strip == 0 || rows_per_strip == (unsigned int) - 1)
		{
			qWarning(QString("TIFF loader: Cannot handle non-stripped images. Loading file %1").arg(filePath).toLatin1());
			TIFFClose(tif);
			return false;
		}

		if (bits_per_sample == 0 ||
		        samples_per_pixel == 0 ||
		        rows_per_strip == 0 ||
		        rows_per_strip > h)
		{
        qWarning() << "TIFF loader: Encountered invalid value 0 in image."
                        << " bits_per_sample " << bits_per_sample
                        << " samples_per_pixel " << samples_per_pixel
                        << " rows_per_strip " << rows_per_strip
                        << " Loading file " << filePath << endl;
			TIFFClose(tif);
			return false;
		}

		// TODO: check others TIFF color-spaces here. Actually, only RGB and MINISBLACK
		// have been tested.
		// Complete description of TIFFTAG_PHOTOMETRIC tag can be found at this url:
		// http://www.awaresystems.be/imaging/tiff/tifftags/photometricinterpretation.html

		TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
		if (photometric != PHOTOMETRIC_RGB &&
		        photometric != PHOTOMETRIC_MINISBLACK &&
		        m_loadFlags & LoadImageData)
		{
			qWarning() << "Can not handle image without RGB color-space: "
			<< photometric;
			TIFFClose(tif);
			return false;
		}

		int colorModel = DImg::COLORMODELUNKNOWN;
		switch (photometric)
		{
			case PHOTOMETRIC_MINISWHITE:
			case PHOTOMETRIC_MINISBLACK:
				colorModel = DImg::GRAYSCALE;
				break;
			case PHOTOMETRIC_RGB:
				colorModel = DImg::RGB;
				break;
			case PHOTOMETRIC_PALETTE:
				colorModel = DImg::INDEXED;
				break;
			case PHOTOMETRIC_MASK:
				colorModel = DImg::MONOCHROME;
				break;
			case PHOTOMETRIC_SEPARATED:
				colorModel = DImg::CMYK;
				break;
			case PHOTOMETRIC_YCBCR:
				colorModel = DImg::YCBCR;
				break;
			case PHOTOMETRIC_CIELAB:
			case PHOTOMETRIC_ICCLAB:
			case PHOTOMETRIC_ITULAB:
				colorModel = DImg::CIELAB;
				break;
			case PHOTOMETRIC_LOGL:
			case PHOTOMETRIC_LOGLUV:
				colorModel = DImg::COLORMODELRAW;
				break;
		}

		if (samples_per_pixel == 4)
			m_hasAlpha = true;
		else
			m_hasAlpha = false;

		if (bits_per_sample == 16)
			m_sixteenBit = true;
		else
			m_sixteenBit = false;

		// -------------------------------------------------------------------
		// Read image ICC profile

		if (m_loadFlags & LoadICCData)
		{
			QMap<int, QByteArray>& metaData = imageMetaData();

			uchar  *profile_data = 0;
			uint32  profile_size;

			if (TIFFGetField(tif, TIFFTAG_ICCPROFILE, &profile_size, &profile_data))
			{
				QByteArray profile_rawdata;
				profile_rawdata.resize(profile_size);
				memcpy(profile_rawdata.data(), profile_data, profile_size);
				metaData.insert(DImg::ICC, profile_rawdata);
			}
			else
			{
				// If ICC profile is null, check Exif metadata.
				checkExifWorkingColorSpace();
			}
		}

		// -------------------------------------------------------------------
		// Get image data.

		uchar* data   = 0;

		if (m_loadFlags & LoadImageData)
		{
			if (observer)
				observer->progressInfo(m_image, 0.1);

			strip_size    = TIFFStripSize(tif);
			num_of_strips = TIFFNumberOfStrips(tif);

			if (bits_per_sample == 16)          // 16 bits image.
			{
				data           = new uchar[w*h*8];
				uchar* strip   = new uchar[strip_size];
				long offset    = 0;
				long bytesRead = 0;

				uint checkpoint = 0;

				for (tstrip_t st = 0; st < num_of_strips; st++)
				{
					if (observer && st == checkpoint)
					{
						checkpoint += granularity(observer, num_of_strips, 0.8);
						if (!observer->continueQuery(m_image))
						{
							delete [] data;
							delete [] strip;
							TIFFClose(tif);
							return false;
						}
						observer->progressInfo(m_image, 0.1 + (0.8 * (((float)st) / ((float)num_of_strips))));
					}

					bytesRead = TIFFReadEncodedStrip(tif, st, strip, strip_size);

					if (bytesRead == -1)
					{
						//kDebug(50003) << "Failed to read strip" << endl;
						delete [] data;
						TIFFClose(tif);
						return false;
					}

					ushort *stripPtr = (ushort*)(strip);
					ushort *dataPtr  = (ushort*)(data + offset);
					ushort *p;

					// tiff data is read as BGR or ABGR or Greyscale

					if (samples_per_pixel == 3)
					{
						for (int i = 0; i < bytesRead / 6; i++)
						{
							p = dataPtr;

							// See B.K.O #148037 : take a care about byte order with Motorola computers.
							if (QSysInfo::ByteOrder == QSysInfo::BigEndian)     // PPC
							{
								p[3] = *stripPtr++;
								p[0] = *stripPtr++;
								p[1] = *stripPtr++;
								p[2] = 0xFFFF;
							}
							else
							{
								p[2] = *stripPtr++;
								p[1] = *stripPtr++;
								p[0] = *stripPtr++;
								p[3] = 0xFFFF;
							}

							dataPtr += 4;
						}

						offset += bytesRead / 6 * 8;
					}
					else if (samples_per_pixel == 1)   // See B.K.O #148400: Greyscale pictures only have _one_ sample per pixel
					{
						for (int i = 0; i < bytesRead / 2; i++)
						{
							// We have to read two bytes for one pixel
							p = dataPtr;

							// See B.K.O #148037 : take a care about byte order with Motorola computers.
							if (QSysInfo::ByteOrder == QSysInfo::BigEndian)     // PPC
							{
								p[3] = 0xFFFF;
								p[0] = *stripPtr;
								p[1] = *stripPtr;
								p[2] = *stripPtr++;
							}
							else
							{
								p[0] = *stripPtr;      // RGB have to be set to the _same_ value
								p[1] = *stripPtr;
								p[2] = *stripPtr++;
								p[3] = 0xFFFF;         // set alpha to 100%
							}
							dataPtr += 4;
						}

						offset += bytesRead * 4;       // The _byte_offset in the data array is, of course, four times bytesRead
					}
					else
					{
						for (int i = 0; i < bytesRead / 8; i++)
						{
							p = dataPtr;

							// See B.K.O #148037 : take a care about byte order with Motorola computers.
							if (QSysInfo::ByteOrder == QSysInfo::BigEndian)     // PPC
							{
								p[3] = *stripPtr++;
								p[0] = *stripPtr++;
								p[1] = *stripPtr++;
								p[2] = *stripPtr++;
							}
							else
							{
								p[2] = *stripPtr++;
								p[1] = *stripPtr++;
								p[0] = *stripPtr++;
								p[3] = *stripPtr++;
							}
							dataPtr += 4;
						}

						offset += bytesRead;
					}
				}

				delete [] strip;
			}
			else       // Non 16 bits images ==> get it on BGRA 8 bits.
			{
				data            = new uchar[w*h*4];
				uchar* strip    = new uchar[w*rows_per_strip*4];
				long offset     = 0;
				long pixelsRead = 0;

				// this is inspired by TIFFReadRGBAStrip, tif_getimage.c
				char          emsg[1024] = "";
				TIFFRGBAImage img;
				uint32        rows_to_read;

				uint checkpoint = 0;

				// test whether libtiff can read format and initiate reading

				if (!TIFFRGBAImageOK(tif, emsg) || !TIFFRGBAImageBegin(&img, tif, 0, emsg))
				{
					qDebug() << "Failed to set up RGBA reading of image, filename "
					<< TIFFFileName(tif) <<  " error message from Libtiff: " << emsg << endl;
					delete [] data;
					delete [] strip;
					TIFFClose(tif);
					return false;
				}

				img.req_orientation = ORIENTATION_TOPLEFT;

				// read strips from image: read rows_per_strip, so always start at beginning of a strip
				for (uint row = 0; row < h; row += rows_per_strip)
				{
					if (observer && row >= checkpoint)
					{
						checkpoint += granularity(observer, h, 0.8);
						if (!observer->continueQuery(m_image))
						{
							delete [] data;
							delete [] strip;
							TIFFClose(tif);
							return false;
						}
						observer->progressInfo(m_image, 0.1 + (0.8 * (((float)row) / ((float)h))));
					}

					img.row_offset  = row;
					img.col_offset  = 0;

					if (row + rows_per_strip > img.height)
						rows_to_read = img.height - row;
					else
						rows_to_read = rows_per_strip;

					// Read data

					if (TIFFRGBAImageGet(&img, (uint32*)strip, img.width, rows_to_read) == -1)
					{
						//kDebug(50003) << "Failed to read image data" << endl;
						delete [] data;
						delete [] strip;
						TIFFClose(tif);
						return false;
					}

					pixelsRead = rows_to_read * img.width;

					uchar *stripPtr = (uchar*)(strip);
					uchar *dataPtr  = (uchar*)(data + offset);
					uchar *p;

					// Reverse red and blue

					for (int i = 0; i < pixelsRead; i++)
					{
						p = dataPtr;

						// See B.K.O #148037 : take a care about byte order with Motorola computers.
						if (QSysInfo::ByteOrder == QSysInfo::BigEndian)     // PPC
						{
							p[3] = *stripPtr++;
							p[0] = *stripPtr++;
							p[1] = *stripPtr++;
							p[2] = *stripPtr++;
						}
						else
						{
							p[2] = *stripPtr++;
							p[1] = *stripPtr++;
							p[0] = *stripPtr++;
							p[3] = *stripPtr++;
						}

						dataPtr += 4;
					}

					offset += pixelsRead * 4;
				}

				TIFFRGBAImageEnd(&img);
				delete [] strip;
			}
		}

		// -------------------------------------------------------------------

		TIFFClose(tif);

		if (observer)
			observer->progressInfo(m_image, 1.0);

		imageWidth()  = w;
		imageHeight() = h;
		imageData()   = data;
		imageSetAttribute("format", "TIFF");
		imageSetAttribute("originalColorModel", colorModel);
		imageSetAttribute("originalBitDepth", bits_per_sample);

		return true;
	}

	bool TIFFLoader::save(const QString& filePath, DImgLoaderObserver *observer)
	{
		uint32 w     = imageWidth();
		uint32 h     = imageHeight();
		uchar  *data = imageData();

		// -------------------------------------------------------------------
		// TIFF error handling. If an errors/warnings occurs during reading,
		// libtiff will call these methods

		TIFFSetWarningHandler(dimg_tiff_warning);
		TIFFSetErrorHandler(dimg_tiff_error);

		// -------------------------------------------------------------------
		// Open the file

		TIFF *tif = TIFFOpen(QFile::encodeName(filePath), "w");
		if (!tif)
		{
			//kDebug(50003) << "Cannot open target image file." << endl;
			return false;
		}

		// -------------------------------------------------------------------
		// Set image properties

		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,     w);
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH,    h);
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,    PHOTOMETRIC_RGB);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG,   PLANARCONFIG_CONTIG);
		TIFFSetField(tif, TIFFTAG_ORIENTATION,    ORIENTATION_TOPLEFT);
		TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_NONE);

		// Image must be compressed using deflate algorithm ?
		QVariant compressAttr = imageGetAttribute("compress");
		bool compress = compressAttr.isValid() ? compressAttr.toBool() : false;

		if (compress)
		{
			TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
			TIFFSetField(tif, TIFFTAG_ZIPQUALITY,  9);
			// NOTE : this tag values aren't defined in libtiff 3.6.1. '2' is PREDICTOR_HORIZONTAL.
			//        Use horizontal differencing for images which are
			//        likely to be continuous tone. The TIFF spec says that this
			//        usually leads to better compression.
			//        See this url for more details:
			//        http://www.awaresystems.be/imaging/tiff/tifftags/predictor.html
			TIFFSetField(tif, TIFFTAG_PREDICTOR,   2);
		}
		else
			TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

		// Image has an alpha channel ?
		if (imageHasAlpha())
		{
			uint16 sampleinfo[1] = { EXTRASAMPLE_UNASSALPHA };
			TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
			TIFFSetField(tif, TIFFTAG_EXTRASAMPLES,    EXTRASAMPLE_ASSOCALPHA, sampleinfo);
		}
		else
		{
			TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
		}

		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16)imageBitsDepth());
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,  TIFFDefaultStripSize(tif, 0));

		// -------------------------------------------------------------------
		// Write meta-data Tags contents.

		DMetadata metaData;
		metaData.setExif(m_image->getExif());
		metaData.setIptc(m_image->getIptc());
		metaData.setXmp(m_image->getXmp());

		// Standard IPTC tag (available with libtiff 3.6.1)

		QByteArray ba = metaData.getIptc(true);
		if (!ba.isEmpty())
		{
#if defined(TIFFTAG_PHOTOSHOP)
			TIFFSetField(tif, TIFFTAG_PHOTOSHOP, (uint32)ba.size(), (uchar *)ba.data());
#endif
		}

		// Standard XMP tag (available with libtiff 3.6.1)

		if (metaData.hasXmp())
		{
#if defined(TIFFTAG_XMLPACKET)
			tiffSetExifDataTag(tif, TIFFTAG_XMLPACKET,            metaData, "Exif.Image.XMLPacket");
#endif
		}

		// Standard Exif ASCII tags (available with libtiff 3.6.1)

		tiffSetExifAsciiTag(tif, TIFFTAG_DOCUMENTNAME,            metaData, "Exif.Image.DocumentName");
		tiffSetExifAsciiTag(tif, TIFFTAG_IMAGEDESCRIPTION,        metaData, "Exif.Image.ImageDescription");
		tiffSetExifAsciiTag(tif, TIFFTAG_MAKE,                    metaData, "Exif.Image.Make");
		tiffSetExifAsciiTag(tif, TIFFTAG_MODEL,                   metaData, "Exif.Image.Model");
		tiffSetExifAsciiTag(tif, TIFFTAG_DATETIME,                metaData, "Exif.Image.DateTime");
		tiffSetExifAsciiTag(tif, TIFFTAG_ARTIST,                  metaData, "Exif.Image.Artist");
		tiffSetExifAsciiTag(tif, TIFFTAG_COPYRIGHT,               metaData, "Exif.Image.Copyright");

		QString soft = metaData.getExifTagString("Exif.Image.Software");
		QString libtiffver(TIFFLIB_VERSION_STR);
		libtiffver.replace('\n', ' ');
		soft.append(QString(" ( %1 )").arg(libtiffver));
		TIFFSetField(tif, TIFFTAG_SOFTWARE, (const char*)soft.toAscii().data());

		// NOTE: All others Exif tags will be written by Exiv2 (<= 0.18)

		// -------------------------------------------------------------------
		// Write ICC profile.

		QByteArray profile_rawdata = m_image->getICCProfil();

		if (!profile_rawdata.isEmpty())
		{
#if defined(TIFFTAG_ICCPROFILE)
			TIFFSetField(tif, TIFFTAG_ICCPROFILE, (uint32)profile_rawdata.size(), (uchar *)profile_rawdata.data());
#endif
		}

		// -------------------------------------------------------------------
		// Write full image data in tiff directory IFD0

		if (observer)
			observer->progressInfo(m_image, 0.1);

		uchar  *pixel;
		double  alpha_factor;
		uint32  x, y;
		uint8   r8, g8, b8, a8 = 0;
		uint16  r16, g16, b16, a16 = 0;
		int     i = 0;

		uint8 *buf = (uint8 *)_TIFFmalloc(TIFFScanlineSize(tif));
		if (!buf)
		{
			//kDebug(50003) << "Cannot allocate memory buffer for main image." << endl;
			TIFFClose(tif);
			return false;
		}

		uint checkpoint = 0;

		for (y = 0; y < h; y++)
		{

			if (observer && y == checkpoint)
			{
				checkpoint += granularity(observer, h, 0.8);
				if (!observer->continueQuery(m_image))
				{
					_TIFFfree(buf);
					TIFFClose(tif);
					return false;
				}
				observer->progressInfo(m_image, 0.1 + (0.8 * (((float)y) / ((float)h))));
			}

			i = 0;

			for (x = 0; x < w; x++)
			{
				pixel = &data[((y * w) + x) * imageBytesDepth()];

				if (imageSixteenBit())          // 16 bits image.
				{
					b16 = (uint16)(pixel[0] + 256 * pixel[1]);
					g16 = (uint16)(pixel[2] + 256 * pixel[3]);
					r16 = (uint16)(pixel[4] + 256 * pixel[5]);

					if (imageHasAlpha())
					{
						// TIFF makes you pre-mutiply the RGB components by alpha

						a16          = (uint16)(pixel[6] + 256 * pixel[7]);
						alpha_factor = ((double)a16 / 65535.0);
						r16          = (uint16)(r16 * alpha_factor);
						g16          = (uint16)(g16 * alpha_factor);
						b16          = (uint16)(b16 * alpha_factor);
					}

					// This might be endian dependent

					buf[i++] = (uint8)(r16);
					buf[i++] = (uint8)(r16 >> 8);
					buf[i++] = (uint8)(g16);
					buf[i++] = (uint8)(g16 >> 8);
					buf[i++] = (uint8)(b16);
					buf[i++] = (uint8)(b16 >> 8);

					if (imageHasAlpha())
					{
						buf[i++] = (uint8)(a16) ;
						buf[i++] = (uint8)(a16 >> 8) ;
					}
				}
				else                            // 8 bits image.
				{
					b8 = (uint8)pixel[0];
					g8 = (uint8)pixel[1];
					r8 = (uint8)pixel[2];

					if (imageHasAlpha())
					{
						// TIFF makes you pre-mutiply the RGB components by alpha

						a8           = (uint8)(pixel[3]);
						alpha_factor = ((double)a8 / 255.0);
						r8           = (uint8)(r8 * alpha_factor);
						g8           = (uint8)(g8 * alpha_factor);
						b8           = (uint8)(b8 * alpha_factor);
					}

					// This might be endian dependent

					buf[i++] = r8;
					buf[i++] = g8;
					buf[i++] = b8;

					if (imageHasAlpha())
						buf[i++] = a8;
				}
			}

			if (!TIFFWriteScanline(tif, buf, y, 0))
			{
				//kDebug(50003) << "Cannot write main image to target file." << endl;
				_TIFFfree(buf);
				TIFFClose(tif);
				return false;
			}
		}

		_TIFFfree(buf);
		TIFFWriteDirectory(tif);

		// -------------------------------------------------------------------
		// Write thumbnail in tiff directory IFD1

		QImage thumb = m_image->smoothScale(160, 120, Qt::KeepAspectRatio).copyQImage();

		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32)thumb.width());
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32)thumb.height());
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_RGB);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
		TIFFSetField(tif, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
		TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT,  RESUNIT_NONE);
		TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_NONE);
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    TIFFDefaultStripSize(tif, 0));

		uchar *pixelThumb;
		uchar *dataThumb = thumb.bits();
		uint8 *bufThumb  = (uint8 *) _TIFFmalloc(TIFFScanlineSize(tif));

		if (!bufThumb)
		{
			//kDebug(50003) << "Cannot allocate memory buffer for thumbnail." << endl;
			TIFFClose(tif);
			return false;
		}

		for (y = 0 ; y < uint32(thumb.height()) ; y++)
		{
			i = 0;

			for (x = 0 ; x < uint32(thumb.width()) ; x++)
			{
				pixelThumb = &dataThumb[((y * thumb.width()) + x) * 4];

				// This might be endian dependent
				bufThumb[i++] = (uint8)pixelThumb[2];
				bufThumb[i++] = (uint8)pixelThumb[1];
				bufThumb[i++] = (uint8)pixelThumb[0];
			}

			if (!TIFFWriteScanline(tif, bufThumb, y, 0))
			{
				//kDebug(50003) << "Cannot write thumbnail to target file." << endl;
				_TIFFfree(bufThumb);
				TIFFClose(tif);
				return false;
			}
		}

		_TIFFfree(bufThumb);
		TIFFClose(tif);

		// -------------------------------------------------------------------

		if (observer)
			observer->progressInfo(m_image, 1.0);

		imageSetAttribute("savedformat", "TIFF");

		saveMetadata(filePath);

		return true;
	}

	bool TIFFLoader::hasAlpha() const
	{
		return m_hasAlpha;
	}

	bool TIFFLoader::sixteenBit() const
	{
		return m_sixteenBit;
	}

	void TIFFLoader::tiffSetExifAsciiTag(TIFF* tif, ttag_t tiffTag,
	                                     const DMetadata &metaData, const char* exifTagName)
	{
		QByteArray tag = metaData.getExifTagData(exifTagName);
		if (!tag.isEmpty())
		{
			QByteArray str(tag.data(), tag.size());
			TIFFSetField(tif, tiffTag, (const char*)str);
		}
	}

	void TIFFLoader::tiffSetExifDataTag(TIFF* tif, ttag_t tiffTag,
	                                    const DMetadata &metaData, const char* exifTagName)
	{
		QByteArray tag = metaData.getExifTagData(exifTagName);
		if (!tag.isEmpty())
		{
			TIFFSetField(tif, tiffTag, (uint32)tag.size(), (char *)tag.data());
		}
	}

}  // namespace Digikam
