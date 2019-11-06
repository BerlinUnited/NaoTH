/*
 * File:   ImageJPEG.cpp
 * Author: Heinrich Mellmann
 *
 * Created on 11. Dezember 2018, 17:24
 */

#include "ImageJPEG.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;


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

  // make sure we have enough space
  // NOTE: the resize does not allocate new memory if there is enough memory already reserved.
  // so, the allocation should happen only the first time.
  jpeg.resize(image->data_size());

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

  // NOTE: we have to use JPOOL_PERMANENT here because aur buffer is allocated permamently 
  // and shall not be freed automatically.
  // documentation can be found in section "Memory management"
  // https://github.com/libjpeg-turbo/libjpeg-turbo/blob/master/libjpeg.txt
  if(!cinfo.dest) {
    cinfo.dest = static_cast<jpeg_destination_mgr*>(
      (*cinfo.mem->alloc_small)((j_common_ptr)(&cinfo), JPOOL_PERMANENT, sizeof(jpeg_destination_mgr))
    );
  }
  
  cinfo.dest->init_destination = init_destination;
  cinfo.dest->empty_output_buffer = empty_output_buffer;
  cinfo.dest->term_destination = term_destination;
  
  cinfo.dest->next_output_byte = (JOCTET*)(jpeg.data()); // HACK: strip 'const' here
  cinfo.dest->free_in_buffer = jpeg.size();
  
  // start compression
  jpeg_start_compress( &cinfo, true);

  JSAMPROW row_pointer[1];
  const int row_stride = cinfo.input_components * cinfo.image_width;

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = image->data() +  cinfo.next_scanline * row_stride;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress( &cinfo );

  // calculate the number of bytes produced by the compression
  jpeg_size = (size_t)((uint8_t*)cinfo.dest->next_output_byte - jpeg.data());

  jpeg_destroy_compress( &cinfo );
}


void Serializer<ImageJPEG>::serialize(const ImageJPEG& parent, std::ostream& stream)
{
  static naothmessages::Image img;

  img.set_height(parent.get().height());
  img.set_width(parent.get().width());
  
  img.set_format(naothmessages::Image_Format_JPEG);

  parent.compressYUYV();
  
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
