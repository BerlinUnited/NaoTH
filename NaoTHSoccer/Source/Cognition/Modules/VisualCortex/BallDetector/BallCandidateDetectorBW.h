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
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/GameColorIntegralImage.h"
#include "Representations/Perception/BodyContour.h"

#include "Representations/Perception/MultiBallPercept.h"

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
  PROVIDE(GameColorIntegralImage)
  PROVIDE(GameColorIntegralImageTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  
  PROVIDE(MultiBallPercept)
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
  int index;
  void subsampling(int x0, int y0, int x1, int y1);
  double sub_img[12*12];

  double estimatedBallRadius(int x, int y) const;
  void addBallPercept(const Vector2i& center, double radius);

  double isBall() 
  {
    /*
    double b = 900;
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
    };*/

    /*
    double b_svm2 = -6.2149751;
    double c_svm2[12*12] = { 0.13789616, 0.06172048, 0.04543606, -0.12398522, -0.04407964, 0.01638802,
-0.10503339, 0.04472641, -0.03867624, 0.11467709, -0.09542641, 0.0236597,
-0.04257897, -0.15340628, 0.02081758, 0.08289126, 0.06104639, -0.02064278,
-0.0036993, -0.05428068, -0.01207813, -0.00661843, 0.01949434, -0.03245114,
-0.11753394, 0.09403242, 0.02195637, -0.01846779, -0.03609667, 0.00869618,
-0.00085385, 0.06080788, 0.05548694, -0.03419191, 0.0151479, 0.03268683,
-0.02608968, -0.04344729, -0.00688843, -0.04503747, 0.01612292, 0.00585518,
0.04060104, -0.09187251, 0.00043233, -0.02098516, -0.02604592, -0.04201949,
-0.04253088, -0.00708971, -0.07039335, 0.04155514, 0.01406311, 0.03099999,
0.04979319, 0.08748386, -0.0341797, 0.02779022, 0.01920887, -0.07627703,
0.05263554, 0.06880526, 0.1543968, -0.04539096, -0.01490764, -0.03085424,
-0.00534079, 0.00906134, -0.03196349, 0.05902785, -0.07472241, 0.02295511,
-0.0220574, 0.01292559, -0.00121304, 0.09848714, 0.04637989, 0.01764909,
0.05951965, -0.02674418, -0.02150457, -0.0287339, 0.09527924, -0.05007134,
-0.04331368, 0.02651598, -0.04490681, -0.09407336, 0.03033152, 0.01736122,
-0.00634055, -0.00261435, 0.04146817, -0.01414771, -0.0520274, 0.07365744,
-0.03399328, -0.03259506, 0.01455693, -0.00322974, 0.02144148, -0.07832799,
0.05998801, -0.02926449, -0.00057815, -0.00300587, -0.06242656, -0.02893527,
-0.00595156, -0.11794544, -0.01143503, 0.02714461, -0.01494841, -0.02011067,
0.01111819, 0.00899911, 0.02655137, -0.00101183, 0.09432822, -0.1048723,
0.00301101, 0.01987474, 0.02232157, 0.08513602, 0.01245202, -0.01134464,
0.0037978, -0.03438385, 0.03088292, 0.04572529, 0.11515793, -0.05131118,
0.09982609, -0.02239204, -0.05394491, -0.11670343, 0.07271566, -0.00459625,
-0.08908211, 0.09441295, -0.00605243, -0.0644505, -0.01011307, 0.01339462};
*/

    double b_svm = 34.72901446;
    double c_svm[12*12] = {
158.11387264, 802.06390968, 1216.63388592, -12.26084957, 634.75439986, -651.61306802, -595.9283762, -33.9727707, -728.7542466, -106.26069631, -245.47242586, -26.82060844, -1091.72648088, -531.8143503, 367.05918414, -309.3310174, 488.90137679, -445.73296891, -443.94492835, -720.32491251, -1243.19906, -1208.71542057, -371.65700273, -340.2385757, -1422.51398503, -458.24925285, -347.39073795, -351.22225344, 642.67286521, 299.87994585, 1273.59574957, 1311.14460139, 418.6569261, -186.21165292, -190.55403714, -792.61283813, -2006.94781476, -1490.97039516, -308.82014866, -1067.97108483, 668.98260492, 426.83082582, 452.37426243, 294.51582416, -318.52665458, -143.04324504, -215.33117066, -1150.73181946, -1745.38302383, -657.74349281, 274.84737797, -401.54282357, 526.96109735, 449.81991877, 1276.15009323, 968.6071164, -502.18396383, 501.6730951, 44.19014534, -972.18319753, -1657.76903625, 554.80344326, 1086.61779356, 79.44008787, 776.2650387, 476.64052722, 1788.04056298, 509.59156045, -400.77652047, 1057.49827582, 23.49996168-1125.95468594, -2048.58361644, -196.68446193, 1175.25351861, 223.50507037, 686.35214182, 937.44412373, 1309.10112647, 456.97208102, 382.64068048, 772.94439194, 460.80359652, -712.40644716, -1426.85636926, -353.26572837, 408.69498582, -131.2932642, 168.58668165, 220.95072671, 1230.17190733, 735.39554012, 135.89108279, 50.32057013, -536.15673453, -631.68918746, -981.63426907, -920.84088993, 276.63541853, -686.60757618, 71.77705689, -424.53191652, 605.12401338, 1159.92745664, 166.79864109, -140.23346701, -815.34649672, -1317.01959182, -869.24314797, -1330.55761322, -636.03157169, -50.83143886, 530.28174411, -831.18342742, 219.16268615, 459.52642469, 38.82602365, 1143.32422284, 44.19014534, -1172.44374058, 193.8746839, 111.88025237, 298.34733965, -415.59171371, 830.16168995, -14.04889014, -25.03256788, 958.13430739, 843.44427699, 1618.17670949, 630.92288436, -109.58134307, 506.52634805, 139.46716391, 201.28228052, -664.12935196, 401.28738921-1217.65562339, -1225.06322001, -183.65730925, -249.30394135, 583.41209226, 184.42361235, -345.85813175
    };

    double v = b_svm;
    for(int i = 0; i < 12*12; ++i) {
      v += sub_img[i]*c_svm[i];
    }

    return (atan(v)+Math::pi_2)/Math::pi;//v > 0;
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
  DOUBLE_CAM_PROVIDE(BallCandidateDetectorBW, GameColorIntegralImage);
          
};//end class BallCandidateDetectorBW

#endif // _BallCandidateDetectorBW_H_
