/**
 * @file Vector3.h
 * Contains template class Vector3 of type V
 *
 */

#ifndef _Vector3_h_
#define _Vector3_h_

#include <cmath>
#include <ostream>
#include <istream>


/** This class represents a 3-vector */
template <class V> class Vector3
{
  public:

  /** The vector values */
  V x, y, z;

  /**
  * Constructors
  */
  Vector3<V>() : x(0),y(0),z(0) {}
  Vector3<V>(V x, V y, V z) : x(x), y(y), z(z) {}

  // conversion copy constructor, used to cast the type
  template<class W>
  Vector3<V>(const Vector3<W>& other) :
    x(static_cast<V>(other.x)),
    y(static_cast<V>(other.y)),
    z(static_cast<V>(other.z))
  {}

  /** Addition of another vector to this one. 
  *\param other The other vector that will be added to this one
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator+=(const Vector3<V>& other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  /** Addition of a scalar to this vector.
  *\param other The v scalar that will be added to each component of this vector.
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator+=(const V v)
  {
    x += v;
    y += v;
    z += v;
    return *this;
  }

  /** Substraction of this vector from another one.
  *\param other The other vector this one will be substracted from 
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator-=(const Vector3<V>& other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  /** Substraction of a scalar from this vector.
  *\param v The scalar that will be substracted from each component of this vector.
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator-=(const V v)
  {
    x -= v;
    y -= v;
    z -= v;
    return *this;
  }

  /** Multiplication of this vector by a factor.
  *\param factor The factor this vector is multiplied by 
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator*=(const V factor)
  {
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
  }

  /** Division of this vector by a factor.
  *\param factor The factor this vector is divided by 
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator/=(const V factor)
  {
    V f = 1/factor;
    return (*this) *= f;
  }

  /** Addition of another vector to this one.
  *\param other The other vector that will be added to this one
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator+(const Vector3<V>& other) const {
    return Vector3<V>(*this) += other;
  }

  /** Addition of a scalar.
  *\param other The v scalar that will be added to x and y
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator+(const V v) const {
    return Vector3<V>(*this) += v;
  }

  /** Subtraction of another vector to this one.
  *\param other The other vector that will be added to this one
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator-(const Vector3<V>& other) const {
    return Vector3<V>(*this) -= other;
  }

  /** Subtraction of a scalar.
  *\param other The v scalar that will be subtracted from x and y
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator-(const V v) const {
    return Vector3<V>(*this) -= v;
  }

  /** Negation of this vector.
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator-() const {
    return Vector3<V>() -= *this;
  }

  /** Inner product of this vector and another one.
  *\param other The other vector this one will be multiplied by 
  *\return The inner product.
  */
  V operator*(const Vector3<V>& other) const {
    return (x*other.x + y*other.y + z*other.z);
  }

  /** Multiplication of this vector by a factor.
  *\param factor The factor this vector is multiplied by 
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator*(const V factor) const {
    return Vector3<V>(*this) *= factor;
  }

  /** Division of this vector by a factor.
  *
  *\param factor The factor this vector is divided by 
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator/(const V factor) const {
    return Vector3<V>(*this) /= factor;
  }

  /** Crossproduct of this vector and another vector.
  *\param other The factor this vector is multiplied with.
  *\return A new object that contains the result of the calculation.
  */
  Vector3<V> operator^(const Vector3<V>& other) const {
    return Vector3<V>(y * other.z - z * other.y, 
                      z * other.x - x * other.z, 
                      x * other.y - y * other.x);
  }

  /** Crossproduct of this vector and another vector.
  *\param other The factor this vector is multiplied with.
  *\return A reference to this object after the calculation.
  */
  Vector3<V>& operator^=(const Vector3<V>& other) {
    *this = *this ^ other; 
    return *this;
  }

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are equal.
  */
  bool operator==(const Vector3<V>& other) const {
    return (x==other.x && y==other.y && z==other.z);
  }

  /** Comparison of another vector with this one.
  *\param other The other vector that will be compared to this one
  *\return Whether the two vectors are unequal.
  */
  bool operator!=(const Vector3<V>& other) const {
    return !(*this == other);
  }

  /**
  * array-like member access.
  * \param i index of coordinate
  * \return reference to x, y or z
  */
  V& operator[](int i) {
    return  (&x)[i];
  }

  V operator[](int i) const {
    return (&x)[i];
  }
  
  /** Calculation of the length of this vector.
  *\return The length.
  */
  V abs() const {
    return static_cast<V>(std::sqrt(static_cast<double>(x*x + y*y + z*z)));
  }

  /** calculate the sqr of length
   */
  V abs2() const { 
    return x*x + y*y + z*z; 
  }

  /** normalize this vector.
  *\param len The length, the vector should be normalized to, default=1.
  *\return the normalized vector.
  */
  Vector3<V> normalize(V len)
  {
    V lenghtOfVector = abs();
    if (lenghtOfVector == 0) return *this;
    return *this = (*this * len) / lenghtOfVector;
  }

 /** normalize this vector.
  *\return the normalized vector.
  */
  Vector3<V> normalize() {
    return normalize(static_cast<V>(1));
  }

  // some prefefined vectors for convenience
  static const Vector3<V> zero;
  static const Vector3<V> e1;
  static const Vector3<V> e2;
  static const Vector3<V> e3;

  void toDataStream(std::ostream& stream) const
  {
    stream.write((const char*) (&x), sizeof (V));
    stream.write((const char*) (&y), sizeof (V));
    stream.write((const char*) (&z), sizeof (V));
  }

  void fromDataStream(std::istream& stream, size_t size)
  {
    if (3 * sizeof (V) == size) {
      stream.read((char*) (&x), sizeof (V));
      stream.read((char*) (&y), sizeof (V));
      stream.read((char*) (&z), sizeof (V));
    }
  }
 
};

//
template<typename V> Vector3<V> const Vector3<V>::zero(0,0,0);
template<typename V> Vector3<V> const Vector3<V>::e1(1,0,0);
template<typename V> Vector3<V> const Vector3<V>::e2(0,1,0);
template<typename V> Vector3<V> const Vector3<V>::e3(0,0,1);


template <typename DATATYPE>
std::ostream& operator <<(std::ostream& ost, const Vector3<DATATYPE>& v)
{
    ost << v.x << " " << v.y << " "<< v.z;
    return ost;
}

template <typename DATATYPE>
std::istream& operator >>(std::istream& ist, Vector3<DATATYPE>& v)
{
    ist >> v.x >> v.y >> v.z;
    return ist;
}

/**
 * convert the polar to the Descartes
 *
 * @param[in] pol the Vector3 of the polar
 * which stands for ( distance, theta, phi )
 *
 * @return Descartes
 */
template <typename DATATYPE>
Vector3<DATATYPE> pol2xyz(const Vector3<DATATYPE> & v)
{
  DATATYPE cz = cos(v.z);
  return Vector3<DATATYPE> (
    v.x * cos(v.y) * cz,
    v.x * sin(v.y) * cz,
    v.x * sin(v.z)
  );
}

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
typedef Vector3<double> Vector3i;

#endif // _Vector3_h_

