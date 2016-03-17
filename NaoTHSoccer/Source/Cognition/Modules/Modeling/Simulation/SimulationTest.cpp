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
	DEBUG_REQUEST_REGISTER("SimulationTest:draw_decision","draw_decision", false);

	//Set Rotation
	globRot = 0.0;
}

SimulationTest::~SimulationTest(){}


void SimulationTest::execute()
{
	DEBUG_REQUEST("SimulationTest:draw_decision",
		globRot = globRot+1.0;

		// hack
		static std::vector<Vector3d> function;
		function.clear();

		// ball just in front of the robot
		getBallModel().position = Vector2d(150, 0);
		getBallModel().valid = true;
		getBallModel().setFrameInfoWhenBallWasSeen(getFrameInfo());

		getRobotPose().isValid = true;

		// iterate over robot pose
    functionMulticolor.clear();
		Pose2D p(0.0, getFieldInfo().carpetRect.min());
		for(p.translation.x = getFieldInfo().carpetRect.min().x; p.translation.x < getFieldInfo().carpetRect.max().x; p.translation.x += 200) 
		{
			for(p.translation.y = getFieldInfo().carpetRect.min().y; p.translation.y < getFieldInfo().carpetRect.max().y; p.translation.y += 200) 
			{
        MultiColorValue v(KickActionModel::numOfActions);
        v.position = p.translation+getBallModel().position; //Draw the ball position later

        for(size_t i=0;i<10;i++)
        {
				  p.rotation =  Math::fromDegrees(globRot);
				  getRobotPose() = p;
				  simulationModule->execute();
          v.values[getKickActionModel().bestAction]++;
        }

        functionMulticolor.push_back(v);
			}
		}
    draw_function_multicolor(functionMulticolor);
		//draw_function(function);
	);
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
    //Color color = black*t + white*(1-t);
	  Color color;

	  if(function[i].z == 0){
		  color = Color(1.0,1.0,1.0,0.7); //White - None
	  }
	  else if(function[i].z == 1){
		  color = Color(255.0/255,172.0/255,18.0/255,0.7); //orange - kick_short
	  }
	  else if(function[i].z == 2){
		  color = Color(232.0/255,43.0/255,0.0/255,0.7); //red - kick_long
	  }
	  else if(function[i].z == 3){
		  color = Color(0.0/255,13.0/255,191.0/255,0.7); //blue - sidekick_left
	  }
	  else if(function[i].z == 4){
		  color = Color(0.0/255,191.0/255,51.0/255,0.7);//green - sidekick_right
	  }
      PEN(color, 20);
      FILLBOX(function[i].x - 50, function[i].y - 50, function[i].x+50, function[i].y+50);
  }
	//Draw Arrow instead
	Pose2D q;
	q.translation.x = 0.0;
	q.translation.y = 0.0;
	q.rotation = Math::fromDegrees(globRot);

	Vector2d ArrowStart = q * Vector2d(0, 0);
	Vector2d ArrowEnd = q * Vector2d(500, 0);

	PEN("000000", 50);
	ARROW(ArrowStart.x,ArrowStart.y,ArrowEnd.x,ArrowEnd.y);
}//end draw_closest_points

void SimulationTest::draw_difference(const std::vector<Vector3d>& function)const{
  if(function.empty()) {
    return;
  }

  double maxValue = function.front().z;
  double minValue = function.front().z;
  for(size_t i = 0; i < function.size(); ++i)
  {
    if(function[i].z > maxValue) {
      maxValue = function[i].z;
    } else if(function[i].z < minValue)
    {
      minValue = function[i].z;
    }
  }

  if(maxValue - minValue == 0) return;

  FIELD_DRAWING_CONTEXT;
  Color white(1.0,1.0,1.0,0.0); // transparent
  Color black(0.0,0.0,0.0,1.0);


  for(size_t i = 0; i < function.size(); ++i)
  {
	  Color color;
    //Test for difference of kick decisions
    if(function[i].z == 0)
    {
		  color = Color(1.0,1.0,1.0,0.7);//White Kick Decision is the same
    }
    else if(function[i].z == 1)
    {
      color = Color(0.0,0.0,0.0,0.7); //Black - Kick Decision is not the same
    }
      PEN(color, 20);
      FILLBOX(function[i].x - 50, function[i].y - 50, function[i].x+50, function[i].y+50);
  }
  //Draw Arrow
	Pose2D q;
	q.translation.x = 0.0;
	q.translation.y = 0.0;
	q.rotation = Math::fromDegrees(globRot);

	Vector2d ArrowStart = q * Vector2d(0, 0);
	Vector2d ArrowEnd = q * Vector2d(500, 0);

	PEN("000000", 50);
	ARROW(ArrowStart.x,ArrowStart.y,ArrowEnd.x,ArrowEnd.y);
}

void SimulationTest::draw_function_multicolor(const std::vector<SimulationTest::MultiColorValue>& function) const
{
  if(function.empty()) {
    return;
  }

  FIELD_DRAWING_CONTEXT;
  std::vector<Color> colors;
  colors.push_back(Color(1.0,1.0,0.0,0.7)); //none
  colors.push_back(Color(255.0/255,172.0/255,18.0/255,0.7));//short
  colors.push_back(Color(232.0/255,43.0/255,0.0/255,0.7));  //long
  colors.push_back(Color(0.0/255,13.0/255,191.0/255,0.7));  //left
  colors.push_back(Color(0.0/255,191.0/255,51.0/255,0.7));  //right


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
    FILLBOX(v.position.x - 100, v.position.y - 100, v.position.x+100, v.position.y+100);
  }

	//Draw Arrow
	Pose2D q;
	q.translation.x = 0.0;
	q.translation.y = 0.0;
	q.rotation = Math::fromDegrees(globRot);

	Vector2d ArrowStart = q * Vector2d(0, 0);
	Vector2d ArrowEnd = q * Vector2d(500, 0);

	PEN("000000", 50);
	ARROW(ArrowStart.x,ArrowStart.y,ArrowEnd.x,ArrowEnd.y);
}