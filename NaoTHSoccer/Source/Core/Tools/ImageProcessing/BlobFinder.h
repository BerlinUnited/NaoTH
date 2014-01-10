/**
* @file BlobFinder.h
*
* @author <a href="mailto:thermann@informatik.hu-berlin.de">Tobias Hermann</a>
* Definition of class BlobFinder
*/

#ifndef __BlobFinder_H_
#define __BlobFinder_H_

// tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Moments2.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/DataStructures/Container.h>

// local tools
#include "Blob.h"

//representations
#include "Representations/Infrastructure/ColoredGrid.h"


class BlobFinder
{
public:
  BlobFinder(const ColoredGrid& coloredGrid);
  virtual ~BlobFinder(){};

  void execute(
    Container<Blob>& result, 
    bool connectedColors[ColorClasses::numOfColors], 
    const Area<int>& searchArea);

  Blob regionGrowExpandArea(
    const Vector2i& startingPoint, 
    ColorClasses::Color color);


private:
  const ColoredGrid& coloredGrid;
  OccupancyGrid visitedPixels;


  struct BlobCreator
  {

    BlobCreator()
      : minValues(1024,1024),
        maxValues(0,0)
    {
    }

    Vector2i minValues; /** left, upper corner: x = minX, y = minY */
    Vector2i maxValues; /** right, lower corner: x = maxX, y = maxY */
    Moments2<1> moments;

    void addPoint(const Vector2i& point)
    {
      if(minValues.x > point.x) minValues.x = point.x;
      if(minValues.y > point.y) minValues.y = point.y;
      if(maxValues.x < point.x) maxValues.x = point.x;
      if(maxValues.y < point.y) maxValues.y = point.y;

      moments.add(point);
    }//end addPoint

    const Blob& createBlob()
    {
      //calculate start and end points of the blobs boundaries
      blob.upperLeft = minValues;
      blob.upperRight.x = maxValues.x;
      blob.upperRight.y = minValues.y;
      blob.lowerLeft.x = minValues.x;
      blob.lowerLeft.y = maxValues.y;
      blob.lowerRight = maxValues;
      
      //and the center of mass
      blob.centerOfMass = moments.getCentroid();

      blob.moments = moments;

      return blob;
    }//end createBlob

  private:
    Blob blob;
  };

};//end class BlobFinder


class WholeArea : public Area<int>
{
public:

  WholeArea() {}

  virtual bool isInside(const Vector2i& point) const
  {
    return true;
  }

  virtual ~WholeArea() {};
};

#endif // __BlobFinder_H_
