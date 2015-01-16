/**
* @file SimpleHogFeatureDetector.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct SimpleHogFeatureDetector
*/

#ifndef _SimpleHogFeatureDetector_H_
#define _SimpleHogFeatureDetector_H_

#include <opencv2/core/core.hpp>

#include "Representations/Perception/Histograms.h"

#include "Tools/ImageProcessing/SimpleHogFeature.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"

#define IMAGE_GET(image,x,y,p) \
  if(!image.isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  image.get(x, y, p);

class SimpleHogFeatureDetector
{
public:
  
  Pixel pixel;  
  
  std::vector<std::vector<int> > hogVector;
  std::vector<Vector2i> maxGradiantVector;
  //std::vector<std::vector<int> > hoiVector;
  //std::vector<Vector2i> maxIntensityVector;

  SimpleHogFeatureDetector()
  {
  };
  
  void clear()
  {
    hogVector.clear();
    //hoiVector.clear();
  }

  void add(const naoth::Image& image, const Vector2i& point)
  {
      SimpleHogFeature hogFeature(image, point);

      Vector2i maxGradiant(hogFeature.hog.max, hogFeature.hog.rawData[hogFeature.hog.max]);
      //Vector2i maxIntensity(hogFeature.hoi.max, hogFeature.hoi.rawData[hogFeature.hoi.max]);

      hogVector.push_back(hogFeature.hog.rawData);
      maxGradiantVector.push_back(maxGradiant);
      //hoiVector.push_back(hogFeature.hoi.rawData);
      //maxIntensityVector.push_back(maxIntensity);

  }

};

#endif //_SimpleHogFeatureDetector_H_
