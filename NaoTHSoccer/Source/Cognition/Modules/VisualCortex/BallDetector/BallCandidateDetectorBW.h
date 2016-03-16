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
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"


BEGIN_DECLARE_MODULE(BallCandidateDetectorBW)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugPlot)
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

  int index;
  void subsampling(int x0, int y0, int x1, int y1);
  double sub_img[12*12];

  double estimatedBallRadius(int x, int y) const;

  bool isBall() 
  {
    double b = 90;
    double c[12*12] = { 
 25932,  15702,   6846, -17953,   3066,  -3821, -12728,  -1612,  -6238,
  7186,  -5509,   3443, -13033, -16871,   7410,   6766,   7571,  -8641,
-10242,  -6562,  -4854, -13567,   -250,  -3972,  -9834,   1721,    518,
 -1857,  -1509,   7285,  11934,   7264,  11275,  -6136,    177,  -1781,
-23625, -12539,  -3232, -11804,  12280,   6689,   7446, -11715,  -6371,
 -3527,  -1579, -17239, -10549,  -4112,   2923,  -6522,   -179,   5383,
 15129,  20456,  -6837,  10723,   4385, -11563,   -526,  26149,  23608,
 -2060,   5365,  -1540,  18612,    814,  -2486,   5316,  -7532,  -7963,
-20148,   4374,   5292,   2913,   2118,   6617,   8751,  -5245,   3941,
 -1475,   4954,  -3169,  -7713,   5451,  -5658,  -7889,   2540,    400,
 -2975,   1468,   3338,    863, -11192,   5592,  -1816,  -6959,   4615,
 -2991,  -4397, -15118,   9835,   5563,   -990,  -4563,  -5550,  -8629,
 -6289, -24550,  -6739,   5755,  -1584, -11495,  -3725,   5236,   -725,
 11885,   5788, -21785,   7068,  -3199,  -4721,  10658,  10446,   6066,
 -5729,   4549,   8097,  20449,  15605,  -4057,  16670,   1699,  -6436,
 -8770,   8313, -15157, -10953,   4903,  -2674,   -283,  -4027,  -5497
    };

    double v = b;
    for(int i = 0; i < 12*12; ++i) {
      v += sub_img[i]*c[i];
    }

    return v > 0;
  }

  class Best {
  public:

    class BallCandidate 
    {
    public:
      BallCandidate() : radius(-1), value(-1) {}
      BallCandidate(const Vector2i& center, double radius, double value) 
        : center(center), radius(radius), value(value)
      {}
      Vector2i center;
      double radius;
      double value;
    };

    std::list<BallCandidate> candidates;

    void add(const Vector2i& center, double radius, double value)
    {
      if(candidates.empty()) {
        candidates.push_back(BallCandidate(center, radius, value));
        return;
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
          i = candidates.insert(i,BallCandidate(center, radius, value));
          break;
        } else if(nextIter(i) == candidates.end()) {
          candidates.push_back(BallCandidate(center, radius, value));
          break;
        }
      }

      /*
      if(candidates.size() > 30) {
        candidates.pop_front();
      }
      */
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
