/** 
 * \file Matrix3x3.h
 * Template classes for various 3x3 matrices.
 *
 * \author Martin Kallnik, martin.kallnik@gmx.de
 * \author Thomas Kindler, thomas.kindler@gmx.de 
 * \author Max Risler
 */

#ifndef _Matrix3x3_h_
#define _Matrix3x3_h_

#include <limits>
#include "Vector3.h"
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
  inline Vector3<V>& operator[](int i) 
  {
    return c[i];
  }

  inline const Vector3<V>& operator [](int i) const
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

typedef Matrix3x3<double> Matrix3d;
typedef Matrix3x3<float> Matrix3f;

#endif // _Matrix3x3_h_
