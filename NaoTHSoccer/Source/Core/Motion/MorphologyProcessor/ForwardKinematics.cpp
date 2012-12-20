/**
* @file ForwardKinematics.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of the class ForwardKinematics
*/
#include "ForwardKinematics.h"

#include <Representations/Infrastructure/FSRData.h>

namespace Kinematics 
{

using namespace naoth;
  
void ForwardKinematics::forwardAllKinematics(Link* theLink)
{
  if(theLink==NULL)
  {
    return;
  }
  theLink->updateAllFromMother();
  
  forwardAllKinematics(theLink->sister);
  forwardAllKinematics(theLink->child);
}//end forwardAllKinematics


void ForwardKinematics::forwardAllKinematicsExcept(Link* theLink, Link* ept)
{
  if(theLink==NULL)
  {
    return;
  }

  if (theLink != ept)
  {
    theLink->updateAllFromMother();
    forwardAllKinematicsExcept(theLink->child, ept);
  }

  forwardAllKinematicsExcept(theLink->sister, ept);
}//end forwardAllKinematicsExcept


// TODO: move it to KinematicChainNao
void ForwardKinematics::calcChestRotation(Link* theLinks, const Vector2d& rotation)
{
  Link* chest = (theLinks + KinematicChain::Torso);
  // Rotation from Inertial sensor data
  chest->R = RotationMatrix::getRotationX(rotation.x);
  chest->R.rotateY(rotation.y);
}//end calcChestRotation


void ForwardKinematics::calcChestAll(
        Link* theLinks,
        //const AccelerometerData& theAccelerometerData,
        //const InertialPercept& theInertialPercept,
        const Vector2<double>& theBodyRotation,
        const Vector3<double>& theBodyAcceleration,
        const double deltaTime)
{
  Link* chest = (theLinks + KinematicChain::Torso);

  // position of the chest is the origin of the whole kinematic chain
  Vector3d O(0, 0, 0);
  chest->p = O;

  // rotation from Inertial sensor data
  chest->R = RotationMatrix::getRotationX(theBodyRotation.x);
  chest->R.rotateY(theBodyRotation.y);

  // acceleration and velocity
  chest->dv = chest->R * ( theBodyAcceleration * 1000 );
  chest->v += chest->dv * deltaTime;
  chest->w = O;
  chest->dw = O;
}//end calcChestAll


double ForwardKinematics::calcChestHeight(const Vector3<double>* fsrPos)
{
  // assume the lowest FSR's z = 0
  double offsetZ = 0;
  for( int i=0; i<FSRData::numOfFSR; i++ )
  {
    if ( offsetZ > fsrPos[i].z )
      offsetZ = fsrPos[i].z;
  }
  return -offsetZ;
}//end calcChestHeight


void ForwardKinematics::calculateKinematicChainAll(
    //const AccelerometerData& theAccelerometerData,
    //const InertialPercept& theInertialPercept,
    const Vector2<double>& theBodyRotation,
    const Vector3<double>& theBodyAcceleration,
    KinematicChain& theKinematicChain,
    Vector3<double>* theFSRPos,
    const double deltaTime)
{
  Link* theLinks = theKinematicChain.theLinks;
  
  // set the values for the chest link
  calcChestAll(theLinks, theBodyRotation, theBodyAcceleration, deltaTime);

  // propagate though the entire kinematic chain beginning from the chest
  forwardAllKinematics(theLinks + KinematicChain::Torso);
  
  // TODO: move the estimation of the chest height to somwhere else
  calcFSRPos(theLinks, theFSRPos);
  double theChestHeight = calcChestHeight(theFSRPos);

  adjustKinematicChain(theLinks, theFSRPos, theChestHeight);
  calcCoMs(theLinks);
  
  theKinematicChain.updateCoM();
}//end update


void ForwardKinematics::adjustKinematicChain(Link* l, Vector3<double> *fsrPos, double chestHeight)
{
  double hipX = l[KinematicChain::Hip].p.x;
  double hipY = l[KinematicChain::Hip].p.y;

  for(int i=0; i<KinematicChain::numOfLinks; i++ )
  {
    l[i].p.x -= hipX;
    l[i].p.y -= hipY;
    l[i].p.z += chestHeight;
  }

  for(int i=0; i<FSRData::numOfFSR; i++)
  {
    fsrPos[i].x -= hipX;
    fsrPos[i].y -= hipY;
    fsrPos[i].z += chestHeight;
  }
}//end adjustKinematicChain


void ForwardKinematics::calcLinkUp(Link* l)
{
  if (NULL == l) return;
  l->updateCoM();
  l->updateMother();
  calcLinkUp(l->mother);
}//end calcLinkUp


void ForwardKinematics::calcCoMs(Link* l)
{
  for(int i=0; i<KinematicChain::numOfLinks; i++)
  {
    l[i].updateCoM();
  }//end for
}//end calcCoMs


void ForwardKinematics::calcFSRPos(const Link* theLink, Vector3<double>* theFSRPos)
{
  const Link* lFoot = &theLink[KinematicChain::LFoot];
  const Link* rFoot = &theLink[KinematicChain::RFoot];

  // left foot
  for (int i = 0; i < FSRData::RFsrFL; i++)
  {
    theFSRPos[i] = lFoot->M * FSRData::offset[i];
  }//end for

  // right foot
  for (int i = FSRData::RFsrFL; i < FSRData::numOfFSR; i++)
  {
    theFSRPos[i] = rFoot->M * FSRData::offset[i];
  }//end for
}//end calcFSRPos


// TODO: does this referencing make sense (Link* &l)?
Link* ForwardKinematics::updateToRoot(Link* &l)
{
  if ( l->mother == NULL )
    return l;

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

  calcCoMs(root);
}//end updateKinematicChainFrom


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
    
  if(abs(localFootLeft.translation.z - localFootRight.translation.z) < 3.f/* magic number */)
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

} // namespace Kinematics
