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
  yuv422(NULL),
  timestamp(0),
  currentBuffer(0),
  bufferCount(0),
  wrongBufferSizeCount(0),
  selfCreatedImage(false)
{
  yuv422 = new unsigned char[cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL];
  selfCreatedImage = true;
}

//copy constructor
Image::Image(const Image& orig) :
DrawingCanvas(),
cameraInfo(orig.cameraInfo)
{
  if(selfCreatedImage)
  {
    delete[] yuv422;
  }
  yuv422 = new unsigned char[cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL];
  
  std::memcpy(yuv422, orig.yuv422, cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL);
  selfCreatedImage = true;
}

Image& Image::operator=(const Image& orig)
{
  cameraInfo = orig.cameraInfo;
  if(selfCreatedImage)
  {
    delete[] yuv422;
  }
  yuv422 = new unsigned char[SIZE_OF_YUV422_PIXEL * cameraInfo.getSize()];
  std::memcpy(yuv422, orig.yuv422, SIZE_OF_YUV422_PIXEL * cameraInfo.getSize());
  selfCreatedImage = true;

  return *this;
}

Image::~Image()
{
  if (selfCreatedImage && yuv422 != NULL)
  {
    delete[] yuv422;
  }//end if
}//end destructor

void Image::setCameraInfo(const CameraInfo& ci)
{
  if (cameraInfo.resolutionHeight != ci.resolutionHeight || cameraInfo.resolutionWidth != ci.resolutionWidth)
  {
    if(selfCreatedImage)
    {
      delete[] yuv422;
    }
    yuv422 = new unsigned char[SIZE_OF_YUV422_PIXEL*ci.getSize()];
  }
  cameraInfo = ci;
}

void Image::wrapImageDataYUV422(unsigned char* data, const unsigned int size)
{
//  ASSERT(size == cameraInfo.size * SIZE_OF_YUV422_PIXEL);
  ASSERT(size >= cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL);
  if(selfCreatedImage)
  {
    delete[] yuv422;
  }

  yuv422 = data;
  selfCreatedImage = false;
}

void Image::copyImageDataYUV422(unsigned char* data, const unsigned int size)
{
//  if(size == cameraInfo.size * SIZE_OF_YUV422_PIXEL)
  if(size >= cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL)
  {
    if(!selfCreatedImage)
    {
      // throw the old pointer away, create a new buffer
      yuv422 = new unsigned char[SIZE_OF_YUV422_PIXEL * cameraInfo.getSize()];
    }

    // just overwrite the old image data
    memcpy(yuv422, data, size);

    selfCreatedImage = true;

  }
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


void Image::toDataStream(ostream& stream) const
{
  // the data has to be converted to a YUV (1 byte for each) array. no interlacing
  naothmessages::Image img;

  if((int)cameraInfo.resolutionHeight != img.height())
  {
    img.set_height(cameraInfo.resolutionHeight);
  }
  if((int)cameraInfo.resolutionWidth != img.width())
  {
    img.set_width(cameraInfo.resolutionWidth);
  }

  img.set_format(naothmessages::Image_Format_YUV422);
  img.set_data(yuv422, cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL);
  img.set_timestamp(timestamp);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  img.SerializePartialToZeroCopyStream(&buf);

}

void Image::fromDataStream(istream& stream)
{
  naothmessages::Image img;

  google::protobuf::io::IstreamInputStream buf(&stream);
  img.ParseFromZeroCopyStream(&buf);

  unsigned int width = img.width();
  unsigned int height = img.height();

  if(img.has_timestamp())
  {
    timestamp = img.timestamp();
  }

  // YUV full
  if(img.format() == naothmessages::Image_Format_YUV)
  {
    if(img.data().size() != 3 * width * height)
    {
      return;
    }

    CameraInfo newCameraInfo;

    newCameraInfo.resolutionHeight = height;
    newCameraInfo.resolutionWidth = width;
    setCameraInfo(newCameraInfo);

    const char* data = img.data().c_str();

    for(unsigned int i=0; i < getIndexSize(); i++)
    {
      unsigned int x = getXOffsetFromIndex(i);
      unsigned int y = getYOffsetFromIndex(i);

      Pixel p;

      p.y = data[i * 3];
      p.u = data[i * 3 + 1];
      p.v = data[i * 3 + 2];

      set(x,y, p);
    }
  }
  // "native" YUV422 data
  else if(img.format() == naothmessages::Image_Format_YUV422)
  {
    if(img.data().size() != SIZE_OF_YUV422_PIXEL * width * height)
    {
      return;
    }

    CameraInfo newCameraInfo;

    newCameraInfo.resolutionHeight = height;
    newCameraInfo.resolutionWidth = width;
    setCameraInfo(newCameraInfo);

    memcpy(yuv422, img.data().c_str(), img.data().size());
  }
  
}//end fromDataStream
                       

void Serializer<Image>::serialize(const Image& representation, std::ostream& stream)
{
  // the data has to be converted to a YUV (1 byte for each) array. no interlacing
  naothmessages::Image img;

  if((int)representation.cameraInfo.resolutionHeight != img.height())
  {
    img.set_height(representation.cameraInfo.resolutionHeight);
  }
  if((int)representation.cameraInfo.resolutionWidth != img.width())
  {
    img.set_width(representation.cameraInfo.resolutionWidth);
  }

  img.set_format(naothmessages::Image_Format_YUV422);
  img.set_data(representation.yuv422, representation.cameraInfo.getSize() * SIZE_OF_YUV422_PIXEL);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  img.SerializeToZeroCopyStream(&buf);
}

void Serializer<Image>::deserialize(std::istream& stream, Image& representation)
{
  naothmessages::Image img;

  google::protobuf::io::IstreamInputStream buf(&stream);
  img.ParseFromZeroCopyStream(&buf);

  unsigned int width = img.width();
  unsigned int height = img.height();

  if(width != naoth::IMAGE_WIDTH || height != naoth::IMAGE_HEIGHT) {
	  THROW("Image size doesn't correspond to the static values IMAGE_WIDTH and IMAGE_HEIGHT.");
  }

  // YUV full
  if(img.format() == naothmessages::Image_Format_YUV)
  {
    if(img.data().size() != 3 * width * height)
    {
      return;
    }

    CameraInfo newCameraInfo;

    newCameraInfo.resolutionHeight = height;
    newCameraInfo.resolutionWidth = width;
    representation.setCameraInfo(newCameraInfo);

    const char* data = img.data().c_str();

    for(unsigned int i=0; i < representation.getIndexSize(); i++)
    {
      unsigned int x = representation.getXOffsetFromIndex(i);
      unsigned int y = representation.getYOffsetFromIndex(i);

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
    if(img.data().size() != SIZE_OF_YUV422_PIXEL * width * height) {
      return;
    }

    CameraInfo newCameraInfo;

    newCameraInfo.resolutionHeight = height;
    newCameraInfo.resolutionWidth = width;
    representation.setCameraInfo(newCameraInfo);

    memcpy(representation.yuv422, img.data().c_str(), img.data().size());
  }
}
