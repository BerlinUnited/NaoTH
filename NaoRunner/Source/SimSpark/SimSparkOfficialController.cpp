/**
 * @file SimSparkOfficialController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief for running in the simulation league: with official simualtor
 *
 */

#include <map>
#include <vector>

#include "SimSparkOfficialController.h"
//#include "Representations/Infrastructure/FieldInfo.h"

void SimSparkOfficialController::get(Image& /*data*/)
{
  ASSERT(isNewImage);
//  theVirtualVisionProvider.theVirtualVision = theVirtualVision;
//  updateInertialSensorByVision();
  isNewImage = false;
}

void SimSparkOfficialController::updateInertialSensorByVision()
{
  if (flagGlobalPositions.empty())
  {
    if (!setupFlagGlobalPositions()) return;
  }

  map<string, Vector3d> flagVision;
  for (map<string, Vector3d>::const_iterator iter = theVirtualVision.data.begin(); iter != theVirtualVision.data.end(); ++iter)
  {
    if (flagGlobalPositions.find(iter->first) != flagGlobalPositions.end())
    {
      flagVision[iter->first] = iter->second;
    }
  }

  if (flagVision.size() < 3) return;

  Vector3d cameraPos = calculateCameraPosFromVision(flagVision);
  RotationMatrix cameraRot = calculateCameraRotFromVision(cameraPos, flagVision);
  RotationMatrix bodyRot = cameraRot;
  bodyRot.rotateY(-theSensorJointData.position[JointData::HeadPitch]).rotateZ(-theSensorJointData.position[JointData::HeadYaw]);
  bodyRot.rotateZ(-bodyRot.getZAngle());
  double angX = atan2(bodyRot[1].z, bodyRot[1].y);
  double angY = atan2(bodyRot[2].x, bodyRot[0].x);
  double k = 0.1;
//  theInertialSensorData.data[InertialSensorData::X] = (1-k)*theInertialSensorData.data[InertialSensorData::X] + k*angX;
//  theInertialSensorData.data[InertialSensorData::Y] = (1-k)*theInertialSensorData.data[InertialSensorData::Y] + k*angY;
}

Vector3d SimSparkOfficialController::calculateCameraPosFromVision(const map<string, Vector3d>& flagVision) const
{
  map<string, Vector3d>::const_iterator fa = flagVision.begin();
  map<string, Vector3d>::const_iterator fend = flagVision.end();
  vector<Vector3d> positions;
  while (fa != fend)
  {
    map<string, Vector3d>::const_iterator fb = fa;
    ++fb;
    while (fb != fend)
    {
      map<string, Vector3d>::const_iterator fc = fb;
      ++fc;
      while (fc != fend)
      {
        Vector3d p = calculateCameraPosFromThreeFlag(fa, fb, fc);
        positions.push_back(p);
        ++fc;
      }
      ++fb;
    }
    ++fa;
  }

  Vector3d pos(0, 0, 0);
  if (!positions.empty())
  {
    for (vector<Vector3d>::const_iterator i = positions.begin(); i != positions.end(); ++i)
    {
      pos += *i;
    }
    pos /= positions.size();
  }
  
  return pos;
}

Vector3d SimSparkOfficialController::calculateCameraPosFromThreeFlag(map<string, Vector3d>::const_iterator fa,
  map<string, Vector3d>::const_iterator fb, map<string, Vector3d>::const_iterator fc) const
{
  const Vector3d& posA = flagGlobalPositions.find(fa->first)->second;
  const Vector3d& polA = fa->second;
  const Vector3d lposA = pol2xyz(polA);
  const Vector3d& posB = flagGlobalPositions.find(fb->first)->second;
  const Vector3d& polB = fb->second;
  const Vector3d lposB = pol2xyz(polB);
  const Vector3d& posC = flagGlobalPositions.find(fc->first)->second;
  const Vector3d& polC = fc->second;
  const Vector3d lposC = pol2xyz(polC);

  double dA2 = Math::sqr(polA[0]);
  double dB2 = Math::sqr(polB[0]);
  double dC2 = Math::sqr(polC[0]);

  double cA = dA2 - posA.abs2();
  double cB = dB2 - posB.abs2();
  double cC = dC2 - posC.abs2();

  double A1 = 2.0 * (posA.x - posB.x);
  double B1 = 2.0 * (posA.y - posB.y);
  double C1 = 2.0 * (posA.z - posB.z);
  double D1 = cB - cA;

  double A2 = 2.0 * (posA.x - posC.x);
  double B2 = 2.0 * (posA.y - posC.y);
  double C2 = 2.0 * (posA.z - posC.z);
  double D2 = cC - cA;

  double a = A1 * B2 - A2*B1;
  double bx = B2 * C1 - B1*C2;
  double cx = B2 * D1 - B1*D2;
  double by = A2 * C1 - A1*C2;
  double cy = A2 * D1 - A1*D2;

  double A = bx * bx + by * by + a*a;
  double B = 2 * (-bx * cx + a * posA.x * bx - by * cy - a * posA.y * by - a * a * posA.z);
  double C = cx * cx - 2 * a * posA.x * cx + cy * cy + 2 * a * posA.y * cy - a * a*cA;

  double delta = sqrt(max(B * B - 4 * A*C, 0.0));

  Vector3d p1, p2;
  p1.z = (-B + delta) / (2 * A);
  p2.z = (-B - delta) / (2 * A);

  // if a == 0 : this only happens that three flags in the line in horizontal plane
  // in current server, this means that x1 == x2 == x3
  if (a == 0)
  {
    p1.y = (D2 - C2 * p1.z) / B2;
    p2.y = (D2 - C2 * p2.z) / B2;

    float d = cA - Math::sqr(p1.z) + 2 * posA.z * p1.z - Math::sqr(p1.y) + 2 * posA.y * p1.y;
    float dd = sqrt(max(Math::sqr(posA.x) + d, 0.0));

    ASSERT(abs(p1.z - p2.z) < 0.1);
    // p1.z and p2.z are equal
    p1.x = posA.x + dd;
    p2.x = posA.x - dd;
  } else
  {
    p1.x = (cx - bx * p1.z) / a;
    p1.y = (-cy + by * p1.z) / a;
    p2.x = (cx - bx * p2.z) / a;
    p2.y = (-cy + by * p2.z) / a;
  }

  // eliminate repeated root
  Vector3d globalPlane = (posB - posA)^(posC - posA);
  double globalAng1 = (posA - p1) * (globalPlane);

  Vector3d localPlane = (lposB - lposA)^(lposC - lposA);
  double localAng = lposA * (localPlane);

  if (Math::sgn(globalAng1) == Math::sgn(localAng))
  {
    return p1;
  } else
  {
    return p2;
  }
}

bool SimSparkOfficialController::setupFlagGlobalPositions()
{
  /*
  FieldInfo fieldInfo;
  fieldInfo.loadFromFile("Config/fieldinfo/S3DL.cfg");//HACK
  switch (theGameInfo.theTeamIndex) {
  case SimSparkGameInfo::TI_LEFT:
    flagGlobalPositions["F1L"] = Vector3d(fieldInfo.xPosOwnGroundline, fieldInfo.yPosLeftFieldBorder, 0);
    flagGlobalPositions["F2L"] = Vector3d(fieldInfo.xPosOwnGroundline, fieldInfo.yPosRightFieldBorder, 0);
    flagGlobalPositions["F1R"] = Vector3d(fieldInfo.xPosOpponentGroundline, fieldInfo.yPosLeftFieldBorder, 0);
    flagGlobalPositions["F2R"] = Vector3d(fieldInfo.xPosOpponentGroundline, fieldInfo.yPosRightFieldBorder, 0);
    flagGlobalPositions["G1L"] = Vector3d(fieldInfo.xPosOwnGoal, fieldInfo.yPosLeftGoal, fieldInfo.goalHeight);
    flagGlobalPositions["G2L"] = Vector3d(fieldInfo.xPosOwnGoal, fieldInfo.yPosRightGoal, fieldInfo.goalHeight);
    flagGlobalPositions["G1R"] = Vector3d(fieldInfo.xPosOpponentGoal, fieldInfo.yPosLeftGoal, fieldInfo.goalHeight);
    flagGlobalPositions["G2R"] = Vector3d(fieldInfo.xPosOpponentGoal, fieldInfo.yPosRightGoal, fieldInfo.goalHeight);
    return true;
    break;
  case SimSparkGameInfo::TI_RIGHT:
    flagGlobalPositions["F1L"] = Vector3d(fieldInfo.xPosOpponentGroundline, fieldInfo.yPosRightFieldBorder, 0);
    flagGlobalPositions["F2L"] = Vector3d(fieldInfo.xPosOpponentGroundline, fieldInfo.yPosLeftFieldBorder, 0);
    flagGlobalPositions["F1R"] = Vector3d(fieldInfo.xPosOwnGroundline, fieldInfo.yPosRightFieldBorder, 0);
    flagGlobalPositions["F2R"] = Vector3d(fieldInfo.xPosOwnGroundline, fieldInfo.yPosLeftFieldBorder, 0);
    flagGlobalPositions["G1L"] = Vector3d(fieldInfo.xPosOpponentGoal, fieldInfo.yPosRightGoal, fieldInfo.goalHeight);
    flagGlobalPositions["G2L"] = Vector3d(fieldInfo.xPosOpponentGoal, fieldInfo.yPosLeftGoal, fieldInfo.goalHeight);
    flagGlobalPositions["G1R"] = Vector3d(fieldInfo.xPosOwnGoal, fieldInfo.yPosRightGoal, fieldInfo.goalHeight);
    flagGlobalPositions["G2R"] = Vector3d(fieldInfo.xPosOwnGoal, fieldInfo.yPosLeftGoal, fieldInfo.goalHeight);
    return true;
    break;
  default:
    return false;
    break;
  }
  */
  return false;
}

RotationMatrix SimSparkOfficialController::calculateCameraRotFromVision(const Vector3d& pos, const map<string, Vector3d>& flagVision) const
{
  map<string, Vector3d>::const_iterator fa = flagVision.begin();
  map<string, Vector3d>::const_iterator fend = flagVision.end();
  vector<RotationMatrix> rotations;
  while (fa != fend)
  {
    map<string, Vector3d>::const_iterator fb = fa;
    ++fb;
    while (fb != fend)
    {
      map<string, Vector3d>::const_iterator fc = fb;
      ++fc;
      while (fc != fend)
      {
        RotationMatrix r = calculateCameraRotFromThreeFlag(pos, fa, fb, fc);
        rotations.push_back(r);
        ++fc;
      }
      ++fb;
    }
    ++fa;
  }

  RotationMatrix rot;
  if (!rotations.empty())
  {
    rot = RotationMatrix(Vector3d(0,0,0), Vector3d(0,0,0), Vector3d(0,0,0));
    for (vector<RotationMatrix>::const_iterator i = rotations.begin(); i != rotations.end(); ++i)
    {
      rot += *i;
    }
    rot /= rotations.size();
  }

  return rot;
}

RotationMatrix SimSparkOfficialController::calculateCameraRotFromThreeFlag(const Vector3d& pos, map<string, Vector3d>::const_iterator fa, map<string, Vector3d>::const_iterator fb, map<string, Vector3d>::const_iterator fc) const
{
  Vector3d relPosFlagA = flagGlobalPositions.find(fa->first)->second - pos;
  Vector3d relPosFlagB = flagGlobalPositions.find(fb->first)->second - pos;
  Vector3d relPosFlagC = flagGlobalPositions.find(fc->first)->second - pos;
  Matrix3x3<double> relMat(relPosFlagA, relPosFlagB, relPosFlagC);
  Vector3d localRelPosFlagA = pol2xyz(fa->second);
  Vector3d localRelPosFlagB = pol2xyz(fb->second);
  Vector3d localRelPosFlagC = pol2xyz(fc->second);
  Matrix3x3<double> localRelMat(localRelPosFlagA, localRelPosFlagB, localRelPosFlagC);
  RotationMatrix rotateMat = relMat * localRelMat.invert();
  return rotateMat;
}
