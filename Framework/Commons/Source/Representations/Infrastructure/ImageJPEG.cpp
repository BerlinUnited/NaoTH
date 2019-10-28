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

  cinfo.image_width = image->width();  
  cinfo.image_height = image->height();
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr;

  jpeg_set_defaults( &cinfo );
  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_start_compress( &cinfo, TRUE );

  linebuf.resize(image->width() * 3);

  JSAMPROW row_pointer[1];
  row_pointer[0] = &linebuf[0];
  while (cinfo.next_scanline < cinfo.image_height) {
      unsigned i, j;
      unsigned offset = cinfo.next_scanline * cinfo.image_width * 2; //offset to the correct row
      for (i = 0, j = 0; i < cinfo.image_width * 2; i += 4, j += 6) { //input strides by 4 bytes, output strides by 6 (2 pixels)
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
  naothmessages::Image img;

  img.set_height(representation.height());
  img.set_width(representation.width());
  
  parent.compress();

// fallback for the format, if no compression is avaliable
#ifdef JPEG_COMPRESS_NONE
  img.set_format(naothmessages::Image_Format_YUV422);
#else
  img.set_format(naothmessages::Image_Format_JPEG);
#endif

  img.set_data(parent.getJPEG(), parent.getJPEGSize());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  img.SerializeToZeroCopyStream(&buf);
}

void Serializer<ImageJPEG>::deserialize(std::istream& /*stream*/, ImageJPEG& /*representation*/)
{
  assert(false); // deserialization is not supported yet
}
