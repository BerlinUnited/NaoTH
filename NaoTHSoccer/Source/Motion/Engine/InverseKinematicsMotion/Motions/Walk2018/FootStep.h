/**
* @file FootStep.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class foot step for walking
*/

#ifndef _FOOTSTEP_H
#define _FOOTSTEP_H

#include <Tools/Math/Pose2D.h>
#include "../IKPose.h"
#include "Tools/Debug/DebugDrawings.h"

class FootStep
{
public:
  enum Foot {
    LEFT = 1,
    NONE = 0,
    RIGHT = -LEFT
  };

  FootStep(){}

  FootStep(const InverseKinematic::FeetPose& feet, Foot liftingFoot):
    theLiftingFoot(liftingFoot)
  {
    switch(theLiftingFoot)
    {
      case LEFT:
      {
        theFootBegin = feet.left;
        theFootEnd = feet.left;
        theSupFoot = feet.right;
        break;
      }
      case RIGHT:
      case NONE:
      {
        theFootBegin = feet.right;
        theFootEnd = feet.right;
        theSupFoot = feet.left;
        break;
      }
    }
  }

  InverseKinematic::FeetPose begin() const
  {
    InverseKinematic::FeetPose feet;

    switch(theLiftingFoot)
    {
      case LEFT:
      {
        feet.left = theFootBegin;
        feet.right = theSupFoot;
        break;
      }
      case RIGHT:
      case NONE:
      {
        feet.left = theSupFoot;
        feet.right = theFootBegin;
      }
    }
    return feet;
  }//end begin

  InverseKinematic::FeetPose end() const
  {
    InverseKinematic::FeetPose feet;

    switch(theLiftingFoot)
    {
      case LEFT:
      {
        feet.left = theFootEnd;
        feet.right = theSupFoot;
        break;
      }
      case RIGHT:
      case NONE:
      {
        feet.left = theSupFoot;
        feet.right = theFootEnd;
      }
    }
    return feet;
  }//end end

  // TODO
  void draw(DrawingCanvas2D& canvas) const
  {
    // foot begin
    canvas.translate(theFootBegin.translation.x, theFootBegin.translation.y);
    canvas.rotate(theFootBegin.rotation.getZAngle());
    canvas.pen(Color::BLUE, 1.0);
    canvas.drawBox(-50, -30, 50, 30);
    canvas.rotate(-theFootBegin.rotation.getZAngle());
    canvas.translate(-theFootBegin.translation.x, -theFootBegin.translation.y);

    // foot end
    canvas.translate(theFootEnd.translation.x, theFootEnd.translation.y);
    canvas.rotate(theFootEnd.rotation.getZAngle());
    canvas.pen(Color::RED, 10.0);
    canvas.drawBox(-50, -30, 50, 30);
    canvas.rotate(-theFootEnd.rotation.getZAngle());
    canvas.translate(-theFootEnd.translation.x, -theFootEnd.translation.y);
  }//end draw

   Foot liftingFoot() const { return theLiftingFoot; }

   const Pose3D& footBegin() const { return theFootBegin; }
   const Pose3D& footEnd() const { return theFootEnd; }
   Pose3D& footEnd() { return theFootEnd; }
   const Pose3D& supFoot() const { return theSupFoot; }
   // hack...
   Pose3D& supFoot() { return theSupFoot; }

   // calculated and used by the FootStepPlanner
   const Pose2D& stepRequest() const { return theStepRequest; }
   Pose2D& stepRequest() { return theStepRequest; }

   const Pose2D& offset() const { return theOffset; }
   Pose2D& offset() { return theOffset; }

private:
   Foot theLiftingFoot;
   Pose3D theFootBegin, theFootEnd, theSupFoot;

   Pose2D theStepRequest;
   Pose2D theOffset;
};

#endif  /* _FOOTSTEP_H */

