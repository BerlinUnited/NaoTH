/*
 * @file MinimalPathPlanner.cpp
 *
 * @author <a href="mailto:johan.hartung@informatik.hu-berlin.de">Johan
 * Hartung</a> Implementation of class MinimalPathPlanner
 */

#include "MinimalPathPlanner.h"
#include "Tools/Math/Polygon.h"
#include "Tools/Math/Line.h"
#include <forward_list>

MinimalPathPlanner::MinimalPathPlanner()
    : target_reached(true),
      stepBuffer({}),
      footToUse(Foot::RIGHT),
      lastStepRequestID(
          getMotionStatus().stepControl.stepRequestID +
          1),  // WalkRequest stepRequestID starts at 0, we have to start at 1
      kickPlanned(false) {
    DEBUG_REQUEST_REGISTER("MinimalPathPlanner:kick", "...", false);
    getDebugParameterList().add(&params);
}

MinimalPathPlanner::~MinimalPathPlanner() {
    getDebugParameterList().remove(&params);
}


void MinimalPathPlanner::execute() {
    getPathStatus().kick_executed = false;

    // Always executed first
    manageStepBuffer();

    // The kick has been executed
    // Tells XABSL to jump into next state
    if (kickPlanned && stepBuffer.empty()) {
      getPathStatus().kick_executed = true;
    }

    // HACK: xabsl set a forced motion request => clear everything
    if (getPathRequest().path2018_routine ==
      PathRequest::PathID::NONE &&
        getMotionRequest().forced) {
        stepBuffer.clear();
        return;
    }

    // hack
    DEBUG_REQUEST(
        "MinimalPathPlanner:kick",
        if (getPathRequest().path2018_routine == PathRequest::PathID::NONE) {
            if (stepBuffer.empty()) {
                forwardKick();
            }
            executeStepBuffer();
        });

    switch (getPathRequest().path2018_routine) {
        case PathRequest::PathID::NONE:
            if (kickPlanned) {
                kickPlanned = false;
            }

            // TODO: should the stepBuffer just be cleared here no matter what?
            if (stepBuffer.empty()) {
                return;
            }
            break;
        case PathRequest::PathID::FORWARDKICK:
            if (nearApproach_forwardKick(params.forwardKickOffset.x,
                                         params.forwardKickOffset.y)) {
                forwardKick();
            }
            break;
    }

    // Always executed last
    executeStepBuffer();

    PLOT("PathPlanner:buffer_size", static_cast<double>(stepBuffer.size()));
}

bool MinimalPathPlanner::farApproach() {
    return false;  
    if (!stepBuffer.empty()) return false;

    Vector2d ballPos        = getBallModel().positionPreview;
    double numPossibleSteps = ballPos.abs() / params.stepLength;

    if (numPossibleSteps <= params.farToNearApproachThreshold) return true;

    double translation_xy = params.stepLength;

    StepBufferElement far_approach_step;
    far_approach_step.debug_name = "far_approach_step";
    far_approach_step.setPose({ballPos.angle(), translation_xy,
                               std::min(translation_xy, std::abs(ballPos.y)) *
                                   (ballPos.y < 0 ? -1 : 1)});
    far_approach_step.setStepType(StepType::WALKSTEP);
    far_approach_step.setCharacter(0.7);
    far_approach_step.setScale(1.0);
    far_approach_step.setCoordinate(Coordinate::Hip);
    far_approach_step.setFoot(Foot::NONE);
    far_approach_step.setSpeedDirection(Math::fromDegrees(0.0));
    far_approach_step.setRestriction(RestrictionMode::HARD);
    far_approach_step.setProtected(false);
    far_approach_step.setTime(250);

    stepBuffer.push_back(far_approach_step);

    return false;
}

bool MinimalPathPlanner::nearApproach_forwardKick(const double offsetX,
                                                  const double offsetY) {
    return true;
}

void MinimalPathPlanner::forwardKick() {
    if (kickPlanned) return;
    // ball
    Vector2d ballPos      = getBallModel().positionPreviewInRFoot;
    Foot actual_foot      = Foot::RIGHT;
    Coordinate coordinate = Coordinate::LFoot;

    // The kick
    StepBufferElement forward_kick_step;
    forward_kick_step
        .setPose({0.0, 0.0, 0.0})  // kick straight forward
        .setStepType(StepType::KICKSTEP)
        .setCharacter(1.0)
        .setScale(0.7)
        .setCoordinate(coordinate)
        .setFoot(actual_foot)
        .setSpeedDirection(Math::fromDegrees(0.0))
        .setRestriction(RestrictionMode::SOFT)
        .setProtected(true)
        .setTime(params.forwardKickTime);

    stepBuffer.push_back(forward_kick_step);

    kickPlanned = true;
}

void MinimalPathPlanner::manageStepBuffer() {
    if (stepBuffer.empty()) return;

    // requested step has been accepted
    if (lastStepRequestID == getMotionStatus().stepControl.stepRequestID) {
        stepBuffer.erase(stepBuffer.begin());
        lastStepRequestID = getMotionStatus().stepControl.stepRequestID + 1;
    }
}

void MinimalPathPlanner::executeStepBuffer() {
    STOPWATCH_START("MinimalPathPlanner:execute_steplist");

    if (stepBuffer.empty()) return;

    footToUse = Foot::RIGHT;

    // set motion request
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.stepControl.stepID =
        getMotionStatus().stepControl.stepID;

    getMotionRequest().walkRequest.coordinate = stepBuffer.front().coordinate;
    getMotionRequest().walkRequest.character  = stepBuffer.front().character;

    getMotionRequest().walkRequest.stepControl.scale = stepBuffer.front().scale;
    getMotionRequest().walkRequest.stepControl.type  = stepBuffer.front().type;
    getMotionRequest().walkRequest.stepControl.time  = stepBuffer.front().time;
    getMotionRequest().walkRequest.stepControl.speedDirection =
        stepBuffer.front().speedDirection;
    getMotionRequest().walkRequest.stepControl.target = stepBuffer.front().pose;
    getMotionRequest().walkRequest.stepControl.restriction =
        stepBuffer.front().restriction;
    getMotionRequest().walkRequest.stepControl.isProtected =
        stepBuffer.front().isProtected;
    getMotionRequest().walkRequest.stepControl.stepRequestID =
        lastStepRequestID;
    getMotionRequest().walkRequest.stepControl.moveLeftFoot =
        (footToUse != Foot::RIGHT);  // false means right foot

    // std::cout << stepBuffer.front().debug_name << " - " <<
    // getMotionRequest().walkRequest.stepControl.moveLeftFoot  << std::endl;
    STOPWATCH_STOP("MinimalPathPlanner:execute_steplist");
}