/**
* @file ActionSimulator.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Stella Alice Schlotter</a>
* Implementation of class ActionSimulator
*/

#include "ActionSimulator.h"
#include <algorithm>

using namespace naoth;
using namespace std;

ActionSimulator::ActionSimulator()
  : // own goal
    ownGoalBackSides({
      Math::LineSegment(getFieldInfo().ownGoalBackLeft, getFieldInfo().ownGoalBackRight),
      Math::LineSegment(getFieldInfo().ownGoalPostLeft, getFieldInfo().ownGoalBackLeft),
      Math::LineSegment(getFieldInfo().ownGoalPostRight, getFieldInfo().ownGoalBackRight) }),
    // opp goal
    oppGoalBackSides({
      Math::LineSegment(getFieldInfo().oppGoalBackLeft      , getFieldInfo().oppGoalBackRight),
      Math::LineSegment(getFieldInfo().opponentGoalPostLeft , getFieldInfo().oppGoalBackLeft),
      Math::LineSegment(getFieldInfo().opponentGoalPostRight, getFieldInfo().oppGoalBackRight)}
    )

{
  DEBUG_REQUEST_REGISTER("Simulation:draw_goal_collisions", "draw goal collisions", false);
  DEBUG_REQUEST_REGISTER("Simulation:draw_potential_field", "draw potential field", false);
}


ActionSimulator::~ActionSimulator(){}

void ActionSimulator::execute()
{

}//end execute



void ActionSimulator::simulateAction(const Action& action, ActionResults& result, size_t numParticles) const
{ 
  // just as a safety measure
  //categorizedBallPositions.clear();
  //categorizedBallPositions.reserve(static_cast<int>(theParameters.numParticles));
  result.reset();

  // current ball position
  Vector2d globalBallStartPosition = getRobotPose() * getBallModel().positionPreview;
  

  // now generate predictions and categorize
  for(size_t j=0; j < numParticles; ++j)
  {
    // predict and calculate shoot line
    Vector2d globalBallEndPosition = getRobotPose() * action.predict(getBallModel().positionPreview, true);

    // check if collision detection with goal has to be performed
    // if the ball start and end positions are inside of the field, you don't need to check
    if(!getFieldInfo().fieldRect.inside(globalBallEndPosition) || !getFieldInfo().fieldRect.inside(globalBallStartPosition))
	  {
      // calculate if there is a collision with the opponent goal and where the ball would stop
      bool collisionWithOppGoal = calculateCollision(oppGoalBackSides, globalBallStartPosition, globalBallEndPosition, globalBallEndPosition);
      bool collisionWithOwnGoal = calculateCollision(ownGoalBackSides, globalBallStartPosition, globalBallEndPosition, globalBallEndPosition);

      // draw balls and goal box if there are collisions
      DEBUG_REQUEST("Simulation:draw_goal_collisions",
      if (collisionWithOppGoal)
      {
        FIELD_DRAWING_CONTEXT;

        PEN("000000", 10);
        BOX(getFieldInfo().oppGoalRect.min().x, getFieldInfo().oppGoalRect.min().y, getFieldInfo().oppGoalRect.max().x, getFieldInfo().oppGoalRect.max().y);

        if (getFieldInfo().oppGoalRect.inside(globalBallEndPosition)) {
          PEN("0000AA66", 1);
        }
        else {
          PEN("FF00AA66", 1);
        }
        const double r = getFieldInfo().ballRadius;
        FILLOVAL(globalBallEndPosition.x, globalBallEndPosition.y, r, r);
      }
      else if (collisionWithOwnGoal){
        FIELD_DRAWING_CONTEXT;

        PEN("000000", 10);
        BOX(getFieldInfo().ownGoalRect.min().x, getFieldInfo().ownGoalRect.min().y, getFieldInfo().ownGoalRect.max().x, getFieldInfo().ownGoalRect.max().y);

        if (getFieldInfo().ownGoalRect.inside(globalBallEndPosition)) {
          PEN("0000AA66", 1);
        }
        else {
          PEN("FF00AA66", 1);
        }
        const double r = getFieldInfo().ballRadius;
        FILLOVAL(globalBallEndPosition.x, globalBallEndPosition.y, r, r);
      }
      );
    }

    // default category
    BallPositionCategory category = classifyBallPosition(globalBallEndPosition);
    result.add(getRobotPose() / globalBallEndPosition, category);
  }
}

bool ActionSimulator::calculateCollision(const vector<Math::LineSegment>& lines, const Vector2d& start, const Vector2d& end, Vector2d& result) const 
{
  Math::LineSegment motionLine(start, end);
  double t_min = motionLine.getLength();

  bool collision = false;
  for(const Math::LineSegment& segment: lines) 
  {
    const double t = motionLine.Line::intersection(segment);

    if(t >= 0 && t < t_min && segment.intersect(motionLine)) {
      t_min = t;
      collision = true;
    }
  }

  if(collision) {
    result = motionLine.point(t_min-getFieldInfo().ballRadius);
  } else {
    result = end;
  }

  return collision;
}

ActionSimulator::BallPositionCategory ActionSimulator::classifyBallPosition( const Vector2d& globalBallPosition ) const
{
  BallPositionCategory category = INFIELD;

  // goal!!
  if(getFieldInfo().oppGoalRect.inside(globalBallPosition)) {
    category = OPPGOAL;
  }
  // own goal :(
  else if(getFieldInfo().ownGoalRect.inside(globalBallPosition)) {
    category = OWNGOAL;
  }
  // inside field
  // HACK: small gap between this and the borders of the goalbox
  //check y coordinates and 
  else if(getFieldInfo().fieldRect.inside(globalBallPosition) ||
    // TODO: do we really need it?
      (globalBallPosition.x <= getFieldInfo().opponentGoalPostRight.x 
    && globalBallPosition.y > getFieldInfo().opponentGoalPostRight.y 
    && globalBallPosition.y < getFieldInfo().opponentGoalPostLeft.y)
    )
  {
    category = INFIELD;
  }
  //Opponent Groundline Out
  else if(globalBallPosition.x > getFieldInfo().xPosOpponentGroundline) {
    category = OPPOUT;
  }
  //Own Groundline out
  else if(globalBallPosition.x < getFieldInfo().xPosOwnGroundline) {
    category = OWNOUT;
  }
  //an der linken Seite raus
  else if(globalBallPosition.y > getFieldInfo().yPosLeftSideline ) {  
    category = LEFTOUT;
  }
  //an der rechten Seite raus
  else if(globalBallPosition.y < getFieldInfo().yPosRightSideline) { 
    category = RIGHTOUT;
  }
  return category;
}


//correction of distance in percentage, angle in degrees
// TODO unify with ballmodel
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
    // assumes that the potential field is well defined inside the opponent goal
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
  
  auto result = std::minmax_element(potential.begin(), potential.end());
  double range = *result.second - *result.first;

  if(range == 0) { return; }

  idx = 0;
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++)
    {
      Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = (potential[idx++] - *result.first) / range;
      Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }
  }
}//end draw_closest_points
