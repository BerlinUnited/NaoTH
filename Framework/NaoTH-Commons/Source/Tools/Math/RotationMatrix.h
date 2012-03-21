/** 
 * \file RotationMatrix.h
 * Template classes for rotation matrices.
 *
 * \author Martin Kallnik, martin.kallnik@gmx.de
 * \author Thomas Kindler, thomas.kindler@gmx.de 
 * \author Max Risler
 */

#ifndef _RotationMatrix_h_
#define _RotationMatrix_h_

#include "Matrix3x3.h"

/**
 * Representation for 3x3 RotationMatrices
 */
template <typename DATATYPE>
class RotationMatrixT : public Matrix3x3<DATATYPE> {
public:
  /** 
   * Default constructor. 
   */
  RotationMatrixT() {}

  /**
   * Constructor.
   *
   * \param  c0  the first column of the matrix.
   * \param  c1  the second column of the matrix.
   * \param  c2  the third column of the matrix.
   */
  RotationMatrixT(
    const Vector3<DATATYPE>& c0,
    const Vector3<DATATYPE>& c1,
    const Vector3<DATATYPE>& c2
  )
  : Matrix3x3<DATATYPE>(c0,c1,c2)
  {
  }

  /**
   * Copy constructor.
   *
   * \param  other  The other matrix that is copied to this one
   */
  RotationMatrixT(const Matrix3x3<DATATYPE>& other):
  Matrix3x3<DATATYPE>(other)
  {
  }

  /**
   * copied from BH-2011
   *
   */
  RotationMatrixT(const Vector3<DATATYPE>& a)
  {
    const DATATYPE angle = a.abs();
    Vector3<DATATYPE> axis = a;

    if(angle != DATATYPE(0))
      axis /= angle; // normalize a, rotation is only possible with unit vectors

    const DATATYPE& x = axis.x, &y = axis.y, &z = axis.z;
    //compute sine and cosine of angle because it is needed quite often for complete matrix
    const DATATYPE si = sin(angle), co = cos(angle);
    //compute all components needed more than once for complete matrix
    const DATATYPE v = 1 - co;
    const DATATYPE xyv = x * y * v;
    const DATATYPE xzv = x * z * v;
    const DATATYPE yzv = y * z * v;
    const DATATYPE xs = x * si;
    const DATATYPE ys = y * si;
    const DATATYPE zs = z * si;
    //compute matrix
    this->c[0].x = x * x * v + co;
    this->c[1].x = xyv - zs;
    this->c[2].x = xzv + ys;
    this->c[0].y = xyv + zs;
    this->c[1].y = y * y * v + co;
    this->c[2].y = yzv - xs;
    this->c[0].z = xzv - ys;
    this->c[1].z = yzv + xs;
    this->c[2].z = z * z * v + co;
  }

  /**
   * Copy constructor.
   *
   * \param  other  The other matrix that is copied to this one
   */
  RotationMatrixT(const DATATYPE yaw, const DATATYPE pitch, const DATATYPE roll)
  {
    fromKardanRPY(yaw, pitch, roll);
  }
  
  /**
   * RotationMatrix from RPY-angles.
   *   Roll  rotates along z axis,
   *   Pitch rotates along y axis,  
   *   Yaw   rotates along x axis
   *
   *   R(roll,pitch,yaw) = R(z,roll)*R(y,pitch)*R(x,yaw)
   *
   * \see  "Robotik 1 Ausgabe Sommersemester 2001" by Prof. Dr. O. von Stryk
   * \attention  RPY-angles are not clearly defined!
   */
  RotationMatrixT& fromKardanRPY(const DATATYPE yaw, const DATATYPE pitch, const DATATYPE roll)
  {
        DATATYPE cy = cos(yaw);
        DATATYPE sy = sin(yaw);
        DATATYPE cp = cos(pitch);
        DATATYPE sp = sin(pitch);
        DATATYPE cr = cos(roll);
        DATATYPE sr = sin(roll);

        this->c[0].x = cr*cp;
        this->c[0].y = -sr * cy + cr * sp*sy;
        this->c[0].z = sr * sy + cr * sp*cy;
        this->c[1].x = sr*cp;
        this->c[1].y = cr * cy + sr * sp*sy;
        this->c[1].z = -cr * sy + sr * sp*cy;
        this->c[2].x = -sp;
        this->c[2].y = cp*sy;
        this->c[2].z = cp*cy;

        return *this;
  }

  /**
   * Invert the matrix.
   *
   * \note: Inverted rotation matrix is transposed matrix.
   */
  RotationMatrixT invert() const
  {
    return this->transpose();
  }

  /** 
   * Rotation around the x-axis.
   *
   * \param   angle  The angle this pose will be rotated by
   * \return  A reference to this object after the calculation.
   */
  RotationMatrixT& rotateX(const DATATYPE angle)
  {
        *this *= getRotationX(angle);
        return *this;
  }

  /** 
   * Rotation around the y-axis.
   *
   * \param   angle  The angle this pose will be rotated by
   * \return  A reference to this object after the calculation.
   */
  RotationMatrixT& rotateY(const DATATYPE angle)
  {
        *this *= getRotationY(angle);
        return *this;
  }

  /** 
   * Rotation around the z-axis.
   *
   * \param   angle  The angle this pose will be rotated by
   * \return  A reference to this object after the calculation.
   */
  RotationMatrixT& rotateZ(const DATATYPE angle)
  {
        *this *= getRotationZ(angle);
        return *this;
  }

  /**
   * Get the x-angle of a RotationMatrix.
   *
   * \return  The angle around the x-axis between the original
   *          and the rotated z-axis projected on the y-z-plane
   */
  DATATYPE getXAngle() const
  {
        DATATYPE h = sqrt(this->c[2].y * this->c[2].y + this->c[2].z * this->c[2].z);
        return h ? acos(this->c[2].z / h) * (this->c[2].y > 0 ? -1 : 1) : 0;
  }

  /**
   * Get the y-angle of a RotationMatrix.
   *
   * \return  The angle around the y-axis between the original
   *          and the rotated x-axis projected on the x-z-plane
   */
  DATATYPE getYAngle() const
  {
        DATATYPE h = sqrt(this->c[0].x * this->c[0].x + this->c[0].z * this->c[0].z);
        return h ? acos(this->c[0].x / h) * (this->c[0].z > 0 ? -1 : 1) : 0;
  }

  /**
   * Get the z-angle of a RotationMatrix.
   *
   * \return  The angle around the z-axis between the original
   *          and the rotated x-axis projected on the x-y-plane
   */
  DATATYPE getZAngle() const
  {
        double h = sqrt(this->c[0].x * this->c[0].x + this->c[0].y * this->c[0].y);
        return h ? acos(this->c[0].x / h) * (this->c[0].y < 0 ? -1 : 1) : 0;
  }

  Vector3<DATATYPE> getAngleAxis() const
  {
    double co = (this->c[0].x + this->c[1].y + this->c[2].z - 1.f) * 0.5;
    if(co > 1.0)
      co = 1.0;
    else if(co < -1.0)
      co = 1.0;
    const double angle = acos(co);
    if(angle == 0.f)
      return Vector3<DATATYPE>();
    Vector3<DATATYPE> result(
      this->c[1].z - this->c[2].y,
      this->c[2].x - this->c[0].z,
      this->c[0].y - this->c[1].x);
    result *= angle / (2.0 * sin(angle));
    return result;
  }

/* rot2omega
   * @return the vector direction is the rotation axis, and the length is the rotation angle
   **/
  Vector3<DATATYPE> toQuaternion() const
  {
    DATATYPE theta = acos((this->c[0][0] + this->c[1][1] + this->c[2][2] - 1) / 2);
    DATATYPE k = theta / 2 / sin(theta);
    if (Math::isNan(k)) {
      k = 0.5;
    }
    return Vector3<DATATYPE> (this->c[1][2] - this->c[2][1],
      this->c[2][0] - this->c[0][2],
      this->c[0][1] - this->c[1][0]) * k;
  }

  void toCompactString(std::ostream& ost) const
  {
      ost << this->c[0].x <<' '<<this->c[0].y <<' '
          << this->c[1].x <<' '<<this->c[1].y <<' '
          << this->c[2].x <<' '<<this->c[2].y;
  }

  /**
   * Create and return a RotationMatrix, rotated around x-axis
   *
   * \param   angle 
   * \return  rotated RotationMatrix
   */
  static RotationMatrixT getRotationX(const DATATYPE angle)
  {
        double c = cos(angle),
               s = sin(angle);
        return RotationMatrixT(Vector3<DATATYPE>(1, 0, 0),
                Vector3<DATATYPE>(0, c, s),
                Vector3<DATATYPE>(0, -s, c));
  }

  /**
   * Create and return a RotationMatrix, rotated around y-axis
   *
   * \param   angle 
   * \return  rotated RotationMatrix
   */
  static RotationMatrixT getRotationY(const DATATYPE angle)
  {
      double c = cos(angle),
             s = sin(angle);
      return RotationMatrixT(Vector3<DATATYPE>(c, 0, -s),
              Vector3<DATATYPE>(0, 1, 0),
              Vector3<DATATYPE>(s, 0, c));
  }

  /**
    * Create and return a RotationMatrix, rotated around z-axis
    *
    * \param   angle
    * \return  rotated RotationMatrix
    */
  static RotationMatrixT getRotationZ(const DATATYPE angle)
  {
      double c = cos(angle),
              s = sin(angle);
      return RotationMatrixT(Vector3<DATATYPE>(c, s, 0),
              Vector3<DATATYPE>(-s, c, 0),
              Vector3<DATATYPE>(0, 0, 1));
  }

  static RotationMatrixT fromQuaternion(const Vector3<DATATYPE>& a, DATATYPE q)
  {
    RotationMatrixT A = RotationMatrixT<DATATYPE > (Vector3<DATATYPE > (0, a.z, -a.y),
      Vector3<DATATYPE > (-a.z, 0, a.x),
      Vector3<DATATYPE > (a.y, -a.x, 0));
    RotationMatrixT A2 = A*A;
    return Rodrigues(A, A2, q);
  }

  static RotationMatrixT interpolate(const RotationMatrixT& R0, const RotationMatrixT& Rf, DATATYPE t)
  {
    RotationMatrixT diff = R0.invert() * Rf;
    Vector3<DATATYPE> w = diff.toQuaternion();
    DATATYPE aerr = w.abs();

    if (aerr > std::numeric_limits<DATATYPE>::epsilon()) {
      w /= aerr;
      return R0 * RotationMatrixT::fromQuaternion(w, aerr * t);
    } else {
      return Rf;
    }
  }
};


template<typename T>
RotationMatrixT<T> Rodrigues(const RotationMatrixT<T>& A, const RotationMatrixT<T>& A2, T q)
{
  typedef Vector3<T> Vector;

  const static RotationMatrixT<T> I = RotationMatrixT<T>(Vector(1,0,0),Vector(0,1,0),Vector(0,0,1));

  return I + A*sin(q) + A2*(1-cos(q));
}

typedef RotationMatrixT<double> RotationMatrix;


#endif // _RotationMatrix_h_
