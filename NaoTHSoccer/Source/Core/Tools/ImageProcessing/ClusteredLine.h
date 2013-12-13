/**
* @file ClusteredLine.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of class ClusteredLine
*/

#include "Edgel.h"
#include "Cognition/Modules/Perception/VisualCortex/Detectors/LineDetectorConstParameters.h"

#ifndef _ClusteredLine_H_
#define _ClusteredLine_H_

class ClusteredLine
{
private:
  // parameter for hessian normal form
  Vector2<double> angleSum;
  double dSum;

  double thicknessSum;
  int clusterId;

public:
  ClusteredLine()
    :
    dSum(0),
    thicknessSum(0.0),
    clusterId(-1),
    count(0)
  {
  };

  ClusteredLine(DoubleEdgel edgel, int clusterId)
    :
    clusterId(clusterId)
  {
    edgels[0] = edgel;
    angle = edgel.center_angle;
    start = edgel.center;
    end = edgel.center;

    angleSum = Vector2<double>(1,0).rotate(angle);
    Vector2<double> normal(angleSum);

    dSum = normal.rotateLeft() * edgel.center;

    thickness = fabs(((double)(edgel.begin - edgel.end).y) * sin(Math::pi_2 - edgel.center_angle));
    thicknessSum = thickness;
    count = 1;
  };

  ~ClusteredLine(){}

  bool check(const DoubleEdgel& edgel) const
  {
    //
    double edgelThickness = fabs(((double)(edgel.begin - edgel.end).y) * sin(Math::pi_2 - edgel.center_angle));
    
    // the normal vector of the line in the hessian normalform
    Vector2<double> lineNormal;
    // distance parameter for the hessian normalform
    double d = 0;

    if(count > 1)
    {
      lineNormal = Vector2<double>(end - start).normalize().rotateLeft();
      d = lineNormal * Vector2<double>(end + start)*0.5;
    }
    else
    {
      lineNormal = Vector2<double>(edgel.center - start).normalize().rotateLeft();

      // distance parameter for the hessian normalform
      d = lineNormal*Vector2<double>(edgel.center + start)*0.5;

      // check the id of the scanline
      if
      (
        //edgels[count - 1].runID != edgel.runID
        //  ||
        abs((int) (edgels[count - 1].ScanLineID - edgel.ScanLineID)) > 1
      )
      {
        return false;
      }
    }

    // distance of the edgel.center to the line
    double distance = lineNormal*Vector2<double>(edgel.center);

    // vector in the line direction
    Vector2<double> lineDirection(Vector2<double>(lineNormal).rotateRight());


    return
    (
      count < SCANLINE_COUNT * (SCANLINE_RESUME_COUNT + 1)
        &&
      // compare with the line angle
      fabs(Math::normalize(lineDirection.angle() - edgel.center_angle)) < MAX_LINE_ANGLE_DIFF
        &&
      // compare the distance to the line
      fabs(distance - d) < 2.0
        &&
      fabs(thickness - edgelThickness) / thickness < 0.4
    );
  }//end check

  bool add(const DoubleEdgel& edgel)
  {
    if(check(edgel))
    {
      //TODO: don't repeat the calculation
      double edgelThickness = fabs(((double)(edgel.begin - edgel.end).y) * sin(Math::pi_2 - edgel.center_angle));

      end = edgel.center;
      
      angleSum += Vector2<double>(1,0).rotate(edgel.center_angle);
      
      Vector2<double> direction = angleSum;
      direction.normalize();

      dSum += direction.rotateLeft() * edgel.center;
      thicknessSum += edgelThickness;
      
      edgels[count++] = edgel;
      thickness = thicknessSum / count;
      angle = direction.angle();
      return true;
    }

    return false;
  }//end add

  int id(){ return clusterId; }

  DoubleEdgel edgels[SCANLINE_COUNT * (SCANLINE_RESUME_COUNT + 1)];
  int count;
  double angle;
  double thickness;
  Vector2<int> start;
  Vector2<int> end;
//  double hDist;
};

#endif //_ClusteredLine_H_
