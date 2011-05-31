/**
* @file FootStep.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class foot step for walking
*/

#ifndef _FOOTSTEP_H
#define	_FOOTSTEP_H

#include <Tools/Math/Pose2D.h>
#include "../IKPose.h"

class FootStep
{
public:
  enum Foot {
    LEFT = 1,
    RIGHT = -LEFT
  };

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
      {
        theFootBegin = feet.right;
        theFootEnd = feet.right;
        theSupFoot = feet.left;
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
      {
        feet.left = theSupFoot;
        feet.right = theFootEnd;
      }
    }
    return feet;
  }//end end

  // TODO
  /*
  void draw() const
  {
    // foot begin
    TRANSLATION(footBegin.translation.x, footBegin.translation.y);
    ROTATION(footBegin.rotation.getZAngle());
    ARROW(0,0,100,0);
    ROTATION(-footBegin.rotation.getZAngle());
    TRANSLATION(-footBegin.translation.x, -footBegin.translation.y);

    // foot end
    TRANSLATION(footEnd.translation.x, footEnd.translation.y);
    ROTATION(footEnd.rotation.getZAngle());
    ARROW(0,0,100,0);
    ROTATION(-footEnd.rotation.getZAngle());
    TRANSLATION(-footEnd.translation.x, -footEnd.translation.y);
  }//end draw
   */
   
   Foot liftingFoot() const { return theLiftingFoot; }
   
private:
   Foot theLiftingFoot;
   Pose3D theFootBegin, theFootEnd, theSupFoot;
};

#endif	/* _FOOTSTEP_H */

