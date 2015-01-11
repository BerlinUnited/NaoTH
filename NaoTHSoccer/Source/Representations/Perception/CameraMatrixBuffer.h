#ifndef CAMERAMATRIXBUFFER_H
#define CAMERAMATRIXBUFFER_H

#include "CameraMatrix.h"

class CameraMatrixBuffer : public RingBuffer<CameraMatrix, 10>, public naoth::Printable
{
public:

  virtual void print(std::ostream& stream) const
  {
    for(int i=0; i < size(); i++)
    {
      stream << "entry " << i << std::endl;
      stream << "=========" << std::endl;
      getEntry(i).print(stream);
      stream << std::endl;
    }
  }//end print

  virtual ~CameraMatrixBuffer() {}
};

class CameraMatrixBufferTop : public RingBuffer<CameraMatrixTop, 10>, public naoth::Printable
{
public:

  virtual void print(std::ostream& stream) const
  {
    for(int i=0; i < size(); i++)
    {
      stream << "entry " << i << std::endl;
      stream << "=========" << std::endl;
      getEntry(i).print(stream);
      stream << std::endl;
    }
  }//end print

  virtual ~CameraMatrixBufferTop() {}
};

#endif // CAMERAMATRIXBUFFER_H
