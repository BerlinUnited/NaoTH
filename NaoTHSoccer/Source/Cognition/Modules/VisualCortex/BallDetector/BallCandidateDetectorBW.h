/**
* @file BallCandidateDetectorBW.h
*
* Definition of class BallCandidateDetectorBW
*/

#ifndef _BallCandidateDetectorBW_H_
#define _BallCandidateDetectorBW_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>

#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/BodyContour.h"

// tools
#include "Tools/DoubleCamHelpers.h"

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"


BEGIN_DECLARE_MODULE(BallCandidateDetectorBW)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  
  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
END_DECLARE_MODULE(BallCandidateDetectorBW)


template <typename Iter>
Iter nextIter(Iter iter)
{
    return ++iter;
}

class BallCandidateDetectorBW: private BallCandidateDetectorBWBase
{
public:
  BallCandidateDetectorBW();
  ~BallCandidateDetectorBW();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

private:
  void integralBild();
  static const int FACTOR = 4;
  int integralImage[640/FACTOR][480/FACTOR];

  int getIntegral(int x0, int y0, int x1, int y1) {
    x0 = std::max(x0-1, 0);
    y0 = std::max(y0-1, 0);
    return integralImage[x1][y1] + integralImage[x0][y0] - integralImage[x0][y1] - integralImage[x1][y0];
  }

  double estimatedBallRadius(const Vector2i& point) const;

  class Best {
  public:

    class BallCandidate 
    {
    public:
      Vector2i center;
      double radius;
      double value;
    };

    std::list<BallCandidate> candidates;

    void add(const Vector2i& center, double radius, double value)
    {
      if(candidates.empty()) {
        candidates.push_back(BallCandidate());
        candidates.back().center = center;
        candidates.back().radius = radius;
        candidates.back().value = value;
      }


      bool stop = false;
      for (std::list<BallCandidate>::iterator i = candidates.begin(); i != candidates.end(); /*nothing*/)
      {
        if (std::max(std::abs((*i).center.x - center.x), std::abs((*i).center.y - center.y)) < ((*i).radius + radius)) {
          if(value > (*i).value) {
            i = candidates.erase(i);
          } else {
            stop = true;
            ++i;
          }
        } else {
          ++i;
        }
      }
      if(stop) {
        return;
      }

      for(std::list<BallCandidate>::iterator i = candidates.begin(); i != candidates.end(); ++i)
      {
        // insert
        if(value < (*i).value) {
          i = candidates.insert(i,BallCandidate());
          (*i).center = center;
          (*i).radius = radius;
          (*i).value = value;
          break;
        } else if(nextIter(i) == candidates.end()) 
        {
          candidates.push_back(BallCandidate());
          
          candidates.back().center = center;
          candidates.back().radius = radius;
          candidates.back().value = value;
          break;
        }
      }


      if(candidates.size() > 30) {
        //candidates.pop_front();
      }
    }

    void clear() {
      candidates.clear();
    }

  } best;

private:     
  
  DOUBLE_CAM_PROVIDE(BallCandidateDetectorBW, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BallCandidateDetectorBW, Image);
  DOUBLE_CAM_REQUIRE(BallCandidateDetectorBW, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BallCandidateDetectorBW, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(BallCandidateDetectorBW, FieldPercept);
  DOUBLE_CAM_REQUIRE(BallCandidateDetectorBW, BodyContour);
 
  DOUBLE_CAM_PROVIDE(BallCandidateDetectorBW, BallPercept);
          
};//end class BallCandidateDetectorBW

#endif // _BallCandidateDetectorBW_H_
