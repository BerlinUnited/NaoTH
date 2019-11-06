/*
 * File:   ImageJPEG.cpp
 * Author: Heinrich Mellmann
 *
 * Created on 11. Dezember 2018, 17:24
 */

// choose the compression method
//#define JPEG_COMPRESS_TURBO
#define JPEG_COMPRESS_DEFAULT
//#define JPEG_COMPRESS_NONE


#include "ImageJPEG.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;


#ifdef JPEG_COMPRESS_DEFAULT

#include <turbojpeg/jpeglib.h>

void ImageJPEG::compress() const
{
  ASSERT(image != NULL);

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, &jpeg, &jpeg_size);

  // image parameters
  cinfo.image_width = image->width();  
  cinfo.image_height = image->height();
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr;
  
  // compression parameters
  jpeg_set_defaults( &cinfo );
  cinfo.dct_method = JDCT_FASTEST;
  jpeg_set_quality(&cinfo, quality, true);

  jpeg_start_compress( &cinfo, true);

  linebuf.resize(image->width() * 3);

  JSAMPROW row_pointer[1];
  row_pointer[0] = &linebuf[0];
  while (cinfo.next_scanline < cinfo.image_height) 
  {
      const unsigned offset = cinfo.next_scanline * cinfo.image_width * 2; //offset to the correct row
      for (unsigned i = 0, j = 0; i < cinfo.image_width * 2; i += 4, j += 6) { //input strides by 4 bytes, output strides by 6 (2 pixels)
          linebuf[j + 0] = image->data()[offset + i + 0]; // Y (unique to this pixel)
          linebuf[j + 1] = image->data()[offset + i + 1]; // U (shared between pixels)
          linebuf[j + 2] = image->data()[offset + i + 3]; // V (shared between pixels)
          linebuf[j + 3] = image->data()[offset + i + 2]; // Y (unique to this pixel)
          linebuf[j + 4] = image->data()[offset + i + 1]; // U (shared between pixels)
          linebuf[j + 5] = image->data()[offset + i + 3]; // V (shared between pixels)
      }
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress( &cinfo );
  jpeg_destroy_compress( &cinfo );
}



// functions needed for the custom jpeg_destination_mgr
static void init_destination (j_compress_ptr /*cinfo*/) {}

// NOTE: this is called if we reach the end of the buffer
static boolean empty_output_buffer (j_compress_ptr /*cinfo*/) {
  assert(false);
  return false;
}

static void term_destination (j_compress_ptr /*cinfo*/) {}


void ImageJPEG::compressYUYV() const
{
  ASSERT(image != NULL);
  jpeg_data.resize(image->data_size());

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  // NOTE:
  // 1) The JPEG standard itself is "color blind" and doesn't specify any particular
  // color space. We interpret the YUV422 image with pixel format (y0,u,y1,v) as a 4-channel image.
  // with y0 and y1 representing separate channels. This results in a reduced line width.
  // 2) libjpeg can perform a color space conversion. We don't want to do that here.
  // source: https://github.com/libjpeg-turbo/libjpeg-turbo/blob/master/libjpeg.txt
  
  // input image parameters
  cinfo.image_width = image->width() / 2;  
  cinfo.image_height = image->height();
  cinfo.input_components = 4; // (y0,u,y1,v)
  cinfo.in_color_space = JCS_UNKNOWN; // it's our special color space
  
  // compression parameters
  // https://github.com/libjpeg-turbo/libjpeg-turbo/blob/master/jcparam.c
  jpeg_set_defaults( &cinfo );
  cinfo.dct_method = JDCT_FASTEST;
  jpeg_set_quality(&cinfo, quality, true);

  // NOTE: jpeg_set_defaults sets the jpeg_color_space based on the in_color_space and 
  //       JCS_UNKNOWN is mapped to JCS_UNKNOWN. We really don't want conversion, so set 
  //       is here again (just to make sure ;)
  jpeg_set_colorspace(&cinfo, cinfo.in_color_space);

  // from the documentation "Special color spaces":
  //   When told that the color space is UNKNOWN, the library will default to using
  //   luminance-quality compression parameters for all color components.  You may
  //   well want to change these parameters.  See the source code for
  //   jpeg_set_colorspace(), in jcparam.c, for details.
  //   https://github.com/libjpeg-turbo/libjpeg-turbo/blob/master/libjpeg.txt
  //
  // set the compression values for the channels of our custom image format.
  // see jpeg_set_colorspace()
  // https://github.com/libjpeg-turbo/libjpeg-turbo/blob/master/jcparam.c
  jpeg_component_info *compptr;
  #define SET_COMP(index, id, hsamp, vsamp, quant, dctbl, actbl) \
    (compptr = &cinfo.comp_info[index], \
     compptr->component_id = (id), \
     compptr->h_samp_factor = (hsamp), \
     compptr->v_samp_factor = (vsamp), \
     compptr->quant_tbl_no = (quant), \
     compptr->dc_tbl_no = (dctbl), \
     compptr->ac_tbl_no = (actbl) )
  //
  SET_COMP(0, 1, 2, 2, 0, 0, 0); // Y0
  SET_COMP(1, 2, 1, 1, 1, 1, 1); // U
  SET_COMP(2, 3, 2, 2, 0, 0, 0); // Y1
  SET_COMP(3, 4, 1, 1, 1, 1, 1); // V
 
  //NOTE: this is a regular way to define a destination manager
  //jpeg_mem_dest(&cinfo, &jpeg, &jpeg_size);

  //
  if(!cinfo.dest) {
    cinfo.dest = static_cast<jpeg_destination_mgr*>(
      (*cinfo.mem->alloc_small)((j_common_ptr)(&cinfo), JPOOL_PERMANENT, sizeof(jpeg_destination_mgr))
    );
  }
  
  cinfo.dest->init_destination = init_destination;
  cinfo.dest->empty_output_buffer = empty_output_buffer;
  cinfo.dest->term_destination = term_destination;
  
  cinfo.dest->next_output_byte = (JOCTET*)(jpeg_data.data()); // HACK: strip 'const' here
  cinfo.dest->free_in_buffer = jpeg_data.size();
  
  // start conpression
  jpeg_start_compress( &cinfo, true);

  JSAMPROW row_pointer[1];
  const int row_stride = cinfo.input_components * cinfo.image_width;

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = image->data() +  cinfo.next_scanline * row_stride;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress( &cinfo );
  jpeg_size = unsigned((char unsigned*)cinfo.dest->next_output_byte - jpeg_data.data());

  jpeg_destroy_compress( &cinfo );
}

#endif

#ifdef JPEG_COMPRESS_TURBO

#include <turbojpeg/turbojpeg.h>

void ImageJPEG::compress() const 
{
  ASSERT(image != NULL);

  planar.resize(image->data_size());
  const uint32_t offsetU = image->width()*image->height();
  const uint32_t offsetV = offsetU + offsetU/2;
  for(uint32_t i = 0, j = 0; i < image->data_size(); i += 4, ++j) {
    planar[j*2 + 0]     = image->data()[i + 0]; // Y
    planar[offsetU + j] = image->data()[i + 1]; // U
    planar[j*2 + 1]     = image->data()[i + 2]; // Y
    planar[offsetV + j] = image->data()[i + 3]; // V
  }

  //static unsigned char *jpeg = (unsigned char *)tjAlloc(640*480*3);
  //unsigned long jpegSize = 0;
  tjhandle handle = tjInitCompress();
  int flags = TJFLAG_FASTUPSAMPLE;
  tjCompressFromYUV(handle, &planar[0], (int)image->width(), 1, (int)image->height(), TJSAMP::TJSAMP_422,  (unsigned char**)&jpeg, &jpeg_size, quality, flags);
  
  //std::cout << tjGetErrorStr() << std::endl;
}
#endif

#ifdef JPEG_COMPRESS_NONE
void ImageJPEG::compress() const 
{
  ASSERT(image != NULL);
  jpeg = image->data();
  jpeg_size = image->data_size();
}
#endif


void Serializer<ImageJPEG>::serialize(const ImageJPEG& parent, std::ostream& stream)
{
  const Image& representation = parent.get();

  // the data has to be converted to a YUV (1 byte for each) array. no interlacing
  static naothmessages::Image img;

  img.set_height(representation.height());
  img.set_width(representation.width());
  
  parent.compressYUYV();
  //parent.compressYUYVDestroy();


// fallback for the format, if no compression is avaliable
#ifdef JPEG_COMPRESS_NONE
  img.set_format(naothmessages::Image_Format_YUV422);
#else
  img.set_format(naothmessages::Image_Format_JPEG);
#endif

  // NOTE: we might want to use .set_allocated_data
  img.set_data(parent.getJPEG(), parent.getJPEGSize());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  img.SerializeToZeroCopyStream(&buf);

  // this is needed if set_allocated_data is used
  //img.release_data();
}

void Serializer<ImageJPEG>::deserialize(std::istream& /*stream*/, ImageJPEG& /*representation*/)
{
  assert(false); // deserialization is not supported yet
}
