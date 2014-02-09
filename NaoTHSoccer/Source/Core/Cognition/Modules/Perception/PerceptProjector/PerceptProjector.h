/**
* @file PerceptProjector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PerceptProjector
*/

#ifndef _PerceptProjector_h_
#define _PerceptProjector_h_

#include <ModuleFramework/Module.h>



#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/CameraMatrixOffset.h"
#include "Representations/Modeling/GoalModel.h"

#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

// Tools
#include "Tools/DataStructures/RingBufferWithSum.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(PerceptProjector)
  REQUIRE(Image)
  REQUIRE(CameraInfo)
  REQUIRE(FieldInfo)
  REQUIRE(SensingGoalModel)
  
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)

  PROVIDE(CameraMatrixOffset)
  PROVIDE(GoalPercept)
  PROVIDE(LinePercept)
  PROVIDE(BallPercept)
END_DECLARE_MODULE(PerceptProjector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class PerceptProjector: public PerceptProjectorBase
{
public:

  PerceptProjector();
  virtual ~PerceptProjector();

  /** executes the module */
  void execute();

private:

  /** */
  bool correct_the_line_percept(
    Vector2d& offset,
    const Math::LineSegment& lineOne,
    const Math::LineSegment& lineTwo) const;

  /** */
  double projectionErrorLine(
    double offsetX,
    double offsetY,
    const Math::LineSegment& lineOne,
    const Math::LineSegment& lineTwo) const;

  double projectionErrorDoubleEdgel(
    double offsetX,
    double offsetY, 
    const DoubleEdgel& edgel) const;

  double operator ()(double x, double y, float /*nil*/) const
  {
    if(getLinePercept().lines.size() > 1)
    {
      // take two random lines
      // (if they are the same no correction is made)
      int idx1 = Math::random((int)getLinePercept().lines.size());
      int idx2 = Math::random((int)getLinePercept().lines.size());
    
      if(idx1 != idx2 &&
          getLinePercept().lines[idx1].type != LinePercept::C &&
          getLinePercept().lines[idx2].type != LinePercept::C)
      {
        return projectionErrorLine(x, y, 
          getLinePercept().lines[idx1].lineInImage.segment, 
          getLinePercept().lines[idx2].lineInImage.segment);
      }
    }

    return 0;
  }

  int last_iteration;
  int last_idx;
  double operator ()(double x, double y, int i)
  {
    if(getScanLineEdgelPerceptTop().numOfSeenEdgels > 10)
    {
      // take two random lines
      // (if they are the same no correction is made)
      if(last_iteration != i) {
        int idx = Math::random((int)getScanLineEdgelPerceptTop().numOfSeenEdgels);
        const DoubleEdgel& p = getScanLineEdgelPerceptTop().scanLineEdgels[idx];
        if((p.begin - p.end).abs2() > 8) {
          last_idx = idx;
        }
      }

      last_iteration = i;
      if(last_idx >= 0 && last_idx < (int)getScanLineEdgelPerceptTop().numOfSeenEdgels) {
        return projectionErrorDoubleEdgel(x, y, getScanLineEdgelPerceptTop().scanLineEdgels[last_idx]);
      }
    }

    last_iteration = i;
    return 0;
  }

  template<class G>
  static bool solveGN21(Vector2d& offset, int iterations, G& g)
  {
    const double epsylon = 1e-8;
    const double minStepSize = Math::fromDegrees(0.1);

    for (int i = 0; i < iterations; i++)
    {
      // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
      double dg11 = g(offset.x + epsylon, offset.y,i);
      double dg12 = g(offset.x - epsylon, offset.y,i);
      double dg1 = (dg11 - dg12) / (2 * epsylon);

      double dg21 = g(offset.x, offset.y + epsylon,i);
      double dg22 = g(offset.x, offset.y - epsylon,i);
      double dg2 = (dg21 - dg22) / (2 * epsylon);

      // Dg(x)^T*Dg(x)
      Vector2d dg(dg1, dg2);

      // derivative too small steps
      if (dg.abs() < epsylon) break;

      // g(x) - target
      double w = g(offset.x, offset.y, i);

      // error while calculating projectionErrorLine
      if(w == -1) return false;

      ASSERT(w > 0 && !Math::isNan(w));

      //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
      Vector2d z_GN = dg * (-w / (dg * dg));
      offset += z_GN;

      // correction step too small
      if(z_GN.abs() < minStepSize) break;
    }//end for

    return true;
  }

  /** */
  RingBufferWithSum<Vector2d, 100> offsetBuffer;
};

#endif //_PerceptProjector_h_
