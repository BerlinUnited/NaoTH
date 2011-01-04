/*
 * File:   Image.cpp
 * Author: Oliver Welter
 *
 * Created on 11. Dezember 2008, 17:24
 */

#include <iterator>
#include <string>

#include "Representations/Infrastructure/Image.h"
#include "PlatformInterface/Platform.h"

#include "Messages/Image.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

Image::Image()
  : yuv422(NULL)
{
  cameraInfo = Platform::getInstance().theCameraInfo;
  yuv422 = new unsigned char[cameraInfo.size*SIZE_OF_YUV422_PIXEL];
  selfCreatedImage = true;
}

//copy constructor

Image::Image(const Image& orig) :
cameraInfo(orig.cameraInfo)
{
  if(selfCreatedImage)
  {
    delete[] yuv422;
  }
  yuv422 = new unsigned char[cameraInfo.size*SIZE_OF_YUV422_PIXEL];
  
  std::memcpy(yuv422, orig.yuv422, cameraInfo.size*SIZE_OF_YUV422_PIXEL);
  selfCreatedImage = true;
}

Image& Image::operator=(const Image& orig)
{
  cameraInfo = orig.cameraInfo;
  if(selfCreatedImage)
  {
    delete[] yuv422;
  }
  yuv422 = new unsigned char[SIZE_OF_YUV422_PIXEL*cameraInfo.size];
  std::memcpy(yuv422, orig.yuv422, SIZE_OF_YUV422_PIXEL*cameraInfo.size);
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
    yuv422 = new unsigned char[SIZE_OF_YUV422_PIXEL*ci.size];
  }
  cameraInfo = ci;
}

void Image::wrapImageDataYUV422(unsigned char* data, unsigned int size)
{
  if(size == cameraInfo.size*SIZE_OF_YUV422_PIXEL)
  {
    if(selfCreatedImage)
    {
      delete[] yuv422;
    }

    yuv422 = data;
    selfCreatedImage = false;
  }
  else
  {
    std::cerr << "[wrapImageDataYUV422] size was wrong, should be "
      << (cameraInfo.size*SIZE_OF_YUV422_PIXEL)
      << " but was " << size << std::endl;
  }
}

void Image::copyImageDataYUV422(unsigned char* data, unsigned int size)
{
  if(size == cameraInfo.size*SIZE_OF_YUV422_PIXEL)
  {
    if(!selfCreatedImage)
    {
      // throw the old pointer away, create a new buffer
      yuv422 = new unsigned char[SIZE_OF_YUV422_PIXEL*cameraInfo.size];
    }

    // just overwrite the old image data
    memcpy(yuv422, data, size);

    selfCreatedImage = true;

  }
}

void Image::print(ostream& stream) const
{
  stream << "ID: " << cameraInfo.cameraID <<'\n'
          << "Focal Length: "<< cameraInfo.focalLength << '\n'
          << "Roll Offset: "<< cameraInfo.cameraRollOffset << '\n'
          << "Tilt Offset: "<< cameraInfo.cameraTiltOffset << '\n'
          << "Size: " << cameraInfo.resolutionWidth << "x" << cameraInfo.resolutionHeight <<'\n'
          << "Optical Center: " << cameraInfo.opticalCenterX << "," << cameraInfo.opticalCenterY <<'\n'
          << "Opening Angle: " << cameraInfo.openingAngleWidth << "," << cameraInfo.openingAngleHeight;
}//end print




void Image::drawPoint(
  unsigned int x,
  unsigned int y,
  unsigned char a,
  unsigned char b,
  unsigned char c)
{
  Pixel p;

  p.y = a;
  p.u = b;
  p.v = c;

  set(x,y, p);
}//end drawPoint
                        
unsigned int Image::width()
{
  return cameraInfo.resolutionWidth;
}//end width

unsigned int Image::height()
{
  return cameraInfo.resolutionHeight;
}//end height

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
  img.set_data(representation.yuv422, representation.cameraInfo.size * SIZE_OF_YUV422_PIXEL);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  img.SerializePartialToZeroCopyStream(&buf);
  //g_debug("size of message: %d", buf.ByteCount());
}

void Serializer<Image>::deserialize(std::istream& stream, Image& representation)
{
  naothmessages::Image img;

  google::protobuf::io::IstreamInputStream buf(&stream);
  img.ParseFromZeroCopyStream(&buf);

  unsigned int width = img.width();
  unsigned int height = img.height();

  // YUV full
  if(img.format() == naothmessages::Image_Format_YUV)
  {
    if(img.data().size() != 3 * width * height)
    {
      return;
    }

    CameraInfo newCameraInfo = Platform::getInstance().theCameraInfo;

    newCameraInfo.resolutionHeight = height;
    newCameraInfo.resolutionWidth = width;
    newCameraInfo.size = width * height;
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
    if(img.data().size() != SIZE_OF_YUV422_PIXEL * width * height)
    {
      return;
    }

    CameraInfo newCameraInfo = Platform::getInstance().theCameraInfo;

    newCameraInfo.resolutionHeight = height;
    newCameraInfo.resolutionWidth = width;
    newCameraInfo.size = width * height;
    representation.setCameraInfo(newCameraInfo);

    memcpy(representation.yuv422, img.data().c_str(), img.data().size());
  }
}