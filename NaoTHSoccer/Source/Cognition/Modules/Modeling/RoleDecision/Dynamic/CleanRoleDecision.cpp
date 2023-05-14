/**
* @file CleanRoleDecision.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#include "CleanRoleDecision.h"
#include <PlatformInterface/Platform.h>

#include <list>

using namespace std;


CleanRoleDecision::CleanRoleDecision()
{
  getDebugParameterList().add(&params);

  DEBUG_REQUEST_REGISTER(
    "RoleDecision:min_ball_distance",
    "Draws the distance in which the balls of the first and second striker are recognized as identical (only with strikerSelection = 3).",
    false
  );
}

CleanRoleDecision::~CleanRoleDecision()
{
  getDebugParameterList().remove(&params);
}

void CleanRoleDecision::execute() {
    computeStrikers();

    // reset second striker decision, if we doesn't want to use the second striker
    if(!params.useSecondStriker) {
        getRoleDecisionModel().secondStriker = std::numeric_limits<unsigned int>::max();
    }

    // set the dynamic role
    setStrikerRole(getRoleDecisionModel().firstStriker);
    setStrikerRole(getRoleDecisionModel().secondStriker);
    // TODO: the dynamic role doesn't work for the goalie with this 'old' striker decision!
    //       the dynamic striker role is set by the behavior of the 'old' decision ...
}//end execute

void CleanRoleDecision::computeStrikers()
{
    // skip "first frame"! (until i add myself to teamcomm)
    if(!getTeamState().hasPlayer(getPlayerInfo().playerNumber)) { return; }

    // container storing robots, which want to be striker, sorted by their playernumber and their time to ball
    std::map<unsigned int, unsigned int> possible_striker;

    // iterate over all robots(messages)
    for (const auto& i: getTeamState().players) {
        unsigned int robotNumber = i.first;
        const auto& player = i.second;

        // if striker lost the ball, he gets a time bonus before he lost the ball completely ...
        double loose_ball_bonus = player.number == getRoleDecisionModel().firstStriker?params.strikerBonusTime:0.0;

        // check if the robot is able to play and sees the ball
        bool isRobotInactive = !getTeamMessagePlayersState().isPlaying(robotNumber)
                || player.ballAge() < 0 //Ball was never seen
                || (player.ballAge() + getFrameInfo().getTimeSince(player.messageFrameInfo.getTime()) > params.maxBallLostTime + loose_ball_bonus); //Ball isn't fresh

        // ignore inactive robots
        if(robotNumber != getPlayerInfo().playerNumber && isRobotInactive) { continue; }

        // for all active robots, which sees the ball AND previously announced to want to be striker OR is striker ...
        if (player.wantsToBeStriker() || player.wasStriker()) {
            // ... remember them as possible striker with their time to ball
            possible_striker[robotNumber] = player.timeToBall();
        }
    }//end for

    // get my own message
    auto ownMessage = getTeamState().getPlayer(getPlayerInfo().playerNumber);
    // i want to be striker, if i'm not the goalie and i'm "active" (not fallen/penalized, see the ball)!!!
    getRoleDecisionModel().wantsToBeStriker = ownMessage.number != 1
                                              && getTeamMessagePlayersState().isPlaying(getPlayerInfo().playerNumber)
                                              && !(ownMessage.ballAge() < 0
                                                   || (ownMessage.ballAge() + getFrameInfo().getTimeSince(ownMessage.messageFrameInfo.getTime()) > params.maxBallLostTime + (getPlayerInfo().playerNumber==getRoleDecisionModel().firstStriker?params.strikerBonusTime:0.0)));

    // if i'm striker, i get a time bonus!
    // NOTE: ownTimeToBall can be negative if the robot is close to ball (!)
    double ownTimeToBall = static_cast<double>(ownMessage.timeToBall()) - (ownMessage.wasStriker() ? 300.0 : 0.0);

    // clear for new striker decision
    getRoleDecisionModel().resetStriker();
    (this->*params.strikerSelectionFunction)(possible_striker, ownTimeToBall);


    PLOT(std::string("CleanRoleDecision:FirstStrikerDecision"), getRoleDecisionModel().firstStriker);
    PLOT(std::string("CleanRoleDecision:SecondStrikerDecision"), getRoleDecisionModel().secondStriker);
    PLOT(std::string("CleanRoleDecision:PossibleStriker"), static_cast<double>(possible_striker.size()));
}

void CleanRoleDecision::strikerSelectionByNumber(std::map<unsigned int, unsigned int>& possible_striker, double& ownTimeToBall)
{
    for (auto it = possible_striker.cbegin(); it != possible_striker.cend(); ++it) {
        //If two robots want to be striker, the one with a smaller number is favoured => is the first in the map!!
        // NOTE: we're doesn't using 'timeToBall' as decision criteria by intention!
        //       if goalie is striker, he get's the first striker
        if(getRoleDecisionModel().firstStriker == std::numeric_limits<unsigned int>::max()) {
            getRoleDecisionModel().firstStriker = it->first;
        } else if (getRoleDecisionModel().secondStriker == std::numeric_limits<unsigned int>::max()) {
            getRoleDecisionModel().secondStriker = it->first;
        }
        // if there's a robot closer to the ball than myself, i don't want to be striker!
        if(it->second < ownTimeToBall) {
            getRoleDecisionModel().wantsToBeStriker = false;
        }
    }
}

void CleanRoleDecision::strikerSelectionByTime(std::map<unsigned int, unsigned int>& possible_striker, double &ownTimeToBall)
{
    unsigned int stFastest = std::numeric_limits<unsigned int>::max(); // time to ball of the fastest player
    unsigned int ndFastest = std::numeric_limits<unsigned int>::max(); // time to ball of the second fastest player

    for (auto it = possible_striker.cbegin(); it != possible_striker.cend(); ++it) {
        // current player faster?
        if(it->second < stFastest) {
            // is there already a "fastest" striker ... but the current player is faster
            if(getRoleDecisionModel().firstStriker != std::numeric_limits<unsigned int>::max()) {
                // make the previous player the "second fastest" player
                getRoleDecisionModel().secondStriker = getRoleDecisionModel().firstStriker;
                ndFastest = stFastest;
            }
            // set the fastest player
            getRoleDecisionModel().firstStriker = it->first;
            stFastest = it->second;
        } else if (it->second < ndFastest) {
            // set the second fastest player
            getRoleDecisionModel().secondStriker = it->first;
            ndFastest = it->second;
        }
        // if there's a 2nd robot closer to the ball than myself, i don't want to be (second)striker!
        if(ndFastest < ownTimeToBall) {
            getRoleDecisionModel().wantsToBeStriker = false;
        }
    }
}

void CleanRoleDecision::strikerSelectionByTimeExceptGoalie(std::map<unsigned int, unsigned int>& possible_striker, double &ownTimeToBall)
{
    unsigned int stFastest = std::numeric_limits<unsigned int>::max(); // time to ball of the fastest player
    unsigned int ndFastest = std::numeric_limits<unsigned int>::max(); // time to ball of the second fastest player

    // NOTE: if the goalie is one of the "possible striker", it gets set as first strike! (sorted map!)
    //       and doesn't change, even if someone is faster (condition!)
    // ATTENTION: we're iterating from the smallest player number to the highest!
    for (auto it = possible_striker.cbegin(); it != possible_striker.cend(); ++it) {
        // is current player clearly faster?
        if(it->second < stFastest && (it->second + params.strikerSelectionDiffThreshold) < stFastest && getRoleDecisionModel().firstStriker != 1) {
            // is there already a "fastest" striker ... but the current player is faster
            if(getRoleDecisionModel().firstStriker != std::numeric_limits<unsigned int>::max()) {
                // make the previous player the "second fastest" player
                getRoleDecisionModel().secondStriker = getRoleDecisionModel().firstStriker;
                ndFastest = stFastest;
            }
            // set the fastest player
            getRoleDecisionModel().firstStriker = it->first;
            stFastest = it->second;
        } else if (it->second < ndFastest && (it->second + params.strikerSelectionDiffThreshold) < ndFastest) {
            // set the second (clearly) fastest player
            getRoleDecisionModel().secondStriker = it->first;
            ndFastest = it->second;
        }
        // if there's a 2nd robot closer to the ball than myself, i don't want to be (second)striker!
        if(ndFastest < ownTimeToBall) {
            getRoleDecisionModel().wantsToBeStriker = false;
        }
    }
}

void CleanRoleDecision::strikerSelectionByTimeExceptGoalieWithBallCompare(std::map<unsigned int, unsigned int>& possible_striker, double &ownTimeToBall)
{
    unsigned int stFastest = std::numeric_limits<unsigned int>::max(); // time to ball of the fastest player
    unsigned int ndFastest = std::numeric_limits<unsigned int>::max(); // time to ball of the second fastest player

    // NOTE: if the goalie is one of the "possible striker", it gets set as first strike! (sorted map!)
    //       and doesn't change, even if someone is faster (condition!)
    // ATTENTION: we're iterating from the smallest player number to the highest!
    for (auto it = possible_striker.cbegin(); it != possible_striker.cend(); ++it) {
        // is current player clearly faster?
        if(it->second < stFastest && (it->second + params.strikerSelectionDiffThreshold) < stFastest && getRoleDecisionModel().firstStriker != 1) {
            // is there already a "fastest" striker ... but the current player is faster
            if(getRoleDecisionModel().firstStriker != std::numeric_limits<unsigned int>::max() && isSecondStrikerDifferentFromFirst(getRoleDecisionModel().firstStriker, it->first)) {
                // make the previous player the "second fastest" player, if they see different balls
                getRoleDecisionModel().secondStriker = getRoleDecisionModel().firstStriker;
                ndFastest = stFastest;
            }
            // set the fastest player
            getRoleDecisionModel().firstStriker = it->first;
            stFastest = it->second;
        } else if (it->second < ndFastest
                   && (it->second + params.strikerSelectionDiffThreshold) < ndFastest
                   && isSecondStrikerDifferentFromFirst(getRoleDecisionModel().firstStriker, it->first))
        {
            // make the previous player the "second fastest" player, if they see different balls
            getRoleDecisionModel().secondStriker = it->first;
            ndFastest = it->second;
        }
        // if there's a 2nd robot closer to the ball than myself, i don't want to be (second)striker!
        if(ndFastest < ownTimeToBall) {
            getRoleDecisionModel().wantsToBeStriker = false;
        }
    }
}

bool CleanRoleDecision::isSecondStrikerDifferentFromFirst(unsigned int firstNumber, unsigned int secondNumber) {
    // retrieve the message of the players
    const auto& first = getTeamState().getPlayer(firstNumber);
    const auto& second = getTeamState().getPlayer(secondNumber);
    // get the global ball position
    Vector2d firstBall = first.pose() * first.ballPosition();
    Vector2d secondBall = second.pose() * second.ballPosition();
    // check if the ball distance is greater than the given parameter distance radius
    double r = (this->*params.ballDifferenceRadius)(second.ballPosition().abs());

    DEBUG_REQUEST("RoleDecision:min_ball_distance",
      FIELD_DRAWING_CONTEXT;
      PEN("000000", 30);
      CIRCLE(firstBall.x, firstBall.y, r);
      TEXT_DRAWING(firstBall.x, firstBall.y+60, secondNumber);
      TEXT_DRAWING(firstBall.x, firstBall.y-60, r);
    );

    return ((firstBall - secondBall).abs2() > r*r);
}

void CleanRoleDecision::setStrikerRole(unsigned int number)
{
    // set the new striker role
    if(getRoleDecisionModel().roles.find(number) != getRoleDecisionModel().roles.cend()) {
        getRoleDecisionModel().roles[number].dynamic = Roles::striker;
    }
}
