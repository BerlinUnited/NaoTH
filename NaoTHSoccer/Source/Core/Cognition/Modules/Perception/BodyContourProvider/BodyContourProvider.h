/*
*@file BodyContourProvider.h
* Module declaration for providing the robot's contour in the image. 
* The Body Contour is used to exclude robot's limbs from image, so that they
* couldn't be confused with other objects
*
*@author Kirill Yasinovskiy
*/

#ifndef _BodyContourProvider_h_
#define _BodyContourProvider_h_

#include <ModuleFramework/Module.h>

#include <vector>

//Tools
#include "Tools/DataStructures/Printable.h"

//Representations
#include "Representations/Perception/BodyContour.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Perception/CameraMatrix.h"
//#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/Image.h"
#include <vector>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(BodyContourProvider)
  REQUIRE(SensorJointData)
  REQUIRE(KinematicChain)
  REQUIRE(CameraMatrix)
  REQUIRE(Image)
  PROVIDE(BodyContour)
END_DECLARE_MODULE(BodyContourProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

/**
* @class BodyContourProvider
* A module that provides the contour of the robot's body in the image.
*/

class BodyContourProvider : public BodyContourProviderBase
{
public:
  // default constructor
  BodyContourProvider();
  
  virtual ~BodyContourProvider(){};

  virtual void execute();

protected:

  class BodyParts : public Printable
  {
  public:
    std::vector< Vector3<double> > torso, // the contour of the torso
                              upperArm, // the contour of the upper arm
                              lowerArm, // the contour of the lower arm
                              upperLeg, // the contour of the upper leg
                              lowerLeg, // the contour of the lower leg
                              foot; // the contour of the foot

                              
  protected:
    void print(ostream& stream) const
    {
      stream << "nothing" << '\n';
    }
  };// end class BodyParts
  
private:
  
  // add a line to BodyContour:
  // take the Position of a robot's limb (Pose3d) and multiply it
  // with the contour of this limb, finally, project the contour into the image
  inline void add(
    const Pose3D& origin, 
    const std::vector< Vector3<double> >& c, 
    float sign, 
    const CameraInfo& cameraInfo,  
    const CameraMatrix& cameraMatrix, 
    BodyContour& bodyContour, 
    BodyContour::bodyPartID id);

  //
  inline void pushLine(const BodyContour::Line& line, BodyContour& bodyContour);

  // check whether a point is within the image 
  inline bool withinImage(const Vector2<double>& point) const;
  //
  inline void cellPos(const BodyContour::Line& line, Vector2<int>& firstCell, Vector2<int>& secondCell) const;
  //
  inline void cellPos(const Vector2<int>& point, Vector2<int>& cell) const;
  //
  inline void setCells(const BodyContour::Line& line, BodyContour& bodyContour) const;
  //
  inline void initializeGrid();
  //
  inline void eraseGrid();

  
  CameraInfo ci;
  BodyParts bodyparts;
  int lineNumber;
};// end class BodyContourProvider

#endif // _BodyContourProvider_h_
