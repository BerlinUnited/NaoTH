#include "RoleDecisionPositionSimulation.h"
#include <Tools/Math/Probabilistics.h>

RoleDecisionPositionSimulation::RoleDecisionPositionSimulation()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:s_draw_simulated_balls", "draw the simulated balls on the field", false);
}


RoleDecisionPositionSimulation::~RoleDecisionPositionSimulation()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionSimulation::execute()
{
    if(getTeamBallModel().valid) {
        // init particles with current ball position
        std::vector<Vector2d> particles(params.numParticles, getTeamBallModel().positionOnField);

        // TODO: calculate driection to the own goal
        // TODO: handle goals/outs/....
        // TODO: handle players on the shooting line

        for (int i = 0; i < params.numIteration; ++i) {
            for(auto& p : particles) {
                p = predict(p, true);
                DEBUG_REQUEST("RoleDecision:Position:s_draw_simulated_balls",
                    FIELD_DRAWING_CONTEXT;
                    PEN(Color(0.67, (static_cast<double>(i)/params.numIteration), (static_cast<double>(i)/params.numIteration)), 20);
                    CIRCLE(p.x, p.y, 10);
                );
            }
        }
    }
}

Vector2d RoleDecisionPositionSimulation::predict(const Vector2d& ball, bool noise) {
    double gforce = Math::g*1e3; // mm/s^2
    double distance;
    double angle;
    if (noise){
        double speed = Math::generateGaussianNoise(params.ball_speed, params.ball_speed_std);
        angle = Math::generateGaussianNoise(Math::fromDegrees(params.ball_angle), Math::fromDegrees(params.ball_angle_std));
        distance = speed*speed / getFieldInfo().ballRRCoefficient / gforce / 2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
    }
    else {
        distance = params.ball_speed*params.ball_speed / getFieldInfo().ballRRCoefficient / gforce / 2.0; // friction*mass*gforce*distance = 1/2*mass*speed*speed
        angle = Math::fromDegrees(params.ball_angle);
    }
  Vector2d predictedAction(distance, 0.0);
  predictedAction.rotate(angle);

  return ball + predictedAction;
}
