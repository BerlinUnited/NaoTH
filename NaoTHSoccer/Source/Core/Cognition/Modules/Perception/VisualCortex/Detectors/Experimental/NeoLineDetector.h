/* 
 * File:   NeoLineDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _NeoLineDetector_H_
#define _NeoLineDetector_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Tools/ImageProcessing/Edgel.h"


// Representations
#include "Representations/Perception/ScanLineEdgelPercept.h"


#include <Tools/DataStructures/ParameterList.h>



class EdgelCluster
{
private:
  // parameter for hessian normal form
  Vector2d angleSum;
  double dSum;
  int _id;

public:
  EdgelCluster()
    :
    dSum(0),
    _id(-1)
  {
  }

  EdgelCluster(int id)
    :
    _id(id)
  {
  }

  ~EdgelCluster(){}

  void add(const Edgel& other)
  {
    // todo: this is dumb
    for(size_t i = 0; i < edgels.size(); i++)
    {
      if(edgels[i].point == other.point) {
        return;
      }
    }
    edgels.push_back(other);
    if(edgels.size() == 1) {
      start = other.point;
      end = other.point;
    }
    else if((start - other.point).abs2() > (start - end).abs2()) {
      end = other.point;
    } else if((end - other.point).abs2() > (start - end).abs2()) {
      start = other.point;
    }
  }

  void draw()
  {
    // todo: this is dumb
    for(size_t i = 1; i < edgels.size(); i++)
    {
      int idx = id() % (unsigned int)ColorClasses::numOfColors;
      LINE_PX((ColorClasses::Color) (idx), 
        edgels[i-1].point.x, edgels[i-1].point.y, edgels[i].point.x, edgels[i].point.y);
      CIRCLE_PX((ColorClasses::Color) (idx),start.x, start.y, 5);
      CIRCLE_PX((ColorClasses::Color) (idx),end.x, end.y, 5);
    }
  }


  int id(){ return _id; }
  size_t size(){ return edgels.size(); }
  double length(){ return (start - end).abs2(); }

  std::vector<Edgel> edgels;

  double angle;
  Vector2<int> start;
  Vector2<int> end;
};




BEGIN_DECLARE_MODULE(NeoLineDetector)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
END_DECLARE_MODULE(NeoLineDetector)


class NeoLineDetector : private NeoLineDetectorBase
{
public:
  NeoLineDetector();
  virtual ~NeoLineDetector();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("NeoLineDetector")
    {
      PARAMETER_REGISTER(x) = 6;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int x;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;
  

  const ScanLineEdgelPercept& getScanLineEdgelPercept()
  {
    if(cameraID == CameraInfo::Top) {
      return NeoLineDetectorBase::getScanLineEdgelPerceptTop();
    } else {
      return NeoLineDetectorBase::getScanLineEdgelPercept();
    }
  }
};

#endif  /* _NeoLineDetector_H_ */

