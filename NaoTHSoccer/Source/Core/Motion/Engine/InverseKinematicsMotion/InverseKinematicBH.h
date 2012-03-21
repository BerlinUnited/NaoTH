/**
 * @file InverseKinematic.h
 * @author Alexander Härtl
 * @author jeff
 */

#pragma once

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Common.h"
#include "RobotDimensionsBH.h"
#include "Representations/Infrastructure/JointData.h"

using namespace naoth;

class InverseKinematicBH
{
public:
  /**
  * The method calculates the joint angles for the legs of the robot from a Pose3D for each leg
  * @param positionLeft The desired position (translation + rotation) of the left foots ankle point
  * @param positionRight The desired position (translation + rotation) of the right foots ankle point
  * @param jointData The instance of JointData where the resulting joints are written into
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @param ratio The ratio of
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJoints(const Pose3D& positionLeft, const Pose3D& positionRight, JointData& jointData, const RobotDimensions& robotDimensions, double ratio = 0.5f)
  {
    bool reachable = true;
    if(!calcLegJoints(positionLeft, jointData, true, robotDimensions))
      reachable = false;
    if(!calcLegJoints(positionRight, jointData, false, robotDimensions))
      reachable = false;

    ratio = Math::clamp(ratio,0.0, 1.0);
    // the hip joints of both legs must be equal, so it is computed as weighted mean and the foot positions are
    // recomputed with fixed joint0 and left open foot rotation (as possible failure)
    double joint0 = jointData.position[JointData::LHipYawPitch] * ratio + jointData.position[JointData::RHipYawPitch] * (1 - ratio);
    if(!calcLegJoints(positionLeft, jointData, joint0, true, robotDimensions))
      reachable = false;
    if(!calcLegJoints(positionRight, jointData, joint0, false, robotDimensions))
      reachable = false;
    return reachable;
  }

private:
  /**
  * The method calculates the joint angles of one leg of the robot from a Pose3D
  * @param position The desired position (translation + rotation) of the foots ankle point
  * @param jointData The instance of JointData where the resulting joints are written into
  * @param left Determines if the left or right leg is calculated
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJoints(const Pose3D& position, JointData& jointData, bool left, const RobotDimensions& robotDimensions)
  {
    Pose3D target(position);
    JointData::JointID firstJoint(left ? JointData::LHipYawPitch : JointData::RHipYawPitch);
    int sign(left ? -1 : 1);
    target.translation.y += (double) robotDimensions.lengthBetweenLegs / 2.f * sign; // translate to origin of leg
    // rotate by 45° around origin for Nao
    // calculating sqrt(2) is faster than calculating the resp. rotation matrix with getRotationX()
    static const double sqrt2_2 = sqrt(2.0f) * 0.5f;
    RotationMatrix rotationX_pi_4 = RotationMatrix(Vector3<double>(1, 0, 0), Vector3<double>(0, sqrt2_2, sqrt2_2 * sign), Vector3<double>(0, sqrt2_2 * -sign, sqrt2_2));
    target.translation = rotationX_pi_4 * target.translation;
    target.rotation = rotationX_pi_4 * target.rotation;

    target = target.invert(); // invert pose to get position of body relative to foot

    // use geometrical solution to compute last three joints
    double length = target.translation.abs();
    double sqrLength = length * length;
    double upperLeg = robotDimensions.upperLegLength;
    double sqrUpperLeg = upperLeg * upperLeg;
    double lowerLeg = robotDimensions.lowerLegLength;
    double sqrLowerLeg = lowerLeg * lowerLeg;
    double cosLowerLeg = (sqrLowerLeg + sqrLength - sqrUpperLeg) / (2 * lowerLeg * length);
    double cosKnee = (sqrUpperLeg + sqrLowerLeg - sqrLength) / (2 * upperLeg * lowerLeg);

    // clip for the case of unreachable position
    //const Range<> clipping(-1.0f, 1.0f);
    bool reachable = true;
    if(!(-1 <= cosKnee && cosKnee <= 1) || (-1 <= cosLowerLeg && cosLowerLeg <= 1))
    {
      cosKnee = Math::clamp(cosKnee,-1.0,1.0); //clipping.limit(cosKnee);
      cosLowerLeg = Math::clamp(cosLowerLeg,-1.0,1.0);//clipping.limit(cosLowerLeg);
      reachable = false;
    }
    double joint3 = Math::pi - acos(cosKnee); // implicitly solve discrete ambiguousness (knee always moves forward)
    double joint4 = -acos(cosLowerLeg);
    joint4 -= atan2(target.translation.x, Vector2<double>(target.translation.y, target.translation.z).abs());
    double joint5 = atan2(target.translation.y, target.translation.z) * sign;

    // calulate rotation matrix before hip joints
    RotationMatrix hipFromFoot;
    hipFromFoot.rotateX(joint5 * -sign);
    hipFromFoot.rotateY(-joint4 - joint3);

    // compute rotation matrix for hip from rotation before hip and desired rotation
    RotationMatrix hip = hipFromFoot.invert() * target.rotation;

    // compute joints from rotation matrix using theorem of euler angles
    // see http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // this is possible because of the known order of joints (z, x, y seen from body resp. y, x, z seen from foot)
    double joint1 = asin(-hip[2].y) * -sign;
    joint1 -= Math::pi_4; // because of the 45°-rotational construction of the Nao legs
    double joint2 = -atan2(hip[2].x, hip[2].z);
    double joint0 = atan2(hip[0].y, hip[1].y) * -sign;

    // set computed joints in jointData
    jointData.position[firstJoint + 0] = joint0;
    jointData.position[firstJoint + 1] = joint1;
    jointData.position[firstJoint + 2] = joint2;
    jointData.position[firstJoint + 3] = joint3;
    jointData.position[firstJoint + 4] = joint4;
    jointData.position[firstJoint + 5] = joint5;

    return reachable;
  }

  /**
  * The method calculates the joint angles of one leg of the Nao from a Pose3D with a fixed first joint
  * This is necessary because the Nao has mechanically connected hip joints, hence not every
  * combination of foot positions can be reached and has to be recalculated with equal joint0 for both legs
  * the rotation of the foot around the z-axis through the ankle-point is left open as "failure"
  * @param position The desired position (translation + rotation) of the foots ankle point
  * @param jointData The instance of JointData where the resulting joints are written into
  * @param joint0 Fixed value for joint0 of the respective leg
  * @param left Determines if the left or right leg is calculated
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJoints(const Pose3D& position, JointData& jointData, double joint0, bool left, const RobotDimensions& robotDimensions)
  {
    Pose3D target(position);
    JointData::JointID firstJoint(left ? JointData::LHipYawPitch : JointData::RHipYawPitch);
    const int sign(left ? -1 : 1);
    target.translation.y += robotDimensions.lengthBetweenLegs / 2 * sign; // translate to origin of leg
    target = Pose3D().rotateZ(joint0 * -sign).rotateX(Math::pi_4 * sign).conc(target); // compute residual transformation with fixed joint0

    // use cosine theorem and arctan to compute first three joints
    double length = target.translation.abs();
    double sqrLength = length * length;
    double upperLeg = robotDimensions.upperLegLength;
    double sqrUpperLeg = upperLeg * upperLeg;
    double lowerLeg = robotDimensions.lowerLegLength;
    double sqrLowerLeg = lowerLeg * lowerLeg;
    double cosUpperLeg = (sqrUpperLeg + sqrLength - sqrLowerLeg) / (2 * upperLeg * length);
    double cosKnee = (sqrUpperLeg + sqrLowerLeg - sqrLength) / (2 * upperLeg * lowerLeg);
    // clip for the case that target position is not reachable
    //const Range<> clipping(-1.0f, 1.0f);
    bool reachable = true;
    /*
    if(!clipping.isInside(cosKnee) || clipping.isInside(upperLeg))
    {
      cosKnee = clipping.limit(cosKnee);
      cosUpperLeg = clipping.limit(cosUpperLeg);
      reachable = false;
    }
    */
    if(!(-1 <= cosKnee && cosKnee <= 1) || (-1 <= upperLeg && upperLeg <= 1))
    {
      cosKnee = Math::clamp(cosKnee,-1.0,1.0); //clipping.limit(cosKnee);
      cosUpperLeg = Math::clamp(cosUpperLeg,-1.0,1.0);//clipping.limit(cosLowerLeg);
      reachable = false;
    }

    double joint1 = target.translation.z == 0.0f ? 0.0f : atan(target.translation.y / -target.translation.z) * sign;
    double joint2 = -acos(cosUpperLeg);
    joint2 -= atan2(target.translation.x, Vector2<double>(target.translation.y, target.translation.z).abs() * -Math::sgn(target.translation.z));
    double joint3 = Math::pi - acos(cosKnee);
    RotationMatrix beforeFoot = RotationMatrix().rotateX(joint1 * sign).rotateY(joint2 + joint3);
    joint1 -= Math::pi_4; // because of the strange hip of Nao

    // compute joints from rotation matrix using theorem of euler angles
    // see http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // this is possible because of the known order of joints (y, x, z) where z is left open and is seen as failure
    RotationMatrix foot = beforeFoot.invert() * target.rotation;
    double joint5 = asin(-foot[2].y) * -sign * -1;
    double joint4 = -atan2(foot[2].x, foot[2].z) * -1;
    //double failure = atan2(foot[0].y, foot[1].y) * sign;

    // set computed joints in jointData
    jointData.position[firstJoint + 0] = joint0;
    jointData.position[firstJoint + 1] = joint1;
    jointData.position[firstJoint + 2] = joint2;
    jointData.position[firstJoint + 3] = joint3;
    jointData.position[firstJoint + 4] = joint4;
    jointData.position[firstJoint + 5] = joint5;

    return reachable;
  }

};
