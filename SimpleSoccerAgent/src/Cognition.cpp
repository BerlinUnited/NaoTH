/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#include "Cognition.h"

#include <Tools/ImageProcessing/ColorModelConversions.h>
#include <Tools/Math/Vector2.h>

using namespace std;
using namespace naoth;

Cognition::Cognition()
  :
  robotIsUpright(true)
{
}

Cognition::~Cognition()
{
}


void Cognition::call()
{
  // perceive the ball etc.
  perception();

  // make a decision what to do next
  decide();
}//end call


void Cognition::perception()
{
  // try to update the ball percept by virtual vision
  detect_ball_in_virtual_vision();

  // look for the ball in the image 
  // ( needed for SPL and in the future for Simulation 3D)
  //detect_ball_in_image();
  

  // update if the robot is upright or lying on the ground
	if ( fabs(theInertialSensorData.data.x) < Math::fromDegrees(45) &&
       fabs(theInertialSensorData.data.y) < Math::fromDegrees(45) )
    robotIsUpright = true;
  else
    robotIsUpright = false;
}//end perception


void Cognition::detect_ball_in_virtual_vision()
{
  std::map<std::string, Vector3<double> >::const_iterator ballData = theVirtualVision.data.find("B");
  if (ballData != theVirtualVision.data.end())
  {
    BlackBoard::getInstance().theBallPercept.wasSeen = true;
    BlackBoard::getInstance().theBallPercept.distance = ballData->second[0];
  }else
  {
    BlackBoard::getInstance().theBallPercept.wasSeen = false;
  }
}//end detect_ball_in_virtual_vision


void Cognition::detect_ball_in_image()
{
  Vector2<int> orange_sum;
  int num_of_orange = 0;

  // the current pixel to scan
  Vector2<int> pixel;

  // scan the whole image
  for(pixel.x = 0; pixel.x < (int)theImage.cameraInfo.resolutionWidth; pixel.x+=2)
  {
    for(pixel.y = 0; pixel.y < (int)theImage.cameraInfo.resolutionHeight; pixel.y+=2)
    {
      // get the color values of pixel (in YUV)
      Pixel p = theImage.get(pixel.x, pixel.y);

      // convert to RGB
      naoth::ColorModelConversions::fromYCbCrToRGB(
        p.y, p.u, p.v,
        p.a, p.b, p.c);

      // check for orange
      if(p.a > 220 && p.b > 200 && p.c < 90)
      {
        // sum all the orange pixels
        orange_sum += pixel;
        num_of_orange++;
      }

    }//end for y
  }//end for x

  if(num_of_orange > 0)
  {
    // center of the orange blob
    Vector2<int> orange_center = orange_sum / num_of_orange;
    BlackBoard::getInstance().theBallPercept.wasSeen = true;
  }else
  {
    BlackBoard::getInstance().theBallPercept.wasSeen = false;
  }
}//end detect_ball


void Cognition::decide()
{
  if (!robotIsUpright)
  {
    if (theInertialSensorData.data.y > 0)
      BlackBoard::getInstance().theMotionRequest.id = MotionRequest::stand_up_from_front;
    else
      BlackBoard::getInstance().theMotionRequest.id = MotionRequest::stand_up_from_back;
  }
  else if ( BlackBoard::getInstance().theBallPercept.wasSeen )
  {
    BlackBoard::getInstance().theMotionRequest.id = MotionRequest::walk_forward;
  }
  else
  {
    BlackBoard::getInstance().theMotionRequest.id = MotionRequest::turn_left;
  }
}//end decide
