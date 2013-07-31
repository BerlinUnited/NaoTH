/**
* @file KinematicChain.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of Kinematic Chain
*/

#include "KinematicChain.h"
#include "Tools/NaoInfo.h"
#include <PlatformInterface/Platform.h>

using namespace naoth;
using namespace std;

KinematicChain::KinematicChain()
  : initialized(false)
{
  for (int i = 0; i < numOfLinks; i++)
  {
    theLinks[i].setId(i);
  }
}

KinematicChain::~KinematicChain()
{
}

#define FROM_ID_TO_NAME(id) case id: return #id
string KinematicChain::getLinkName(LinkID j) 
{
  switch (j)
  {
    FROM_ID_TO_NAME(Torso);
    FROM_ID_TO_NAME(Neck);
    FROM_ID_TO_NAME(Head);
    FROM_ID_TO_NAME(LShoulder);
    FROM_ID_TO_NAME(LBicep);
    FROM_ID_TO_NAME(LElbow);
    FROM_ID_TO_NAME(LForeArm);
    FROM_ID_TO_NAME(LPelvis);
    FROM_ID_TO_NAME(LHip);
    FROM_ID_TO_NAME(LThigh);
    FROM_ID_TO_NAME(LTibia);
    FROM_ID_TO_NAME(LAnkle);
    FROM_ID_TO_NAME(LFoot);
    FROM_ID_TO_NAME(RShoulder);
    FROM_ID_TO_NAME(RBicep);
    FROM_ID_TO_NAME(RElbow);
    FROM_ID_TO_NAME(RForeArm);
    FROM_ID_TO_NAME(RPelvis);
    FROM_ID_TO_NAME(RHip);
    FROM_ID_TO_NAME(RThigh);
    FROM_ID_TO_NAME(RTibia);
    FROM_ID_TO_NAME(RAnkle);
    FROM_ID_TO_NAME(RFoot);
    FROM_ID_TO_NAME(Hip);
    default:
        return "Unknown Link";
  }
}

string KinematicChain::getLinkName(const Kinematics::Link& link)
{
  return getLinkName((LinkID) link.id);
}

string KinematicChain::getLinkName(const Kinematics::Link *link)
{
  return getLinkName((LinkID) link->id);
}

void KinematicChain::buildLinkChains()
{
  // Torso -- Neck -- Head
  theLinks[Torso].connect(theLinks[Neck])
                 .connect(theLinks[Head]);

  // Torso -- Shoulder -- Bicep -- Elbow -- ForeArm
  // Left
  theLinks[Torso].connect(theLinks[LShoulder])
                 .connect(theLinks[LBicep])
                 .connect(theLinks[LElbow])
                 .connect(theLinks[LForeArm]);
  // Right
  theLinks[Torso].connect(theLinks[RShoulder])
                 .connect(theLinks[RBicep])
                 .connect(theLinks[RElbow])
                 .connect(theLinks[RForeArm]);

  // Torso -- Pelvis -- Hip -- Thigh -- Tibia -- Ankle -- Foot
  // Left
  theLinks[Torso].connect(theLinks[LPelvis])
                 .connect(theLinks[LHip])
                 .connect(theLinks[LThigh])
                 .connect(theLinks[LTibia])
                 .connect(theLinks[LAnkle])
                 .connect(theLinks[LFoot]);
  // Right
  theLinks[Torso].connect(theLinks[RPelvis])
                 .connect(theLinks[RHip])
                 .connect(theLinks[RThigh])
                 .connect(theLinks[RTibia])
                 .connect(theLinks[RAnkle])
                 .connect(theLinks[RFoot]);

  // Toros -- Hip
  theLinks[Torso].connect(theLinks[Hip]);

//  for (int i = 0; i < numOfLinks; i++)
//  {
//    cout << "Test[" << getLinkName(theLinks[i]) << "] " << test(theLinks[i]) << '\n';
//  }
}//end buildLinkChains

void KinematicChain::initMassesInfo()
{
  const Configuration& cfg = Platform::getInstance().theConfiguration;
  string gn = "mass";
  sumMass = 0;

  for( int i = 0; i<numOfLinks; i++)
  {
    string name = getLinkName((LinkID)i);
    string keyMass = name+"Mass";
    string keyX = name+"CoG.x";
    string keyY = name+"CoG.y";
    string keyZ = name+"CoG.z";
    
    double m = 0;
    Vector3d cog;
    
    if ( cfg.hasKey(gn, keyMass) ) {
      m = cfg.getDouble(gn, keyMass);
    } else {
      THROW("KinematicChain can not get mass of "+name);
    }
    
    if ( cfg.hasKey(gn, keyX) ) {
      cog.x = cfg.getDouble(gn, keyX);
    } else {
      THROW("KinematicChain can not get cog.x of "+name);
    }
    
    if ( cfg.hasKey(gn, keyY) ) {
      cog.y = cfg.getDouble(gn, keyY);
    } else {
      THROW("KinematicChain can not get cog.y of "+name);
    }
    
    if ( cfg.hasKey(gn, keyZ) ) {
      cog.z = cfg.getDouble(gn, keyZ);
    } else {
      THROW("KinematicChain can not get cog.z of "+name);
    }

    theLinks[i].setMass(m, cog);
    sumMass += theLinks[i].mass;
  }

  // the value of total mass in SDK is NOT correct
//  ASSERT(sumMass == NaoInfo::TotalMass);
}//end initMassesInfo


void KinematicChain::initJointsAxes()
{
  // precalculate the axes for the joints
  const Vector3d x(1,0,0);
  const Vector3d y(0,1,0);
  const Vector3d z(0,0,1);
  // those are 45° axes for the LHipYawPitch/RHipYawPitch
  const Vector3d lyz(0, sqrt(2.0)/2.0, -sqrt(2.0)/2.0);
  const Vector3d ryz(0, sqrt(2.0)/2.0,  sqrt(2.0)/2.0);

  // set the axes for the joints
  // head
  theLinks[Neck     ].setJoint(z,   JointData::HeadYaw);
  theLinks[Head     ].setJoint(y,   JointData::HeadPitch);
  // left arm
  theLinks[LShoulder].setJoint(y,   JointData::LShoulderPitch);
  theLinks[LBicep   ].setJoint(z,   JointData::LShoulderRoll);
  theLinks[LElbow   ].setJoint(x,   JointData::LElbowYaw);
  theLinks[LForeArm ].setJoint(z,   JointData::LElbowRoll);
  // left leg
  theLinks[LPelvis  ].setJoint(lyz, JointData::LHipYawPitch);
  theLinks[LHip     ].setJoint(x,   JointData::LHipRoll);
  theLinks[LThigh   ].setJoint(y,   JointData::LHipPitch);
  theLinks[LTibia   ].setJoint(y,   JointData::LKneePitch);
  theLinks[LAnkle   ].setJoint(y,   JointData::LAnklePitch);
  theLinks[LFoot    ].setJoint(x,   JointData::LAnkleRoll);
  // right arm
  theLinks[RShoulder].setJoint(y,   JointData::RShoulderPitch);
  theLinks[RBicep   ].setJoint(z,   JointData::RShoulderRoll);
  theLinks[RElbow   ].setJoint(x,   JointData::RElbowYaw);
  theLinks[RForeArm ].setJoint(z,   JointData::RElbowRoll);
  // right leg
  theLinks[RPelvis  ].setJoint(ryz, JointData::RHipYawPitch);
  theLinks[RHip     ].setJoint(x,   JointData::RHipRoll);
  theLinks[RThigh   ].setJoint(y,   JointData::RHipPitch);
  theLinks[RTibia   ].setJoint(y,   JointData::RKneePitch);
  theLinks[RAnkle   ].setJoint(y,   JointData::RAnklePitch);
  theLinks[RFoot    ].setJoint(x,   JointData::RAnkleRoll);
}//end initJointsAxes

void KinematicChain::initJointsInfo(JointData& jointData)
{
  for (int i = 0; i < numOfLinks; i++)
  {
    // link the joint data 
    // ACHTUNG: the invalid jointID of a link is set to -1
    //          so, don't use a simple cast to JointData::JointID (!)
    JointData::JointID id = JointData::getJointID(theLinks[i].jointID);
    if (id != JointData::numOfJoint)
    {
      theLinks[i].q = &(jointData.position[id]);
      theLinks[i].dq = &(jointData.dp[id]);
      theLinks[i].ddq = &(jointData.ddp[id]);
    }
  }
}//end initJointsInfo

void KinematicChain::initLinksInfo()
{  
  theLinks[Neck].setLink(0, 0, NaoInfo::NeckOffsetZ);
  theLinks[Head].setLink(0, 0, 0);

  theLinks[LShoulder].setLink(0, NaoInfo::ShoulderOffsetY, NaoInfo::ShoulderOffsetZ);
  theLinks[LBicep].setLink(0, 0, 0);
  theLinks[LElbow].setLink(NaoInfo::UpperArmLength,NaoInfo::ElbowOffsetY,0);
  theLinks[LForeArm].setLink(0, 0, 0);

  theLinks[LPelvis].setLink(0, NaoInfo::HipOffsetY, -NaoInfo::HipOffsetZ);
  theLinks[LHip].setLink(0, 0, 0);
  theLinks[LThigh].setLink(0, 0, 0);

  theLinks[LTibia].setLink(0,0,-NaoInfo::ThighLength);

  theLinks[LAnkle].setLink(0,0,-NaoInfo::TibiaLength);
  theLinks[LFoot].setLink(0, 0, 0);

  theLinks[RShoulder].setLink(0, -NaoInfo::ShoulderOffsetY, NaoInfo::ShoulderOffsetZ);
  theLinks[RBicep].setLink(0, 0, 0);
  theLinks[RElbow].setLink(NaoInfo::UpperArmLength,-NaoInfo::ElbowOffsetY,0);
  theLinks[RForeArm].setLink(0, 0, 0);

  theLinks[RPelvis].setLink(0,-NaoInfo::HipOffsetY,-NaoInfo::HipOffsetZ);
  theLinks[RHip].setLink(0, 0, 0);
  theLinks[RThigh].setLink(0, 0, 0);

  theLinks[RTibia].setLink(0,0,-NaoInfo::ThighLength);

  theLinks[RAnkle].setLink(0,0,-NaoInfo::TibiaLength);
  theLinks[RFoot].setLink(0, 0, 0);

  theLinks[Hip].setLink(0, 0, -NaoInfo::HipOffsetZ);
}//end initLinksInfo

void KinematicChain::init(JointData& jointData)
{
  initJointsAxes();
  initJointsInfo(jointData);
  initMassesInfo();
  initLinksInfo();
  buildLinkChains();

  initialized = true;
}

string KinematicChain::test(const Kinematics::Link& node) const
{
  if (NULL!=node.mother)
  {
    if (NULL==node.mother->child)
    {
      return getLinkName(node) + "'s mother is " + getLinkName(node.mother) + ", but " + getLinkName(node.mother) + " has no child";
    }
    if (node.mother->child!=&node)
    {
      Kinematics::Link* s = node.mother->child;
      while(NULL != s)
      {
        s = s->sister;
        if (s==&node)
        {
          break;
        }
      }
      if(s!=&node)
      {
        return getLinkName(node) + "'s mother is " + getLinkName(node.mother) + ", but " + getLinkName(node.mother) + "'s child isn't him";
      }
    }
  }

  if (NULL!=node.sister)
  {
    if(NULL==node.mother)
    {
      return getLinkName(node) + " and " + getLinkName(node.sister) + " are sisters, but " + getLinkName(node) + " has no mother";
    }
    if(NULL==node.sister->mother)
    {
      return getLinkName(node) + " and "+ getLinkName(node.sister) + " are sisters, but " + getLinkName(node.sister) + " has no mother";
    }
    if(node.mother != node.sister->mother )
    {
      return getLinkName(node)+" and " + getLinkName(node.sister) + " are sisters with different mothers";
    }
  }

  if (NULL!=node.child)
  {
    if(NULL == node.child->mother)
    {
      return getLinkName(node) + "'s child is " + getLinkName(node.child) + ", but " + getLinkName(node.child) + " has no mother";
    }
    if(node.child->mother != &node )
    {
      return getLinkName(node) + "'s child is " + getLinkName(node.child) + ", but " + getLinkName(node.child) + "'s mother isn't her";
    }
  }
  return "ok";
}//end test

void KinematicChain::updateCoM()
{
  CoM = Vector3d::zero;
  for(int i=0; i<numOfLinks; i++)
  {
    CoM += (theLinks[i].c * theLinks[i].mass);
  }
  CoM/=sumMass;
}//end updateCoM

Vector2d KinematicChain::calculateZMP() const
{
  // Px = S((z''+g)x-(z-Pz)x'') / S(z''+g)
  const static double g = -9810;
  const static double pz = 0;
  Vector2d zmp;
  double as = 0;
  for(int i=0; i<numOfLinks; i++)
  {
    double zg = theLinks[i].dv.z + g;
    double m = theLinks[i].mass;
    as += (zg*m);
    double zp = theLinks[i].c.z-pz;
    zmp.x += (( zg*theLinks[i].c.x - zp*theLinks[i].dv.x )*m);
    zmp.y += (( zg*theLinks[i].c.y - zp*theLinks[i].dv.y )*m);
  }
  zmp/=as;
  return zmp;
}//end calculateZMP

void KinematicChain::print(ostream& stream) const
{
  for(int i=0;i<numOfLinks;i++)
  {
    stream << getLinkName((LinkID)i) << ": "<< theLinks[i].p <<"\n";
  }
  stream << "CoM: " << CoM << "\n";
}//end print

std::ostream & operator<<(std::ostream& os, const Kinematics::Link& node)
{
  os << KinematicChain::getLinkName(node);

  if (NULL != node.sister)
  {
    if (NULL != node.child)
    {
      if (NULL == node.child->sister)
        os << "---";
      else
        os << "-+-";

      os << *(node.child);
    }
    os << '\n';
    int count = 0;
    Kinematics::Link* m = node.mother;
    while (NULL != m)
    {
      count += static_cast<int>(KinematicChain::getLinkName(m).length());
      m = m->mother;
    }
    for (int i = 0; i < count; i++)
    {
      os << " ";
    }
    os << " +-" << *(node.sister);
  } else
  {
    if (NULL != node.child)
    {
      if (NULL == node.child->sister)
        os << "---";
      else
        os << "-+-";
      os << *(node.child);
    }
  }
  return os;
}//end operator<<
