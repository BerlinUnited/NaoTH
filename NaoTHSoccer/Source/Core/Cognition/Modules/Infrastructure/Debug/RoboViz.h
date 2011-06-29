/**
 * @file RoboViz.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a module use RoboViz to show debug information
 */

#ifndef ROBOVIZ_H
#define ROBOVIZ_H

#include <ModuleFramework/Module.h>
#include <Tools/Communication/Broadcast/BroadCaster.h>
#include <Tools/Math/Vector3.h>
#include "Representations/Modeling/PlayerInfo.h"

BEGIN_DECLARE_MODULE(RoboViz)
  REQUIRE(PlayerInfo)
END_DECLARE_MODULE(RoboViz)

class RoboViz: public RoboVizBase
{
  public:
  RoboViz();

  virtual void execute();

  protected:
  void swapBuffers(const string* setName);
  void drawLine(const Vector3d& pa, const Vector3d& pb, double thickness, const Vector3d& color, const string* setName);

  private:
  void test();
  void renderStaticShapes();
  void renderAnimatedShapes();

  BroadCaster theBroadCaster;
  std::string drawCmd;
};

#endif // ROBOVIZ_H
