/**
* @file Simulation.cpp
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Stella Alice Schlotter</a>
* Implementation of class SimulationOLD
*/

#include "Simulation.h"

using namespace naoth;
using namespace std;

Simulation::Simulation()
{
  simulationModule = registerModule<ActionSimulator>(std::string("ActionSimulator"), true);
  DEBUG_REQUEST_REGISTER("Simulation:draw_best_action", "best action", false);
  DEBUG_REQUEST_REGISTER("Simulation:use_Parameters", "use the Parameters", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:None", "DrawNone", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Short", "DrawShortKick", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Left", "DrawLeftKick", false);
  DEBUG_REQUEST_REGISTER("Simulation:ActionTarget:Right", "DrawRightKick", false);
  getDebugParameterList().add(&theParameters);

  //calculate the actions  
  action_local.reserve(KickActionModel::numOfActions);

  action_local.push_back(ActionSimulator::Action(KickActionModel::none, ActionSimulator::ActionParams(), theParameters.friction));
  action_local.push_back(ActionSimulator::Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
  //action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left
  //action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right

  actionsConsequences.resize(action_local.size());
}

Simulation::~Simulation()
{
  getDebugParameterList().remove(&theParameters);
}

void Simulation::execute()
{
  DEBUG_REQUEST("Simulation:use_Parameters",
    action_local.clear();
    action_local.reserve(KickActionModel::numOfActions);

    action_local.push_back(ActionSimulator::Action(KickActionModel::none, ActionSimulator::ActionParams(), theParameters.friction));
    action_local.push_back(ActionSimulator::Action(KickActionModel::kick_short, theParameters.kick_short, theParameters.friction)); // short
    //action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_left, theParameters.sidekick_left, theParameters.friction)); // left
    //action_local.push_back(ActionSimulator::Action(KickActionModel::sidekick_right, theParameters.sidekick_right, theParameters.friction)); // right

    actionsConsequences.resize(action_local.size());
  );

  //Proceed with Calculations only if ball is seen in the last 1 second
  //HACK: collides with conditions (head movement) in behavior
  // -> this does not trust the valid flag
  if (!getBallModel().valid || getFrameInfo().getTimeInSeconds() >= getBallModel().getFrameInfoWhenBallWasSeen().getTimeInSeconds() + 1)
  {
    return;
  }
  // simulate the consequences for all actions
  STOPWATCH_START("Simulation:simulateAction");
  for (size_t i = 0; i < action_local.size(); i++) {
    simulationModule->getModuleT()->simulateAction(action_local[i], actionsConsequences[i], 30);
  }
  STOPWATCH_STOP("Simulation:simulateAction");

  // plot projected actions
  DEBUG_REQUEST("Simulation:ActionTarget:None", draw_action_results(actionsConsequences[0], Color(1.0, 1.0, 1.0, 0.7)););
  DEBUG_REQUEST("Simulation:ActionTarget:Short", draw_action_results(actionsConsequences[1], Color(255.0 / 255, 172.0 / 255, 18.0 / 255, 0.7)););
  //DEBUG_REQUEST("Simulation:ActionTarget:Left", draw_action_results(actionsConsequences[2], Color(0.0 / 255, 13.0 / 255, 191.0 / 255, 0.7)););
  //DEBUG_REQUEST("Simulation:ActionTarget:Right", draw_action_results(actionsConsequences[3], Color(0.0 / 255, 191.0 / 255, 51.0 / 255, 0.7)););


  // now decide which action to execute given their consequences
  size_t best_action = decide_smart(actionsConsequences);
  getKickActionModel().bestAction = action_local[best_action].id();
  //Arghhh BUG: this expected ball pos is not affected by goal boarders/ obstacles
  getKickActionModel().expectedBallPos = getRobotPose() * action_local[best_action].predict(getBallModel().positionPreview, false);

  DEBUG_REQUEST("Simulation:draw_best_action",
    FIELD_DRAWING_CONTEXT;
    /*
    PEN("FF69B4", 35);
    std::string name = action_local[best_action].name();
    TEXT_DRAWING(getRobotPose().translation.x+100, getRobotPose().translation.y-200, name);
    */
    PEN("000000", 1);
    Vector2d expectedBallPos = getKickActionModel().expectedBallPos;
    FILLOVAL(expectedBallPos.x, expectedBallPos.y, 50, 50);

    if(getKickActionModel().bestAction == KickActionModel::none) {
      PEN("000000", 50);
      Vector2d globalBall = getRobotPose() * getBallModel().positionPreview;
      FILLOVAL(globalBall.x, globalBall.y, 100, 100);
    } else {
      Vector2d globalBall = getRobotPose() * getBallModel().positionPreview;
      Vector2d action_vector = (expectedBallPos - globalBall).normalize();
      Vector2d from = globalBall + action_vector * 100;
      Vector2d to = globalBall + action_vector * 350;

      PEN("FF0000", 50);
      ARROW(from.x, from.y, to.x, to.y);
    }
  );

  //Debug stuff
  //simulationModule->getModuleT()->draw_potential_field();  // -2 bis 2
}//end execute

size_t Simulation::decide_smart(const std::vector<ActionSimulator::ActionResults>& actionsConsequences) const
{
  std::vector<size_t> acceptableActions;

  // select acceptable actions
  for (size_t i = 0; i < actionsConsequences.size(); ++i)
  {
    const ActionSimulator::ActionResults& results = actionsConsequences[i];

    // if an own-goal is detected, ignore the action
    if (results.category(ActionSimulator::BallPositionCategory::OWNGOAL) > 0) {
      continue;
    }

    // ignore actions with too high chance of kicking out
    double score = results.likelihood(ActionSimulator::BallPositionCategory::INFIELD) + results.likelihood(ActionSimulator::BallPositionCategory::OPPGOAL);
    // TODO: maybe rename to kick_out_percentage?
    if (score <= max(0.0, theParameters.good_threshold_percentage)) {
      continue;
    }

    //all actions which are not too bad
    acceptableActions.push_back(i);
  }

  // no acceptable actions
  // HACK: we should set default action explicitely
  if (acceptableActions.empty()) {
    return 0; //assumes 0 is the turn action
  }
  // there is a clear decision
  if (acceptableActions.size() == 1) {
    return acceptableActions.front();
  }

  // select the subset of actions with highest goal chance
  std::vector<size_t> goalActions;
  for (size_t i = 0; i < acceptableActions.size(); ++i)
  {
    const ActionSimulator::ActionResults& results = actionsConsequences[acceptableActions[i]];

    // chance of scoring a goal must be significant
    // TODO find parameters where this is relevant
    // TODO minGoalParticles is absolute values, should be relative to number of particles
    if (results.category(ActionSimulator::BallPositionCategory::OPPGOAL) < theParameters.minGoalParticles) {
      // This action can still be choosen according to the potential field, but it is not considered a goal action
      continue;
    }

    // there is no other action to compare yet
    if (goalActions.empty()) {
      goalActions.push_back(acceptableActions[i]);
      continue;
    }

    // the action with the highest chance of scoring the goal is the best
    if (actionsConsequences[goalActions.front()].category(ActionSimulator::BallPositionCategory::OPPGOAL) < results.category(ActionSimulator::BallPositionCategory::OPPGOAL)) {
      goalActions.clear();
      goalActions.push_back(acceptableActions[i]);
      continue;
    }

    if (actionsConsequences[goalActions.front()].category(ActionSimulator::BallPositionCategory::OPPGOAL) == results.category(ActionSimulator::BallPositionCategory::OPPGOAL)) {
      goalActions.push_back(acceptableActions[i]);
      continue;
    }
  }

  // go for the goal action!
  if (goalActions.size() == 1) {
    return goalActions.front();
  }

  // no goal actions => select one of the acceptable actions based on strategic value
  if (goalActions.empty())
  {
    //TODO check image in paper/teamreport for potential field definition
    size_t best_action = 0;
    double bestValue = std::numeric_limits<double>::max(); // assuming potential is [-inf, inf]
    for (std::vector<size_t>::const_iterator it = acceptableActions.begin(); it != acceptableActions.end(); ++it)
    {
      double potential = simulationModule->getModuleT()->evaluateAction(actionsConsequences[*it]);
      if (potential < bestValue) {
        best_action = *it;
        // bestvalue is the mean of the samples
        bestValue = potential;
      }
    }
    // get the value of current position
    double current_potential = simulationModule->getModuleT()->evaluateAction(getRobotPose().translation);
    // make sure we dont kick the ball to a similar position on the potential field
    // choose a kick action only if it improves the ball position significantly
    if (bestValue < (current_potential - theParameters.significance_thresh)) {
      return best_action;
    } else {
      return 0; // assumes that 0 means turn action
    }
    
  }
  else{
    // find min of goalActions (min value is best)
    size_t best_action = 0;
    double bestValue = std::numeric_limits<double>::max(); // assuming potential is [-inf, inf]
    for (std::vector<size_t>::const_iterator it = goalActions.begin(); it != goalActions.end(); ++it)
    {
      double potential = simulationModule->getModuleT()->evaluateAction(actionsConsequences[*it]);
      if (potential < bestValue) {
        best_action = *it;
        bestValue = potential;
      }
    }
    return best_action;
  }
  // should never happen
  ASSERT(false);
}

void Simulation::draw_action_results(const ActionSimulator::ActionResults& actionsResults, const Color& color) const
{
  FIELD_DRAWING_CONTEXT;
  std::vector<ActionSimulator::CategorizedBallPosition>::const_iterator ballPosition = actionsResults.positions().begin();
  for (; ballPosition != actionsResults.positions().end(); ++ballPosition)
  {
    Vector2d ball = getRobotPose() * ballPosition->pos();
    if (ballPosition->cat() == ActionSimulator::BallPositionCategory::INFIELD)
    {
      PEN("000000", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
      PEN(color, 1);
      FILLOVAL(ball.x, ball.y, 40, 40);
    }
    else if (ballPosition->cat() == ActionSimulator::BallPositionCategory::OPPGOAL)
    {
      PEN("336600", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
    }
    else //Outside Field
    {
      PEN("66FF33", 1);
      FILLOVAL(ball.x, ball.y, 50, 50);
    }
  }
}