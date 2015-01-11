/**
* @file ForwardKinematics.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of the class ForwardKinematics
*/
#include "ForwardKinematics.h"

#include <Tools/NaoInfo.h>

namespace Kinematics 
{

using namespace naoth;
  
void ForwardKinematics::forwardAllKinematics(Link* theLink)
{
  if(theLink==NULL) {
    return;
  }
  theLink->updateAllFromMother();
  
  forwardAllKinematics(theLink->sister);
  forwardAllKinematics(theLink->child);
}

void ForwardKinematics::forwardAllKinematics(Link& theLink)
{
  forwardAllKinematics(&theLink);
}

void ForwardKinematics::forwardAllKinematics(KinematicChain& theKinematicChain, KinematicChain::LinkID start)
{
  forwardAllKinematics(theKinematicChain.getLink(start));
}


void ForwardKinematics::forwardAllKinematicsExcept(Link* theLink, Link* ept)
{
  if(theLink==NULL) {
    return;
  }

  if (theLink != ept)
  {
    theLink->updateAllFromMother();
    forwardAllKinematicsExcept(theLink->child, ept);
  }

  forwardAllKinematicsExcept(theLink->sister, ept);
}//end forwardAllKinematicsExcept

void ForwardKinematics::setRotation(Link& link, const Vector2d& rotation)
{
  link.R = RotationMatrix::getRotationX(rotation.x).rotateY(rotation.y);
}


void ForwardKinematics::updateChest(
        KinematicChain& kinematicChain,
        const Vector2d& theBodyRotation,
        const Vector3d& theBodyAcceleration,
        const double deltaTime)
{
  Link& chest = kinematicChain.getLink(KinematicChain::Torso);

  // position of the chest is the origin of the whole kinematic chain
  const Vector3d O(0, 0, 0);
  chest.p = O;

  // rotation from inertial sensor
  chest.R = RotationMatrix::getRotationX(theBodyRotation.x);
  chest.R.rotateY(theBodyRotation.y);

  // acceleration and velocity
  chest.dv = chest.R * ( theBodyAcceleration * 1000 );
  chest.v += chest.dv * deltaTime;
  chest.w = O;
  chest.dw = O;
}//end updateChest


void ForwardKinematics::updateKinematicChainAll(
    const Vector2d& theBodyRotation,
    const Vector3d& theBodyAcceleration,
    const double deltaTime,
    KinematicChain& theKinematicChain,
    Vector3d* theFSRPos)
{
  // set the values for the chest link
  updateChest(
    theKinematicChain, 
    theBodyRotation, 
    theBodyAcceleration, 
    deltaTime);
  
  // propagate though the entire kinematic chain beginning from the chest
  forwardAllKinematics(theKinematicChain, KinematicChain::Torso);

  // transfor the FSR to the global coordinates of the kinematic chain
  // i.e., to the chest coordinates
  updateFSRPos(theKinematicChain, theFSRPos);
  
  // assume the foot is on the ground and use the z-distance to the
  // lowest FSR as an estimation for the actual height of the robots chest
  double chestHeight = theFSRPos[getLowestFSR(theFSRPos)].z;

  // the origin of the kinematic chain is the projection of the robots hip on the ground
  const Vector3d& hipPosition = theKinematicChain.getLink(KinematicChain::Hip).p;
  Vector3d origin(-hipPosition.x, -hipPosition.y, -chestHeight);

  // translate the whole kinematic chain to the new origin
  translate(theKinematicChain, origin);

  //
  updateCoM(theKinematicChain);

  // recalculate the new positions for the FSR
  updateFSRPos(theKinematicChain, theFSRPos);
}//end updateKinematicChainAll


void ForwardKinematics::translate(KinematicChain& theKinematicChain, const Vector3d& t)
{
  for(int i=0; i<KinematicChain::numOfLinks; i++ ) {
    theKinematicChain.getLink(i).p += t;
  }
}


void ForwardKinematics::calcLinkUp(Link* l)
{
  if (NULL == l) return;
  l->updateCoM();
  l->updateMother();
  calcLinkUp(l->mother);
}//end calcLinkUp


void ForwardKinematics::updateCoM(KinematicChain& theKinematicChain)
{
  for(int i=0; i<KinematicChain::numOfLinks; i++) {
    theKinematicChain.getLink(i).updateCoM();
  }
  theKinematicChain.updateCoM();
}//end updateCoM

void ForwardKinematics::updateCoM(Link* l)
{
  for(int i=0; i<KinematicChain::numOfLinks; i++) {
    l[i].updateCoM();
  }
}//end updateCoM


// TODO: does this referencing make sense (Link* &l)?
Link* ForwardKinematics::updateToRoot(Link* l)
{
  if ( l->mother == NULL ) {
    return l;
  }

  while(true)
  {
    l->updateMother();
    if ( l->mother->mother == NULL)
    {
      return l->mother;
    }
    l = l->mother;
  }

  ASSERT(false);
  return l;
}//end updateToRoot

void ForwardKinematics::updateKinematicChainFrom(Link* l)
{
  Link* root = NULL;
  if (l->mother == NULL)
  {
    root = l;
    forwardAllKinematics(root);
  } else
  {
    root = updateToRoot(l);
    forwardAllKinematicsExcept(root, l);
  }

  updateCoM(root);
}//end updateKinematicChainFrom

void ForwardKinematics::updateKinematicChainFrom(KinematicChain& theKinematicChain, KinematicChain::LinkID start)
{
  updateKinematicChainFrom(&theKinematicChain.getLink(start));
}


RotationMatrix ForwardKinematics::calcChestFeetRotation(const KinematicChain& theKinematicChain)
{
  RotationMatrix calculatedRotation;

  // calculate rotation based on foot - torso transformation
  const Pose3D& footLeft = theKinematicChain.theLinks[KinematicChain::LFoot].M;
  const Pose3D& footRight = theKinematicChain.theLinks[KinematicChain::RFoot].M;
  const Pose3D& body = theKinematicChain.theLinks[KinematicChain::Torso].M;

  // local in chest
  Pose3D localFootLeft(body.local(footLeft));
  Pose3D localFootRight(body.local(footRight));
    
  if(fabs(localFootLeft.translation.z - localFootRight.translation.z) < 3.f/* magic number */)
  {
    // use average of the calculated rotation of each leg
    double meanX = (localFootLeft.rotation.getXAngle() + localFootRight.rotation.getXAngle())*0.5;
    double meanY = (localFootLeft.rotation.getYAngle() + localFootRight.rotation.getYAngle())*0.5;

    //calculatedRotation.fromKardanRPY(0.0, meanY, meanX);
    calculatedRotation.rotateX(meanX).rotateY(meanY);
  }
  else if(localFootLeft.translation.z > localFootRight.translation.z)
  {
    // use left foot
    calculatedRotation = localFootLeft.rotation;
  }
  else
  {
    // use right foot
    calculatedRotation = localFootRight.rotation;
  }

  return calculatedRotation.invert();
}//end calcChestFeetRotation

void ForwardKinematics::updateFSRPos(const KinematicChain& kinematicChain, Vector3d* theFSRPos)
{
  const Pose3D& lFootPose = kinematicChain.getLink(KinematicChain::LFoot).M;
  const Pose3D& rFootPose = kinematicChain.getLink(KinematicChain::RFoot).M;

  // left foot
  for (int i = 0; i < FSRData::RFsrFL; i++) {
    theFSRPos[i] = lFootPose * NaoInfo::FSRPositions[i];
  }

  // right foot
  for (int i = FSRData::RFsrFL; i < FSRData::numOfFSR; i++) {
    theFSRPos[i] = rFootPose * NaoInfo::FSRPositions[i];
  }
}


int ForwardKinematics::getLowestFSR(const Vector3d* theFSRPos)
{
  int idx = 0;
  double offsetZ = theFSRPos[0].z;
  
  for(int i=1; i<FSRData::numOfFSR; i++) {
    if ( offsetZ > theFSRPos[i].z ) {
      offsetZ = theFSRPos[i].z;
      idx = i;
    }
  }
  return idx;
}

} // namespace Kinematics
