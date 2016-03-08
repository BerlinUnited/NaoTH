/**
* @file SimulationTest.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SimulationTest
*/

#include "SimulationTest.h"

using namespace naoth;
using namespace std;

SimulationTest::SimulationTest()
{
  simulationModule = registerModule<Simulation>(std::string("Simulation"), true);
}

SimulationTest::~SimulationTest(){}


void SimulationTest::execute()
{
  // hack
  static std::vector<Vector3d> function;
  function.clear();

  // ball just in front of the robot
  getBallModel().position = Vector2d(150, 0);
  getBallModel().valid = true;
  getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

  getRobotPose().isValid = true;

  // iterate over robot pose
  Pose2D p(0.0, getFieldInfo().carpetRect.min());
  for(p.translation.x = getFieldInfo().carpetRect.min().x; p.translation.x < getFieldInfo().carpetRect.max().x; p.translation.x += 100) 
  {
    for(p.translation.y = getFieldInfo().carpetRect.min().y; p.translation.y < getFieldInfo().carpetRect.max().y; p.translation.y += 100) 
    {
      getRobotPose() = p;
      simulationModule->execute();

      function.push_back(Vector3d(p.translation.x, p.translation.y, getKickActionModel().bestAction));
    }
  }

  draw_function(function);
}//end execute

void SimulationTest::draw_function(const std::vector<Vector3d>& function) const
{
  if(function.empty()) {
    return;
  }

  double maxValue = function.front().z;
  double minValue = function.front().z;
  for(size_t i = 0; i < function.size(); ++i) {
    if(function[i].z > maxValue) {
      maxValue = function[i].z;
    } else if(function[i].z < minValue) {
      minValue = function[i].z;
    }
  }

  if(maxValue - minValue == 0) return;

  FIELD_DRAWING_CONTEXT;
  Color white(1.0,1.0,1.0,0.0); // transparent
  Color black(0.0,0.0,0.0,1.0);


  for(size_t i = 0; i < function.size(); ++i) {
    double t = (function[i].z - minValue) / (maxValue - minValue);
    Color color = black*t + white*(1-t);
    PEN(color, 20);
    FILLBOX(function[i].x - 50, function[i].y - 50, function[i].x+50, function[i].y+50);
  }
}//end draw_closest_points
