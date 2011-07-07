/**
 * @file RoboViz.h
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief a module use RoboViz to show debug information
 */

#ifndef ROBOVIZ_H
#define ROBOVIZ_H

#include <ModuleFramework/Module.h>
#include <Tools/Communication/Broadcast/BroadCaster.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/FieldInfo.h>
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Perception/BallPercept.h"

BEGIN_DECLARE_MODULE(RoboViz)
  REQUIRE(RobotInfo)
  REQUIRE(FieldInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotPose)
  REQUIRE(KinematicChain)
  REQUIRE(MotionRequest)
  REQUIRE(BallModel)
  REQUIRE(SoccerStrategy)
  REQUIRE(BallPercept)
  REQUIRE(MotionStatus)
END_DECLARE_MODULE(RoboViz)

class RoboViz: public RoboVizBase
{
  public:
  RoboViz();

  ~RoboViz();

  virtual void execute();

  protected:
  void swapBuffers(const string& setName);
  void drawCircle(const Vector2d& pos, double radius, double thickness, const Vector3<unsigned char>& color, const string& setName);
  void drawLine(const Vector3d& pa, const Vector3d& pb, double thickness, const Vector3<unsigned char>& color, const string& setName);
  void drawSphere(const Vector3d& pos, float radius, const Vector3<unsigned char>& color, const string& setName);
  void drawPolygon(const list<Vector3d>& vertex, const Vector3<unsigned char>& color, unsigned char alpha, const string& setName);
  void drawPoint(const Vector3d& p, double size, const Vector3<unsigned char>& color, const string& setName);
  void drawAnnotation(const string& text, const Vector3d& pos, const Vector3<unsigned char>& color, const string& setName);
  void drawAgentAnnotation(const string& text, const Vector3<unsigned char>& color);
  void cleanAgentAnnotation();


  private:
  int getAgentAnnotationID();

  void drawRobotPose();

  void drawMotionRequest();

  void drawBallAndAttackDir();

  // test functions
  void test();
  void testStaticShapes();
  void testAnimatedShapes();

  BroadCaster* theBroadCaster;
  std::list<std::string> drawCommands;
};

#endif // ROBOVIZ_H
