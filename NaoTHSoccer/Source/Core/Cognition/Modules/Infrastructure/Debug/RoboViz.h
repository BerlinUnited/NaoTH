/**
 * @file RoboViz.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a module use RoboViz to show debug information
 */

#ifndef ROBOVIZ_H
#define ROBOVIZ_H

#include <ModuleFramework/Module.h>
#include <Tools/Communication/Broadcast/BroadCaster.h>

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
  void drawLine(float x1, float y1, float z1, float x2, float y2, float z2, float thickness, float r, float g, float b, const string* setName);

  private:
  void test();
  void renderStaticShapes();
  void renderAnimatedShapes();

  BroadCaster theBroadCaster;
  std::string drawCmd;
};

#endif // ROBOVIZ_H
