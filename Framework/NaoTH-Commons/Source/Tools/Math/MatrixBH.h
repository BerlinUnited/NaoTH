/**
* @file MatrixBH.h
* Contains template class Matrix of type V and size mxn
* @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
* @author Max Risler
* @author Colin Graf
*/

#pragma once

#include "Tools/Debug/NaoTHAssert.h"

template <int n, class V> struct VectorData
{
  V v[n];
};

template <class V> struct VectorData<2, V>
{
  V x;
  V y;
};

template <class V> struct VectorData<3, V>
{
  V x;
  V y;
  V z;
};

/** This class represents a n-dimensional vector */
template <int n = 2, class V = float> class Vector : public VectorData<n, V>
{
public:
  /** Default constructor. */
  Vector<n, V>()
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] = V();
  }

  /** constructor for a 2-dimensional vector. */
  Vector<n, V>(V x, V y)
  {
    ASSERT(n == 2);
    (*this)[0] = x;
    (*this)[1] = y;
  }

  /** constructor for a 3-dimensional vector. */
  Vector<n, V>(V x, V y, V z)
  {
    ASSERT(n == 3);
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
  }

  /** constructor for a 4-dimensional vector. */
  Vector<n, V>(V x, V y, V z, V w)
  {
    ASSERT(n == 4);
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = w;
  }
  /** constructor for a n-dimensional vector. */
  /*
  Vector<n, V>(V x, V y, V z, V w, ...)
  {
    (*this)[0] = x;
    (*this)[1] = y;
    (*this)[2] = z;
    (*this)[3] = w;
    va_list vl;
    va_start(vl, w);
    for(int i = 4; i < n; ++i)
      (*this)[i] = va_arg(vl, V);
    va_end(vl);
  }
  */

  /**
  * Copy constructor
  * @param other The other vector that is copied to this one
  */
  Vector<n, V>(const Vector<n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] = other[i];
  }

  /**
  * Assignment operator
  * @param other The other vector that is assigned to this one
  * @return A reference to this object after the assignment.
  */
  Vector<n, V>& operator=(const Vector<n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] = other[i];
    return *this;
  }

  /**
  * Addition of another vector to this one.
  * @param other The other vector that will be added to this one
  * @return A reference to this object after the calculation.
  */
  Vector<n, V>& operator+=(const Vector<n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] += other[i];
    return *this;
  }

  /**
  * Substraction of this vector from another one.
  * @param other The other vector this one will be substracted from
  * @return A reference to this object after the calculation.
  */
  Vector<n, V>& operator-=(const Vector<n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] -= other[i];
    return *this;
  }

  /**
  * Multiplication of this vector by a factor.
  * @param factor The factor this vector is multiplied by
  * @return A reference to this object after the calculation.
  */
  Vector<n, V>& operator*=(const V& factor)
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] *= factor;
    return *this;
  }

  /**
  * Division of this vector by a factor.
  * @param factor The factor this vector is divided by
  * @return A reference to this object after the calculation.
  */
  Vector<n, V>& operator/=(const V& factor)
  {
    for(int i = 0; i < n; ++i)
      (*this)[i] /= factor;
    return *this;
  }

  /**
  * Addition of another vector to this one.
  * @param other The other vector that will be added to this one
  * @return A new object that contains the result of the calculation.
  */
  Vector<n, V> operator+(const Vector<n, V>& other) const
  {
    return Vector<n, V>(*this) += other;
  }

  /**
  * Subtraction of another vector to this one.
  * @param other The other vector that will be added to this one
  * @return A new object that contains the result of the calculation.
  */
  Vector<n, V> operator-(const Vector<n, V>& other) const
  {
    return Vector<n, V>(*this) -= other;
  }

  /**
  * Inner product of this vector and another one.
  * @param other The other vector this one will be multiplied by
  * @return The inner product.
  */
  V operator*(const Vector<n, V>& other) const
  {
    V result = (*this)[0] * other[0];
    for(int i = 1; i < n; ++i)
      result += (*this)[i] * other[i];
    return result;
  }

  /**
  * Multiplication of this vector by a factor.
  * @param factor The factor this vector is multiplied by
  * @return A new object that contains the result of the calculation.
  */
  Vector<n, V> operator*(const V& factor) const
  {
    return Vector<n, V>(*this) *= factor;
  }

  /**
  * Division of this vector by a factor.
  * @param factor The factor this vector is divided by
  * @return A new object that contains the result of the calculation.
  */
  Vector<n, V> operator/(const V& factor) const
  {
    return Vector<n, V>(*this) /= factor;
  }

  /**
  * Negation of this vector.
  * @return A new object that contains the result of the calculation.
  */
  Vector<n, V> operator-() const
  {
    return Vector<n, V>() -= *this;
  }

  /**
  * Comparison of another vector with this one.
  * @param other The other vector that will be compared to this one
  * @return Whether the two vectors are equal.
  */
  bool operator==(const Vector<n, V>& other) const
  {
    for(int i = 0; i < n; ++i)
      if((*this)[i] != other[i])
        return false;
    return true;
  }

  /**
  * Comparison of another vector with this one.
  * @param other The other vector that will be compared to this one
  * @return Whether the two vectors are unequal.
  */
  bool operator!=(const Vector<n, V>& other) const
  {
    for(int i = 0; i < n; ++i)
      if((*this)[i] != other[i])
        return true;
    return false;
  }

  /**
  * array-like member access.
  * @param i index of coordinate
  * @return reference to the coordinate
  */
  inline V& operator[](int i)
  {
    return ((V*)this)[i];
  }

  /**
  * array-like member access.
  * @param i index of coordinate
  * @return reference to the coordinate
  */
  inline const V& operator[](int i) const
  {
    return ((const V*)this)[i];
  }

  /**
  * Calculation of the square length of this vector.
  * @return length*length.
  */
  V sqr() const
  {
    V result = (*this)[0] * (*this)[0];
    for(int i = 1; i < n; ++i)
      result += (*this)[i] * (*this)[i];
    return result;
  }

  /**
  * Calculation of the length of this vector.
  * @return length*length.
  */
  V abs() const
  {
    return sqrt(sqr());
  }

};

typedef Vector<2, float> Vector2f;
typedef Vector<3, float> Vector3f;
typedef Vector<4, float> Vector4f;

typedef Vector<2, int> Vector2i;
typedef Vector<3, int> Vector3i;
typedef Vector<4, int> Vector4i;


/** This class represents a mxn-matrix */
template <int m = 2, int n = 2, class V = float> class Matrix
{
public:
  /** The columns of the matrix */
  Vector<m, V> c[n];

  /** Default constructor. */
  Matrix<m, n, V>() {}

  Matrix<m, n, V>(V v)
  {
    ASSERT(m == n);
    const int mnm = n < m ? n : m;
    for(int i = 0; i < mnm; ++i)
      c[i][i] = v;
  }

  /** Constructor */
  Matrix<m, n, V>(const Vector<m, V>& c0, const Vector<m, V>& c1)
  {
    ASSERT(n == 2);
    c[0] = c0;
    c[1] = c1;
  }

  /** Constructor */
  Matrix<m, n, V>(const Vector<m, V>& c0, const Vector<m, V>& c1, const Vector<m, V>& c2)
  {
    ASSERT(n == 3);
    c[0] = c0;
    c[1] = c1;
    c[2] = c2;
  }

  /** Constructor */
  Matrix<m, n, V>(const Vector<m, V>& c0, const Vector<m, V>& c1, const Vector<m, V>& c2, const Vector<m, V>& c3)
  {
    ASSERT(n == 4);
    c[0] = c0;
    c[1] = c1;
    c[2] = c2;
    c[3] = c3;
  }

  /** Constructor */
  /*
  Matrix<m, n, V>(const Vector<m, V>& c0, const Vector<m, V>& c1, const Vector<m, V>& c2, const Vector<m, V>& c3, ...)
  {
    c[0] = c0;
    c[1] = c1;
    c[2] = c2;
    c[3] = c3;
    va_list vl;
    va_start(vl, c3);
    for(int i = 4; i < n; ++i)
      (*this)[i] = va_arg(vl, const Vector<m, V>&);
    va_end(vl);
  }
  */

  /**
  * Assignment operator
  * @param other The other matrix that is assigned to this one
  * @return A reference to this object after the assignment.
  */
  Matrix<m, n, V>& operator=(const Matrix<m, n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      c[i] = other.c[i];
    return *this;
  }

  /**
  * Array-like member access.
  * @param i index
  * @return reference to column
  */
  inline Vector<m, V>& operator[](int i)
  {
    return c[i];
  }

  /**
  * const array-like member access.
  * @param i index
  * @return reference to column
  */
  inline const Vector<m, V>& operator[](int i) const
  {
    return c[i];
  }

  /**
  * Multiplication of this matrix by vector.
  * @param vector The vector this one is multiplied by
  * @return A reference to a new vector containing the result of the calculation.
  */
  Vector<m, V> operator*(const Vector<n, V>& vector) const
  {
    Vector<m, V> result = c[0] * vector[0];
    for(int i = 1; i < n; ++i)
      result += c[i] * vector[i];
    return result;
  }

  /**
  * Multiplication of this matrix by another matrix.
  * @param other The other matrix this one is multiplied by
  * @return An object containing the result of the calculation.
  */
  template<int o> Matrix<m, o, V> operator*(const Matrix<n, o, V>& other) const
  {
    Matrix<m, o, V> result;
    for(int i = 0; i < n; ++i)
      for(int j = 0; j < m; ++j)
        for(int k = 0; k < o; ++k)
          result.c[k][j] += c[i][j] * other.c[k][i];
    return result;
  }

  /**
  * Multiplication of this matrix by another matrix.
  * @param other The other matrix this one is multiplied by
  * @return A reference this object after the calculation.
  */
  Matrix<n, n, V> operator*=(const Matrix<n, n, V>& other)
  {
    return *this = *this * other;
  }

  /**
  * Multiplication of this matrix by a factor.
  * @param factor The factor this matrix is multiplied by
  * @return A reference to this object after the calculation.
  */
  Matrix<m, n, V>& operator*=(const V& factor)
  {
    for(int i = 0; i < n; ++i)
      c[i] *= factor;
    return *this;
  }

  /**
  * Multiplication of this matrix by a factor.
  * @param factor The factor this matrix is multiplied by
  * @return A new object that contains the result of the calculation.
  */
  Matrix<m, n, V> operator*(const V& factor) const
  {
    return Matrix<m, n, V>(*this) *= factor;
  }

  /**
  * Division of this matrix by a factor.
  * @param factor The factor this matrix is divided by
  * @return A reference to this object after the calculation.
  */
  Matrix<m, n, V>& operator/=(const V& factor)
  {
    for(int i = 0; i < n; ++i)
      c[i] /= factor;
    return *this;
  }

  /**
  * Division of this matrix by a factor.
  * @param factor The factor this matrix is divided by
  * @return A new object that contains the result of the calculation.
  */
  Matrix<m, n, V> operator/(const V& factor) const
  {
    return Matrix<m, n, V>(*this) /= factor;
  }

  /**
  * Adds another matrix.
  * @param other The other matrix that is added to this one
  * @return A reference to this object after the calculation.
  */
  Matrix<m, n, V>& operator+=(const Matrix<m, n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      c[i] += other.c[i];
    return *this;
  }

  /**
  * Computes the sum of two matrices
  * @param other Another matrix
  * @return The sum
  */
  Matrix<m, n, V> operator+(const Matrix<m, n, V>& other) const
  {
    return Matrix<m, n, V>(*this) += other;
  }

  /**
  * Subtracts another matrix.
  * @param other The other matrix that is subtracted from this one
  * @return A reference to this object after the calculation.
  */
  Matrix<m, n, V>& operator-=(const Matrix<m, n, V>& other)
  {
    for(int i = 0; i < n; ++i)
      c[i] -= other.c[i];
    return *this;
  }

  /**
  * Computes the difference of two matrices
  * @param other Another matrix
  * @return The difference
  */
  Matrix<m, n, V> operator-(const Matrix<m, n, V>& other) const
  {
    return Matrix<m, n, V>(*this) -= other;
  }

  /**
  * Comparison of another matrix with this one.
  * @param other The other matrix that will be compared to this one
  * @return Whether the two matrices are equal.
  */
  bool operator==(const Matrix<m, n, V>& other) const
  {
    for(int i = 0; i < n; ++i)
      if(c[i] != other.c[i])
        return false;
    return true;
  }

  /**
  * Comparison of another matrix with this one.
  * @param other The other matrix that will be compared to this one
  * @return Whether the two matrixs are unequal.
  */
  bool operator!=(const Matrix<m, n, V>& other) const
  {
    for(int i = 0; i < n; ++i)
      if(c[i] != other.c[i])
        return true;
    return false;
  }

  /**
  * Transpose the matrix
  * @return A new object containing transposed matrix
  */
  Matrix<n, m, V> transpose() const
  {
    Matrix<n, m, V> result;
    for(int i = 0; i < n; ++i)
      for(int j = 0; j < m; ++j)
        result.c[j][i] = c[i][j];
    return result;
  }

  /**
  * Calculation of the determinant of this matrix.
  * @return The determinant.
  */
  V det() const;

  /**
  * Calculate the adjoint of this matrix.
  * @return the adjoint matrix.
  */
  Matrix<m, n, V> adjoint() const;

  /**
  * Calculate the inverse of this matrix.
  * @return The inverse matrix
  */
  Matrix<m, n, V> invert() const;

  /**
  * Solves the system A*x=b where A is the actual matrix
  * @param b Vector b
  * @param x Solution x
  * @return Whether solving was possible
  */
  bool solve(const Vector<n, V>& b, Vector<n, V>& x) const;
};


typedef Matrix<2, 2, float> Matrix2x2f;
typedef Matrix<3, 3, float> Matrix3x3f;
typedef Matrix<4, 4, float> Matrix4x4f;
typedef Matrix<4, 2, float> Matrix4x2f;
typedef Matrix<2, 4, float> Matrix2x4f;
