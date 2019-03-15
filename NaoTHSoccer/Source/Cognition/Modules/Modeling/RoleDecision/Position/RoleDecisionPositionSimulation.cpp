#include "RoleDecisionPositionSimulation.h"
#include <Tools/Math/Probabilistics.h>

RoleDecisionPositionSimulation::RoleDecisionPositionSimulation()
{
    getDebugParameterList().add(&params);
    //DEBUG_REQUEST_REGISTER("RoleDecision:Position:draw_positions_static", "draw role positions on the field", false);
}


RoleDecisionPositionSimulation::~RoleDecisionPositionSimulation()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionSimulation::execute()
{
    // current ball position
    Vector2d globalBallStartPosition = getTeamBallModel().positionOnField;

    std::vector<Vector2d> particles;

    for(int j=0; j < params.numParticles; ++j) {
        particles.push_back(predict(globalBallStartPosition, true));
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
