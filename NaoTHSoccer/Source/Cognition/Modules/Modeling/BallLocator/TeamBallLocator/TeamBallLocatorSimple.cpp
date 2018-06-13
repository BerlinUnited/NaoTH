#include "TeamBallLocatorSimple.h"

TeamBallLocatorSimple::TeamBallLocatorSimple()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("TeamBallLocatorSimple:draw_teamball_input", "draw all the balls uses for teamball", false);
    DEBUG_REQUEST_REGISTER("TeamBallLocatorSimple:draw_ball_on_field", "draw the team ball model on the field", false);
}

TeamBallLocatorSimple::~TeamBallLocatorSimple()
{
    getDebugParameterList().remove(&params);
}

void TeamBallLocatorSimple::execute() {
    // valid balls of all player
    std::vector<Ball> balls;

    // iterate over all robots(messages) and collect 'valid' balls
    for (auto const& i : getTeamMessage().data) {
        unsigned int playerNumber = i.first;
        const TeamMessageData& msg = i.second;

        // check if the robot is able to play
        bool isRobotInactive = msg.fallen || msg.custom.isPenalized;

        // TODO: check for "DEAD" robots!? (see CleanRoleDecision)

        // inactive robots
        if(isRobotInactive) { continue; }

        // ballage + network delay
        double ballAge = msg.ballAge + static_cast<double>(getTeamMessageTimeStatistics().getPlayersTimeInMilliSeconds(playerNumber) - msg.custom.timestamp);

        // -1 means "ball never seen", ballAge (incl. network delay) should be small
        if(msg.ballAge >= 0 && ballAge <= params.maxBallAge)
        {
            // global position of the ball and time last seen
            balls.push_back(Ball(msg.pose * msg.ballPosition));
        }
    }

    DEBUG_REQUEST("TeamBallLocatorSimple:draw_teamball_input",
        FIELD_DRAWING_CONTEXT;
        PEN("0000FF", 20);
        for(const Ball& b : balls) {
            CIRCLE(b.pos.x, b.pos.y, 50);
        }
    );

    // Canopy: iterate through all (valid) balls and cluster them based on the loose (t1) & tight distance (t2)
    //         https://en.wikipedia.org/wiki/Canopy_clustering_algorithm
    // NOTE: equal sized clusters are currently not handled!
    unsigned int largest = 0;
    for (unsigned int i = 0; i < balls.size(); ++i) {
        if(balls[i].valid) {
            for (unsigned int j = i+1; j < balls.size(); ++j) {
                if(balls[j].valid) {
                    double distance = (balls[i].pos + balls[j].pos).abs();
                    // check 'loose' distance
                    if(distance <= (params.t1*params.t1)) {
                        balls[i].add(balls[j]);
                        // check 'tight' distance
                        if(distance <= (params.t2*params.t2)) {
                            balls[j].valid = false;
                        }
                        // remember the largest cluster
                        if(i != largest && balls[i].size > balls[largest].size) {
                            largest = i;
                        }
                    }
                }
            } // for j
        }
    } // for i

    // do we got any valid balls
    if(balls.size() > 0) {
        getTeamBallModel().valid = true;
        getTeamBallModel().time = getFrameInfo().getTime();
        getTeamBallModel().rmse = 0.0;
        getTeamBallModel().positionOnField = balls[largest].center();
        getTeamBallModel().position = getRobotPose() / getTeamBallModel().positionOnField;
    } else if(getFrameInfo().getTimeSince(getTeamBallModel().time) > params.maxTimeTbIsValidWithoutUpdate) {
        // no update since some time
        getTeamBallModel().valid = false;
    }

    DEBUG_REQUEST("TeamBallLocatorSimple:draw_ball_on_field",
        FIELD_DRAWING_CONTEXT;
        PEN((getTeamBallModel().valid ? "FF0000" : "999999"), 20);
        FILLOVAL(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 50, 50);
        TEXT_DRAWING(getTeamBallModel().positionOnField.x+100, getTeamBallModel().positionOnField.y+100, (balls.size() > 0 ? balls[largest].size : 0));
    );
}
