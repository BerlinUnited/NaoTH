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
  isStandingUp(true)
{
}

void Cognition::call()
{
  //
  perception();

  //
  decide();
}//end call

Cognition::~Cognition()
{
}

void Cognition::perception()
{

  // try to update the ball percept by virtual vision
  /*
  std::map<std::string, Vector3<double> >::const_iterator ballData = theVirtualVision.data.find("B");
  if (false && ballData != theVirtualVision.data.end())
  {
    BlackBoard::getInstance().theBallPercept.wasSeen = true;
    BlackBoard::getInstance().theBallPercept.distance = ballData->second[0];
  }
  else
  {  
    BlackBoard::getInstance().theBallPercept.wasSeen = false;
  }
  */

  // look for the ball in the image (needed for SPL)
  detect_ball();


  // update if the robot is standing up
  if ( abs(theInertialSensorData.data[InertialSensorData::X]) < Math::fromDegrees(45)
    && abs(theInertialSensorData.data[InertialSensorData::Y]) < Math::fromDegrees(45) )
    isStandingUp = true;
  else
    isStandingUp = false;
}//end perception


void Cognition::detect_ball()
{
  Vector2<int> orange_sum;
  int num_of_orange = 0;

  // the current pixel to scan
  Vector2<int> pixel;

  // scan the whole image
  for(pixel.x = 0; pixel.x < theImage.cameraInfo.resolutionWidth; pixel.x+=2)
  {
    for(pixel.y = 0; pixel.y < theImage.cameraInfo.resolutionHeight; pixel.y+=2)
    {
      // get the color values of pixel (in YUV)
      Pixel p = theImage.get(pixel.x, pixel.y);

      // convert to RGB
      naoth::ColorModelConversions::fromYCbCrToRGB(
        p.y, p.u, p.v,
        p.r, p.g, p.b);

      // check for orange
      if(p.r > 220 && p.g > 200 && p.b < 90)
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
  if (!isStandingUp)
  {
    if (theInertialSensorData.data[InertialSensorData::Y] > 0)
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
