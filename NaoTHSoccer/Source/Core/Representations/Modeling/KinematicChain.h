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
    LForeArm,
    LPelvis,
    LHip,
    LThigh,
    LTibia,
    LAnkle,
    LFoot,
    RShoulder,
    RBicep,
    RElbow,
    RForeArm,
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
  void initMassesInfo();

  void initJointsInfo(naoth::JointData& jointData);

  void initLinksInfo();

  void buildLinkChains();

public:

  Kinematics::Link theLinks[numOfLinks];
  Vector3<double> CoM; // center of mass

  KinematicChain();
  virtual ~KinematicChain();
  static string getLinkName(const Kinematics::Link& node);
  static string getLinkName(const Kinematics::Link *node);
  static string getLinkName(LinkID link);

  string test(const Kinematics::Link& node) const;
  void init(naoth::JointData& jointData);

  void updateCoM();
  Vector2<double> calculateZMP() const;

  virtual void print(ostream& stream) const;

  bool is_initialized() { return initialized; }

private:
    double sumMass;
    bool initialized;
};

std::ostream & operator<<(std::ostream& os, const Kinematics::Link& node);

#endif  /* _KINEMATICCHAIN_H */
