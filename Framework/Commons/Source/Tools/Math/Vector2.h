#ifndef __Vector2_h__
#define __Vector2_h__

#include <cmath>
#include <ostream>

/** This class represents a 2-vector */
template <class V> class Vector2{
  public:
  /** The vector values */
  V x,y;

  /** Default constructor. */
  Vector2<V>():x(0),y(0)
  {
  }
  /** Default constructor. */
  Vector2<V>(V x, V y):x(x),y(y)
  {
  }

  // copy constructor
  template<class W>
  Vector2<V>(Vector2<W> other):x((V)other.x),y((V)other.y)
  {
  }

  /** Assignment operator
  *\param other The other vector that is assigned to this one
  *\return A reference to this object after the assignment.
  */
  Vector2<V>& operator=(const Vector2<V>& other)
  {
    x=other.x;y=other.y;
    return *this;
  }

  /** Assignment operator which converts a vector from another type
  *\param other The other vector that is assigned to this one
  *\return A reference to this object after the assignment.
  */
  template<class W>
  Vector2<V>& operator=(const Vector2<W>& other)
  {
    x=(V)other.x;y=(V)other.y;
    return *this;
  }

  /** Addition of another vector to this one.
  *\param other The other vector that will be added to this one
  *\return A reference to this object after the calculation.
  */
  Vector2<V>& operator+=(const Vector2<V>& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  /** Substraction of this vector from another one.
  *\param other The other vector this one will be substracted from
  *\return A reference to this object after the calculation.
  */
  Vector2<V>& operator-=(const Vector2<V>& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  /** Multiplication of this vector by a factor.
  *\param factor The factor this vector is multiplied by
  *\return A reference to this object after the calculation.
  */
  Vector2<V>& operator*=(const V& factor)
  {
    x *= factor;
    y *= factor;
    return *this;
  }

  /** Division of this vector by a factor.
  *\param factor The factor this vector is divided by
  *\return A reference to this object after the calculation.
  */
  Vector2<V>& operator/=(const V& factor)
  {
    if (factor == 0) return *this;
    x /= factor;
    y /= factor;
    return *this;
  }

  /** Addition of another vector to this one.
  *\param other The other vector that will be added to this one
  *\return A new object that contains the result of the calculation.
  */
  Vector2<V> operator+(const Vector2<V>& other) const
    {return Vector2<V>(*this) += other;}

  /** Subtraction of another vector to this one.
  *\param other The other vector that will be added to this one
  *\return A new object that contains the result of the calculation.
  */
  Vector2<V> operator-(const Vector2<V>& other) const
    {return Vector2<V>(*this) -= other;}

  /** Negation of this vector.
  *\return A new object that contains the result of the calculation.
  */
  Vector2<V> operator-() const
    {return Vector2<V>() -= *this;}

  /** Inner product of this vector and another one.
  *\param other The other vector this one will be multiplied by
  *\return The inner product.
  */
  V operator*(const Vector2<V>& other) const
  {
    return x * other.x + y * other.y;
  }

  /** Multiplication of this vector by a factor.
  *\param factor The factor this vector is multiplied by
  *\return A new object that contains the result of the calculation.
  */
  Vector2<V> operator*(const V& factor) const
    {return Vector2<V>(*this) *= factor;}

  /** Division of this vector by a factor.
  *
  *\param factor The factor this vector is divided by
  *\return A new object that contains the result of the calculation.
  */
  Vector2<V> operator/(const V& factor) const
    {return Vector2<V>(*this) /= factor;}

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are equal.
  */
  bool operator==(const Vector2<V>& other) const
  {
    return (x==other.x && y==other.y);
  }

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one.
  *\return Whether the two vectors are unequal.
  */
  bool operator!=(const Vector2<V>& other) const
    {return !(*this == other);}

  /** Calculation of the length of this vector.
  *\return The length.
  */
  V abs() const
  {return static_cast<V>(std::sqrt(static_cast<double>(x*x+y*y)));}

  /** calculation of the length^2 */
  V abs2() const
  { return x*x+y*y; }

  /** normalize this vector.
  * (changes the object)
  *\param len The length, the vector should be normalized to, default=1.
  *\return the normalized vector.
  */
  Vector2<V>& normalize(V len)
  {
    if (x == 0 && y == 0) return *this;
    return *this = (*this * len) / abs();
  }

  /** normalize this vector.
  * (changes the object)
  *\return the normalized vector.
  */
  Vector2<V>& normalize()
  {
    if (abs() == 0) return *this;
    return *this /= abs();
  }

  /** transpose this vector.
  * (changes the object)
  *\return the transposed vector.
  */
  Vector2<V>& transpose()
  { 
    V buffer = x;
    x = y;
    y = buffer;
    return *this;
  }

  /** the vector is rotated left by 90 degrees.
  * (changes the object)
  *\return the rotated vector.
  */
  Vector2<V>& rotateLeft()
  { 
    V buffer = -y;
    y = x;
    x = buffer;
    return *this;
  }

  /** the vector around a given angle
   * (changes the object)
   */
  Vector2<V>& rotate(double angle)
  { 
    double s=sin(angle);
    double c=cos(angle);
    V bufferX = x;
    V bufferY = y;
    x = static_cast<V>(c*bufferX - s*bufferY);
    y = static_cast<V>(s*bufferX + c*bufferY);
    return *this;
  }

  /** the vector is rotated right by 90 degrees.
  * (changes the object)
  *\return the rotated vector.
  */
  Vector2<V>& rotateRight()
  { 
    V buffer = -x;
    x = y;
    y = buffer;
    return *this;
  }

  /**
  * array-like member access.
  * \param i index of coordinate
  * \return reference to x or y
  */
  V& operator[](int i)
  {
    return  (&x)[i];
  }
 
  V operator[](int i) const
  {
      return (&x)[i];
  }

  /** 
   * Calculation of the angle of this vector 
   * The result is allways double.
   */
  double angle() const
  {return std::atan2(static_cast<double>(y),static_cast<double>(x));}
};

template <typename DATATYPE>
std::ostream& operator <<(std::ostream& ost, const Vector2<DATATYPE>& v)
{
    ost << v.x << " " << v.y;
    return ost;
}

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;
typedef Vector2<int> Vector2i;

#endif // __Vector2_h__
