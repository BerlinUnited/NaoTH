/**
* @file GoalFeatureDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalFeatureDetector
*/

#ifndef _GoalFeatureDetector_H_
#define _GoalFeatureDetector_H_

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/FieldColorPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/GoalFeaturePercept.h"
#include "Representations/Perception/FieldPercept.h"

// tools
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"

#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/RingBuffer.h>
#include <Tools/DataStructures/RingBufferWithSum.h>
#include "Tools/ImageProcessing/MaximumScan.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include <vector>

#include "Tools/naoth_opencv.h"
#include "Tools/ImageProcessing/Edgel.h"

BEGIN_DECLARE_MODULE(GoalFeatureDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(FrameInfo)

  PROVIDE(GoalFeaturePercept)
  PROVIDE(GoalFeaturePerceptTop)
END_DECLARE_MODULE(GoalFeatureDetector)


class GoalFeatureDetector: private GoalFeatureDetectorBase
{
public:

  GoalFeatureDetector();
  virtual ~GoalFeatureDetector(){}

  // override the Module execute method
  virtual bool execute(CameraInfo::CameraID id, bool horizon = true);

  void execute()
  {
    bool topScanned = execute(CameraInfo::Top);

    if(!topScanned) {
      execute(CameraInfo::Top, false);
      if( !execute(CameraInfo::Bottom)) {
        execute(CameraInfo::Bottom, false);
      }
    }
  }

private:
  static const int imageBorderOffset = 5;
  CameraInfo::CameraID cameraID;

  RingBuffer<Vector2i, 5> pointBuffer;
  RingBufferWithSum<double, 5> valueBuffer;
  RingBufferWithSum<double, 5> valueBufferY;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("GoalFeatureParameters")
    {
      PARAMETER_REGISTER(numberOfScanlines) = 5;
      PARAMETER_REGISTER(scanlinesDistance) = 6;
      PARAMETER_REGISTER(thresholdUV) = 60;
      PARAMETER_REGISTER(thresholdY) = 140;
      PARAMETER_REGISTER(colorRegionDeviation) = 2;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    virtual ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int numberOfScanlines;
    int scanlinesDistance;
    int thresholdUV;
    int thresholdY;
    double colorRegionDeviation;
  };


private:

  Parameters params;

  template<typename V, int SIZE>
  class Diff {
    public: inline V operator()(const RingBuffer<V, SIZE>& values) const {
      return (-values[4] - 2.0*values[3] + 0.0*values[2] + 2.0*values[1] + values[0])/3.0;
    }
  };

  template<typename V, int SIZE>
  class Gauss {
    public: inline V operator()(const RingBuffer<V, SIZE>& values) const {
      return (values[4] + 2.0*values[3] + 4.0*values[2] + 2.0*values[1] + values[0])/10.0;
    }
  };

  template<template<typename V, int SIZE> class F, class T, typename V>
  class Filter
  {
  private:
    static const int SIZE = 5;
    RingBuffer<T, SIZE> pointBuffer;
    RingBuffer<V, SIZE> valueBuffer;
    F<V,SIZE> function;

  public:
    inline bool ready() const {
      return pointBuffer.isFull();
    }

    inline void add(const T& point, V value) {
      pointBuffer.add(point);
      valueBuffer.add(value);
    }

    inline V value() const {
      ASSERT(valueBuffer.isFull());
      //return 0.1*(-valueBuffer[4] - 2.0*valueBuffer[3] + 0.0*valueBuffer[2] + 2.0*valueBuffer[1] + valueBuffer[0]);
      return function(valueBuffer);
    }

    inline const Vector2i& point() const {
      return pointBuffer[2];
    }
  };

  void findfeaturesColor(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY);
  void findfeatures(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY);
  Vector2d calculateGradientUV(const Vector2i& point) const;

  double edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2) const
  {
    double s = 0.0;
    if(e1.direction*e2.direction > 0) {
      Vector2d v = (e2.point - e1.point).rotateRight().normalize();
      s = 1.0-0.5*(fabs(e1.direction*v) + fabs(e2.direction*v));
    }

    return s;
  }

private:
  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, Image);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, FieldPercept);

  DOUBLE_CAM_PROVIDE(GoalFeatureDetector, GoalFeaturePercept);

};//end class GoalFeatureDetector

#endif // _GoalFeatureDetector_H_
