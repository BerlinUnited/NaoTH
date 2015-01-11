#ifndef __Blob_H_
#define __Blob_H_

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Moments2.h>
#include <Tools/ColorClasses.h>

/**
 * class Blob                        
 */
class Blob
{
public:
  Blob() : blobMass(0), color(ColorClasses::none) {}
  
  Vector2i upperLeft;
  Vector2i upperRight;
  Vector2i lowerLeft;
  Vector2i lowerRight;
  Vector2i centerOfMass;
  Moments2<1>  moments;
  int blobMass;
  ColorClasses::Color color;

  void draw()
  {
    // TODO: dra the blob in image
  }
};

#endif // __Blob_H__

