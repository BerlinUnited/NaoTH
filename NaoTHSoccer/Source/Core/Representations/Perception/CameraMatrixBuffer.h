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

class CameraMatrixBuffer2 : public RingBuffer<CameraMatrixTop, 10>, public naoth::Printable
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

  virtual ~CameraMatrixBuffer2() {}
};

#endif // CAMERAMATRIXBUFFER_H
