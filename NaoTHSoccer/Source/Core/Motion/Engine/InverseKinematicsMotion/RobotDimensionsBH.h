/**
 * @file RobotDimensions.h
 *
 * Description of the Dimensions of the Kondo Robot
 *
 * @author Cord Niehaus
 */

#pragma once

#include "Tools/Math/Vector3.h"


class RobotDimensions
{
public:
  RobotDimensions()
  {
    lengthBetweenLegs = 100.f;
    upperLegLength = 100.f;
    lowerLegLength = 102.9f; // 102.75 / 102.74
    heightLeg5Joint = 45.19f;
    zLegJoint1ToHeadPan = 211.5f;
    xHeadTiltToCamera = 48.8f;
    zHeadTiltToCamera = 23.81f;
    headTiltToCameraTilt = 0.698132f;
    xHeadTiltToUpperCamera = 53.9f;
    zHeadTiltToUpperCamera = 67.9f;
    headTiltToUpperCameraTilt = 0.f;
    armOffset = Vector3<double>(0,98,185);
    yElbowShoulder = 15.f;
    upperArmLength = 105.f;
    lowerArmLength = 130.f; // estimated
    imageRecordingTime = 0.034f;
    imageRecordingDelay = -0.003f;
  }

private:
  float xHeadTiltToCamera;         //!<forward offset between head tilt joint and lower camera
  float zHeadTiltToCamera;         //!<height offset between head tilt joint and lower camera
  float headTiltToCameraTilt;      //!<tilt of lower camera against head tilt

  float xHeadTiltToUpperCamera;    //!<forward offset between head tilt joint and upper camera
  float zHeadTiltToUpperCamera;    //!<height offset between head tilt joint and upper camera
  float headTiltToUpperCameraTilt; //!<tilt of upper camera against head tilt

public:
  float lengthBetweenLegs;         //!<length between leg joints LL1 and LR1
  float upperLegLength;            //!<length between leg joints LL2 and LL3 in z-direction
  float lowerLegLength;            //!<length between leg joints LL3 and LL4 in z-direction
  float heightLeg5Joint;           //!<height of leg joints LL4 and LR4 of the ground

  float zLegJoint1ToHeadPan;       //!<height offset between LL1 and head pan joint

  Vector3<double> armOffset;             //! The offset of the first left arm joint relative to the middle between the hip joints
  float yElbowShoulder;            //! The offset between the elbow joint and the shoulder joint in y
  float upperArmLength;            //! The length between the shoulder and the elbow in y-direction
  float lowerArmLength;            //!< height off lower arm starting at arm2/arm3

  float imageRecordingTime;        //! Time the camera requires to take an image (in s, for motion compensation, may depend on exposure).
  float imageRecordingDelay;       //! Delay after the camera took an image (in s, for motion compensation).

  /**
   * forward offset between head tilt joint and current camera
   * @param lowerCamera true, if lower camera is in use, false otherwise.
   */
  inline float getXHeadTiltToCamera(bool lowerCamera) const { return lowerCamera ? xHeadTiltToCamera : xHeadTiltToUpperCamera; }

  /**
   * height offset between head tilt joint and current camera
   * @param lowerCamera true, if lower camera is in use, false otherwise.
   */
  inline float getZHeadTiltToCamera(bool lowerCamera) const { return lowerCamera ? zHeadTiltToCamera : zHeadTiltToUpperCamera; }

  /**
   * tilt of current camera against head tilt
   * @param lowerCamera true, if lower camera is in use, false otherwise.
   */
  inline float getHeadTiltToCameraTilt(bool lowerCamera) const { return lowerCamera ? headTiltToCameraTilt : headTiltToUpperCameraTilt; }
};
