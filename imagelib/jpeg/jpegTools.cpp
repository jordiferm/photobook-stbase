//==============================================
//  copyright            : (C) 2003-2005 by Will Stokes
//==============================================
//  This program is free software; you can redistribute it
//  and/or modify it under the terms of the GNU General
//  Public License as published by the Free Software
//  Foundation; either version 2 of the License, or
//  (at your option) any later version.
//==============================================

//-----------------------------
//On windows we must force boolean to be a unsigned char, why?
//for some reason libjpeg tries to redefine it to int and causes
//struct size mismatches when calling libjpeg functions :(
#include <qglobal.h>

//PLATFORM_SPECIFIC_CODE
#ifdef Q_OS_WIN
typedef unsigned char boolean;
#define HAVE_BOOLEAN
#endif
//-----------------------------

//Systemwide includes
#include <stdio.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qimage.h>
#include <setjmp.h>
#define XMD_H
extern "C" {
#include <jpeglib.h>
#include "jpegInternal.h"
}

//Projectwide includes
#include "../imageTools.h"
#include "jpegTools.h"

//=============================================
//Code extracted from GwenView/KDE
struct GVJPEGFatalError : public jpeg_error_mgr 
{
  jmp_buf mJmpBuffer;
  
  static void handler(j_common_ptr cinfo) 
  {
    GVJPEGFatalError* error=static_cast<GVJPEGFatalError*>(cinfo->err);
    (error->output_message)(cinfo);
    longjmp(error->mJmpBuffer,1);
  }
};
//=============================================
bool scaleJPEG(QString fileIn, QImage& scaledImage, 
               int targetWidth, int targetHeight)
{
  //get jpeg info
  struct jpeg_decompress_struct cinfo;

  //open file
  FILE* inputFile=fopen( fileIn.ascii(), "rb" );
  if(!inputFile) return false;

  //error checking on jpg file
  struct GVJPEGFatalError jerr;
  cinfo.err = jpeg_std_error(&jerr);
  cinfo.err->error_exit = GVJPEGFatalError::handler;
  if (setjmp(jerr.mJmpBuffer)) 
  {
    jpeg_destroy_decompress(&cinfo);
    fclose(inputFile);
    return false;
  }

  //get jpeg resolution
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, inputFile);
  jpeg_read_header(&cinfo, TRUE);

  //find optimal scale fraction (must be power of two)
  int origWidth = (int)cinfo.image_width;
  int origHeight = (int)cinfo.image_height;
  int num = 1;
  int denom = 1;
  
  //if image is bigger than target, scale down by adjusting the denominator
  if( origWidth > targetWidth || origHeight > targetHeight )
  {
    while( denom < 8 && 
           ( origWidth / (denom*2) >= targetWidth ||  origHeight / (denom*2) >= targetHeight )
         )
    { denom = denom*2; }
  }

  //set scaling fraction
  cinfo.scale_num=num;
  cinfo.scale_denom=denom;

  //create intermediary image scaled by power of 2
  jpeg_start_decompress(&cinfo);

  switch(cinfo.output_components) 
  {
    //Black and White Image
    case 1:
      {
        scaledImage.create( cinfo.output_width, cinfo.output_height, 8, 256 );
        for (int i=0; i<256; i++)
        {
          scaledImage.setColor(i, qRgb(i,i,i));
        }
      }
      break;
    
    //24 or 32 bit color image
    case 3:
    case 4:
      scaledImage.create( cinfo.output_width, cinfo.output_height, 32 );
      break;
    
    //Unknown color depth
    default:
      jpeg_destroy_decompress(&cinfo);
      fclose(inputFile);
      return false;
  }

  //fast scale image
  uchar** lines = scaledImage.jumpTable();
  while (cinfo.output_scanline < cinfo.output_height) 
  {
    jpeg_read_scanlines(&cinfo, lines + cinfo.output_scanline, cinfo.output_height);
  }
  jpeg_finish_decompress(&cinfo);

  
  //expand 8 to 32bits per pixel since QImage wants 32
  if ( cinfo.output_components == 1 ) 
  {
    scaledImage = scaledImage.convertDepth( 32, Qt::AutoColor );
  }
  
  //expand 24 to 32bits per pixel since QImage wants 32
  if ( cinfo.output_components == 3 ) 
  {
    for (uint j=0; j<cinfo.output_height; j++) 
    {
      uchar *in = scaledImage.scanLine(j) + cinfo.output_width*3;
      QRgb *out = (QRgb*)( scaledImage.scanLine(j) );

      for (uint i=cinfo.output_width; i--; ) 
      {
        in-=3;
        out[i] = qRgb(in[0], in[1], in[2]);
      }
    }
  }
 
  //use slower smooth scale technique to scale to final size from intermediate image 
  if( scaledImage.width() != targetWidth || scaledImage.height() != targetHeight )
  {
    int clampedTargetWidth = targetWidth;
    int clampedTargetHeight = targetHeight;
    //clamp scaling up to < 2x
    if(QMIN( ((float)targetWidth)/origWidth, ((float)targetHeight)/origHeight ) > 2)    
    {
      clampedTargetWidth = 2*origWidth;
      clampedTargetHeight = 2*origHeight;
    }
    
    scaledImage = scaledImage.scaled(QSize(clampedTargetWidth, clampedTargetHeight), Qt::KeepAspectRatio);
  }
  jpeg_destroy_decompress(&cinfo);
  fclose(inputFile);
  return true;
}
//=============================================
bool transformJPEG( QString fileIn, QString fileOut, TRANSFORM_CODE transformation )
{  
  struct jpeg_decompress_struct      srcinfo;
  struct jpeg_compress_struct          dstinfo;
  struct jpeg_error_mgr                      jsrcerr, jdsterr;
  FILE * input_file;
  FILE * output_file;
  jpeg_transform_info transformoption;
  jvirt_barray_ptr *                                 src_coef_arrays;
  jvirt_barray_ptr *                                 dst_coef_arrays;
  
  //setup transofrmation option struct, this was done in jpegtran by the
  //parse_switches function in jpegtran.c
	switch( transformation )
  {
    case ROTATE_90:
       transformoption.transform = JXFORM_ROT_90;
       break;
    case ROTATE_270:
       transformoption.transform = JXFORM_ROT_270;
       break;
    case FLIP_H:
       transformoption.transform = JXFORM_FLIP_H;
       break;
    case FLIP_V:
       transformoption.transform = JXFORM_FLIP_V;
       break;
    default:
      return false;
  }  
  transformoption.trim = TRUE;
  transformoption.force_grayscale = FALSE;
 
  // Initialize the JPEG decompression object with default error handling. 
  srcinfo.err = jpeg_std_error(&jsrcerr);
  jpeg_create_decompress(&srcinfo);
  
  // Initialize the JPEG compression object with default error handling. 
  dstinfo.err = jpeg_std_error(&jdsterr);
  jpeg_create_compress(&dstinfo);
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
  //what does this stuff do?
  dstinfo.err->trace_level = 0;
  jsrcerr.trace_level = jdsterr.trace_level;
  srcinfo.mem->max_memory_to_use = dstinfo.mem->max_memory_to_use;
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
  //open input and output files
  if ((input_file = fopen(QFile::encodeName(fileIn), "rb")) == NULL) return false;
  if ((output_file = fopen(QFile::encodeName(fileOut), "wb")) == NULL) return false;

  // Specify data source for decompression 
  jpeg_stdio_src(&srcinfo, input_file);

  // Enable saving of extra markers that we want to copy 
  jcopy_markers_setup(&srcinfo, JCOPYOPT_COMMENTS);

  // Read file header 
  (void) jpeg_read_header(&srcinfo, TRUE);
  
  // Any space needed by a transform option must be requested before
  // jpeg_read_coefficients so that memory allocation will be done right.
  jtransform_request_workspace(&srcinfo, &transformoption);

  // Read source file as DCT coefficients 
  src_coef_arrays = jpeg_read_coefficients(&srcinfo);

  // Initialize destination compression parameters from source values 
  jpeg_copy_critical_parameters(&srcinfo, &dstinfo);
      
  // Adjust destination parameters if required by transform options;
  // also find out which set of coefficient arrays will hold the output.   
  dst_coef_arrays = jtransform_adjust_parameters(&dstinfo, src_coef_arrays,
                                                 &transformoption);

  // Specify data destination for compression 
  jpeg_stdio_dest(&dstinfo, output_file);

  // Start compressor (note no image data is actually written here) 
  jpeg_write_coefficients(&dstinfo, dst_coef_arrays);

  // Copy to the output file any extra markers that we want to preserve 
  jcopy_markers_execute(&srcinfo, &dstinfo);

  // Execute image transformation, if any 
  jtransform_execute_transformation(&srcinfo, &dstinfo,
                                                                   src_coef_arrays,
                                                                   &transformoption);
                                                                   
  // Finish compression and release memory 
  jpeg_finish_compress(&dstinfo);
  jpeg_destroy_compress(&dstinfo);
  (void) jpeg_finish_decompress(&srcinfo);
  jpeg_destroy_decompress(&srcinfo);

  // Close files
  fclose(input_file);
  fclose(output_file);
    
  //success
  return true;
}
//=============================================

