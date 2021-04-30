/**
* @file BodyStateProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BodyStateProvider
*/

#include "BodyStateProvider.h"

// Debug
#include "Tools/Debug/DebugModify.h"

BodyStateProvider::BodyStateProvider()
{
  getDebugParameterList().add(&params);
}

BodyStateProvider::~BodyStateProvider()
{
  getDebugParameterList().remove(&params);
}

void BodyStateProvider::execute()
{
  // update the fall down state (written by Heinrich Mellmann at 1 am in Magdeburg)
  updateTheFallDownState();

  // ... :)
  updateTheFootState();
  PLOT("BodyStateProvider:standByLeftFoot", getBodyState().standByLeftFoot);
  PLOT("BodyStateProvider:standByRightFoot", getBodyState().standByRightFoot);
  PLOT("BodyStateProvider:foot_state_time", getFrameInfo().getTimeSince(getBodyState().foot_state_time));

  // 
  updateTheLegTemperature();
  updateIsLiftedUp();
  updateIsReadyToWalk();
  
  // why do we need this?
  if(getBatteryData().current < -0.5) {
    getBodyState().isDischarging = true;
  } else {
    getBodyState().isDischarging = false;
  }

  if(getBatteryData().current >= params.batteryChargingThreshold) {
    getBodyState().isCharging = true;
  } else {
    getBodyState().isCharging = false;
  }

  batteryChargeBuffer.add(getBatteryData().charge);
  if(batteryChargeBuffer.isFull()) {
    getBodyState().batteryCharge = batteryChargeBuffer.getAverage();
  } else {
    getBodyState().batteryCharge = getBatteryData().charge;
  }

}//end execute

void BodyStateProvider::updateTheFootState()
{
  bool old_standByLeftFoot = getBodyState().standByLeftFoot;
  bool old_standByRightFoot = getBodyState().standByRightFoot;

  getBodyState().standByLeftFoot = getGroundContactModel().leftGroundContact;
  getBodyState().standByRightFoot = getGroundContactModel().rightGroundContact;

  if(old_standByLeftFoot != getBodyState().standByLeftFoot ||
     old_standByRightFoot != getBodyState().standByRightFoot)
  {
    getBodyState().foot_state_time = getFrameInfo().getTime();
  }
}//end updateTheFootState


void BodyStateProvider::updateTheFallDownState()
{
  BodyState::State old_fall_down_state = getBodyState().fall_down_state;

  // buffer the inertial data
  inertialBuffer.add(getInertialSensorData().data);

  Vector2d avg = inertialBuffer.getAverage();

  // the state is undefined by default
  getBodyState().fall_down_state = BodyState::undefined;

  if(fabs(avg.x) < params.upright_threshold && fabs(avg.y) < params.upright_threshold) {
    getBodyState().fall_down_state = BodyState::upright;
  }

  if(avg.x < -params.getup_threshold) {
    getBodyState().fall_down_state = BodyState::lying_on_left_side;
  } else if(avg.x > params.getup_threshold) {
    getBodyState().fall_down_state = BodyState::lying_on_right_side;
  }

  if(avg.y < -params.getup_threshold) {
    getBodyState().fall_down_state = BodyState::lying_on_back;
  } else if(avg.y > params.getup_threshold) {
    getBodyState().fall_down_state = BodyState::lying_on_front;
  }

  if(old_fall_down_state != getBodyState().fall_down_state) {
    getBodyState().fall_down_state_time = getFrameInfo().getTime();
  }
}//end updateTheFallDownState


void BodyStateProvider::updateTheLegTemperature()
{
  static naoth::JointData::JointID leftLegId[] = 
  {
    naoth::JointData::LHipPitch,
    naoth::JointData::LHipRoll,
    naoth::JointData::LKneePitch,
    naoth::JointData::LAnklePitch,
    naoth::JointData::LAnkleRoll
  };

  static naoth::JointData::JointID rightLegId[] = 
  {
    naoth::JointData::RHipPitch,
    naoth::JointData::RHipRoll,
    naoth::JointData::RKneePitch,
    naoth::JointData::RAnklePitch,
    naoth::JointData::RAnkleRoll
  };

  double tempL(0);
  double tempR(0);

  for(int i = 0; i < 5; i++)
  {
    tempL = std::max(tempL, getSensorJointData().temperature[leftLegId[i]]);
    tempR = std::max(tempR, getSensorJointData().temperature[rightLegId[i]]);
  }

  getBodyState().temperatureLeftLeg = tempL;
  getBodyState().temperatureRightLeg = tempR;
}//end updateTheLegTemperature

void BodyStateProvider::updateIsLiftedUp()
{
  double lifted_up_time = getMotionStatus().currentMotion == motion::walk? params.lifted_up_time_walk: params.lifted_up_time;

  getBodyState().isLiftedUp = 
     getBodyState().fall_down_state == BodyState::upright && 
    !getBodyState().standByLeftFoot && 
    !getBodyState().standByRightFoot && // no foot is on the ground
     getFrameInfo().getTimeSince(getBodyState().foot_state_time) > lifted_up_time;
}

void BodyStateProvider::updateIsReadyToWalk()
{
    if(getBodyState().fall_down_state == BodyState::upright // not fallen
            && !getBodyState().isLiftedUp                   // not lifted up
            && determineReadyToWalkState())                 // ready to walk
    {
        getBodyState().readyToWalk = true;
    } else {
        getBodyState().readyToWalk = false;
    }
}

bool BodyStateProvider::determineReadyToWalkState()
{
    // already walking or definitly standing
    return getMotionStatus().currentMotion == motion::walk
       || (getMotionStatus().currentMotion == motion::stand && getMotionStatus().target_reached);
}
