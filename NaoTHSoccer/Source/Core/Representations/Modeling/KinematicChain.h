/**
* @file KinematicChain.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of Kinematic Chain
*/

#ifndef _KINEMATICCHAIN_H
#define _KINEMATICCHAIN_H

#include "Tools/Kinematics/Link.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/Configuration.h"


class KinematicChain : public naoth::Printable
{
public:
  enum LinkID
  {
    Torso,
    Neck,
    Head,
    LShoulder,
    LBicep,
    LElbow,
    LForeArm, // motorized or passive
    LHand, // only avaliable when in H25 with motorized LForeArm
    LPelvis,
    LHip,
    LThigh,
    LTibia,
    LAnkle,
    LFoot,
    RShoulder,
    RBicep,
    RElbow,
    RForeArm, // motorized or passive
    RHand, // only avaliable when in H25 with motorized RForeArm
    RPelvis,
    RHip,
    RThigh,
    RTibia,
    RAnkle,
    RFoot,
    Hip,
    numOfLinks
  };

private:

  void buildLinkChains();

  /**
  * Set the positions of the links relatively to the parent link.
  */
  void initLinkPositions();

  /**
  * Define the axes for the movable links (joints)
  */
  void initJointsAxes();

  /**
  * Create a link between the movable links of the kinematic chain 
  * and the corresponding values in the JointData (position, velocity, acceleration).
  */
  void linkJointData(naoth::JointData& jointData);

  /**
  * read from config
  */
  void initMassesInfo();

public:

  Kinematics::Link theLinks[numOfLinks];
  Vector3d CoM; // center of mass


  inline Kinematics::Link& getLink(LinkID id) {
    return theLinks[id];
  }

  inline const Kinematics::Link& getLink(LinkID id) const {
    return theLinks[id];
  }

  inline Kinematics::Link& getLink(int i) {
    assert(i > -1 && i < numOfLinks);
    return theLinks[i];
  }


  KinematicChain();
  virtual ~KinematicChain();
  static std::string getLinkName(const Kinematics::Link& node);
  static std::string getLinkName(const Kinematics::Link *node);
  static std::string getLinkName(LinkID link);

  std::string test(const Kinematics::Link& node) const;
  void init(naoth::JointData& jointData);

  void updateCoM();
  Vector2d calculateZMP() const;

  virtual void print(std::ostream& stream) const;

  bool is_initialized() { return initialized; }

private:
  double sumMass;
  bool initialized;
};

std::ostream & operator<<(std::ostream& os, const Kinematics::Link& node);

// declare two different representations: 
// based on the measured and requested joint positions
class KinematicChainSensor: public KinematicChain {};
class KinematicChainMotor: public KinematicChain {};

#endif  /* _KINEMATICCHAIN_H */
