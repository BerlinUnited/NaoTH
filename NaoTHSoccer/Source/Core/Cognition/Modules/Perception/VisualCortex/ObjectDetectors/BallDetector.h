/**
* @file BallDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallDetector
*/

#ifndef __BallDetector_H_
#define __BallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>

#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/FieldInfo.h"
//#include "Representations/Perception/BlobPercept.h"
#include "Representations/Perception/FieldPercept.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/BallPercept.h"

// tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/SpiderScan.h"



BEGIN_DECLARE_MODULE(BallDetector)
  REQUIRE(Image)
  REQUIRE(ColoredGrid)
//  REQUIRE(ColorTable64)
  REQUIRE(CameraMatrix)
//  REQUIRE(FieldPercept)
//  REQUIRE(BlobPercept)
  REQUIRE(FieldInfo)

  PROVIDE(BallPercept)
END_DECLARE_MODULE(BallDetector)


class SimpleColorClassifier: public ColorClassifier
{
public:

  SimpleColorClassifier() {}

  inline ColorClasses::Color getColorClass(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    return get(a, b, c);
  }

  inline ColorClasses::Color getColorClass(const Pixel& p) const
  {
    return get(p.a, p.b, p.c);
  }

  inline ColorClasses::Color get(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    double d = (Math::sqr((255.0 - (double)b)) + Math::sqr((double)c)) / (2.0*255.0);
    unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);

    if(t > 120)
      return ColorClasses::orange;
    else
      return ColorClasses::none;
  }//end get
};


class BallDetector: private BallDetectorBase
{
public:
  BallDetector();
  ~BallDetector(){};

  void execute(const Vector2<int>& start);

  // override the Module execute method
  virtual void execute();
 
private:
  typedef PointList<20> BallPointList;

  bool connectedColors[ColorClasses::numOfColors];
  BlobFinder theBlobFinder;

  bool calculateCircle( const BallPointList& ballPoints, Vector2<double>& center, double& radius );
  bool randomScan(ColorClasses::Color color, Vector2<int>& result, const Vector2<int>& orgMin, const Vector2<int>& orgMax) const;

  void regionGrowExpandArea( const Vector2<int>& startingPoint, Vector2<double>& result, double& radius);

  double calculateBase(Vector2<int>& x, Vector2<int>& y, Vector2<int>& z);

  const SimpleColorClassifier simpleColorClassifier;

  const ColorClassifier& getColorTable64() const
  {
    return simpleColorClassifier;
  }

};//end class BallDetector

#endif // __BallDetector_H_
