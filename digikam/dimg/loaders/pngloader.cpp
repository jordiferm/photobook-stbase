/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-11-01
 * Description : a PNG image loader for DImg framework.
 *
 * Copyright (C) 2005-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#define PNG_BYTES_TO_CHECK 4

#include "pngloader.h"

// C ANSI includes.

extern "C"
{
#include <unistd.h>
}

// C++ includes.

#include <cstdlib>
#include <cstdio>

// Qt includes.

#include <QFile>
#include <QByteArray>
#include <QSysInfo>
#include <QtDebug> 

#include <kexiv2.h>

// Local includes.

#include "version.h"
#include "dimg.h"
#include "dimgloaderobserver.h"

#ifdef Q_CC_MSVC
void _ReadProc(struct png_struct_def *png_ptr, unsigned char *data, unsigned int size)
{
    FILE* file_handle = (FILE*)png_get_io_ptr(png_ptr);
    fread(data, size, 1, file_handle);
}
#endif

namespace Digikam
{

PNGLoader::PNGLoader(DImg* image)
         : DImgLoader(image)
{
    m_hasAlpha   = false;
    m_sixteenBit = false;
}

bool PNGLoader::load(const QString& filePath, DImgLoaderObserver *observer)
{
    png_uint_32  w32, h32;
    int          width, height;
    FILE        *f;
    int          bit_depth, color_type, interlace_type;
    png_structp  png_ptr  = NULL;
    png_infop    info_ptr = NULL;
    int          colorModel = DImg::COLORMODELUNKNOWN;

    readMetadata(filePath, DImg::PNG);

    // -------------------------------------------------------------------
    // Open the file

    f = fopen(QFile::encodeName(filePath), "rb");
    if ( !f )
    {
        qDebug() << "Cannot open image file." << endl;
        return false;
    }

    unsigned char buf[PNG_BYTES_TO_CHECK];

    fread(buf, 1, PNG_BYTES_TO_CHECK, f);
    if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
    {
        qDebug() << "Not a PNG image file." << endl;
        fclose(f);
        return false;
    }
    rewind(f);

    // -------------------------------------------------------------------
    // Initialize the internal structures

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        qDebug() << "Invalid PNG image file structure." << endl;
        fclose(f);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        qDebug() << "Cannot reading PNG image file structure." << endl;
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(f);
        return false;
    }

    // -------------------------------------------------------------------
    // PNG error handling. If an error occurs during reading, libpng
    // will jump here

    // setjmp-save cleanup
    class CleanupData
    {
    public:
        CleanupData()
        {
            data  = 0;
            lines = 0;
            f     = 0;
        }
        ~CleanupData()
        {
            delete [] data;
            freeLines();
            if (f) fclose(f);
        }
        void setData(uchar *d)   { data = d; }
        void setLines(uchar **l) { lines = l; }
        void setFile(FILE *file) { f = file; }
        void takeData()  { data = 0; }
        void freeLines() { if (lines) free(lines); lines = 0; }
        uchar *data;
        uchar **lines;
        FILE  *f;
    };
    CleanupData *cleanupData = new CleanupData;
    cleanupData->setFile(f);

    if (setjmp(png_ptr->jmpbuf))
    {
        qDebug() << "Internal libPNG error during reading file. Process aborted!";
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        delete cleanupData;
        return false;
    }

#ifdef Q_CC_MSVC
    png_set_read_fn(png_ptr, f, _ReadProc);
#else
    png_init_io(png_ptr, f);
#endif

    // -------------------------------------------------------------------
    // Read all PNG info up to image data

    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
                 (png_uint_32 *) (&h32), &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    width  = (int)w32;
    height = (int)h32;

    m_sixteenBit = (bit_depth == 16);

    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGB:            // RGB
            m_hasAlpha = false;
            colorModel = DImg::RGB;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:     // RGBA
            m_hasAlpha = true;
            colorModel = DImg::RGB;
            break;
        case PNG_COLOR_TYPE_GRAY:          // Grayscale
            m_hasAlpha = false;
            colorModel = DImg::GRAYSCALE;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:    // Grayscale + Alpha
            m_hasAlpha = true;
            colorModel = DImg::GRAYSCALE;
            break;
        case PNG_COLOR_TYPE_PALETTE:       // Indexed
            m_hasAlpha = false;
            colorModel = DImg::INDEXED;
            break;
    }

    uchar *data  = 0;

    if (m_loadFlags & LoadImageData)
    {
        // TODO: Endianness:
        // You may notice that the code for little and big endian
        // below is now identical. This was found to work by PPC users.
        // If this proves right, all the conditional clauses can be removed.

        if (bit_depth == 16)
        {
    #ifdef ENABLE_DEBUG_MESSAGES
            qDebug() << "PNG in 16 bits/color/pixel." << endl;
    #endif
            switch (color_type)
            {
                case PNG_COLOR_TYPE_RGB :            // RGB
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_RGB" << endl;
    #endif
                    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
                        png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                    else                                                         // PPC
                        png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);

                    break;

                case PNG_COLOR_TYPE_RGB_ALPHA :     // RGBA
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_RGB_ALPHA" << endl;
    #endif
                    break;

                case PNG_COLOR_TYPE_GRAY :          // Grayscale
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_GRAY" << endl;
    #endif
                    png_set_gray_to_rgb(png_ptr);

                    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
                        png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                    else                                                         // PPC
                        png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);

                    break;

                case PNG_COLOR_TYPE_GRAY_ALPHA :	// Grayscale + Alpha
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_GRAY_ALPHA" << endl;
    #endif
                    png_set_gray_to_rgb(png_ptr);

                    break;

                case PNG_COLOR_TYPE_PALETTE :       // Indexed
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_PALETTE" << endl;
    #endif
                    png_set_palette_to_rgb(png_ptr);

                    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
                        png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);
                    else                                                         // PPC
                        png_set_add_alpha(png_ptr, 0xFFFF, PNG_FILLER_AFTER);

                    break;

                default:
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG color type unknown." << endl;
    #endif
                    delete cleanupData;
                    return false;
            }
        }
        else
        {
    #ifdef ENABLE_DEBUG_MESSAGES
            qDebug() << "PNG in >=8 bits/color/pixel." << endl;
    #endif
            png_set_packing(png_ptr);

            switch (color_type)
            {
                case PNG_COLOR_TYPE_RGB :           // RGB
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_RGB" << endl;
    #endif
                    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
                        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                    else                                                         // PPC
                        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

                    break;

                case PNG_COLOR_TYPE_RGB_ALPHA :     // RGBA
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_RGB_ALPHA" << endl;
    #endif
                    break;

                case PNG_COLOR_TYPE_GRAY :          // Grayscale
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_GRAY" << endl;
    #endif
                    png_set_gray_1_2_4_to_8(png_ptr);
                    png_set_gray_to_rgb(png_ptr);

                    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
                        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                    else                                                         // PPC
                        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

                    break;

                case PNG_COLOR_TYPE_GRAY_ALPHA :    // Grayscale + alpha
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_GRAY_ALPHA" << endl;
    #endif
                    png_set_gray_to_rgb(png_ptr);
                    break;

                case PNG_COLOR_TYPE_PALETTE :       // Indexed
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG in PNG_COLOR_TYPE_PALETTE" << endl;
    #endif
                    png_set_packing(png_ptr);
                    png_set_palette_to_rgb(png_ptr);

                    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)           // Intel
                        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
                    else                                                         // PPC
                        png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

                    break;

                default:
    #ifdef ENABLE_DEBUG_MESSAGES
                    qDebug() << "PNG color type unknown." << endl;
    #endif
                    delete cleanupData;
                    return false;
            }
        }

        if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(png_ptr);

        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)      // Intel
            png_set_bgr(png_ptr);
        else                                                    // PPC
            png_set_bgr(png_ptr);
            //png_set_swap_alpha(png_ptr);

        if (observer)
            observer->progressInfo(m_image, 0.1);

        // -------------------------------------------------------------------
        // Get image data.

        // Call before png_read_update_info and png_start_read_image()
        // for non-interlaced images number_passes will be 1
        int number_passes = png_set_interlace_handling(png_ptr);

        png_read_update_info(png_ptr, info_ptr);

        if (m_sixteenBit)
            data = new uchar[width*height*8];  // 16 bits/color/pixel
        else
            data = new uchar[width*height*4];  // 8 bits/color/pixel
        cleanupData->setData(data);

        uchar **lines = 0;
        lines = (uchar **)malloc(height * sizeof(uchar *));
        cleanupData->setLines(lines);
        if (!lines)
        {
            qDebug() << "Cannot allocate memory to load PNG image data." << endl;
            png_read_end(png_ptr, info_ptr);
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
            delete cleanupData;
            return false;
        }

        for (int i = 0; i < height; i++)
        {
            if (m_sixteenBit)
                lines[i] = data + (i * width * 8);
            else
                lines[i] = data + (i * width * 4);
        }

        // The easy way to read the whole image
        // png_read_image(png_ptr, lines);
        // The other way to read images is row by row. Necessary for observer.
        // Now we need to deal with interlacing.

        for (int pass = 0; pass < number_passes; pass++)
        {
            int y;
            int checkPoint = 0;
            for (y = 0; y < height; y++)
            {
                if (observer && y == checkPoint)
                {
                    checkPoint += granularity(observer, height, 0.7);
                    if (!observer->continueQuery(m_image))
                    {
                        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
                        delete cleanupData;
                        return false;
                    }
                    // use 10% - 80% for progress while reading rows
                    observer->progressInfo(m_image, 0.1 + (0.7 * ( ((float)y)/((float)height) )) );
                }

                png_read_rows(png_ptr, lines+y, NULL, 1);
            }
        }

        cleanupData->freeLines();

        // Swap bytes in 16 bits/color/pixel for DImg

        if (m_sixteenBit)
        {
            uchar ptr[8];   // One pixel to swap

            for (int p = 0; p < width*height*8; p+=8)
            {
                memcpy (&ptr[0], &data[p], 8);  // Current pixel

                data[ p ] = ptr[1];  // Blue
                data[p+1] = ptr[0];
                data[p+2] = ptr[3];  // Green
                data[p+3] = ptr[2];
                data[p+4] = ptr[5];  // Red
                data[p+5] = ptr[4];
                data[p+6] = ptr[7];  // Alpha
                data[p+7] = ptr[6];
            }
        }
    }

    if (observer)
        observer->progressInfo(m_image, 0.9);

    // -------------------------------------------------------------------
    // Read image ICC profile

    if (m_loadFlags & LoadICCData)
    {
        QMap<int, QByteArray>& metaData = imageMetaData();

        png_charp   profile_name, profile_data=NULL;
        png_uint_32 profile_size;
        int         compression_type;

        png_get_iCCP(png_ptr, info_ptr, &profile_name, &compression_type, &profile_data, &profile_size);

        if (profile_data != NULL)
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
    // Get embedded text data.

    png_text* text_ptr;
    int num_comments = png_get_text(png_ptr, info_ptr, &text_ptr, NULL);

    /*
    Standard Embedded text includes in PNG :

    Title            Short (one line) title or caption for image
    Author           Name of image's creator
    Description      Description of image (possibly long)
    Copyright        Copyright notice
    Creation Time    Time of original image creation
    Software         Software used to create the image
    Disclaimer       Legal disclaimer
    Warning          Warning of nature of content
    Source           Device used to create the image
    Comment          Miscellaneous comment; conversion from GIF comment

    Extra Raw profiles tag are used by ImageMagick and defines at this URL :
    http://search.cpan.org/src/EXIFTOOL/Image-ExifTool-5.87/html/TagNames/PNG.html#TextualData
    */

    if (m_loadFlags & LoadICCData)
    {
        for (int i = 0; i < num_comments; i++)
        {
            // Check if we have a Raw profile embedded using ImageMagick technique.

            if (memcmp(text_ptr[i].key, "Raw profile type exif", 21) != 0 ||
                memcmp(text_ptr[i].key, "Raw profile type APP1", 21) != 0 ||
                memcmp(text_ptr[i].key, "Raw profile type iptc", 21) != 0)
            {
                imageSetEmbbededText(text_ptr[i].key, text_ptr[i].text);

    #ifdef ENABLE_DEBUG_MESSAGES
                qDebug() << "Reading PNG Embedded text: key=" << text_ptr[i].key
                        << " text=" << text_ptr[i].text << endl;
    #endif
            }
        }
    }

    // -------------------------------------------------------------------

    if (m_loadFlags & LoadImageData)
        png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
    cleanupData->takeData();
    delete cleanupData;

    if (observer)
        observer->progressInfo(m_image, 1.0);

    imageWidth()  = width;
    imageHeight() = height;
    imageData()   = data;
    imageSetAttribute("format", "PNG");
    imageSetAttribute("originalColorModel", colorModel);
    imageSetAttribute("originalBitDepth", bit_depth);

    return true;
}

bool PNGLoader::save(const QString& filePath, DImgLoaderObserver *observer)
{
    FILE          *f;
    png_structp    png_ptr;
    png_infop      info_ptr;
    uchar         *ptr, *data = 0;
    uint           x, y, j;
    png_bytep      row_ptr;
    png_color_8    sig_bit;
    int            quality     = 75;
    int            compression = 3;

    // -------------------------------------------------------------------
    // Open the file

    f = fopen(QFile::encodeName(filePath), "wb");
    if ( !f )
    {
        qDebug() << "Cannot open target image file." << endl;
        return false;
    }

    // -------------------------------------------------------------------
    // Initialize the internal structures

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        qDebug() << "Invalid target PNG image file structure." << endl;
        fclose(f);
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        qDebug() << "Cannot create PNG image file structure." << endl;
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        fclose(f);
        return false;
    }

    // -------------------------------------------------------------------
    // PNG error handling. If an error occurs during writing, libpng
    // will jump here

    // setjmp-save cleanup
    class CleanupData
    {
    public:
        CleanupData()
        {
            data  = 0;
            f     = 0;
        }
        ~CleanupData()
        {
            delete [] data;
            if (f) fclose(f);
        }
        void setData(uchar *d)   { data = d; }
        void setFile(FILE *file) { f = file; }

        uchar *data;
        FILE  *f;
    };
    CleanupData *cleanupData = new CleanupData;
    cleanupData->setFile(f);

    if (setjmp(png_ptr->jmpbuf))
    {
        qDebug() << "Internal libPNG error during writing file. Process aborted!" << endl;
        png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
        png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
        delete cleanupData;
        return false;
    }

    png_init_io(png_ptr, f);

    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)      // Intel
        png_set_bgr(png_ptr);
    else                                                    // PPC
        png_set_swap_alpha(png_ptr);

    if (imageHasAlpha())
    {
        png_set_IHDR(png_ptr, info_ptr, imageWidth(), imageHeight(), imageBitsDepth(),
                     PNG_COLOR_TYPE_RGB_ALPHA,  PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        if (imageSixteenBit())
            data = new uchar[imageWidth() * 8 * sizeof(uchar)];
        else
            data = new uchar[imageWidth() * 4 * sizeof(uchar)];
    }
    else
    {
        png_set_IHDR(png_ptr, info_ptr, imageWidth(), imageHeight(), imageBitsDepth(),
                     PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        if (imageSixteenBit())
            data = new uchar[imageWidth() * 6 * sizeof(uchar)];
        else
            data = new uchar[imageWidth() * 3 * sizeof(uchar)];
    }
    cleanupData->setData(data);

    sig_bit.red   = imageBitsDepth();
    sig_bit.green = imageBitsDepth();
    sig_bit.blue  = imageBitsDepth();
    sig_bit.alpha = imageBitsDepth();
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);

    // -------------------------------------------------------------------
    // Quality to convert to compression

    QVariant qualityAttr = imageGetAttribute("quality");
    quality = qualityAttr.isValid() ? qualityAttr.toInt() : 90;

    if (quality < 1)
        quality = 1;
    if (quality > 99)
        quality = 99;

    quality     = quality / 10;
    compression = 9 - quality;

    if (compression < 0)
        compression = 0;
    if (compression > 9)
        compression = 9;

    png_set_compression_level(png_ptr, compression);

    // -------------------------------------------------------------------
    // Write ICC profile.

    QByteArray profile_rawdata = m_image->getICCProfil();

    if (!profile_rawdata.isEmpty())
    {
        png_set_iCCP(png_ptr, info_ptr, (png_charp)("icc"), PNG_COMPRESSION_TYPE_BASE, profile_rawdata.data(), profile_rawdata.size());
    }

    // -------------------------------------------------------------------
    // Write embedded Text

    typedef QMap<QString, QString> EmbeddedTextMap;
    EmbeddedTextMap map = imageEmbeddedText();

    for (EmbeddedTextMap::iterator it = map.begin(); it != map.end(); ++it)
    {
        if (it.key() != QString("Software") && it.key() != QString("Comment"))
        {
            QByteArray key = it.key().toAscii();
            QByteArray value = it.value().toAscii();
            png_text text;
            text.key  = key.data();
            text.text = value.data();
#ifdef ENABLE_DEBUG_MESSAGES
            qDebug() << "Writing PNG Embedded text: key=" << text.key << " text=" << text.text << endl;
#endif
            text.compression = PNG_TEXT_COMPRESSION_zTXt;
            png_set_text(png_ptr, info_ptr, &(text), 1);
        }
    }

    // Update 'Software' text tag.
	//TODO: change it for the current software name.
    QString software("digiKam ");
    //software.append(digiKamVersion());
    QString libpngver(PNG_HEADER_VERSION_STRING);
    libpngver.replace('\n', ' ');
    software.append(QString(" (%1)").arg(libpngver));
    QByteArray softwareAsAscii = software.toAscii();
    png_text text;
    text.key  = (png_charp)("Software");
    text.text = softwareAsAscii.data();
#ifdef ENABLE_DEBUG_MESSAGES
    qDebug() << "Writing PNG Embedded text: key=" << text.key << " text=" << text.text << endl;
#endif
    text.compression = PNG_TEXT_COMPRESSION_zTXt;
    png_set_text(png_ptr, info_ptr, &(text), 1);

    // There is an unsolved problem in the following code block, see bug #151552.
    // If exiv2 supports writing to PNG, which it does since 0.18, this code is not needed
    // and should not be used therefore.
    if (KExiv2Iface::KExiv2::supportMetadataWritting("image/png"))
    {
        // Write embedded Raw profiles metadata (Exif/Iptc) in text tag using ImageMagick technique.
        // Write digiKam comment like an iTXt chunk using UTF8 encoding.
        // NOTE: iTXt will be enable by default with libpng >= 1.3.0.

        typedef QMap<int, QByteArray> MetaDataMap;
        MetaDataMap metaDataMap = imageMetaData();

        for (MetaDataMap::iterator it = metaDataMap.begin(); it != metaDataMap.end(); ++it)
        {
            QByteArray ba = it.value();

            switch (it.key())
            {

                #ifdef PNG_iTXt_SUPPORTED

                // TODO : this code is not yet tested. It require libpng 1.3.0.

                case(DImg::COM):
                {
                    png_text comment;
                    comment.key         = "Comment";
                    comment.text        = ba.data();
                    comment.itxt_length = ba.size();
                    comment.compression = PNG_ITXT_COMPRESSION_zTXt;
                    png_set_text(png_ptr, info_ptr, &(comment), 1);

                    qDebug() << "Writing digiKam comment into iTXt PNG chunk : " << ba << endl;
                    break;
                }
                #endif

                case(DImg::EXIF):
                {
                    const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
                    QByteArray profile;

                    // If bytes array do not start with ImageMagick header, Exif metadata have been created from
                    // scratch using Exiv2. In this case, we need to add Exif header from start.
                    if (memcmp(ba.data(), "exif",    4) != 0 &&
                        memcmp(ba.data(), "iptc",    4) != 0 &&
                        memcmp(ba.data(), "xmp",     3) != 0 &&
                        memcmp(ba.data(), "profile", 7) != 0)
                    {
                        profile = QByteArray();
                        profile.resize(ba.size() + sizeof(ExifHeader));
                        memcpy(profile.data(), ExifHeader, sizeof(ExifHeader));
                        memcpy(profile.data() + sizeof(ExifHeader), ba.data(), ba.size());
                    }
                    else
                    {
                        profile = ba;
                    }

                    writeRawProfile(png_ptr, info_ptr, (png_charp)("exif"), profile.data(), (png_uint_32) profile.size());
                    break;
                }
                case(DImg::IPTC):
                {
                    writeRawProfile(png_ptr, info_ptr, (png_charp)("iptc"), ba.data(), (png_uint_32) ba.size());
                    break;
                }
                case(DImg::XMP):
                {
                    writeRawProfile(png_ptr, info_ptr, (png_charp)("xmp"), ba.data(), (png_uint_32) ba.size());
                    break;
                }
                default:
                    break;
            }
        }
    }

    if (observer)
        observer->progressInfo(m_image, 0.2);

    // -------------------------------------------------------------------
    // Write image data

    png_write_info(png_ptr, info_ptr);
    png_set_shift(png_ptr, &sig_bit);
    png_set_packing(png_ptr);
    ptr = imageData();

    uint checkPoint = 0;
    for (y = 0; y < imageHeight(); y++)
    {

        if (observer && y == checkPoint)
        {
            checkPoint += granularity(observer, imageHeight(), 0.8);
            if (!observer->continueQuery(m_image))
            {
                png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
                png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
                delete cleanupData;
                return false;
            }
            observer->progressInfo(m_image, 0.2 + (0.8 * ( ((float)y)/((float)imageHeight()) )));
        }

        j = 0;

        for (x = 0; x < imageWidth()*imageBytesDepth(); x+=imageBytesDepth())
        {
            if (imageSixteenBit())
            {
                if (imageHasAlpha())
                {
                    data[j++] = ptr[x+1];  // Blue
                    data[j++] = ptr[ x ];
                    data[j++] = ptr[x+3];  // Green
                    data[j++] = ptr[x+2];
                    data[j++] = ptr[x+5];  // Red
                    data[j++] = ptr[x+4];
                    data[j++] = ptr[x+7];  // Alpha
                    data[j++] = ptr[x+6];
                }
                else
                {
                    data[j++] = ptr[x+1];  // Blue
                    data[j++] = ptr[ x ];
                    data[j++] = ptr[x+3];  // Green
                    data[j++] = ptr[x+2];
                    data[j++] = ptr[x+5];  // Red
                    data[j++] = ptr[x+4];
                }
            }
            else
            {
                if (imageHasAlpha())
                {
                    data[j++] = ptr[ x ];  // Blue
                    data[j++] = ptr[x+1];  // Green
                    data[j++] = ptr[x+2];  // Red
                    data[j++] = ptr[x+3];  // Alpha
                }
                else
                {
                    data[j++] = ptr[ x ];  // Blue
                    data[j++] = ptr[x+1];  // Green
                    data[j++] = ptr[x+2];  // Red
                }
            }
        }

        row_ptr = (png_bytep) data;

        png_write_rows(png_ptr, &row_ptr, 1);
        ptr += (imageWidth() * imageBytesDepth());
    }

    // -------------------------------------------------------------------

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
    png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);

    delete cleanupData;

    imageSetAttribute("savedformat", "PNG");

    saveMetadata(filePath);

    return true;
}

bool PNGLoader::hasAlpha() const
{
    return m_hasAlpha;
}

bool PNGLoader::sixteenBit() const
{
    return m_sixteenBit;
}

void PNGLoader::writeRawProfile(png_struct *ping, png_info *ping_info, char *profile_type,
                                char *profile_data, png_uint_32 length)
{
    png_textp      text;

    register long  i;

    uchar         *sp;

    png_charp      dp;

    png_uint_32    allocated_length, description_length;

    const uchar hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

    qDebug() << "Writing Raw profile: type=" << profile_type << ", length=" << length << endl;

    text               = (png_textp) png_malloc(ping, (png_uint_32) sizeof(png_text));
    description_length = strlen((const char *) profile_type);
    allocated_length   = (png_uint_32) (length*2 + (length >> 5) + 20 + description_length);

    text[0].text   = (png_charp) png_malloc(ping, allocated_length);
    text[0].key    = (png_charp) png_malloc(ping, (png_uint_32) 80);
    text[0].key[0] = '\0';

    concatenateString(text[0].key, "Raw profile type ", 4096);
    concatenateString(text[0].key, (const char *) profile_type, 62);

    sp = (uchar*)profile_data;
    dp = text[0].text;
    *dp++='\n';

    copyString(dp, (const char *) profile_type, allocated_length);

    dp += description_length;
    *dp++='\n';

    formatString(dp, allocated_length-strlen(text[0].text), "%8lu ", length);

    dp += 8;

    for (i=0; i < (long) length; i++)
    {
        if (i%36 == 0)
            *dp++='\n';

        *(dp++)=(char) hex[((*sp >> 4) & 0x0f)];
        *(dp++)=(char) hex[((*sp++ ) & 0x0f)];
    }

    *dp++='\n';
    *dp='\0';
    text[0].text_length = (png_size_t) (dp-text[0].text);
    text[0].compression = -1;

    if (text[0].text_length <= allocated_length)
        png_set_text(ping, ping_info,text, 1);

    png_free(ping, text[0].text);
    png_free(ping, text[0].key);
    png_free(ping, text);
}

size_t PNGLoader::concatenateString(char *destination, const char *source, const size_t length)
{
    register char       *q;

    register const char *p;

    register size_t      i;

    size_t               count;

    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    while ((i-- != 0) && (*q != '\0'))
        q++;

    count = (size_t) (q-destination);
    i     = length-count;

    if (i == 0)
        return(count+strlen(p));

    while (*p != '\0')
    {
        if (i != 1)
        {
            *q++=(*p);
            i--;
        }
        p++;
    }

    *q='\0';

    return(count+(p-source));
}

size_t PNGLoader::copyString(char *destination, const char *source, const size_t length)
{
    register char       *q;

    register const char *p;

    register size_t      i;

    if ( !destination || !source || length == 0 )
        return 0;

    p = source;
    q = destination;
    i = length;

    if ((i != 0) && (--i != 0))
    {
        do
        {
            if ((*q++=(*p++)) == '\0')
                break;
        }
        while (--i != 0);
    }

    if (i == 0)
    {
        if (length != 0)
            *q='\0';

        do
        {
        }
        while (*p++ != '\0');
    }

    return((size_t) (p-source-1));
}

long PNGLoader::formatString(char *string, const size_t length, const char *format,...)
{
    long n;

    va_list operands;

    va_start(operands,format);
    n = (long) formatStringList(string, length, format, operands);
    va_end(operands);
    return(n);
}

long PNGLoader::formatStringList(char *string, const size_t length, const char *format, va_list operands)
{
    int n = vsnprintf(string, length, format, operands);

    if (n < 0)
        string[length-1] = '\0';

    return((long) n);
}

}  // namespace Digikam
