/**
* @file ClusteredLine.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of class ClusteredLine
*/

#include "Edgel.h"
#include "LineDetectorConstParameters.h"

#ifndef __ClusteredLine_H_
#define __ClusteredLine_H_

class ClusteredLine
{
private:
  // parameter for hessian normal form
  Vector2<double> angleSum;

  double thicknessSum;
  int clusterId;

public:
  ClusteredLine()
    :
    thicknessSum(0.0),
    clusterId(-1),
    count(0),
    vertical(false)
  {
  }

  ClusteredLine(Edgel edgel, int clusterId)
    :
    clusterId(clusterId)
  {
    edgels[0] = edgel;
    beginAngle = edgel.begin_angle;
    endAngle = edgel.end_angle;
    angle = edgel.center_angle;
    start = edgel.center;
    end = edgel.center;

    angleSum = Vector2<double>(1,0).rotate(angle);

    beginThickness = edgel.thickness;
    endThickness = beginThickness;
    thickness = beginThickness;
    thicknessSum = thickness;
    count = 1;

    angleChange = 0.0;
    beginAngleChange = 0.0;
    endAngleChange = 0.0;
    thicknessChange = 0.0;

  }

  ~ClusteredLine(){}

  bool weightedCheck(const Edgel& edgel)
  {
    int goodParams = 0;

    if(count >= MAX_NUMBER_OF_EDGELS_LINE_SEGMENT)
    {
      return false;
    }

    double thicknessDiff;

    if(edgels[count - 1].thickness < edgel.thickness)
    {
      thicknessDiff = fabs(edgels[count - 1].thickness / edgel.thickness);
    }
    else
    {
      thicknessDiff = fabs(edgel.thickness / edgels[count - 1].thickness );
    }

    if(fabs(thicknessDiff - thicknessChange) < 0.20)
    {
      goodParams++;
    }

    // the normal vector of the line in the hessian normalform
    Vector2<double> lineNormal;
    // distance parameter for the hessian normalform
    double d = 0;

    if(count > 1)
    {
      lineNormal = Vector2<double>(end - start).normalize().rotateLeft();
      d = lineNormal * Vector2<double>(end + start) * 0.5;
   }
    else
    {
      lineNormal = Vector2<double>(edgel.center - start).normalize().rotateLeft();
      // distance parameter for the hessian normalform
      d = lineNormal*Vector2<double>(edgel.center + start) * 0.5;
    }

    // distance of the edgel.center to the line
    double distance = lineNormal*Vector2<double>(edgel.center);
    if(fabs(distance - d) < 2.0)
    {
      goodParams++;
    }
    else
    {
      goodParams--;
    }

    // vector in the line direction
    Vector2<double> lineDirection(Vector2<double>(lineNormal).rotateRight());
    double lineDirectionAngle = Math::normalizePositive(lineDirection.angle());
    double lineSegmentDirectionAngle = Math::normalizePositive((edgel.center - edgels[count - 1].center).angle());

    double beginAngleDiff = (edgel.begin_angle - edgels[count - 1].begin_angle);
    double centerAngleDiff = (edgel.center_angle - edgels[count - 1].center_angle);
    double endAngleDiff = (edgel.end_angle - edgels[count - 1].end_angle);

    if(fabs(beginAngleDiff - beginAngleChange) < MAX_LINE_ANGLE_DIFF)
    {
      if(edgel.thickness >= 3.0)
      {
        goodParams++;
      }
    }
    else
    {
      goodParams--;
    }
    if(fabs(centerAngleDiff - angleChange) < MAX_LINE_ANGLE_DIFF)
    {
      goodParams++;
    }
    else
    {
      goodParams--;
    }
    if(fabs(endAngleDiff - endAngleChange) < MAX_LINE_ANGLE_DIFF)
    {
      if(edgel.thickness >= 3.0)
      {
        goodParams++;
      }
    }
    else
    {
      goodParams--;
    }

    if((fabs(lineDirectionAngle - edgel.center_angle)) < MAX_LINE_ANGLE_DIFF)
    {
      goodParams++;
    }
    else
    {
      goodParams--;
    }

    if((fabs(lineSegmentDirectionAngle - edgel.center_angle)) < MAX_LINE_ANGLE_DIFF)
    {
      goodParams++;
    }
    else
    {
      goodParams -= 6;
    }


    if(edgel.vertical)
    {
      if(edgels[count - 1].vertical)
      {
        goodParams++;
      }
      else
      {
        goodParams--;
      }
      if(edgels[count - 1].ScanLineID == edgel.ScanLineID)
      {
        goodParams++;
      }
      if(edgels[count - 1].runID != edgel.runID)
      {
        goodParams++;
      }
    }
    else
    {
      if(!edgels[count - 1].vertical)
      {
        goodParams++;
      }
      else
      {
        goodParams--;
      }
      if(abs((int) (edgels[count - 1].ScanLineID - edgel.ScanLineID)) == 1)
      {
        goodParams++;
      }
      if(abs((int) (edgels[count - 1].ScanLineID - edgel.ScanLineID)) > 2)
      {
        goodParams--;
      }
      if(edgels[count - 1].ScanLineID == edgel.ScanLineID)
      {
        goodParams--;
      }
      if(edgels[count - 1].runID == edgel.runID)
      {
       goodParams++;
      }
      else
      {
        goodParams--;
      }

    }

    if(goodParams >= 6)
    {
      double countInv = 1 / count;
      thicknessChange *= (1 - countInv);
      thicknessChange += countInv * thicknessDiff;
      beginAngleChange *= (1 - countInv);
      beginAngleChange += countInv * beginAngleDiff;
      angleChange *= (1 - countInv);
      angleChange += countInv * centerAngleDiff;
      endAngleChange *= (1 - countInv);
      endAngleChange += countInv * endAngleDiff;

      return true;
    }
    return false;
  }//end weightedCheck


  bool add(const Edgel& edgel)
  {
    if(weightedCheck(edgel))
    {
      end = edgel.center;
      endThickness = edgel.thickness;

      angleSum += Vector2<double>(1,0).rotate(edgel.center_angle);

      Vector2<double> direction = angleSum;
      direction.normalize();

      thicknessSum += edgel.thickness;

      if(count < 1)
      {
        vertical = edgel.vertical;
      }
      edgels[count++] = edgel;
      thickness = thicknessSum / count;
      angle = direction.angle();
      return true;
    }

    return false;
  }//end add

  int id(){ return clusterId; }

  Edgel edgels[MAX_NUMBER_OF_EDGELS_LINE_SEGMENT];
  int count;
  double angle;
  double beginAngle;
  double endAngle;
  double angleChange;
  double beginAngleChange;
  double endAngleChange;
  double thickness;
  double beginThickness;
  double endThickness;
  double thicknessChange;

  bool vertical;
  Vector2<int> start;
  Vector2<int> end;
//  double hDist;
};

#endif //__ClusteredLine_H_
