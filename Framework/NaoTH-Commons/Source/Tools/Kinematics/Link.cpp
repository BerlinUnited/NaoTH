/**
 * @file Link.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Implementation of Kinematic Link
 */

#include "Link.h"
#include "Tools/Debug/NaoTHAssert.h"

namespace Kinematics {

Link::Link() 
  :
  _id(-1),
  _sister(NULL),
  _child(NULL),
  _mother(NULL),
  _jointID(-1),

  _mass(0.0),

  // public access references
  id(_id),
  jointID(_jointID),
  a(_a),
  sister(_sister),
  child(_child),
  mother(_mother),
  mass(_mass),
  b(_b),


  p(M.translation),
  R(M.rotation),
  
  q(NULL),
  dq(NULL),
  ddq(NULL)
{
}

Link::~Link()
{
}

void Link::normalizeJointAngle()
{
  (*q) = Math::normalizeAngle(*q);
}

Link& Link::connect(Link& c)
{
  ASSERT(c._mother == NULL);
  c._mother = this;

  if (_child == NULL)
  {
    _child = &c;
  } else
  {
    Link* s = _child;
    while (s->_sister != NULL)
    {
      s = s->_sister;
    }
    s->_sister = &c;
  }

  return c;
}

void Link::updateFromMother()
{
  if (NULL != mother)
  {
    p = mother->R * b + mother->p;
    if (NULL != q)
    {
      R = mother->R * Rodrigues(A, A2, *q);
    } else
    {
      R = mother->R;
    }
  }
  //        else{
  //            p = Vector3<double>(0,0,0);
  //            R = RotationMatrix(Vector3<double>(1,0,0),Vector3<double>(0,1,0),Vector3<double>(0,0,1));
  //        }
}

void Link::updateAllFromMother()
{
  if (NULL != mother)
  {
    p = mother->R * b + mother->p;
    R = mother->R;
//    w = mother->w;
//    v = mother->v;
//    dw = mother->dw;
//    dv = mother->dv;

    // rotation
    if (NULL != q)
    {
//      Vector3<double> sw = R * a;
//      Vector3<double> sv = p ^ sw;
//      Vector3<double> dsv = (w ^ sv) + (v ^ sw);
//      Vector3<double> dsw = w ^ sw;

      R *= Rodrigues(A, A2, *q);

      // senity check if R is still a kind of a rotation matrix...
      assert(fabs(1.0-fabs(R.det())) < 0.001);

      // velocity
//      w += (sw * (*dq));
//      v += (sv * (*dq));

      // acceleration
//      dw += dsw * (*dq) + sw * (*ddq);
//      dv += dsv * (*dq) + sv * (*ddq);
    }
  }
}

void Link::updateMother()
{
  if (NULL == mother) return;

  mother->R = R * Rodrigues(A, A2, -(*(q)));
  mother->p = p - mother->R * b;
}

void Link::updateCoM()
{
  c = R * d + p;
}

void Link::setMass(double m, const Vector3<double>& cog)
{
  _mass = m;
  d = cog;
}

void Link::setJoint(const Vector3<double>& axis, int JointId)
{
  _a = axis;
  _jointID = JointId;

  A = Matrix3x3<double>(Vector3d(0, a.z, -a.y), Vector3d(-a.z, 0, a.x), Vector3d(a.y, -a.x, 0));
  A2 = A * A;
}

void Link::setLink(double x, double y, double z)
{
  _b.x = x;
  _b.y = y;
  _b.z = z;
}

} // namespace Kinematics
