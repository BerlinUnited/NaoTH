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
  
  Vector2<int> upperLeft;
  Vector2<int> upperRight;
  Vector2<int> lowerLeft;
  Vector2<int> lowerRight;
  Vector2<int> centerOfMass;
  Moments2<1>  moments;
  int blobMass;
  ColorClasses::Color color;

  void draw()
  {
    // TODO: dra the blob in image
  }
};

#endif // __Blob_H__

