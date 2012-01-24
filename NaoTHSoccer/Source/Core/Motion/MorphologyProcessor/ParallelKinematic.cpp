/**
 * @file ParallelKinematic.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "ParallelKinematic.h"

using namespace naoth;

namespace Kinematics {

  void ParallelKinematic::calculateLegs(const Pose& pose, JointData& jointData) {
    double alpha = -0.5 * pose.alpha.z;

    // left leg
    jointData.position[JointData::LHipPitch] = alpha + pose.alpha.x - (pose.rotation.y / 4);
    jointData.position[JointData::LKneePitch] = pose.alpha.z + (pose.rotation.y / 2);
    jointData.position[JointData::LAnklePitch] = alpha - pose.alpha.x - (pose.rotation.y / 4);

    jointData.position[JointData::LHipRoll] = -pose.alpha.y + (pose.rotation.x / 2);
    jointData.position[JointData::LAnkleRoll] = pose.alpha.y + (pose.rotation.x / 2);


    // right leg
    jointData.position[JointData::RHipPitch] = alpha + pose.alpha.x - (pose.rotation.y / 4);
    jointData.position[JointData::RKneePitch] = pose.alpha.z + (pose.rotation.y / 2) ;
    jointData.position[JointData::RAnklePitch] = alpha - pose.alpha.x - (pose.rotation.y / 4);

    jointData.position[JointData::RHipRoll] = -pose.alpha.y + (pose.rotation.x / 2);
    jointData.position[JointData::RAnkleRoll] = pose.alpha.y + (pose.rotation.x / 2);
  }//end calculateLegs

  void ParallelKinematic::calculateLegs(const Pose& poseLeft, const Pose& poseRight, JointData& jointData) {
    double alphaLeft = -0.5 * poseLeft.alpha.z;

    // left leg
    jointData.position[JointData::LHipPitch] = alphaLeft + poseLeft.alpha.x - (poseLeft.rotation.y / 4);
    jointData.position[JointData::LKneePitch] = poseLeft.alpha.z - (poseLeft.rotation.y / 2);
    jointData.position[JointData::LAnklePitch] = alphaLeft - poseLeft.alpha.x - (poseLeft.rotation.y / 4);

    jointData.position[JointData::LHipRoll] = -poseLeft.alpha.y + (poseLeft.rotation.x / 2);
    jointData.position[JointData::LAnkleRoll] = poseLeft.alpha.y + (poseLeft.rotation.x / 2);


    double alphaRight = -0.5 * poseRight.alpha.z;

    // right leg
    jointData.position[JointData::RHipPitch] = alphaRight + poseRight.alpha.x - (poseRight.rotation.y / 4);
    jointData.position[JointData::RKneePitch] = poseRight.alpha.z - (poseRight.rotation.y / 2);
    jointData.position[JointData::RAnklePitch] = alphaRight - poseRight.alpha.x - (poseRight.rotation.y / 4);

    jointData.position[JointData::RHipRoll] = -poseRight.alpha.y + (poseRight.rotation.x / 2);
    jointData.position[JointData::RAnkleRoll] = poseRight.alpha.y + (poseRight.rotation.x / 2);
  }//end calculateLegs

}//end namespace Kinematics