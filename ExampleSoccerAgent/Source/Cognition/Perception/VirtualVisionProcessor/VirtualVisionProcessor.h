/*
 * @file VirtualVisionProcessor.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief calculate perceptions from virtual vision
 *
 */

#ifndef _VIRTUAL_VISION_PROCESSOR_H
#define _VIRTUAL_VISION_PROCESSOR_H

#include <set>

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include "Representations/Perception/BallPercept.h"

BEGIN_DECLARE_MODULE(VirtualVisionProcessor)
  REQUIRE(CameraInfo)
  //REQUIRE(CameraMatrix)
  REQUIRE(VirtualVision)
  REQUIRE(FrameInfo)

  PROVIDE(BallPercept)
END_DECLARE_MODULE(VirtualVisionProcessor)

class VirtualVisionProcessor: public VirtualVisionProcessorBase
{
public:
  VirtualVisionProcessor();
  virtual ~VirtualVisionProcessor();

  virtual void execute();

private:
  /* calculate the object's position in chest coordinates from  virtual vision information (polar)
   */
  Vector3<double> calculatePosition(const Vector3<double>& pol);

  void updateBall();

private:
  //void tokenize(const string& str, std::vector<string>& tokens,const string& delimiters = " ");
};

#endif // _VIRTUAL_VISION_PROCESSOR_H
