/*
 * File:   Image.cpp
 * Author: Oliver Welter
 *
 * Created on 11. Dezember 2008, 17:24
 */

#include <iterator>
#include <string>

#include "Image.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

Image::Image()
  :
  selfCreatedImage(true),
  yuv422(NULL),
  timestamp(0),
  currentBuffer(0),
  bufferCount(0),
  wrongBufferSizeCount(0)
  
{
  yuv422 = new unsigned char[data_size()];
}

Image::Image(const Image& orig)
{
  cameraInfo = orig.cameraInfo;
  if(selfCreatedImage) {
    delete[] yuv422;
  }

  yuv422 = new unsigned char[data_size()];
  std::memcpy(yuv422, orig.yuv422, data_size());
  selfCreatedImage = true;
}

Image& Image::operator=(const Image& orig)
{
  cameraInfo = orig.cameraInfo;
  if(selfCreatedImage) {
    delete[] yuv422;
  }

  yuv422 = new unsigned char[data_size()];
  std::memcpy(yuv422, orig.yuv422, data_size());
  selfCreatedImage = true;

  return *this;
}

Image::~Image()
{
  if (selfCreatedImage) {
    delete[] yuv422;
  }
}

void Image::setCameraInfo(const CameraInfo& ci)
{
  if (cameraInfo.resolutionHeight != ci.resolutionHeight || cameraInfo.resolutionWidth != ci.resolutionWidth)
  {
    if(selfCreatedImage) {
      delete[] yuv422;
    }
    cameraInfo = ci;
    yuv422 = new unsigned char[data_size()];
  }
}

void Image::wrapImageDataYUV422(unsigned char* data, const unsigned int size)
{
  ASSERT(size == data_size());
  if(selfCreatedImage) {
    delete[] yuv422;
  }

  yuv422 = data;
  selfCreatedImage = false;
}

void Image::copyImageDataYUV422(const unsigned char* data, const unsigned int size)
{
  ASSERT(size == data_size());
  
  // create own buffer if necessary
  if(!selfCreatedImage) {
    yuv422 = new unsigned char[data_size()];
    selfCreatedImage = true;
  }

  // just overwrite the old image data
  memcpy(yuv422, data, size);
}

void Image::print(ostream& stream) const
{
  stream  << "Timestamp: " << timestamp << endl
          << "Image Buffer: "<< currentBuffer << " / " << bufferCount << endl
          << "Wrong Buffer Size Count: "<< wrongBufferSizeCount << endl
          << "Camera Info:"<< endl
          << "============"<< endl
          << cameraInfo << endl
          << "============"<< endl
          ;
}//end print


void Serializer<Image>::serialize(const Image& representation, std::ostream& stream)
{
  // the data has to be converted to a YUV (1 byte for each) array. no interlacing
  naothmessages::Image img;

  img.set_height(representation.height());
  img.set_width(representation.width());
  img.set_format(naothmessages::Image_Format_YUV422);
  img.set_data(representation.data(), representation.data_size());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  img.SerializeToZeroCopyStream(&buf);
}

void Serializer<Image>::deserialize(std::istream& stream, Image& representation)
{
  naothmessages::Image img;

  google::protobuf::io::IstreamInputStream buf(&stream);
  img.ParseFromZeroCopyStream(&buf);

  //TODO: deprecated
  if(img.width() != (int)naoth::IMAGE_WIDTH || img.height() != (int)naoth::IMAGE_HEIGHT) {
	  THROW("Image size doesn't correspond to the static values IMAGE_WIDTH and IMAGE_HEIGHT.");
  }

  // YUV444
  if(img.format() == naothmessages::Image_Format_YUV)
  {
    // check the integrity
    ASSERT(img.data().size() != Image::PIXEL_SIZE_YUV444 * img.width() * img.height());

    CameraInfo newCameraInfo;
    newCameraInfo.resolutionHeight = img.height();
    newCameraInfo.resolutionWidth = img.width();
    representation.setCameraInfo(newCameraInfo);

    const char* data = img.data().c_str();

    // HACK: copy the image pixel by pixel because the internal structure only 
    //       representa the image in the YUV422 format
    for(int i=0; i < img.width()*img.height(); i++)
    {
      unsigned int x = i % newCameraInfo.resolutionWidth;
      unsigned int y = i / newCameraInfo.resolutionWidth;

      Pixel p;
      p.y = data[i * 3];
      p.u = data[i * 3 + 1];
      p.v = data[i * 3 + 2];

      representation.set(x,y, p);
    }
  }
  // "native" YUV422 data
  else if(img.format() == naothmessages::Image_Format_YUV422)
  {
    // check the integrity
    ASSERT(img.data().size() == Image::PIXEL_SIZE_YUV422 * img.width() * img.height());

    CameraInfo newCameraInfo;
    newCameraInfo.resolutionHeight = img.height();
    newCameraInfo.resolutionWidth = img.width();
    representation.setCameraInfo(newCameraInfo);
    
    const unsigned char* data = reinterpret_cast<const unsigned char*>(img.data().c_str());
    const unsigned int size = static_cast<unsigned int>(img.data().size());
    representation.copyImageDataYUV422(data,size);
  } else {
    THROW("Unknown image format.");
  }
}
