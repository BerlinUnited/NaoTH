
#include <Tools/Math/Vector3.h>

template <class V = double> class Matrix2x3;
template <class V = double> class Matrix3x2;

/**
* @class Matrix3x2
* An incomplete implementation of Matrix3x2.
*/
template <class V> class Matrix3x2
{
public:
  Vector3<V> c0; /**< The first column of the matrix. */
  Vector3<V> c1; /**< The second column of the matrix. */

  /** Default constructor. */
  Matrix3x2<V>() {}

  /**
  * Constructor; initializes each column of the matrix.
  * @param c0 The first column of the matrix.
  * @param c1 The second column of the matrix.
  */
  Matrix3x2<V>(const Vector3<V>& c0, const Vector3<V>& c1) :
    c0(c0), c1(c1) {}

  /**
  * Adds another matrix to this one (component by component).
  * @param other The matrix to add.
  * @return A reference this object after the calculation.
  */
  Matrix3x2<V>& operator+=(const Matrix3x2<V>& other)
  {
    c0 += other.c0;
    c1 += other.c1;
    return *this;
  }

  /**
  * Computes the sum of two matrices
  * @param other Another matrix
  * @return The sum
  */
  Matrix3x2<V> operator+(const Matrix3x2<V>& other) const
  {
    return Matrix3x2<V>(*this) += other;
  }

  /**
  * Division of this matrix by a factor.
  * @param factor The factor this matrix is divided by
  * @return A reference to this object after the calculation.
  */
  Matrix3x2<V>& operator/=(const V& factor)
  {
    c0 /= factor;
    c1 /= factor;
    return *this;
  }

  /**
  * Multiplication of this matrix by a factor.
  * @param factor The factor this matrix is multiplied by
  * @return A reference to this object after the calculation.
  */
  Matrix3x2<V>& operator*=(const V& factor)
  {
    c0 *= factor;
    c1 *= factor;
    return *this;
  }

  /**
  * Transposes the matrix.
  * @return A new object containing transposed matrix
  */
  Matrix2x3<V> transpose()
  {
    return Matrix2x3<V>(Vector2<V>(c0.x, c1.x), Vector2<V>(c0.y, c1.y), Vector2<V>(c0.z, c1.z));
  }
};

/**
* @class Matrix2x3
* An incomplete implementation of Matrix2x3.
*/
template <class V> class Matrix2x3
{
public:
  Vector2<V> c0; /**< The first column of the matrix. */
  Vector2<V> c1; /**< The second column of the matrix. */
  Vector2<V> c2; /**< The third column of the matrix. */

  /** Default constructor. */
  Matrix2x3<V>() {}

  /**
  * Constructor; initializes each column of the matrix.
  * @param c0 The first column of the matrix.
  * @param c1 The second column of the matrix.
  * @param c2 The third column of the matrix.
  */
  Matrix2x3<V>(const Vector2<V>& c0, const Vector2<V>& c1, const Vector2<V>& c2) :
    c0(c0), c1(c1), c2(c2) {}

  /**
  * Multiplication of this matrix by vector.
  * @param vector The vector this one is multiplied by
  * @return A new vector containing the result of the calculation.
  */
  Vector2<V> operator*(const Vector3<V>& vector) const
  {
    return c0 * vector.x + c1 * vector.y + c2 * vector.z;
  }

  /**
  * Multiplication of this matrix by a 3x3 matrix.
  * @param matrix The other matrix this one is multiplied by .
  * @return A new matrix containing the result of the calculation.
  */
  Matrix2x3<V> operator*(const Matrix3x3<V>& matrix) const
  {
    return Matrix2x3<V>(*this * matrix[0], *this * matrix[1], *this * matrix[2]);
  }

  /**
  * Multiplication of this matrix by a 3x2 matrix.
  * @param matrix The other matrix this one is multiplied by .
  * @return A new matrix containing the result of the calculation.
  */
  Matrix2x2<V> operator*(const Matrix3x2<V>& matrix) const
  {
    return Matrix2x2<V>(*this * matrix.c0, *this * matrix.c1);
  }

  /**
  * Transposes the matrix.
  * @return A new object containing transposed matrix
  */
  Matrix3x2<V> transpose()
  {
    return Matrix2x3<V>(Vector2<V>(c0.x, c1.x, c2.x), Vector2<V>(c0.y, c1.y, c2.y));
  }
};

typedef Matrix3x2<double> Matrix3x2d;
typedef Matrix2x3<double> Matrix2x3d;