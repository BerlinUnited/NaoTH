/**
* @file ActionSimulator.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Implementation of class ActionSimulator
*/

#include "ActionSimulator.h"

using namespace naoth;
using namespace std;

ActionSimulator::ActionSimulator()
{
 
}

ActionSimulator::~ActionSimulator(){}

void ActionSimulator::execute()
{

}//end execute



void ActionSimulator::simulateAction( const Action& action ) const
{ 

}

ActionSimulator::BallPositionCategory ActionSimulator::classifyBallPosition( const Vector2d& globalBallPosition ) const
{
  BallPositionCategory category = INFIELD;

  // goal!!
  if(getFieldInfo().oppGoalRect.inside(globalBallPosition)) {
    category = OPPGOAL;
  }
  // inside field
  // HACK: small gap between this and the borders of the goalbox
  //check y coordinates and 
  else if(getFieldInfo().fieldRect.inside(globalBallPosition) ||
      (globalBallPosition.x <= getFieldInfo().opponentGoalPostRight.x 
    && globalBallPosition.y > getFieldInfo().opponentGoalPostRight.y 
    && globalBallPosition.y < getFieldInfo().opponentGoalPostLeft.y)
    )
  {
    category = INFIELD;
  }
  // own goal
  else if(getFieldInfo().ownGoalRect.inside(globalBallPosition)) {
    category = OWNGOAL;
  }
  //Opponent Groundline Out - Ball einen Meter hinter Roboter mind ansto hhe. jeweils seite wo ins ausgeht
  else if(globalBallPosition.x > getFieldInfo().xPosOpponentGroundline) {
    category = OPPOUT;
  }
  //Own Groundline out -  an der seite wo raus geht
  else if(globalBallPosition.x < getFieldInfo().xPosOwnGroundline) {
    category = OWNOUT;
  }
  //an der linken Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
  else if(globalBallPosition.y > getFieldInfo().yPosLeftSideline ) {  
    category = LEFTOUT;
  }
  //an der rechten Seite raus -> ein meter hinter roboter oder wo ins ausgeht ein meter hinter
  else if(globalBallPosition.y < getFieldInfo().yPosRightSideline) { 
    category = RIGHTOUT;
  }
  return category;
}


//correction of distance in percentage, angle in degrees
Vector2d ActionSimulator::Action::predict(const Vector2d& ball, bool noise) const
{
	double gforce = Math::g*1e3; // mm/s^2
	double distance;
	double angle;
	if (noise){
		double speed = Math::generateGaussianNoise(action_speed, action_speed_std);
		angle = Math::generateGaussianNoise(Math::fromDegrees(action_angle), Math::fromDegrees(action_angle_std));
		distance = speed*speed / friction / gforce / 2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
	}
	else {
		distance = action_speed*action_speed / friction / gforce / 2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
		angle = Math::fromDegrees(action_angle);
	}  
  Vector2d predictedAction(distance, 0.0);
  predictedAction.rotate(angle);

  return ball + predictedAction;
}


double ActionSimulator::evaluateAction(const Vector2d& a) const
{
  double xPosOpponentGoal = getFieldInfo().xPosOpponentGoal;
  double yPosLeftSideline = getFieldInfo().yPosLeftSideline;
  double xPosOwnGoal = getFieldInfo().xPosOwnGoal;

  double sigmaX = xPosOpponentGoal/2.0;
  double sigmaY = yPosLeftSideline/2.5;
  double slopeX = -1.0/xPosOpponentGoal;
  
  double f = 0.0;
  f += slope(a.x, a.y, slopeX, 0.0);
  f -= gaussian(a.x, a.y, xPosOpponentGoal, 0.0, sigmaX, sigmaY);
  f += gaussian(a.x, a.y, xPosOwnGoal, 0.0, 1.5*sigmaX, sigmaY);
  
  return f;
}

double ActionSimulator::evaluateAction(const ActionResults& results) const
{
  double sumPotential = 0.0;
  double numberOfActions = 0.0;
  for(ActionResults::Positions::const_iterator p = results.positions().begin(); p != results.positions().end(); ++p)
  {
    if(p->cat() == INFIELD || p->cat() == OPPGOAL) {
      sumPotential += evaluateAction(getRobotPose() * p->pos());
      numberOfActions++;
    }
  }

  ASSERT(numberOfActions > 0);
  sumPotential /= numberOfActions;
  return sumPotential;
}

void ActionSimulator::draw_potential_field() const
{
  static const int ySize = 20;
  static const int xSize = 30;
  double yWidth = 0.5*getFieldInfo().yFieldLength/(double)ySize;
  double xWidth = 0.5*getFieldInfo().xFieldLength/(double)xSize;

  FIELD_DRAWING_CONTEXT;
  Color white(0.0,0.0,1.0,0.5); // transparent
  Color black(1.0,0.0,0.0,0.5);

  // create new sample set
  std::vector<double> potential(xSize*ySize);
  int idx = 0;

  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      potential[idx] = evaluateAction(point);
      idx++;
    }
  }
  
  // TODO: replace by std::minmax_element in C++11
  double maxValue = *std::max_element(potential.begin(), potential.end());
  double minValue = *std::min_element(potential.begin(), potential.end());
  double range = maxValue - minValue;

  if(range == 0) { return; }

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = (potential[idx++] - minValue) / range;
      Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }
  }
}//end draw_closest_points
