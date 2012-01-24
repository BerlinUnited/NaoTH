/** 
 * \file Matrix.h
 * Template classes for various 3x3 matrices.
 *
 * \author Martin Kallnik, martin.kallnik@gmx.de
 * \author Thomas Kindler, thomas.kindler@gmx.de 
 * \author Max Risler
 */

#ifndef __Matrix_h__
#define __Matrix_h__

#include <limits>
#include "Vector3.h"
#include "Matrix2x2.h"
#include "Common.h"

/**
 * This class represents a 3x3-matrix 
 *
 */
template <class V> class Matrix3x3 {
public:
  /** 
   * The columns of the matrix 
   */
  Vector3<V> c[3];

  /**
   * Default constructor. 
   */
  Matrix3x3<V>()
  {
    c[0]=Vector3<V>(1,0,0);
    c[1]=Vector3<V>(0,1,0);
    c[2]=Vector3<V>(0,0,1);
  }

  /**
   * Constructor.
   *
   * \param  c0  the first column of the matrix.
   * \param  c1  the second column of the matrix.
   * \param  c2  the third column of the matrix.
  */
  Matrix3x3<V>(
    const Vector3<V>& c0, 
    const Vector3<V>& c1, 
    const Vector3<V>& c2
  )
  {
    c[0] = c0;
    c[1] = c1;
    c[2] = c2;
  }

    V operator()(size_t i, size_t j) const
    {
        return c[i][j];
    }

  /**
   * Adds this matrix with another matrix.
   *
   * \param  other  The matrix this one is added to
   * \return         A new vector containing the result
   *                 of the calculation.
  */
  Matrix3x3<V> operator+(const Matrix3x3<V>& other) const
  {
    Matrix3x3<V> res(*this);
    res += other;
    return  res;
  }
   /**
   * Adds this matrix to another matrix.
   * 
   * \param  other  The other matrix that is added to this one
   * \return        A reference this object after the calculation.
  */
  Matrix3x3<V>& operator+=(const Matrix3x3<V>& other)
  {
    (*this).c[0]+=other.c[0]; 
    (*this).c[1]+=other.c[1]; 
    (*this).c[2]+=other.c[2];
    return *this;
  }

  /**
   * Substract this matrix with another matrix.
   *
   * \param  other  The matrix this one is substracted to
   * \return         A new vector containing the result
   *                 of the calculation.
  */
  Matrix3x3<V> operator-(const Matrix3x3<V>& other) const
  {
    Matrix3x3<V> res(*this);
    res -= other;
    return res;
  }

  /**
   * this matrix substracts another matrix.
   *
   * \param  other  The other matrix that is substracted to this one
   * \return        A reference this object after the calculation.
   */
  Matrix3x3<V>& operator-=(const Matrix3x3<V>& other)
  {
    (*this).c[0]-=other.c[0];
    (*this).c[1]-=other.c[1];
    (*this).c[2]-=other.c[2];
    return *this;
  }

  /**
   * Multiplication of this matrix by vector.
   *
   * \param  vector  The vector this one is multiplied by 
   * \return         A new vector containing the result
   *                 of the calculation.
  */
  Vector3<V> operator*(const Vector3<V>& vector) const
  {
    return (c[0]*vector.x + c[1]*vector.y + c[2]*vector.z);
  }

  /**
   * Multiplication of this matrix by another matrix.
   *
   * \param  other  The other matrix this one is multiplied by 
   * \return        A new matrix containing the result
   *                of the calculation.
  */
  Matrix3x3<V> operator*(const Matrix3x3<V>& other) const
  {
    return Matrix3x3<V>(
      (*this)*other.c[0], 
      (*this)*other.c[1], 
      (*this)*other.c[2]
    );
  }

  /**
   * Multiplication of this matrix by another matrix.
   * 
   * \param  other  The other matrix this one is multiplied by 
   * \return        A reference this object after the calculation.
  */
  Matrix3x3<V>& operator*=(const Matrix3x3<V>& other)
  {
    return *this = *this * other;
  }

  /**
   * Multiplication of this matrix by a factor.
   *
   * \param  factor  The factor this matrix is multiplied by 
   * \return         A reference to this object after the calculation.
  */
  Matrix3x3<V>& operator*=(const V& factor)
  {
    c[0] *= factor;
    c[1] *= factor;
    c[2] *= factor;
    return *this;
  }

  /**
   * Division of this matrix by a factor.
   *
   * \param  factor  The factor this matrix is divided by 
   * \return         A reference to this object after the calculation.
   */
  Matrix3x3<V>& operator/=(const V& factor)
  {
    *this *= 1 / factor;
    return *this;
  }

  /**
   * Multiplication of this matrix by a factor.
   *
   * \param  factor  The factor this matrix is multiplied by 
   * \return         A new object that contains the result of the calculation.
   */
  Matrix3x3<V> operator*(const V& factor) const
  {
    return Matrix3x3<V>(*this) *= factor;
  }

  /**
   * Division of this matrix by a factor.
   *
   * \param  factor  The factor this matrix is divided by 
   * \return         A new object that contains the result of the calculation.
   */
  Matrix3x3<V> operator/(const V& factor) const
  {
    return Matrix3x3<V>(*this) /= factor;
  }

  /**
   * Comparison of another matrix with this one.
   *
   * \param  other  The other matrix that will be compared to this one
   * \return        Whether the two matrices are equal.
   */
  bool operator==(const Matrix3x3<V>& other) const
  {
    return (
      c[0] == other.c[0] && 
      c[1] == other.c[1] && 
      c[2] == other.c[2]
    );
  }

  /**
   * Comparison of another matrix with this one.
   *
   * \param  other  The other matrix that will be compared to this one
   * \return        Whether the two matrixs are unequal.
   */
  bool operator!=(const Matrix3x3<V>& other) const
  {
    return !(*this == other);
  }

  /**
   * Array-like member access.
   * \param  i index
   * \return reference to column
   */
  Vector3<V>& operator[](int i) 
  {
    return c[i];
  }

  const Vector3<V>& operator [](int i) const
  {
      return c[i];
  }
  
  /**
   * Transpose the matrix
   *
   * \return  A new object containing transposed matrix
   */
  Matrix3x3<V> transpose() const
  {
    return Matrix3x3<V>(
      Vector3<V>(c[0].x, c[1].x, c[2].x),
      Vector3<V>(c[0].y, c[1].y, c[2].y),
      Vector3<V>(c[0].z, c[1].z, c[2].z)
    );
  }
  
  /**
   * Calculation of the determinant of this matrix.
   * 
   * \return The determinant.
   */
  V det() const 
  { 
    return 
      c[0].x * (c[1].y * c[2].z - c[1].z * c[2].y) +
      c[0].y * (c[1].z * c[2].x - c[1].x * c[2].z) +
      c[0].z * (c[1].x * c[2].y - c[1].y * c[2].x);
  }
  
  /**
   * Calculate determinant of 2x2 Submatrix  
   * | a b |
   * | c d |
   *
   * \return  determinant.
   */
  static V det2(V a, V b, V c, V d)
  {
    return a*d - b*c;
  }

  /**
   * Calculate the adjoint of this matrix.
   *
   * \return the adjoint matrix.
   */
  Matrix3x3<V> adjoint() const
  {
    return Matrix3x3<V>(
      Vector3<V>(
        det2(c[1].y, c[2].y, c[1].z, c[2].z), 
        det2(c[2].x, c[1].x, c[2].z, c[1].z), 
        det2(c[1].x, c[2].x, c[1].y, c[2].y)
      ),
      Vector3<V>(
        det2(c[2].y, c[0].y, c[2].z, c[0].z), 
        det2(c[0].x, c[2].x, c[0].z, c[2].z), 
        det2(c[2].x, c[0].x, c[2].y, c[0].y)
      ),
      Vector3<V>(
        det2(c[0].y, c[1].y, c[0].z, c[1].z), 
        det2(c[1].x, c[0].x, c[1].z, c[0].z), 
        det2(c[0].x, c[1].x, c[0].y, c[1].y)      
      )
    );
  
  }  

  /**
   * Calculate the inverse of this matrix.
   *
   * \return The inverse matrix
   */
  Matrix3x3<V> invert() const
  {
    return adjoint().transpose() / det();
  }

  void toDataStream(std::ostream& stream) const
  {
    for(int i=0; i<3; i++) {
      c[i].toDataStream(stream);
    }
  }

  void fromDataStream(std::istream& stream, size_t size)
  {
    if (9 * sizeof (V) == size) {
      for (int i = 0; i < 3; i++) {
        c[i].fromDataStream(stream, 3 * sizeof (V));
      }
    }
  }
};

template <typename DATATYPE>
std::ostream& operator <<(std::ostream& ost, const Matrix3x3<DATATYPE>& v)
{
    ost << v.c[0] << " " << v.c[1] << " "<< v.c[2];
    return ost;
}

template <typename DATATYPE>
std::istream& operator >>(std::istream& ist, Matrix3x3<DATATYPE>& v)
{
    ist >> v.c[0] >> v.c[1] >> v.c[2];
    return ist;
}

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

#endif // __Matrix_h__
