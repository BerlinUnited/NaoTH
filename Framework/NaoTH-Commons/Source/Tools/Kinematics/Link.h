/**
* @file Link.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of Kinematic Link
*/

#ifndef _LINK_H
#define _LINK_H

#include "Tools/Math/Pose3D.h"

namespace Kinematics 
{

class Link
{
private:
  ///////////////////////////////////////////////////////
  // const variables in running time
  ///////////////////////////////////////////////////////
  int _id;
  Link* _sister;
  Link* _child;
  Link* _mother;
  int _jointID;
  Vector3d _a; //joint-axis vector
  RotationMatrix A;
  RotationMatrix A2;
  Vector3d _b; // relative position to mother joint

  double _mass; // mass of Link (grams)
  Vector3d d; // vector from joint to CoM

public: // public access references
  int const & id;
  int const & jointID;
  Vector3d const & a;
  Link * const & sister;
  Link * const & child;
  Link * const & mother;
  double const & mass;
  Vector3d const & b;

public:
  ///////////////////////////////////////////////////////////////////////////////////
  // state variable : they are changed in running time
  ///////////////////////////////////////////////////////////////////////////////////
  Pose3D M; // transform matrix;
  Vector3d& p; // position in world
  Vector3d v; // linear velocity
  Vector3d dv; // linear acceleration
  RotationMatrix& R; // rotation in world
  Vector3d w; // rotation velocity
  Vector3d dw; // rotation acceleration
  Vector3d c; // position of Center of Mass
  double *q; // joint-position
  double *dq; // joint velocity
  double *ddq; // joint acceleration

  Link();
  ~Link();

  void normalizeJointAngle();

  Link& connect(Link& c);

  //
  void updateFromMother();
  
  //
  void updateAllFromMother();


  void updateMother();

  void updateCoM();


  //
  void setId(int i){ _id = i; }

  void setMass(double m, const Vector3<double>& cog);

  void setJoint(const Vector3<double>& axis, int JointId);

  // set the relative position to mother joint
  void setLink(double x, double y, double z);
};

}  // namespace Kinematics

#endif  /* _LINK_H */

