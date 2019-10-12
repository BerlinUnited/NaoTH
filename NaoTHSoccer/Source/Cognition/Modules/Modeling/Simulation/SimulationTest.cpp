/**
* @file SimulationTest.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class SimulationTest
*/

#include "SimulationTest.h"

using namespace naoth;
using namespace std;

SimulationTest::SimulationTest()
  : globRobotRotation(0.0)
{
  simulationModule = registerModule<Simulation>(std::string("Simulation"), true);
	DEBUG_REQUEST_REGISTER("SimulationTest:draw_decision","draw_decision", false);
}

SimulationTest::~SimulationTest(){}


void SimulationTest::execute()
{
	//DEBUG_REQUEST("SimulationTest:draw_decision",

		// ball just in front of the robot
		getBallModel().position = Vector2d(150, 0);
		getBallModel().positionPreview = getBallModel().position;
		getBallModel().valid = true;
		getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

		getRobotPose().isValid = true;

		// iterate over robot pose
    functionMulticolor.clear();
		Vector2d p(getFieldInfo().carpetRect.min());

		for(p.x = getFieldInfo().carpetRect.min().x+CELL_WIDTH; p.x < getFieldInfo().carpetRect.max().x; p.x += 2*CELL_WIDTH) {
			for(p.y = getFieldInfo().carpetRect.min().y+CELL_WIDTH; p.y < getFieldInfo().carpetRect.max().y; p.y += 2*CELL_WIDTH) 
			{
        MultiColorValue v(KickActionModel::numOfActions);
        v.position = p; //Draw the ball position later
        for(size_t i=0; i<10; ++i)
        {
          getRobotPose().translation = p;
				  getRobotPose().rotation =  Math::fromDegrees(globRobotRotation);
				  
          // run the simulation module
          simulationModule->execute();

          v.values[getKickActionModel().bestAction]++;
        }
        functionMulticolor.push_back(v);
			}
		}


    draw_function_multicolor(functionMulticolor);
		
    // Draw arrow indicating robot's orientation
	  Pose2D q(Math::fromDegrees(globRobotRotation), 0.0, 0.0);
	  Vector2d arrowStart = q * Vector2d(0, 0);
	  Vector2d arrowEnd = q * Vector2d(500, 0);

	  PEN("000000", 50);
	  ARROW(arrowStart.x,arrowStart.y,arrowEnd.x,arrowEnd.y);


    globRobotRotation += 10.0;
	//);
}//end execute


void SimulationTest::draw_function_multicolor(const std::vector<SimulationTest::MultiColorValue>& function) const
{
  if(function.empty()) {
    return;
  }

  FIELD_DRAWING_CONTEXT;
  std::vector<Color> colors;
  colors.push_back(Color(1.0,1.0,0.0,0.7)); //none
  colors.push_back(Color(0.0/255,234.0/255,255.0/255,0.7)); //short
  colors.push_back(Color(255.0/255,43.0/255,0.0/255,0.7));  //long
  colors.push_back(Color(0.0/255,13.0/255,191.0/255,0.7));  //left
  colors.push_back(Color(0.0/255,191.0/255,51.0/255,0.7));  //right

  // assert that we have enough colors
  ASSERT(colors.size() >= function[0].values.size());

  for(size_t i = 0; i < function.size(); ++i) 
  {
    const MultiColorValue& v = function[i];
    double sum = 0;
    for(size_t j = 0; j < v.values.size(); ++j) {
      sum += v.values[j];
    }

    Color color(0.0,0.0,0.0,0.0);
    for(size_t j = 0; j < v.values.size(); ++j) {
      color += v.values[j]/sum*colors[j];
    }

    PEN(color, 0);
    FILLBOX(v.position.x-CELL_WIDTH, v.position.y-CELL_WIDTH, v.position.x+CELL_WIDTH, v.position.y+CELL_WIDTH);
  }
}