#include "TeamBallLocatorCanopyCluster.h"

TeamBallLocatorCanopyCluster::TeamBallLocatorCanopyCluster()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("TeamBallLocatorCanopyCluster:draw_all_balls", "draws all communicated balls on the field", false);
    DEBUG_REQUEST_REGISTER("TeamBallLocatorCanopyCluster:draw_teamball_input", "draw all the balls uses for teamball", false);
    DEBUG_REQUEST_REGISTER("TeamBallLocatorCanopyCluster:draw_ball_on_field", "draw the team ball model on the field", false);
}

TeamBallLocatorCanopyCluster::~TeamBallLocatorCanopyCluster()
{
    getDebugParameterList().remove(&params);
}

void TeamBallLocatorCanopyCluster::execute() {
    // valid balls of all player
    std::vector<Ball> balls;

    // iterate over all robots(messages) and collect 'valid' balls
    for (const auto& it: getTeamState().players) {
        unsigned int playerNumber = it.first;
        const auto& player = it.second;

        // check if the robot is able to play (inactive robots)
        if(params.enablePlayingCheck && !getTeamMessagePlayersState().isPlaying(playerNumber)) { continue; }

        // ballage + network delay
        double ballAge = player.ballAge();
        if(params.enableNtpAdjustment && getTeamMessageNTP().isNtpActive(playerNumber)) {
            ballAge += static_cast<double>(getTeamMessageNTP().getTimeInMilliSeconds(playerNumber) - player.messageTimestamp);
        }

        DEBUG_REQUEST("TeamBallLocatorCanopyCluster:draw_all_balls",
            FIELD_DRAWING_CONTEXT;
            PEN("666666", 20);
            auto drawing_position = player.pose() * player.ballPosition();
            CIRCLE(drawing_position.x, drawing_position.y, 50);
            TEXT_DRAWING(drawing_position.x+100, drawing_position.y+100, playerNumber);
            TEXT_DRAWING(drawing_position.x+100, drawing_position.y-100, ballAge);
        );

        // -1 means "ball never seen", ballAge (incl. network delay) should be small
        if(player.ballAge() >= 0 && ballAge <= params.maxBallAge)
        {
            Vector2d globalBallPosition = player.pose() * player.ballPosition();

            // if activated via parameter, skip balls, which are outside the field!
            if(!params.ballsAreOnlyValidOnField || getFieldInfo().insideCarpet(globalBallPosition) ) {

              // global position of the ball and time last seen
              balls.push_back(Ball(globalBallPosition));
            }
        }
    }

    DEBUG_REQUEST("TeamBallLocatorCanopyCluster:draw_teamball_input",
        FIELD_DRAWING_CONTEXT;
        PEN("0000FF", 20);
        for(const Ball& b : balls) {
            CIRCLE(b.pos.x, b.pos.y, 50);
            PEN("7f00ff", 20);
            CIRCLE(b.pos.x, b.pos.y, params.t1);
            PEN("007fff", 20);
            CIRCLE(b.pos.x, b.pos.y, params.t2);
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
                    double distance = (balls[i].pos - balls[j].pos).abs2();
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

    DEBUG_REQUEST("TeamBallLocatorCanopyCluster:draw_ball_on_field",
        FIELD_DRAWING_CONTEXT;
        PEN((getTeamBallModel().valid ? "FF0000" : "999999"), 20);
        FILLOVAL(getTeamBallModel().positionOnField.x, getTeamBallModel().positionOnField.y, 50, 50);
        TEXT_DRAWING(getTeamBallModel().positionOnField.x+100, getTeamBallModel().positionOnField.y+100, (balls.size() > 0 ? balls[largest].size : 0));
    );
}
